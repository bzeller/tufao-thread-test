#include <QCoreApplication>

#include <Tufao/HttpPluginServer>
#include <Tufao/HttpFileServer>
#include <Tufao/NotFoundHandler>
#include <Tufao/HttpServerRequestRouter>
#include <Tufao/HttpServer>
#include <Tufao/threadedhttprequestdispatcher.h>
#include <Tufao/threadedhttppluginserver.h>
#include <functional>
#include <QDebug>
#include <QThread>

#include "testhandler.h"

using namespace Tufao;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#if 0

    auto threadInit = []{

        tDebug()<<"Running Thread initializer ";

        HttpServerRequestRouter* router = new HttpServerRequestRouter();
        TestHandler *handler = new TestHandler(router);

        router->map({
                {QRegularExpression{"^/thread[0-9]$"}, *handler},
                {QRegularExpression{""}, HttpFileServer::handler("public")},
                {QRegularExpression{""}, NotFoundHandler::handler()}
        });

        return router;
    };

    HttpServer server;

    ThreadedHttpRequestDispatcher dispatch(threadInit);
    dispatch.setThreadPoolSize(20);

    QObject::connect(&server, &HttpServer::requestReady,
                     &dispatch, &ThreadedHttpRequestDispatcher::handleRequest);

    server.listen(QHostAddress::Any, 8080);
#else
     HttpServer server;

     ThreadedHttpPluginServer pServer;
     pServer.setThreadPoolSize(20);
     pServer.setConfig("config.json");

     QObject::connect(&server, &HttpServer::requestReady,
                      &pServer, &ThreadedHttpPluginServer::handleRequest);

     server.listen(QHostAddress::Any, 8080);

#endif
    return a.exec();
}
