#include <QCoreApplication>
#include <QUrl>
#include <QTimer>
#include <QSettings>
#include "GelfTcpClient.h"
#include "GelfUdpServer.h"

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    // configuration
    
    QString configFile = "/etc/graylog/gelfudp2tcp.cfg";
    if (argc == 2) {
        configFile = argv[1];
    }
    
    QSettings settings(configFile, QSettings::IniFormat);

    QUrl gelfUdpUrl(settings.value("gelfUdpUrl", "udp://localhost:12201").toUrl());

    GelfTcpClient::Config tcpClientConfig;
    tcpClientConfig.gelfTcpUrl = QUrl(settings.value("gelfTcpUrl", "ssl://logs.apifocal.org:12201").toUrl());
    tcpClientConfig.tcpMessageSeparator = settings.value("nullMessageSeparator", true).toBool() ? '\0' : '\n';
    tcpClientConfig.reconnectDelayMillis = settings.value("reconnectDelayMillis", 5000).toInt();

    tcpClientConfig.clientCertificate = settings.value("sslClientCertificate", "/etc/graylog/client.crt").toString();
    tcpClientConfig.clientPrivkey = settings.value("sslClientKey", "/etc/graylog/client.key").toString();

    // prepare
    
    GelfTcpClient tcpClient(tcpClientConfig);
    GelfUdpServer udpServer(gelfUdpUrl);

    QObject::connect(&udpServer, SIGNAL(messageReady(QByteArray)),
                     &tcpClient, SLOT(postMessage(QByteArray)));

    // dump stats once in a while
    QTimer statsTimer;
    statsTimer.setInterval(5000);
    QObject::connect(&statsTimer, SIGNAL(timeout()),
                     &tcpClient, SLOT(dumpStats()));
    statsTimer.start();

    // and... action
    return app.exec();
}
