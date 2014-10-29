#ifndef CRYPTO_H
#define CRYPTO_H

#include <QByteArray>


// Magical values that god only knows where comes from

/// 128 bit AES key for snaps
#define BLOBKEY "M02cnQ51Ji97vwT4"

/// Magical secret for hash tokens
#define SECRET "iEk21fuwZApXlz93750dmW22pw389dPwOk"

/// Pattern for mixing bytes between hashes for tokens
static const int MERGE_PATTERN[] = {0, 0, 0, 1,
                                    1, 1, 0, 1,
                                    1, 1, 1, 0,
                                    1, 1, 1, 0,
                                    0, 0, 1, 1,
                                    1, 1, 0, 1,
                                    0, 1, 0, 1,
                                    1, 1, 1, 0,
                                    1, 1, 0, 1,
                                    0, 0, 0, 1,
                                    0, 0, 1, 1,
                                    1, 0, 0, 1,
                                    1, 0, 0, 0,
                                    1, 1, 0, 0,
                                    0, 1, 0, 0,
                                    0, 1, 1, 0 };

/// Default token, used for signing in
#define DEFAULT_TOKEN "m198sOkJEn37DjqZ32lpRu76xmw288xSQ9"

namespace Crypto
{
    QByteArray pad(QByteArray data, int blocksize = 16);
    QByteArray requestToken(QByteArray secA, QByteArray secB);
    QByteArray decryptStory(QByteArray data, QByteArray key, QByteArray iv);
    QByteArray decrypt(QByteArray data);
    QByteArray encrypt(QByteArray data);
}

#endif // CRYPTO_H
