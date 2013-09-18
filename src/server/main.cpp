#include <QCoreApplication>

#include <Tufao/HttpPluginServer>
#include <Tufao/HttpFileServer>
#include <Tufao/NotFoundHandler>
#include <Tufao/HttpServerRequestRouter>
#include <Tufao/ThreadedHttpServer>
#include <Tufao/httpconnectionhandler.h>

#include <functional>
#include <QDebug>
#include <QThread>

#include "testhandler.h"

using namespace Tufao;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#if 0
    //NON threaded Test

    HttpServerRequestRouter router;
    TestHandler *handler = new TestHandler(&router);

    router.map({
            {QRegularExpression{"^/thread[0-9]+$"}, *handler},
            {QRegularExpression{""}, HttpFileServer::handler("public")},
            {QRegularExpression{""}, NotFoundHandler::handler()}
    });

    HttpServer server;

    QObject::connect(&server, &HttpServer::requestReady,
                     &router, &HttpServerRequestRouter::handleRequest);

    server.listen(QHostAddress::Any, 8080);

#else
#if 1
    //BASIC THREAD TEST

    auto threadInit = [](AbstractConnectionHandler* connHandler, void**){

        tDebug()<<"Running Thread initializer ";

        HttpServerRequestRouter* router = new HttpServerRequestRouter(connHandler);
        TestHandler *handler = new TestHandler(router);

        router->map({
                {QRegularExpression{"^/thread[0-9]+$"}, *handler},
                {QRegularExpression{""}, HttpFileServer::handler("public")},
                {QRegularExpression{""}, NotFoundHandler::handler()}
        });

        QObject::connect(connHandler, &AbstractConnectionHandler::requestReady,
                         router, &HttpServerRequestRouter::handleRequest);
    };

    ThreadedHttpServer server;
    server.setRequestHandlerFactory(threadInit);
    server.setThreadPoolSize(20);
    server.listen(QHostAddress::Any, 8080);
#else
    //PLUGIN BASED THREAD TEST
     HttpServer server;

     ThreadedHttpPluginServer pServer;
     pServer.setThreadPoolSize(50);
     pServer.setConfig("config.json");

     QObject::connect(&server, &HttpServer::requestReady,
                      &pServer, &ThreadedHttpPluginServer::handleRequest);

     server.listen(QHostAddress::Any, 8080);

#endif
#endif
    return a.exec();
}
