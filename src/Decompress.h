#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include <QByteArray>

QByteArray uncompressGzip(const QByteArray& compressed);
QByteArray uncompressZlib(const QByteArray& compressed);

#endif // DECOMPRESS_H
