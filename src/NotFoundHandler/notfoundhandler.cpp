#include "notfoundhandler.h"
#include <QtCore/QtPlugin>
#include <Tufao/HttpServerResponse>


std::function<bool (Tufao::HttpServerRequest &, Tufao::HttpServerResponse &)>
NotFoundHandler::createHandler(const QHash<QString, Tufao::HttpServerPlugin *> &dependencies
                               , const QVariant &customData)
{
    //Create a copy
    QVariant custom = customData;
    return [custom](Tufao::HttpServerRequest &, Tufao::HttpServerResponse &res){
        res.writeHead(Tufao::HttpResponseStatus::NOT_FOUND);
        res.end(custom.toByteArray());
        return true;
    };
}
