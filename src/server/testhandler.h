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
        virtual bool handleRequest(Tufao::HttpServerRequest &request, Tufao::HttpServerResponse &response);

    public slots:
        void theEnd ();

    private:
        Tufao::HttpServerRequest  *currentRequest;
        Tufao::HttpServerResponse *currentResponse;
        
};

#endif // TESTHANDLER_H
