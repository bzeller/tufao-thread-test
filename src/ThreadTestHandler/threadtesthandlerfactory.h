#ifndef THREADTESTHANDLERFACTORY_H
#define THREADTESTHANDLERFACTORY_H

#include <Tufao/HttpServerPlugin>
class ThreadTestHandlerFactory: public QObject, Tufao::HttpServerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID TUFAO_HTTPSERVERPLUGIN_IID)
    Q_INTERFACES(Tufao::HttpServerPlugin)
public:
    std::function<bool(Tufao::HttpServerRequest&, Tufao::HttpServerResponse&)>
    createHandler(const QHash<QString, Tufao::HttpServerPlugin*> &dependencies,
                  const QVariant &customData = QVariant()) override;
};


#endif // THREADTESTHANDLERFACTORY_H
