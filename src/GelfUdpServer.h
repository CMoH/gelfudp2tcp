/*
 * Copyright (C) Ciprian Ciubotariu
 *
 */

#ifndef GELFUDPSERVER_H
#define GELFUDPSERVER_H

#include <QUrl>
#include <QUdpSocket>

/**
 * Gelf UDP server.
 *
 * Gelf protocol code adapted from:
 *      https://github.com/Graylog2/graylog2-server/blob/master/graylog2-server/src/main/java/org/graylog2/inputs/codecs/gelf/GELFMessage.java
 * and companion files there.
 */
class GelfUdpServer : public QObject
{
    Q_OBJECT

    QUrl listenUrl;
    QUdpSocket udpSocket;

public:
    explicit GelfUdpServer ( const QUrl& listenUrl );

signals:
    void messageReady ( QByteArray gelfMessage );

private slots:
    void readDatagrams();
    void processDatagram ( const QByteArray& datagram );

private:
    enum GelfMessageType {
        CompressedZlib,
        CompressedGzip,
        Chunked,
        Uncompressed,
        Unsupported
    };
    static const qint8 compressedZlibMarker[2];
    static const qint8 compressedGzipMarker[2];
    static const qint8 chunkedMarker[2];

    static GelfMessageType decodeGelfMessageType ( const QByteArray& datagram );
    static QByteArray uncompressGzip(const QByteArray& message );
    static QByteArray uncompressZlib(const QByteArray& message );

    void handleMessageChunk ( const QByteArray & datagram );

};

#endif // GELFUDPSERVER_H
