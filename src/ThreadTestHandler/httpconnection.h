#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <QObject>
#include <Tufao/HttpServerRequest>
#include <Tufao/HttpServerResponse>
#include <QMetaType>

class HttpConnection : public QObject
{
        Q_OBJECT
    public:
        explicit HttpConnection(Tufao::HttpServerRequest &request, Tufao::HttpServerResponse &parent);

        inline Tufao::HttpServerRequest& request (){
            return m_request;
        }

        inline Tufao::HttpServerResponse& response(){
            return m_response;
        }
        
    signals:
        void requestReady();
        void requestData();
        void requestEnd();
        void requestClose();
        void requestUpgrade();
        void responseFinished();
        void doIt();

        
    private slots:
        void onResponseFinished ();

    private:
        Tufao::HttpServerRequest &m_request;
        Tufao::HttpServerResponse &m_response;
        
};

#endif // HTTPCONNECTION_H
