#include "testhandler.h"
#include <QDebug>
#include <QThread>
#include <Tufao/HttpServerResponse>

using namespace Tufao;

TestHandler::TestHandler(QObject *parent) :
    QObject(parent)
{
}

bool TestHandler::handleRequest(HttpServerRequest &, HttpServerResponse &response)
{
    qDebug()<<"Started Long Running Request";
    QThread::sleep(10); //some long running task
    qDebug()<<"Finished Long Running Request";
    response.writeHead(200, "OK");
    response.end("Hello World\n");
    qDebug()<<"End was called";
}
