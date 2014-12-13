/* This file is part of the KDE libraries
   Copyright (C) 2000, 2006 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kfilterdev.h"
#include <qmimedatabase.h>

#include <QDebug>

static KCompressionDevice::CompressionType findCompressionByFileName(const QString &fileName)
{
    if (fileName.endsWith(QLatin1String(".gz"), Qt::CaseInsensitive)) {
        return KCompressionDevice::GZip;
    }
    else {
        // not a warning, since this is called often with other mimetypes (see #88574)...
        // maybe we can avoid that though?
        //qDebug() << "findCompressionByFileName : no compression found for " << fileName;
    }

    return KCompressionDevice::None;
}

KFilterDev::KFilterDev(const QString &fileName)
    : KCompressionDevice(fileName, findCompressionByFileName(fileName))
{
}

KCompressionDevice::CompressionType KFilterDev::compressionTypeForMimeType(const QString &mimeType)
{
    if (mimeType == QLatin1String("application/x-gzip")) {
        return KCompressionDevice::GZip;
    }
    QMimeDatabase db;
    const QMimeType mime = db.mimeTypeForName(mimeType);
    if (mime.isValid()) {
        if (mime.inherits(QString::fromLatin1("application/x-gzip"))) {
            return KCompressionDevice::GZip;
        }
    }

    // not a warning, since this is called often with other mimetypes (see #88574)...
    // maybe we can avoid that though?
    //qDebug() << "no compression found for" << mimeType;
    return KCompressionDevice::None;
}

