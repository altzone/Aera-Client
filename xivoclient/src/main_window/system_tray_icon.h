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

#ifndef __SYSTEM_TRAY_ICON_H__
#define __SYSTEM_TRAY_ICON_H__

#include <QSystemTrayIcon>
#include <xletlib/functests.h>

#include "menu_availability.h"

class MainWindow;
namespace Ui {
    class MainWindow;
}

class SystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
    FUNCTESTED

    public:
        SystemTrayIcon(MainWindow *parent);
        ~SystemTrayIcon();

    private slots:
        void initialize();
        void setSystrayIcon(const QIcon & icon);
        void setSystrayTitle(const QString &);
        void systrayActivated(QSystemTrayIcon::ActivationReason);
        void systrayMsgClicked();

    private:
        MainWindow *mainWindow() const;
        void setUi(Ui::MainWindow *ui);
        QMenu *m_availability;
        MenuAvailability *m_menu_availability;
};

#endif
