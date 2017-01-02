#include "GelfChunkedMessage.h"

const int GelfChunkedMessage::HEADER_SEQNUM = 10;
const int GelfChunkedMessage::HEADER_SEQCOUNT = 11;
const int GelfChunkedMessage::HEADER_HASH_START = 2;
const int GelfChunkedMessage::HEADER_HASH_LENGTH = 8;
const int GelfChunkedMessage::HEADER_LENGTH = 12;

GelfChunkedMessage::GelfChunkedMessage()
{
}

GelfChunkedMessage::GelfChunkedMessage(const QByteArray& datagram)
    : datagram(datagram),
      recvTime(QTime::currentTime())
{
}

bool GelfChunkedMessage::isValid() const
{
    return recvTime.isValid();
}

quint64 GelfChunkedMessage::getMessageId() const
{
    return *reinterpret_cast<const quint64*>(datagram.constData() + HEADER_HASH_START);
}

const QTime& GelfChunkedMessage::getRecvTime() const
{
    return recvTime;
}

quint8 GelfChunkedMessage::getSequenceCount() const
{
    return datagram.at(HEADER_SEQCOUNT);
}

quint8 GelfChunkedMessage::getSequenceNumber() const
{
    return datagram.at(HEADER_SEQNUM);
}

void GelfChunkedMessage::appendData(QByteArray& destination) const
{
    if (isValid()) {
        destination.append(datagram.constData() + HEADER_LENGTH, datagram.size() - HEADER_LENGTH);
    }
}
