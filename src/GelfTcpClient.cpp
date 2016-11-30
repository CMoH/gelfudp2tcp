/*
 * Copyright (C) Ciprian Ciubotariu
 *
 */

#include "GelfTcpClient.h"
#include <QFile>
#include <QUrl>
#include <QTimer>
#include <QTimerEvent>
#include <QSslKey>
#include <QSslConfiguration>

#include "GelfTcpClient.moc"


GelfTcpClient::GelfTcpClient(const GelfTcpClient::Config& serverSettings)
    : config(serverSettings), socket(NULL), queueTimerId(0)
{
    messageSeparatorBuf.append(config.tcpMessageSeparator);

    QTimer::singleShot(0, this, SLOT(connectToServer()));
}

GelfTcpClient::~GelfTcpClient()
{
    if (socket != NULL) {
        socket->close();
    }
    if (queueTimerId != 0) {
        killTimer(queueTimerId);
    }
}

bool GelfTcpClient::postMessage(const QByteArray& message)
{
    messageQueue.enqueue(message);
    ++stats.received;
}

void GelfTcpClient::processQueue()
{
    if (!messageQueue.isEmpty() && isConnected()) {
        const QByteArray& message = messageQueue.head();
        bool sent = true;
        sent &= (socket->write(message) == message.size());
        sent &= (socket->write(messageSeparatorBuf) == messageSeparatorBuf.size());
        if (sent) {
            messageQueue.dequeue();
            ++stats.sent;
        }
    }
}

void GelfTcpClient::connected()
{
    qDebug() << "connected to " << config.gelfTcpUrl.toString();
    ++stats.connects;

    queueTimerId = startTimer(0);
    if (queueTimerId == 0) {
        qWarning() << "failed to start timer";
        exit(2);
    }
}

void GelfTcpClient::disconnected()
{
    killTimer(queueTimerId);
    queueTimerId = 0;

    qDebug() << "disconnected from " << config.gelfTcpUrl.toString();
    ++stats.disconnects;

    if (socket != NULL) {
        socket->close();
        socket = NULL;
    }

    QTimer::singleShot(config.reconnectDelayMillis, this, SLOT(connectToServer()));
}

bool GelfTcpClient::isConnected() const
{
    return socket != NULL
           && socket->isOpen()
           && socket->isWritable();
}

void GelfTcpClient::connectToServer()
{
    const QUrl& url = config.gelfTcpUrl;

    qDebug() << "connecting to " << url.toString();
    ++stats.connectAttempts;

    if (url.scheme() == "ssl") {

        QSslConfiguration clientSslConfig;

        clientSslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);

        qDebug() << "loading client certificate from " << config.clientCertificate;
        QList<QSslCertificate> clientCerts = QSslCertificate::fromPath(config.clientCertificate);
        if (clientCerts.isEmpty()) {
            qWarning() << "could not find any client certs";
        }
        clientSslConfig.setLocalCertificate(clientCerts.at(0));

        qDebug() << "loading client private key from " << config.clientPrivkey;
        QFile privkeyFile(config.clientPrivkey);
        privkeyFile.open(QFile::ReadOnly);
        QSslKey privkey(&privkeyFile, QSsl::Rsa);
        clientSslConfig.setPrivateKey(privkey);
        privkeyFile.close();

        QSslSocket* sslSocket = new QSslSocket(this);
        socket = sslSocket;

        connect(sslSocket, SIGNAL(encrypted()),
                this, SLOT(connected()));
        connect(sslSocket, SIGNAL(sslErrors(QList<QSslError>)),
                this, SLOT(reportSslErrors(QList<QSslError>)));
        connect(sslSocket, SIGNAL(disconnected()),
                this, SLOT(disconnected()));

        sslSocket->setSslConfiguration(clientSslConfig);
        
        sslSocket->connectToHostEncrypted(url.host(), url.port());

    } else if (url.scheme() == "tcp") {

        QTcpSocket* tcpSocket = new QTcpSocket(this);
        socket = tcpSocket;

        connect(tcpSocket, SIGNAL(connected()),
                this, SLOT(connected()));
        connect(tcpSocket, SIGNAL(disconnected()),
                this, SLOT(disconnected()));

        tcpSocket->connectToHost(url.host(), url.port());

    } else {

        qWarning() << "unknown scheme in url " << url;
        exit(1);

    }

}

void GelfTcpClient::dumpStats()
{
    qDebug() << "stats"
             << ": connectionAttempts=" << stats.connectAttempts
             << ": connects=" << stats.connects
             << "; disconnects=" << stats.disconnects
             << "; received=" << stats.received
             << "; sent=" << stats.sent
             << "; lost=" << stats.lost
             << "; queueSize=" << messageQueue.size()
             ;
}

void GelfTcpClient::timerEvent(QTimerEvent* te)
{
    QObject::timerEvent(te);
    processQueue();
}

void GelfTcpClient::reportSslErrors(const QList< QSslError >& sslErrors)
{
    foreach (const QSslError & error, sslErrors) {
        qWarning() << "SSL connection error:" << error.errorString();
    }
}

GelfTcpClient::Stats::Stats()
    : connectAttempts(0),
      connects(0),
      disconnects(0),
      received(0),
      sent(0),
      lost(0)
{

}
