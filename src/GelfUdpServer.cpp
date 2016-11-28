/*
 * Copyright (C) Ciprian Ciubotariu
 *
 */

#include "GelfUdpServer.h"
#include <QHostInfo>

#include "GelfUdpServer.moc"

GelfUdpServer::GelfUdpServer(const QUrl &url)
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

void GelfUdpServer::processDatagram(const QByteArray &datagram)
{
    // TODO: check compression
    // TODO: check chunked message

    // single non-encoded json message
    emit messageReady(datagram);
}
