/*
 * Copyright (C) Ciprian Ciubotariu
 *
 */

#ifndef GELFUDPSERVER_H
#define GELFUDPSERVER_H

#include <QUrl>
#include <QUdpSocket>

class GelfUdpServer : public QObject
{
    Q_OBJECT

    QUrl listenUrl;
    QUdpSocket udpSocket;

    //

// TODO: chunks
//    QHash<QByteArray, QHash<uint8_t, QByteArray> > chunks; // incomplete

public:
    explicit GelfUdpServer ( const QUrl &listenUrl );

signals:
    void messageReady ( QByteArray gelfMessage );

private slots:
    void readDatagrams();
    void processDatagram ( const QByteArray &datagram );
};

#endif // GELFUDPSERVER_H
