#include "abstractjsonhandler.h"
#include "httpconnection.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QByteArray>
#include <QDebug>
#include <QMetaType>
#include <Tufao/ThreadedHttpServer>
#include <QUrl>
#include <QUrlQuery>
#include <QStringList>

AbstractJsonHandler::AbstractJsonHandler(QObject *parent) :
    QObject(parent), initialized(false), currentConnection(0)
{
    Tufao::tDebug()<<"JSON Init";
}


bool AbstractJsonHandler::handleRequest(Tufao::HttpServerRequest &request, Tufao::HttpServerResponse &response)
{
    Tufao::tDebug()<<"JSON handleRequest "<<request.method();
    //only post and get requests are allowed for now
    if(request.method() != "POST" && request.method() != "GET"){
        response.writeHead(Tufao::HttpResponseStatus::METHOD_NOT_ALLOWED);
        response.end();
        return true;
    }

    HttpConnection* connection = new HttpConnection(request,response);

    //wait for the full message
    connect(connection,&HttpConnection::requestEnd,this,&AbstractJsonHandler::onRequestEnd);
    return true;
}

HttpConnection *AbstractJsonHandler::connection()
{
    return currentConnection;
}

void AbstractJsonHandler::sendError(HttpConnection* conn, const QJsonValue &id, const int code, const QString &message) const
{
    QJsonObject envelope;
    envelope.insert(QString("jsonrpc"),QString("2.0"));
    envelope.insert(QString("id"),id.type() == QJsonValue::Undefined ? QJsonValue::Null : id);

    QJsonObject error;
    error.insert(QString("code"),code);
    error.insert(QString("message"),message);

    envelope.insert(QString("error"),error);

    QJsonDocument doc(envelope);
    conn->response().writeHead(Tufao::HttpResponseStatus::OK);
    conn->response().end(doc.toJson());
}

void AbstractJsonHandler::createMethodCache()
{
    const QMetaObject* meta = metaObject();
    int offset = meta->methodOffset();
    int count  = meta->methodCount();

    for(int i = offset; i < count; i++){
        QMetaMethod method = meta->method(i);
        if(method.access() != QMetaMethod::Public)
            continue;
        if(method.methodType() != QMetaMethod::Slot
                && method.methodType() != QMetaMethod::Method)
            continue;

        Method m;
        m.paramTypes = QVector<int>(10,QMetaType::UnknownType);
        method.getParameterTypes(m.paramTypes.data());

        m.paramNames = method.parameterNames();
        m.argCount   = method.parameterCount();
        m.name       = method.name();
        m.metaMethod = method;
        m.returns    = QByteArray(method.typeName()).count();

        QByteArray methodId = m.name + QByteArray::number(m.argCount);
        if(methodCache.contains(methodId)){
            qWarning()<<"Can not register the method "<<m.name<<".\n"
                     <<"It's not possible to have multiple overloads with the same param count";
            continue;
        }
        methodCache.insert(m.name + QByteArray::number(m.argCount),m);
    }

    initialized = true;
}

void AbstractJsonHandler::onRequestEnd()
{
    if(!initialized)
        createMethodCache();

    HttpConnection* connection = qobject_cast<HttpConnection*>(sender());
    if(connection){
        Tufao::HttpServerRequest& request = connection->request();
        QJsonDocument doc;
        if(request.method() == "GET"){
            QJsonObject rObj;
            rObj.insert(QString("jsonrpc"),QString("2.0"));
            rObj.insert(QString("id"),QJsonValue::Null);
            rObj.insert(QString("method"),request.url().path().split('/').last());

            QUrlQuery Query(request.url());
            QVariantMap params;
            QList<QPair<QString, QString> > enc = Query.queryItems(QUrl::FullyEncoded);
            for (int i = 0; i < enc.count(); i++) {
                params.insert(
                            QUrl::fromPercentEncoding (enc.at(i).first.toLatin1()),
                            QUrl::fromPercentEncoding (enc.at(i).second.toLatin1())
                            );
            }

            rObj.insert(QString("params"),QJsonObject::fromVariantMap(params));
            doc = QJsonDocument(rObj);
        }else{
            QByteArray data = request.readBody();
            if(data.isEmpty()){
                sendError(connection, QJsonValue::Null,InvalidRequest,"Emtpy Request received");
                return;
            }

            QJsonParseError error;
            doc = QJsonDocument::fromJson(data,&error);
            if(error.error != QJsonParseError::NoError){
                sendError(connection,QJsonValue::Null,ParseError,error.errorString());
                return;
            }
        }
        //@TODO implement batch calls
        //check if request is valid
        if(!doc.isObject()){
            sendError(connection, QJsonValue::Null,InvalidRequest,"Top element MUST be a Object");
            return;
        }
        QJsonObject requestJson = doc.object();
        if(!requestJson.contains("jsonrpc")){
            sendError(connection, requestJson.value("id"),InvalidRequest,"Jsonrpc version string required");
            return;
        }
        if(requestJson.value("jsonrpc").toString() != "2.0" ){
            sendError(connection, requestJson.value("id"),InvalidRequest,"Server only supports JSONRPC v. 2.0");
            return;
        }
        if(!requestJson.contains("method")){
            sendError(connection, requestJson.value("id"),InvalidRequest,"Method name required");
            return;
        }

        int paramCount = 0;
        QJsonArray paramArray;
        QJsonObject namedParamObj;

        QJsonValue params = requestJson.value("params");
        if(params.isArray()){
            paramArray = params.toArray();
            paramCount = paramArray.size();
        }else{
            if(!params.isObject()){
                sendError(connection, requestJson.value("id"),InvalidRequest,"params needs to be a object OR array");
                return;
            }
            namedParamObj = params.toObject();
            paramCount = namedParamObj.size();
        }

        QByteArray methodId = requestJson.value("method").toString().toUtf8() + QByteArray::number(paramCount);
        if(!methodCache.contains(methodId)){
            sendError(connection, requestJson.value("id"),MethodNotFound,"Unknown Method: "+requestJson.value("method").toString());
            return;
        }

        //we have to create the variant representation for every argument first
        //or we would have a dangling pointer if we call data() on a QVariant that
        //goes out of scope
        QVariantList args;
        Method &method = methodCache[methodId];

        if(namedParamObj.size()){
            for(int i = 0; i < method.argCount; i++){
                const QByteArray& curr = method.paramNames.at(i);

                if(!namedParamObj.contains(curr)){
                    sendError(connection, requestJson.value("id"),InvalidParams,"Invalid param: "+QString(curr));
                    return;
                }

                QVariant parm = namedParamObj.value(curr).toVariant();
                if(parm.type() != method.paramTypes[i]){
                    if(!parm.convert(method.paramTypes[i])){
                        sendError(connection, requestJson.value("id"),InvalidParams,"Invalid param Type: "+QString(curr));
                    }
                }
                args.append(parm);
            }
        }else{
            for(int i = 0; i < method.argCount; i++){
                QVariant parm = paramArray.at(i).toVariant();
                if(parm.type() != method.paramTypes[i]){
                    if(!parm.convert(method.paramTypes[i])){
                        sendError(connection, requestJson.value("id"),InvalidParams,"Invalid param Type at offset "+QString::number(i));
                    }
                }
                args.append(parm);
            }
        }

        QList<QGenericArgument> vargs;
        foreach (const QVariant &ar, args) {
            vargs.append(QGenericArgument(ar.typeName(), ar.data()));
        }

        //set the current connection so the slot can access it
        currentConnection = connection;

        QVariant returnValue;

        bool ok = false;
        if (method.returns) {
            int returnType = QMetaType::type(method.metaMethod.typeName());
            void* retA = QMetaType::create(returnType);
            QGenericReturnArgument ret(method.metaMethod.typeName(), retA);
            if (args.count() > 9)
            {
                ok = method.metaMethod.invoke(this,
                                              ret,
                                              (vargs.at(0)),
                                              (vargs.at(1)),
                                              (vargs.at(2)),
                                              (vargs.at(3)),
                                              (vargs.at(4)),
                                              (vargs.at(5)),
                                              (vargs.at(6)),
                                              (vargs.at(7)),
                                              (vargs.at(8)),
                                              (vargs.at(9))
                                              );
            }
            else if (args.count() > 8)
            {
                ok = method.metaMethod.invoke(this,
                                              ret,
                                              (vargs.at(0)),
                                              (vargs.at(1)),
                                              (vargs.at(2)),
                                              (vargs.at(3)),
                                              (vargs.at(4)),
                                              (vargs.at(5)),
                                              (vargs.at(6)),
                                              (vargs.at(7)),
                                              (vargs.at(8))
                                              );
            }
            else if (args.count() > 7)
            {
                ok = method.metaMethod.invoke(this,
                                              ret,
                                              (vargs.at(0)),
                                              (vargs.at(1)),
                                              (vargs.at(2)),
                                              (vargs.at(3)),
                                              (vargs.at(4)),
                                              (vargs.at(5)),
                                              (vargs.at(6)),
                                              (vargs.at(7))
                                              );
            }
            else if (args.count() > 6)
            {
                ok = method.metaMethod.invoke(this,
                                              ret,
                                              (vargs.at(0)),
                                              (vargs.at(1)),
                                              (vargs.at(2)),
                                              (vargs.at(3)),
                                              (vargs.at(4)),
                                              (vargs.at(5)),
                                              (vargs.at(6))
                                              );
            }
            else if (args.count() > 5)
            {
                ok = method.metaMethod.invoke(this,
                                              ret,
                                              (vargs.at(0)),
                                              (vargs.at(1)),
                                              (vargs.at(2)),
                                              (vargs.at(3)),
                                              (vargs.at(4)),
                                              (vargs.at(5))
                                              );
            }
            else if (args.count() > 4)
            {
                ok = method.metaMethod.invoke(this,
                                              ret,
                                              (vargs.at(0)),
                                              (vargs.at(1)),
                                              (vargs.at(2)),
                                              (vargs.at(3)),
                                              (vargs.at(4))
                                              );
            }
            else if (args.count() > 3)
            {
                ok = method.metaMethod.invoke(this,
                                              ret,
                                              (vargs.at(0)),
                                              (vargs.at(1)),
                                              (vargs.at(2)),
                                              (vargs.at(3))
                                              );
            }
            else if (args.count() > 2)
            {
                ok = method.metaMethod.invoke(this,
                                              ret,
                                              (vargs.at(0)),
                                              (vargs.at(1)),
                                              (vargs.at(2))
                                              );
            }
            else if (args.count() > 1)
            {
                ok = method.metaMethod.invoke(this,
                                              ret,
                                              (vargs.at(0)),
                                              (vargs.at(1))
                                              );
            }
            else if (args.count() > 0)
            {
                ok = method.metaMethod.invoke(this,
                                              ret,
                                              (vargs.at(0))
                                              );
            }
            else
            {
                ok = method.metaMethod.invoke(this,
                                              ret
                                              );
            }
            returnValue = QVariant(returnType,retA);
            QMetaType::destruct(returnType,retA);
        } else {
            if (args.count() > 9)
            {
                ok = method.metaMethod.invoke(this,
                                              (vargs.at(0)),
                                              (vargs.at(1)),
                                              (vargs.at(2)),
                                              (vargs.at(3)),
                                              (vargs.at(4)),
                                              (vargs.at(5)),
                                              (vargs.at(6)),
                                              (vargs.at(7)),
                                              (vargs.at(8)),
                                              (vargs.at(9))
                                              );
            }
            else if (args.count() > 8)
            {
                ok = method.metaMethod.invoke(this,
                                              (vargs.at(0)),
                                              (vargs.at(1)),
                                              (vargs.at(2)),
                                              (vargs.at(3)),
                                              (vargs.at(4)),
                                              (vargs.at(5)),
                                              (vargs.at(6)),
                                              (vargs.at(7)),
                                              (vargs.at(8))
                                              );
            }
            else if (args.count() > 7)
            {
                ok = method.metaMethod.invoke(this,
                                              (vargs.at(0)),
                                              (vargs.at(1)),
                                              (vargs.at(2)),
                                              (vargs.at(3)),
                                              (vargs.at(4)),
                                              (vargs.at(5)),
                                              (vargs.at(6)),
                                              (vargs.at(7))
                                              );
            }
            else if (args.count() > 6)
            {
                ok = method.metaMethod.invoke(this,
                                              (vargs.at(0)),
                                              (vargs.at(1)),
                                              (vargs.at(2)),
                                              (vargs.at(3)),
                                              (vargs.at(4)),
                                              (vargs.at(5)),
                                              (vargs.at(6))
                                              );
            }
            else if (args.count() > 5)
            {
                ok = method.metaMethod.invoke(this,
                                              (vargs.at(0)),
                                              (vargs.at(1)),
                                              (vargs.at(2)),
                                              (vargs.at(3)),
                                              (vargs.at(4)),
                                              (vargs.at(5))
                                              );
            }
            else if (args.count() > 4)
            {
                ok = method.metaMethod.invoke(this,
                                              (vargs.at(0)),
                                              (vargs.at(1)),
                                              (vargs.at(2)),
                                              (vargs.at(3)),
                                              (vargs.at(4))
                                              );
            }
            else if (args.count() > 3)
            {
                ok = method.metaMethod.invoke(this,
                                              (vargs.at(0)),
                                              (vargs.at(1)),
                                              (vargs.at(2)),
                                              (vargs.at(3))
                                              );
            }
            else if (args.count() > 2)
            {
                ok = method.metaMethod.invoke(this,
                                              (vargs.at(0)),
                                              (vargs.at(1)),
                                              (vargs.at(2))
                                              );
            }
            else if (args.count() > 1)
            {
                ok = method.metaMethod.invoke(this,
                                              (vargs.at(0)),
                                              (vargs.at(1))
                                              );
            }
            else if (args.count() > 0)
            {
                ok = method.metaMethod.invoke(this,
                                              (vargs.at(0))
                                              );
            }
            else
            {
                ok = method.metaMethod.invoke(this
                                              );
            }
        }

        currentConnection = 0;

        if (!ok) {
            sendError(connection, requestJson.value("id"),InternalError,"Execution failed");
            return;
        }


        QJsonObject envelope;
        envelope.insert(QString("jsonrpc"),QString("2.0"));
        envelope.insert(QString("id"),requestJson.value("id"));
        envelope.insert(QString("result"),QJsonValue::fromVariant(returnValue));

        QJsonDocument resultDoc(envelope);
        connection->response().writeHead(Tufao::HttpResponseStatus::OK);
        connection->response().end(resultDoc.toJson());
        return;

    }
}
