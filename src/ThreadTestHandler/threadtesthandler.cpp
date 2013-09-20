#include "threadtesthandler.h"
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <Tufao/HttpServerResponse>
#include <Tufao/threadedhttppluginserver.h>

#include "httpconnection.h"

using namespace Tufao;

#define debug() qDebug()<<"["<<QThread::currentThreadId()<<"] "

TestHandler::TestHandler(QObject *parent) :
    QObject(parent)
{
}

TestHandler::~TestHandler()
{
    debug()<<"TestHandler destroyed";
}

bool TestHandler::handleRequest(HttpServerRequest &request, HttpServerResponse &response)
{
    tDebug()<<"Started Long Running Request";
    HttpConnection* conn = new HttpConnection(request,response);
    connect(conn,SIGNAL(doIt()),this,SLOT(theEnd()));

    QTimer::singleShot(10 * 1000,conn,SIGNAL(doIt())); //some long running async task

    return true;
}

void TestHandler::theEnd()
{

    HttpConnection* c = qobject_cast<HttpConnection*>(sender());
    if(!c)
        tDebug()<<"SOMETHING WENT TERRIBILY WRONG";
    else{
        tDebug()<<"Finished Long Running Request";
        c->response().writeHead(200, "OK");
        c->response().end("Hello World\n");
        c->response().flush();
        tDebug()<<"End was called";
    }
}
