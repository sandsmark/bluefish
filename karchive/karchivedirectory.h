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
#ifndef KARCHIVEDIRECTORY_H
#define KARCHIVEDIRECTORY_H

#include <sys/stat.h>
#include <sys/types.h>

#include <QtCore/QDate>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include "karchiveentry.h"

class KArchiveDirectoryPrivate;
class KArchiveFile;
/**
 * Represents a directory entry in a KArchive.
 * @short A directory in an archive.
 *
 * @see KArchive
 * @see KArchiveFile
 */
class KArchiveDirectory : public KArchiveEntry
{
public:
    /**
     * Creates a new directory entry.
     * @param archive the entries archive
     * @param name the name of the entry
     * @param access the permissions in unix format
     * @param date the date (in seconds since 1970)
     * @param user the user that owns the entry
     * @param group the group that owns the entry
     * @param symlink the symlink, or QString()
     */
    KArchiveDirectory(KArchive *archive, const QString &name, int access, const QDateTime &date,
                      const QString &user, const QString &group,
                      const QString &symlink);

    virtual ~KArchiveDirectory();

    /**
     * Returns a list of sub-entries.
     * Note that the list is not sorted, it's even in random order (due to using a hashtable).
     * Use sort() on the result to sort the list by filename.
     *
     * @return the names of all entries in this directory (filenames, no path).
     */
    QStringList entries() const;

    /**
     * Returns the entry in the archive with the given name.
     * The entry could be a file or a directory, use isFile() to find out which one it is.
     * @param name may be "test1", "mydir/test3", "mydir/mysubdir/test3", etc.
     * @return a pointer to the entry in the directory, or a null pointer if there is no such entry.
     */
    const KArchiveEntry *entry(const QString &name) const;

    /**
     * Returns the file entry in the archive with the given name.
     * If the entry exists and is a file, a KArchiveFile is returned.
     * Otherwise, a null pointer is returned.
     * This is a convenience method for entry(), when we know the entry is expected to be a file.
     *
     * @param name may be "test1", "mydir/test3", "mydir/mysubdir/test3", etc.
     * @return a pointer to the file entry in the directory, or a null pointer if there is no such file entry.
     * @since 5.3
     */
    const KArchiveFile *file(const QString &name) const;

    /**
     * @internal
     * Adds a new entry to the directory.
     */
    void addEntry(KArchiveEntry *);

    /**
     * @internal
     * Adds a new entry to the directory.
     */
    void removeEntry(KArchiveEntry *);

    /**
     * Checks whether this entry is a directory.
     * @return true, since this entry is a directory
     */
    bool isDirectory() const Q_DECL_OVERRIDE;

    /**
     * Extracts all entries in this archive directory to the directory
     * @p dest.
     * @param dest the directory to extract to
     * @param recursive if set to true, subdirectories are extracted as well
     * @return true on success, false if the directory (dest + '/' + name()) couldn't be created
     */
    bool copyTo(const QString &dest, bool recursive = true) const;

protected:
    virtual void virtual_hook(int id, void *data);
private:
    KArchiveDirectoryPrivate *const d;
};

#endif
