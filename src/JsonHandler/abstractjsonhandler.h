#ifndef ABSTRACTJSONHANDLER_H
#define ABSTRACTJSONHANDLER_H

#include <Tufao/AbstractHttpServerRequestHandler>
#include <QJsonObject>
#include <QJsonValue>
#include <QMetaMethod>
#include <QVector>

class HttpConnection;

class AbstractJsonHandler : public QObject, Tufao::AbstractHttpServerRequestHandler
{
        Q_OBJECT
    public:

        struct Method{
                QByteArray name;
                QMetaMethod metaMethod;
                bool returns;
                int argCount;

                QVector<int> paramTypes;
                QList<QByteArray> paramNames;

        };

        enum ErrorCodes{
            ParseError       = -32700, /* Invalid JSON was received by the server.
                                          An error occurred on the server while parsing the JSON text.*/
            InvalidRequest   = -32600, //The JSON sent is not a valid Request object.
            MethodNotFound   = -32601, //The method does not exist / is not available.
            InvalidParams    = -32602, //Invalid method parameter(s).
            InternalError    = -32603, //Internal JSON-RPC error.
            FirstServerError = -32000, //Reserved for implementation-defined server-errors.
            LastServerError  = -32099
        };

        explicit AbstractJsonHandler(QObject *parent = 0);

        // AbstractHttpServerRequestHandler interface
        virtual bool handleRequest(Tufao::HttpServerRequest &request, Tufao::HttpServerResponse &response) Q_DECL_OVERRIDE;

        HttpConnection* connection ();

    protected:
        void sendError(HttpConnection* conn, const QJsonValue& id, const int code, const QString& message) const;
        void createMethodCache ();
        
    signals:
        
    protected slots:
        void onRequestEnd ();

    private:
        bool initialized;
        HttpConnection* currentConnection;
        QMap<QByteArray, Method> methodCache;
        


};

#endif // ABSTRACTJSONHANDLER_H
