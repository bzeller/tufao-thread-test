#include "fileserverhandler.h"
#include <QtCore/QtPlugin>
#include <Tufao/HttpServerResponse>
#include <Tufao/HttpFileServer>
#include <Tufao/ThreadedHttpServer>


std::function<bool (Tufao::HttpServerRequest &, Tufao::HttpServerResponse &)>
FileServerHandler::createHandler(const QHash<QString, HttpServerPlugin *> &dependencies,
                                 const QVariant &customData)
{
    Tufao::tDebug()<<"Loading Plugin FileServer";
    return Tufao::HttpFileServer::handler(customData.toString());
}
