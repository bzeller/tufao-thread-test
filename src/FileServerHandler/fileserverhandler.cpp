#include "fileserverhandler.h"
#include <QtCore/QtPlugin>
#include <Tufao/HttpServerResponse>
#include <Tufao/HttpFileServer>


std::function<bool (Tufao::HttpServerRequest &, Tufao::HttpServerResponse &)>
FileServerHandler::createHandler(const QHash<QString, HttpServerPlugin *> &dependencies,
                                 const QVariant &customData)
{
    return Tufao::HttpFileServer::handler(customData.toString());
}
