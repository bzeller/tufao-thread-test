#include "httpconnection.h"

HttpConnection::HttpConnection(Tufao::HttpServerRequest &request, Tufao::HttpServerResponse &parent) :
    QObject(&request), m_request(request),m_response(parent)
{
    connect(&request,&Tufao::HttpServerRequest::ready,this,&HttpConnection::requestReady);
    connect(&request,&Tufao::HttpServerRequest::data,this,&HttpConnection::requestData);
    connect(&request,&Tufao::HttpServerRequest::end,this,&HttpConnection::requestEnd);
    connect(&request,&Tufao::HttpServerRequest::close,this,&HttpConnection::requestClose);
    connect(&request,&Tufao::HttpServerRequest::upgrade,this,&HttpConnection::requestUpgrade);
    connect(&parent,&Tufao::HttpServerResponse::finished,this,&HttpConnection::responseFinished);
    connect(&parent,&Tufao::HttpServerResponse::finished,this,&HttpConnection::onResponseFinished);
}

//this class only is valid for as long as the response object exists
void HttpConnection::onResponseFinished()
{
    disconnect(&m_request,0,this,0);
    disconnect(&m_response,0,this,0);
    this->deleteLater();
}
