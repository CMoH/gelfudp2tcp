#include "Decompress.h"

#include <zlib.h>


// adapted from http://blog.lugru.com/2010/06/compressing-decompressing-web-gzip-stream/
QByteArray uncompressGzip(const QByteArray& compressed)
{
    const int GZIP_HEADER_LEN = 10;
    const int GZIP_TRAILER_LEN = 12;

    const int buffersize = 16384;
    quint8 buffer[buffersize];

    z_stream cmpr_stream;
    cmpr_stream.next_in = reinterpret_cast<unsigned char *>(const_cast<char *>(compressed.constData() + GZIP_HEADER_LEN));
    cmpr_stream.avail_in = compressed.size() - GZIP_HEADER_LEN - GZIP_TRAILER_LEN;
    cmpr_stream.total_in = 0;

    cmpr_stream.next_out = buffer;
    cmpr_stream.avail_out = buffersize;
    cmpr_stream.total_out = 0;

    cmpr_stream.zalloc = Z_NULL;
    cmpr_stream.zalloc = Z_NULL;

    QByteArray uncompressed;
    if (inflateInit2(&cmpr_stream, -8) != Z_OK) {
        return uncompressed;
    }

    do {
        int status = inflate(&cmpr_stream, Z_SYNC_FLUSH);

        if (status == Z_OK || status == Z_STREAM_END) {
            uncompressed.append(QByteArray::fromRawData(
                                    (char*)buffer,
                                    buffersize - cmpr_stream.avail_out));
            cmpr_stream.next_out = buffer;
            cmpr_stream.avail_out = buffersize;
        } else {
            inflateEnd(&cmpr_stream);
        }

        if (status == Z_STREAM_END) {
            inflateEnd(&cmpr_stream);
            break;
        }

    } while (cmpr_stream.avail_out == 0);

    return uncompressed;
}


QByteArray uncompressZlib(const QByteArray& compressed)
{
    return qUncompress(compressed);
}
