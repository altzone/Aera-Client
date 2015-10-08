/* XiVO Client
 * Copyright (C) 2007-2014 Avencall
 *
 * This file is part of XiVO Client.
 *
 * XiVO Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with a Section 7 Additional
 * Permission as follows:
 *   This notice constitutes a grant of such permission as is necessary
 *   to combine or link this software, or a modified version of it, with
 *   the OpenSSL project's "OpenSSL" library, or a derivative work of it,
 *   and to copy, modify, and distribute the resulting work. This is an
 *   extension of the special permission given by Trolltech to link the
 *   Qt code with the OpenSSL library (see
 *   <http://doc.trolltech.com/4.4/gpl.html>). The OpenSSL library is
 *   licensed under a dual license: the OpenSSL License and the original
 *   SSLeay license.
 *
 * XiVO Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XiVO Client.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QString>
#include <QPixmap>

#include <baseengine.h>
#include <xletlib/directory_entry_manager.h>

#include "directory_entry_model.h"

DirectoryEntryModel::DirectoryEntryModel(const DirectoryEntryManager & directory_entry_manager,
                                         QObject *parent)
    : QAbstractTableModel(parent),
      m_directory_entry_manager(directory_entry_manager)
{
    connect(b_engine, SIGNAL(clearingCache()),
            this, SLOT(clearCache()));
    connect(&m_directory_entry_manager, SIGNAL(directoryEntryAdded(int)),
            this, SLOT(addDirectoryEntry(int)));
    connect(&m_directory_entry_manager, SIGNAL(directoryEntryUpdated(int)),
            this, SLOT(updateDirectoryEntry(int)));
    connect(&m_directory_entry_manager, SIGNAL(directoryEntryDeleted(int)),
            this, SLOT(deleteDirectoryEntry(int)));

    this->registerListener("directory_headers");
}

void DirectoryEntryModel::addField(const QString &name, const QString &type)
{
    enum ColumnType t;
    if (type == "name") {
        t = NAME;
    } else if (type == "number") {
        t = NUMBER;
    } else if (type == "status") {
        t = STATUS_ICON;
    } else {
        t = OTHER;
    }
    unsigned int column_index = m_fields.size();
    beginInsertColumns(QModelIndex(), column_index, column_index);
    m_fields.append(QPair<QString, enum ColumnType>(name, t));
    endInsertColumns();
}

void DirectoryEntryModel::addDirectoryEntry(int entry_index) {
    int inserted_row = entry_index;
    beginInsertRows(QModelIndex(), inserted_row, inserted_row);
    endInsertRows();
    this->refreshEntry(inserted_row);
}

void DirectoryEntryModel::updateDirectoryEntry(int entry_index) {
    this->refreshEntry(entry_index);
}

void DirectoryEntryModel::deleteDirectoryEntry(int entry_index) {
    this->removeRow(entry_index);
}

void DirectoryEntryModel::clearCache()
{
    this->removeRows(0, this->rowCount(), QModelIndex());
}

void DirectoryEntryModel::refreshEntry(int row_id)
{
    unsigned first_column_index = 0;
    unsigned last_column_index = this->columnCount() - 1;
    QModelIndex cell_changed_start = createIndex(row_id, first_column_index);
    QModelIndex cell_changed_end = createIndex(row_id, last_column_index);
    emit dataChanged(cell_changed_start, cell_changed_end);
}

int DirectoryEntryModel::rowCount(const QModelIndex&) const
{
    return m_directory_entry_manager.entryCount();
}

int DirectoryEntryModel::columnCount() const
{
    return m_fields.size();
}

int DirectoryEntryModel::columnCount(const QModelIndex&) const
{
    return this->columnCount();
}

QVariant DirectoryEntryModel::data(const QModelIndex &index, int role) const
{
    int row = index.row(), column = index.column();

    if (!this->isColumnValid(column)) {
        return QVariant();
    }

    const DirectoryEntry & entry = m_directory_entry_manager.getEntry(row);

    switch(role) {
    case Qt::DecorationRole:
        return this->dataDecoration(entry, column);
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case  Qt::DisplayRole:
        return this->dataDisplay(entry, column);
    case Qt::ToolTipRole:
        return this->dataTooltip(entry, column);
    case Qt::UserRole:
        return this->dataSearch(entry);
    default:
        return QVariant();
    }
}

QVariant DirectoryEntryModel::headerData(int column,
                                         Qt::Orientation orientation,
                                         int role) const
{
    if (!this->isColumnValid(column)) {
        return QVariant();
    }

    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch(role) {
    case  Qt::DisplayRole:
        return this->headerText(column);
    case Qt::UserRole:
        return this->headerType(column);
    default:
        return QVariant();
    }
}

QString DirectoryEntryModel::headerText(int column) const
{
    return this->m_fields[column].first;
}

enum ColumnType DirectoryEntryModel::headerType(int column) const
{
    return this->m_fields[column].second;
}


QVariant DirectoryEntryModel::dataDisplay(const DirectoryEntry & entry, int column) const
{
    const QString &name = this->headerText(column);
    enum ColumnType type = this->headerType(column);
    return entry.getField(name, type);
}

QVariant DirectoryEntryModel::dataDecoration(const DirectoryEntry & entry, int column) const
{
    enum ColumnType type = this->headerType(column);
    if (type != STATUS_ICON) {
        return QVariant();
    }
    return entry.statusIcon();
}

QVariant DirectoryEntryModel::dataTooltip(const DirectoryEntry & entry, int column) const
{
    enum ColumnType type = this->headerType(column);
    if (type != STATUS_ICON) {
        return QVariant();
    }
    return entry.statusText();
}

QVariant DirectoryEntryModel::dataSearch(const DirectoryEntry & entry) const
{
    return entry.searchList();
}

bool DirectoryEntryModel::removeRows(int row, int count, const QModelIndex & index)
{
    if (count > 0) {
        beginRemoveRows(index, row, row + count - 1);
        endRemoveRows();
    }
    return true;
}

void DirectoryEntryModel::parseCommand(const QVariantMap &command)
{
    const QVariantList &headers = command["headers"].toList();
    foreach (const QVariant &field, headers) {
        const QVariantList &field_info = field.toList();
        const QString &name = field_info[0].toString();
        const QString &type = field_info[1].toString();

        this->addField(name, type);
    }
}

int DirectoryEntryModel::getNumberColumnIndex() const
{
    for (int i = 0; i < m_fields.size(); ++i) {
        const QPair<QString, enum ColumnType> &field = m_fields[i];
        if (field.second == NUMBER) {
            return i;
        }
    }
    return -1;
}

int DirectoryEntryModel::getNameColumnIndex() const
{
    for (int i = 0; i < m_fields.size(); ++i) {
        const QPair<QString, enum ColumnType> &field = m_fields[i];
        if (field.second == NAME) {
            return i;
        }
    }
    return -1;
}

bool DirectoryEntryModel::isColumnValid(int col) const
{
    return col >= 0 && col < m_fields.size();
}
