#include <QCoreApplication>
#include <QUrl>
#include <QTimer>
#include "GelfTcpClient.h"
#include "GelfUdpServer.h"

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    // configuration

    QUrl listenUrl("udp://localhost:12201");

    GelfTcpClient::Config tcpClientConfig;
    tcpClientConfig.serverUrl = QUrl("ssl://logs.apifocal.org:12201");
    tcpClientConfig.clientCertificate = "/etc/graylog/client.crt";
    tcpClientConfig.clientPrivkey = "/etc/graylog/client.key";
    tcpClientConfig.tcpMessageSeparator = '\0';
    tcpClientConfig.reconnectDelayMillis = 5000;

    // prepare
    
    GelfTcpClient tcpClient(tcpClientConfig);
    GelfUdpServer udpServer(listenUrl);

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
