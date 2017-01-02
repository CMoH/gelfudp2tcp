#include "GelfChunkedMessageAssembler.h"
#include <algorithm>

const int GelfChunkedMessageAssembler::GELF_CHUCKED_MAX_ELAPSED_MSEC = 5000;

GelfChunkedMessageAssembler::GelfChunkedMessageAssembler(QObject* parent)
    : QObject(parent),
      timer(this)
{
    connect(&timer, SIGNAL(timeout()),
            this, SLOT(checkSequences()));

    timer.setInterval(1000);
    timer.start();
}

void GelfChunkedMessageAssembler::processDatagram(const QByteArray& datagram)
{
    if (datagram.size() < GelfChunkedMessage::HEADER_LENGTH) {
        // ++ stats.dropped;
        return;
    }

    GelfChunkedMessage chunk(datagram);
    if (chunk.getSequenceNumber() < chunk.getSequenceCount()) {

        sequence_map_type::iterator it = sequences.find(chunk.getMessageId());

        Sequence* seq;
        if (it == sequences.end()) {
            it = sequences.insert(chunk.getMessageId(), Sequence());
            seq = &it.value();
            seq->chunks.resize(chunk.getSequenceCount());
        } else {
            seq = &it.value();
        }
        seq->chunks[chunk.getSequenceNumber()] = chunk;
        seq->lastReceiveTime = chunk.getRecvTime();
    }
}

void GelfChunkedMessageAssembler::checkSequences()
{
    QList<quint64> toRemove;
    for (sequence_map_type::iterator it = sequences.begin(), itEnd = sequences.end(); it != itEnd; ++it) {
        bool sequnceComplete = std::find_if_not(it->chunks.begin(), it->chunks.end(),
                                                std::mem_fun_ref(&GelfChunkedMessage::isValid))
                               == it->chunks.end();
        if (sequnceComplete) {
            // all chunks received - assemble the message
            QByteArray fullMessage;
            foreach (const GelfChunkedMessage & chunk, it->chunks) {
                chunk.appendData(fullMessage);
            }
            emit messageReady(fullMessage);
            toRemove.append(it.key());
        } else if (it->lastReceiveTime.elapsed() > GELF_CHUCKED_MAX_ELAPSED_MSEC) {
            toRemove.append(it.key());
        }
    }

    // remove sequences that are complete or timeouted
    foreach (quint64 messageId, toRemove) {
        sequences.remove(messageId);
    }
}


#include "GelfChunkedMessageAssembler.moc"



