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

#ifndef __QUEUE_MEMBER_HEADER_H__
#define __QUEUE_MEMBER_HEADER_H__

#include <QWidget>
#include <QString>

#include <storage/queueinfo.h>

class QLabel;

class QueueMembersHeader : public QWidget
{
    Q_OBJECT

    public:
        QueueMembersHeader(QWidget * parent = NULL);

    public slots:
        void changeWatchedQueue(const QString & queue_id);
        void updateQueueStatus(const QString & queue_id);
        void updateQueueMember(const QString &queue_member_id);
        void removeQueueMember(const QString &queue_member_id);
    private:
        void updateContent();
        void clearContent();
        QString buildHeaderText(const QueueInfo * queue) const;

        QString m_queue_id;
        QLabel * m_header_label;
};

#endif
