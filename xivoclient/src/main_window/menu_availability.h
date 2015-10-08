/* XiVO Client
 * Copyright (C) 2013-2014 Avencall
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

#ifndef __MENU_AVAILABILITY_H__
#define __MENU_AVAILABILITY_H__

#include <QMenu>
#include <xletlib/functests.h>

class MainWindow;
class QAction;

class MenuAvailability : public QMenu
{
    Q_OBJECT
    FUNCTESTED

    public:
        MenuAvailability(QMenu *parent);
        ~MenuAvailability();

    private slots:
        void checksAvailState();
        void updatePresence();
        void setAvailability();
        void updateUserStatus(const QString &);
        void setStatusNotLogged();
        void setStatusLogged();
        void confUpdated();

    private:
        void setMenuAvailabilityEnabled(bool);
        void setEnabledMenus(const QString & state);
        void clearPresence();
        void syncPresence();
        bool isValidPresence(const QString &presence) const;
        void addNewPresence(const QString &state, const QString &name);
        QString getCurrentState() const;

        QMenu *m_menu_availability;
        QHash<QString, QAction *>m_availabilities;
        QActionGroup *m_availability_action_group;
};

#endif
