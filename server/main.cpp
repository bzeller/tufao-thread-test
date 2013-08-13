#include <QCoreApplication>

#include <Tufao/HttpPluginServer>
#include <Tufao/HttpFileServer>
#include <Tufao/NotFoundHandler>
#include <Tufao/HttpServerRequestRouter>
#include <Tufao/HttpServer>
#include <Tufao/threadedhttprequestdispatcher.h>
#include <functional>

#include "testhandler.h"

using namespace Tufao;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    auto threadInit = []{

        HttpServerRequestRouter* router = new HttpServerRequestRouter();
        TestHandler *handler = new TestHandler(router);

        router->map({
                {QRegularExpression{"^/thread[0-9]$"}, *handler},
                {QRegularExpression{""}, HttpFileServer::handler("public")},
                {QRegularExpression{""}, NotFoundHandler::handler()}
        });

        return router;
    };



    TestHandler handler;
    HttpServerRequestRouter router;

    router.map({
            {QRegularExpression{"^/thread$"}, handler},
            {QRegularExpression{""}, HttpFileServer::handler("public")},
            {QRegularExpression{""}, NotFoundHandler::handler()}
    });


    HttpServer server;

    ThreadedHttpRequestDispatcher dispatch(threadInit);
    dispatch.setThreadPoolSize(20);

    QObject::connect(&server, &HttpServer::requestReady,
                     &dispatch, &ThreadedHttpRequestDispatcher::handleRequest);

    server.listen(QHostAddress::Any, 8080);

    return a.exec();
}
