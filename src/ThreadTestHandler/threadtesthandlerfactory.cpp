#include "threadtesthandlerfactory.h"
#include "threadtesthandler.h"
#include <QtCore/QtPlugin>
#include <QtCore/QSharedPointer>


std::function<bool (Tufao::HttpServerRequest &, Tufao::HttpServerResponse &)>
ThreadTestHandlerFactory::createHandler(const QHash<QString, Tufao::HttpServerPlugin *> &dependencies,
                                        const QVariant &customData)
{
    QSharedPointer<TestHandler> handler(new TestHandler);

    auto functor = [handler](Tufao::HttpServerRequest &req, Tufao::HttpServerResponse &res){
        return handler->handleRequest(req,res);
    };

    return functor;
}
