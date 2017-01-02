/*
 * Copyright (C) Ciprian Ciubotariu
 *
 */

#include "GelfUdpServer.h"
#include <QHostInfo>

#include "GelfUdpServer.moc"

const quint8 GelfUdpServer::compressedZlibMarker[2] = { 0x78, 0x9c };
const quint8 GelfUdpServer::compressedGzipMarker[2] = { 0x1f, 0x8b };
const quint8 GelfUdpServer::chunkedMarker[2]        = { 0x1e, 0x0f };

GelfUdpServer::GelfUdpServer(const QUrl& url)
    : listenUrl(url)
{
    connect(&udpSocket, SIGNAL(readyRead()),
            this, SLOT(readDatagrams()));

    QHostInfo hostInfo = QHostInfo::fromName(url.host());
    if (hostInfo.addresses().isEmpty()) {
        qWarning() << "cannot find network interface to bind to for " << url;
        exit(1);
    }

    QHostAddress addr = hostInfo.addresses().at(0);
    udpSocket.bind(addr, url.port());

    qDebug() << "listening to " << url.toString() << " on interface " << addr.toString();

}

void GelfUdpServer::readDatagrams()
{
    while (udpSocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket.pendingDatagramSize());
        udpSocket.readDatagram(datagram.data(), datagram.size());
        processDatagram(datagram);
    }
}

void GelfUdpServer::processDatagram(const QByteArray& datagram)
{
    switch (decodeGelfMessageType(datagram)) {
    case CompressedGzip:
//         ++stats.receivedCompressedGzip;
        emit messageReady(uncompressGzip(datagram));
        break;
    case CompressedZlib:
//         ++stats.receivedCompressedZlib;
        emit messageReady(uncompressZlib(datagram));
        break;
    case Uncompressed:
//         ++stats.receivedUncompressed;
        // single non-encoded json message
        emit messageReady(datagram);
        break;
    case Chunked:
//         ++stats.receivedChunked;
        emit messageChunkReceived(datagram);
        break;
    case Unsupported:
    default:
//         ++stats.receivedUnsupported;
        ;
    }
}

GelfUdpServer::GelfMessageType GelfUdpServer::decodeGelfMessageType(const QByteArray& datagram)
{
    if (datagram.size() < 2) {
        return Unsupported;
    }

    quint8 first = datagram.at(0);
    quint8 second = datagram.at(1);

    if (first == compressedZlibMarker[0]) {
        // zlib's second byte is for flags and a checksum; it
        // differs based on compression level used and whether
        // a dictionary is used.  What the RFC guarantees is
        // that "CMF and FLG, when viewed as a 16-bit unsigned
        // integer stored in MSB order (CMF*256 + FLG), is a
        // multiple of 31"
        if ((256 * qint16(first) + qint16(second)) % 31 == 0) {
            return CompressedZlib;
        }
    } else if (first == compressedGzipMarker[0] && second == compressedGzipMarker[1]) {
        return CompressedGzip;
    } else if (first == chunkedMarker[0] && second == chunkedMarker[1]) {
        return Chunked;
    } else {
        // by default assume the payload to be "raw, uncompressed" GELF, parsing will fail if it's malformed.
        return Uncompressed;
    }

    return Unsupported;
}

QByteArray GelfUdpServer::uncompressGzip(const QByteArray& message)
{
    // TODO: this seems to not work out of the box - search internet for workaround
    return qUncompress(message);
}

QByteArray GelfUdpServer::uncompressZlib(const QByteArray& message)
{
    return qUncompress(message);
}

