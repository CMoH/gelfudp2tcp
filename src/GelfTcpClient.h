/*
 * Copyright (C) Ciprian Ciubotariu
 *
 */

#ifndef GELFTCPCLIENT_H
#define GELFTCPCLIENT_H

#include <QBuffer>
#include <QUrl>
#include <QQueue>
#include <QSslSocket>

/**
 * A resilient gelf-tcp client.
 *
 * Continuously attempts to connect to the logging server.
 * Buffers the data while disconnected.
 */
class GelfTcpClient : public QObject
{
    Q_OBJECT

public:
    /**
     * Server settings.
     */
    struct Config {
        QUrl gelfTcpUrl;
        QString clientCertificate;
        QString clientPrivkey;
        QChar tcpMessageSeparator;
        int reconnectDelayMillis;
    };

private:
    Config config;
    QIODevice* socket;
    QByteArray messageSeparatorBuf;

    QQueue<QByteArray> messageQueue;
    int queueTimerId;

    // some stats
    struct Stats {
        Stats();

        quint64 connectAttempts;
        quint64 connects;
        quint64 disconnects;
        quint64 received;
        quint64 sent;
        quint64 lost;
    } stats;

public:

    explicit GelfTcpClient ( const Config& serverSettings );
    virtual ~GelfTcpClient();

    bool isConnected() const;

public slots:
    void connectToServer();

    void connected();
    void disconnected();

    bool postMessage ( const QByteArray& message );

    void dumpStats();

protected:
    virtual void timerEvent ( QTimerEvent* );

private slots:
    void processQueue();
    void reportSslErrors ( const QList<QSslError>& sslErrors );
};

#endif // GELFTCPCLIENT_H
