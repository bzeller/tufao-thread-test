#ifndef TESTHANDLER_H
#define TESTHANDLER_H

#include <QObject>
#include <Tufao/AbstractHttpServerRequestHandler>

class TestHandler : public QObject,
        public Tufao::AbstractHttpServerRequestHandler

{
        Q_OBJECT

    public:
        explicit TestHandler(QObject * parent = 0);
        virtual bool handleRequest(Tufao::HttpServerRequest &, Tufao::HttpServerResponse &response);
        
};

#endif // TESTHANDLER_H
