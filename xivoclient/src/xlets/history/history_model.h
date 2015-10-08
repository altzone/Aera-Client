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

#ifndef __HISTORY_MODEL_H__
#define __HISTORY_MODEL_H__

#include <QList>
#include <QModelIndex>
#include <QSize>
#include <QVariant>
#include <QWidget>

#include <xletlib/abstract_table_model.h>
#include <ipbxlistener.h>

#include "history_enum.h"
#include "history_item.h"

class HistoryModel : public AbstractTableModel
{
    Q_OBJECT

    public:
        HistoryModel(QWidget * parent = NULL);
        void updateHistory(const QVariantMap &p);

    protected:
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex&, int) const;
        virtual QVariant headerData(int , Qt::Orientation, int) const;
        virtual QList<int> columnDisplayBold() const;
        virtual QList<int> columnDisplaySmaller() const;

    private:
        void initializeHistory(const QVariantMap &p);
        QString prettyPrintDuration(int duration, int mode = ALLCALL) const;

        QList<HistoryItem> m_history_item;
        QList<QString> m_headers;
        static QSize icon_size;
};

#endif
