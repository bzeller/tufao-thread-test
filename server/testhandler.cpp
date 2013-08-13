#include "testhandler.h"
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <Tufao/HttpServerResponse>

using namespace Tufao;

#define debug() qDebug()<<"["<<QThread::currentThreadId()<<"] "

TestHandler::TestHandler(QObject *parent) :
    QObject(parent) , currentRequest(0), currentResponse(0)
{
}

bool TestHandler::handleRequest(HttpServerRequest &request, HttpServerResponse &response)
{
    debug()<<"Started Long Running Request";
    currentRequest = &request;
    currentResponse = &response;

    QTimer::singleShot(10 * 1000,this,SLOT(theEnd())); //some long running async task

    return true;
}

void TestHandler::theEnd()
{
    debug()<<"Finished Long Running Request";
    currentResponse->writeHead(200, "OK");
    currentResponse->end("Hello World\n");
    currentResponse->flush();
    debug()<<"End was called";

    currentResponse = 0;
    currentRequest  = 0;
}
