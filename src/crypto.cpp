#include "crypto.h"
#include <QCryptographicHash>
#include <QDebug>

extern "C" {
#include "aes.h"
}

QByteArray Crypto::pad(QByteArray data, int blocksize)
{
    int padCount = blocksize - (data.length() % blocksize);
    for (int i=0; i < padCount; i++) {
        data += char(padCount);
    }
    return data;
}

QByteArray Crypto::decrypt(QByteArray inputData)
{
    if (inputData.length() % 16) {
        qWarning() << "encoded data not a multiple of 16";
        return inputData;
    }

    // ECB, by hand, because everything else sucks
    QByteArray ret(inputData.length(), '\0');
    aes_context ctx;
    aes_set_key(&ctx, (uint8*)BLOBKEY, 128);
    char *decodedBuffer = ret.data();
    for (int i=0; i<inputData.length(); i+=16) {
        aes_decrypt(&ctx, (unsigned char*)inputData.constData() + i, (unsigned char*)decodedBuffer + i);
    }

    return ret;
}

QByteArray Crypto::decryptStory(QByteArray input, QByteArray key, QByteArray iv)
{
    if (input.length() % 16) {
        qWarning() << "encoded data not a multiple of 16";
        return input;
    }

    // CBC, by hand, because everything else sucks
    QByteArray ret;
    aes_context ctx;
    aes_set_key(&ctx, (uint8*)key.constData(), 128);
    char *inputBuffer = input.data();
    char *ivBuffer = iv.data();
    for (int i=0; i<input.length() / 16; i++) {
        for (int j=0; j<16; j++) {
            inputBuffer[i + j] ^= ivBuffer[j];
        }
        aes_decrypt(&ctx, (unsigned char*)input.data() + (i * 16), (unsigned char*)ivBuffer);
        ret += QByteArray::fromRawData(ivBuffer, 16);
    }

    return ret;
}

QByteArray Crypto::encrypt(QByteArray inputData)
{
    inputData = pad(inputData);

    // ECB, by hand, because everything else sucks
    QByteArray ret(inputData.length(), '\0');
    char *encodedBuffer = ret.data();
    aes_context ctx;
    aes_set_key(&ctx, (uint8*)BLOBKEY, 128);
    for (int i=0; i<inputData.length(); i+=16) {
        aes_encrypt(&ctx, (unsigned char*)inputData.constData() + i, (unsigned char*)encodedBuffer + i);
    }

    return ret;
}



QByteArray Crypto::requestToken(QByteArray secA, QByteArray secB)
{
    QByteArray hashA = QCryptographicHash::hash(SECRET + secA, QCryptographicHash::Sha256).toHex();
    QByteArray hashB = QCryptographicHash::hash(secB + SECRET, QCryptographicHash::Sha256).toHex();

    QByteArray res;
    for (int i=0; i<64; i++) {
        if (MERGE_PATTERN[i]) {
            res += hashB[i];
        } else {
            res += hashA[i];
        }
    }

    return res;
}
