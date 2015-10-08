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

#include <QVariant>

#include "people_entry_sort_filter_proxy_model.h"
#include "people_enum.h"


PeopleEntrySortFilterProxyModel::PeopleEntrySortFilterProxyModel(QObject *parent)
    : AbstractSortFilterProxyModel(parent)
{
}

void PeopleEntrySortFilterProxyModel::setFilterMode(PeopleMode mode)
{
    if (m_mode != mode) {
        m_mode = mode;
        invalidateFilter();
    }
}

bool PeopleEntrySortFilterProxyModel::filterAcceptsColumn(int source_column,
                                                          const QModelIndex &/*source_parent*/) const
{
    if (m_mode == PERSONAL_CONTACT_MODE) {
        return true;
    }

    const QVariant &column_type = sourceModel()->headerData(source_column, Qt::Horizontal, Qt::UserRole);
    if (column_type == PERSONAL_CONTACT) {
        return false;
    }
    return true;
}

bool PeopleEntrySortFilterProxyModel::lessThan(const QModelIndex &left,
                                               const QModelIndex &right) const
{
    const QVariant &column_type = sourceModel()->headerData(left.column(), Qt::Horizontal, Qt::UserRole);

    if (column_type == FAVORITE || column_type == AGENT) {
        const QVariant &left_data = sourceModel()->data(left, SORT_FILTER_ROLE);
        const QVariant &right_data = sourceModel()->data(right, SORT_FILTER_ROLE);
        return left_data < right_data;
    } else {
        return AbstractSortFilterProxyModel::lessThan(left, right);
    }
}
