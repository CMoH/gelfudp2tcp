#ifndef GELFCHUNKEDMESSAGE_H
#define GELFCHUNKEDMESSAGE_H

#include <QByteArray>
#include <QTime>

class GelfChunkedMessage
{

    QByteArray datagram;
    QTime recvTime;

public:
    // some constants
    static const int HEADER_SEQNUM;
    static const int HEADER_SEQCOUNT;
    static const int HEADER_HASH_START;
    static const int HEADER_HASH_LENGTH;
    static const int HEADER_LENGTH;

    explicit GelfChunkedMessage ();
    explicit GelfChunkedMessage ( const QByteArray& datagram );
    
    bool isValid() const;

    quint64 getMessageId() const;
    quint8 getSequenceNumber() const;
    quint8 getSequenceCount() const;
    const QTime& getRecvTime() const;

    /** Appends chunked data to the given byte buffer. */
    void appendData ( QByteArray& destination ) const;

private:
};

#endif // GELFCHUNKEDMESSAGE_H
