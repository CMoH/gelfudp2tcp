#ifndef GELFCHUNKEDMESSAGEASSEMBLER_H
#define GELFCHUNKEDMESSAGEASSEMBLER_H

#include <qt4/QtCore/qobject.h>
#include <QHash>
#include <QTimer>
#include <QVector>
#include "GelfChunkedMessage.h"

class GelfChunkedMessageAssembler : public QObject
{
    Q_OBJECT
    
    struct Sequence {
        QVector<GelfChunkedMessage> chunks;
        QTime lastReceiveTime;
    };

    typedef QHash<quint64, Sequence> sequence_map_type;
    sequence_map_type sequences;
    
    QTimer timer;
    
    static const int GELF_CHUCKED_MAX_ELAPSED_MSEC;

public:
    
    explicit GelfChunkedMessageAssembler ( QObject* parent = 0 );
    
public slots:
    void processDatagram(const QByteArray & datagram);
    void checkSequences();
    
signals:
    void messageReady(QByteArray gelfMessage);

private:
};

#endif // GELFCHUNKEDMESSAGEASSEMBLER_H
