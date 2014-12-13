/* This file is part of the KDE libraries
   Copyright (C) 2000-2005 David Faure <faure@kde.org>
   Copyright (C) 2003 Leo Savernik <l.savernik@aon.at>

   Moved from ktar.h by Roberto Teixeira <maragato@kde.org>

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
#ifndef KARCHIVEFILE_H
#define KARCHIVEFILE_H

#include "karchiveentry.h"

class KArchiveFilePrivate;
/**
 * Represents a file entry in a KArchive.
 * @short A file in an archive.
 *
 * @see KArchive
 * @see KArchiveDirectory
 */
class KArchiveFile : public KArchiveEntry
{
public:
    /**
     * Creates a new file entry. Do not call this, KArchive takes care of it.
     * @param archive the entries archive
     * @param name the name of the entry
     * @param access the permissions in unix format
     * @param date the date (in seconds since 1970)
     * @param user the user that owns the entry
     * @param group the group that owns the entry
     * @param symlink the symlink, or QString()
     * @param pos the position of the file in the directory
     * @param size the size of the file
     */
    KArchiveFile(KArchive *archive, const QString &name, int access, const QDateTime &date,
                 const QString &user, const QString &group, const QString &symlink,
                 qint64 pos, qint64 size);

    /**
     * Destructor. Do not call this, KArchive takes care of it.
     */
    virtual ~KArchiveFile();

    /**
     * Position of the data in the [uncompressed] archive.
     * @return the position of the file
     */
    qint64 position() const;
    /**
     * Size of the data.
     * @return the size of the file
     */
    qint64 size() const;
    /**
     * Set size of data, usually after writing the file.
     * @param s the new size of the file
     */
    void setSize(qint64 s);

    /**
     * Returns the data of the file.
     * Call data() with care (only once per file), this data isn't cached.
     * @return the content of this file.
     */
    virtual QByteArray data() const;

    /**
     * This method returns QIODevice (internal class: KLimitedIODevice)
     * on top of the underlying QIODevice. This is obviously for reading only.
     *
     * WARNING: Note that the ownership of the device is being transferred to the caller,
     * who will have to delete it.
     *
     * The returned device auto-opens (in readonly mode), no need to open it.
     * @return the QIODevice of the file
     */
    virtual QIODevice *createDevice() const;

    /**
     * Checks whether this entry is a file.
     * @return true, since this entry is a file
     */
    bool isFile() const Q_DECL_OVERRIDE;

    /**
     * Extracts the file to the directory @p dest
     * @param dest the directory to extract to
     * @return true on success, false if the file (dest + '/' + name()) couldn't be created
     */
    bool copyTo(const QString &dest) const;

protected:
    virtual void virtual_hook(int id, void *data);
private:
    KArchiveFilePrivate *const d;
};

#endif
