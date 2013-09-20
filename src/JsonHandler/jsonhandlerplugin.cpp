#include "jsonhandlerplugin.h"
#include "jsonhandler.h"

#include <QtCore/QtPlugin>
#include <Tufao/HttpServerResponse>
#include <Tufao/ThreadedHttpServer>
#include <functional>


std::function<bool (Tufao::HttpServerRequest &, Tufao::HttpServerResponse &)>
JsonHandlerPlugin::createHandler(const QHash<QString, Tufao::HttpServerPlugin *> &dependencies,
                                        const QVariant &customData)
{
    QSharedPointer<JsonHandler> handler(new JsonHandler);

    auto functor = [handler](Tufao::HttpServerRequest &req, Tufao::HttpServerResponse &res){
        Tufao::tDebug()<<"JSON Handler was called";
        return handler->handleRequest(req,res);
    };

    return functor;
}
