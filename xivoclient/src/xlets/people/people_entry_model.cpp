/* XiVO Client
 * Copyright (C) 2007-2015 Avencall
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

#include <QAction>
#include <QIcon>
#include <cassert>

#include <baseengine.h>
#include <message_factory.h>

#include "people_actions.h"
#include "people_entry_model.h"

#define IN_USE 1

PeopleEntryModel::PeopleEntryModel(QWidget *parent)
    : AbstractTableModel(parent)
{
    this->m_type_map["agent"] = AGENT;
    this->m_type_map["favorite"] = FAVORITE;
    this->m_type_map["callable"] = CALLABLE;
    this->m_type_map["name"] = NAME;
    this->m_type_map["number"] = NUMBER;
    this->m_type_map["personal"] = PERSONAL_CONTACT;
    this->m_type_map["status"] = STATUS_ICON;
}

void PeopleEntryModel::addField(const QString &name, const QString &type)
{
    ColumnType t = this->m_type_map.value(type, OTHER);
    m_fields.append(QPair<QString, enum ColumnType>(name.toUpper(), t));
}

void PeopleEntryModel::addIndexByType(const QString &type, int column)
{
    ColumnType enum_type = this->m_type_map.value(type, OTHER);
    m_type_to_indices[enum_type].append(column);
}

void PeopleEntryModel::refreshEntry(int row_id)
{
    unsigned first_column_index = 0;
    unsigned last_column_index = this->columnCount() - 1;
    QModelIndex cell_changed_start = createIndex(row_id, first_column_index);
    QModelIndex cell_changed_end = createIndex(row_id, last_column_index);
    emit dataChanged(cell_changed_start, cell_changed_end);
}

void PeopleEntryModel::clearFields()
{
    this->beginResetModel();
    m_type_to_indices.clear();
    m_fields.clear();
    this->endResetModel();
}

void PeopleEntryModel::clearEntries()
{
    this->beginResetModel();
    m_people_entries.clear();
    this->endResetModel();
}

QList<int> PeopleEntryModel::columnDisplayBold() const
{
    return QList<int>() << m_type_to_indices[NAME];
}

int PeopleEntryModel::rowCount(const QModelIndex&) const
{
    return m_people_entries.size();
}

int PeopleEntryModel::columnCount(const QModelIndex&) const
{
    return m_fields.size();
}

QVariant PeopleEntryModel::data(const QModelIndex &index, int role) const
{
    int row = index.row(), column = index.column();
    if (row < 0 || row >= this->rowCount() || column < 0 || column >= this->columnCount()) {
        return QVariant();
    }
    const PeopleEntry &entry = m_people_entries[row];
    ColumnType column_type = this->headerType(column);

    switch(role) {
    case Qt::DecorationRole:
        return this->dataDecoration(entry, column);
    case Qt::DisplayRole:
        if (column_type != AGENT && column_type != FAVORITE && column_type != PERSONAL_CONTACT) {
            return entry.data(column);
        }
        break;
    case NUMBER_ROLE:
        return this->dataNumber(entry, column);
    case INDICATOR_COLOR_ROLE:
        return this->dataIndicatorColor(entry, column);
    case UNIQUE_SOURCE_ID_ROLE:
        if (column_type == FAVORITE || column_type == PERSONAL_CONTACT) {
            QVariantMap favorite_key;
            favorite_key["source"] = entry.sourceName();
            favorite_key["source_entry_id"] = entry.sourceEntryId();
            return favorite_key;
        }
    case SORT_FILTER_ROLE:
        return this->dataSortFilter(entry, column);
    default:
        break;
    }
    return AbstractTableModel::data(index, role);
}

QVariant PeopleEntryModel::headerData(int column, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || column < 0 || column >= this->columnCount()) {
        return QVariant();
    }

    switch(role) {
    case Qt::DisplayRole:
        return this->headerText(column);
    case Qt::UserRole:
        return this->headerType(column);
    default:
        return QVariant();
    }
}

QString PeopleEntryModel::headerText(int column) const
{
    return this->m_fields[column].first;
}

enum ColumnType PeopleEntryModel::headerType(int column) const
{
    return this->m_fields[column].second;
}

QVariant PeopleEntryModel::dataDecoration(const PeopleEntry &entry, int column) const
{
    ColumnType column_type = this->headerType(column);

    switch (column_type) {
    case AGENT:
    {
        const QString &agent_status = entry.agentStatus();
        if (agent_status == "logged_in") {
            return QIcon(":/images/agent-on.svg").pixmap(QSize(20, 20));
        } else if (agent_status == "logged_out") {
            return QIcon(":/images/agent-off.svg").pixmap(QSize(20, 20));
        }
    }
    break;
    case FAVORITE:
    {
        if (entry.sourceEntryId().isEmpty()) {
            break;
        }
        if (entry.data(column).toBool()) {
            return QIcon(":/images/star-filled.svg").pixmap(QSize(12, 12));
        } else {
            return QIcon(":/images/star-empty.svg").pixmap(QSize(12, 12));
        }
    }
    default:
        break;
    }
    return QVariant();
}

QVariant PeopleEntryModel::dataIndicatorColor(const PeopleEntry &entry, int column) const
{
    ColumnType column_type = this->headerType(column);

    switch (column_type) {
    case NAME: // user
    {
        if (entry.userId() == 0) {
            return QVariant();
        }
        QString user_status = entry.userStatus();
        const QVariantMap &status_map = b_engine->getOptionsUserStatus()[QString("%1").arg(user_status)].toMap();
        const QString &color = status_map["color"].toString();
        return QColor(color);
    }
    break;
    case NUMBER: // endpoint
    {
        if (entry.data(column).toString().isEmpty()) {
            return QVariant();
        }
        if (entry.endpointId() == 0) {
            return QVariant();
        }
        int endpoint_status = entry.endpointStatus();
        const QVariantMap &status_map = b_engine->getOptionsPhoneStatus()[QString("%1").arg(endpoint_status)].toMap();
        const QString &color = status_map["color"].toString();
        return QColor(color);
    }
    break;
    default:
        break;
    }
    return QVariant();
}

QVariant PeopleEntryModel::dataNumber(const PeopleEntry &entry, int column) const
{
    ColumnType column_type = this->headerType(column);

    switch (column_type) {
    case NUMBER:
        return this->getAvailableActions(entry, column);
    default:
        return QVariant();
    }
}

QVariant PeopleEntryModel::getAvailableActions(const PeopleEntry &entry, int column) const
{
    QVariantList number_items;
    const QString &title = this->headerText(column);
    const QString &number = entry.data(column).toString();
    number_items.append(newAction(title, number, CALL));
    if (m_endpoint_status == IN_USE) {
        number_items.append(newAction(title, number, BLIND_TRANSFER));
        number_items.append(newAction(title, number, ATTENDED_TRANSFER));
    }

    const QList<int> &callable_indexes = m_type_to_indices[CALLABLE];
    foreach(int column, callable_indexes) {
        const QString &title = this->headerText(column);
        const QString &number = entry.data(column).toString();
        number_items.append(newAction(title, number, CALLABLE_CALL));
        if (m_endpoint_status == IN_USE) {
            number_items.append(newAction(title, number, BLIND_TRANSFER));
            number_items.append(newAction(title, number, ATTENDED_TRANSFER));
        }
    }
    return number_items;
}

QVariant PeopleEntryModel::newAction(const QString &label, const QVariant &value, PeopleAction action) const
{
    QVariantMap item;
    item["label"] = label;
    item["value"] = value;
    item["action"] = action;
    return item;
}

QVariantList PeopleEntryModel::newIdAsList(const QString &xivo_uuid, int id) const
{
    return QVariantList() << xivo_uuid << id;
}

QVariant PeopleEntryModel::dataSortFilter(const PeopleEntry &entry, int column) const
{
    ColumnType column_type = this->headerType(column);

    switch (column_type) {
    case AGENT:
        return entry.agentStatus();
    case FAVORITE:
    {
        if (entry.sourceEntryId().isEmpty()) {
            break;
        }
        return entry.data(column);
    }
    case PERSONAL_CONTACT:
        return entry.data(column);
    default:
        break;
    }
    return QVariant();
}

int PeopleEntryModel::getNameColumnIndex() const
{
    return m_type_to_indices.value(NAME).value(0,-1);
}

bool PeopleEntryModel::favoriteStatus(const QVariantMap &unique_source_entry_id) const
{
    QPair<QString, QString> id(unique_source_entry_id["source"].toString(),
                               unique_source_entry_id["source_entry_id"].toString());

    foreach(const PeopleEntry &entry, m_people_entries) {
        if (entry.uniqueSourceId() == id) {
            const QList<int> &columns = m_type_to_indices[FAVORITE];
            foreach(int column, columns) {
                return entry.data(column).toBool();
            }
        }
    }
    return false;
}

void PeopleEntryModel::removeRowFromSourceEntryId(const QString &source, const QString &source_entry_id)
{
    RelationSourceID id(source, source_entry_id);
    for (int i = 0; i < this->rowCount(); ++i) {
        PeopleEntry &entry = m_people_entries[i];
        if (entry.uniqueSourceId() == id) {
            this->removeRow(i);
        }
    }
}

bool PeopleEntryModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || count < 1 || row + count > m_people_entries.count() ){
        return false;
    }
    this->beginRemoveRows(parent, row, row + count - 1);
    m_people_entries.remove(row, count);
    this->endRemoveRows();
    return true;
}

void PeopleEntryModel::setAgentStatusFromAgentId(const RelationID &id, const QString &status)
{
    for (int i = 0; i < m_people_entries.size(); ++i) {
        PeopleEntry &entry = m_people_entries[i];
        if (entry.uniqueAgentId() == id) {
            entry.setAgentStatus(status);
            this->refreshEntry(i);
        }
    }
}

void PeopleEntryModel::setEndpointStatusFromEndpointId(const RelationID &id, int status)
{
    if (id == m_endpoint) {
        m_endpoint_status = status;
    }

    for (int i = 0; i < m_people_entries.size(); ++i) {
        PeopleEntry &entry = m_people_entries[i];
        if (entry.uniqueEndpointId() == id) {
            entry.setEndpointStatus(status);
            this->refreshEntry(i);
        }
    }
}

void PeopleEntryModel::setUserStatusFromUserId(const RelationID &id, const QString &status)
{
    for (int i = 0; i < m_people_entries.size(); ++i) {
        PeopleEntry &entry = m_people_entries[i];
        if (entry.uniqueUserId() == id) {
            entry.setUserStatus(status);
            this->refreshEntry(i);
        }
    }
}

void PeopleEntryModel::setFavoriteStatusFromSourceId(const RelationSourceID &id, bool status)
{
    for (int i = 0; i < m_people_entries.size(); ++i) {
        PeopleEntry &entry = m_people_entries[i];
        if (entry.uniqueSourceId() == id) {
            const QList<int> &columns = m_type_to_indices[FAVORITE];
            foreach(int column, columns) {
                entry.setData(column, status);
                this->refreshEntry(i);
            }
        }
    }
}

void PeopleEntryModel::parseAgentStatusUpdate(const QVariantMap &result)
{
    RelationID id(result["data"].toMap()["xivo_uuid"].toString(),
                  result["data"].toMap()["agent_id"].toInt());
    QString new_status = result["data"].toMap()["status"].toString();

    this->setAgentStatusFromAgentId(id, new_status);
}

void PeopleEntryModel::parseEndpointStatusUpdate(const QVariantMap &result)
{
    RelationID id(result["data"].toMap()["xivo_uuid"].toString(),
                  result["data"].toMap()["endpoint_id"].toInt());
    int new_status = result["data"].toMap()["status"].toInt();

    this->setEndpointStatusFromEndpointId(id, new_status);
}

void PeopleEntryModel::parseUserStatusUpdate(const QVariantMap &result)
{
    RelationID id(result["data"].toMap()["xivo_uuid"].toString(),
                  result["data"].toMap()["user_id"].toInt());
    const QString &new_status = result["data"].toMap()["status"].toString();

    this->setUserStatusFromUserId(id, new_status);
}

void PeopleEntryModel::parsePeopleFavoriteUpdate(const QVariantMap &result)
{
    RelationSourceID id(result["source"].toString(), result["source_entry_id"].toString());
    bool new_status = result["favorite"].toBool();

    this->setFavoriteStatusFromSourceId(id, new_status);
}

void PeopleEntryModel::parsePeopleHeadersResult(const QVariantMap &result)
{
    const QVariantList &headers = result["column_headers"].toList();
    const QVariantList &types = result["column_types"].toList();
    if (headers.length() != types.length() || headers.length() == 0){
        return;
    }

    this->clearFields();

    int first_column = 0;
    int last_column = headers.length() - 1;
    this->beginInsertColumns(QModelIndex(), first_column, last_column);
    for (int i = 0; i < headers.length() ; i++) {
        const QString &name = headers[i].toString();
        const QString &type = types[i].toString();

        this->addField(name, type);
        this->addIndexByType(type, i);
    }
    this->endInsertColumns();
}

void PeopleEntryModel::parsePeopleSearchResult(const QVariantMap &result)
{
    const QList<QVariant> &entries = result["results"].toList();
    QVariantList endpoint_ids;
    QVariantList agent_ids;
    QVariantList user_ids;

    this->beginResetModel();
    m_people_entries.clear();
    foreach(const QVariant &result, entries) {
        QVariantMap entry_map = result.toMap();
        const QVariantMap &relations = entry_map["relations"].toMap();

        PeopleEntry entry(entry_map["column_values"].toList(),
                          relations["xivo_id"].toString(),
                          entry_map["source"].toString(),
                          relations["source_entry_id"].toString(),
                          relations["agent_id"].toInt(),
                          relations["endpoint_id"].toInt(),
                          relations["user_id"].toInt()
                         );
        const QString &xivo_id = entry.xivoUuid();
        if (entry.agentId() != 0) {
            agent_ids.push_back(newIdAsList(xivo_id, entry.agentId()));
        }
        if (entry.endpointId() != 0) {
            endpoint_ids.push_back(newIdAsList(xivo_id, entry.endpointId()));
        }
        if (entry.userId() != 0) {
            user_ids.push_back(newIdAsList(xivo_id, entry.userId()));
        }

        m_people_entries.append(entry);
    }
    this->endResetModel();

    if (!agent_ids.empty()) {
        b_engine->sendJsonCommand(MessageFactory::registerAgentStatus(agent_ids));
    }
    if (!endpoint_ids.empty()) {
        b_engine->sendJsonCommand(MessageFactory::registerEndpointStatus(endpoint_ids));
    }
    if (!user_ids.empty()) {
        b_engine->sendJsonCommand(MessageFactory::registerUserStatus(user_ids));
    }
}

void PeopleEntryModel::setEndpoint(const QString &xivo_id, int endpoint_id)
{
    m_endpoint = RelationID(xivo_id, endpoint_id);
    QVariantList endpoints_to_register;
    endpoints_to_register.push_back(newIdAsList(xivo_id, endpoint_id));
    b_engine->sendJsonCommand(MessageFactory::registerEndpointStatus(endpoints_to_register));
}
