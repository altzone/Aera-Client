/* XiVO Client
 * Copyright (C) 2013-2015 Avencall
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

#include <baseengine.h>

#include "system_tray_icon.h"
#include "main_window.h"


SystemTrayIcon::SystemTrayIcon(MainWindow *parent)
    : QSystemTrayIcon(parent)
{
    this->m_availability = new QMenu(parent);
    this->m_availability->setTitle(tr("&Availability"));
    this->m_menu_availability = new MenuAvailability(this->m_availability);

    this->connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(systrayActivated(QSystemTrayIcon::ActivationReason)));
    this->connect(this, SIGNAL(messageClicked()), SLOT(systrayMsgClicked()));
    this->connect(parent, SIGNAL(initialized()), SLOT(initialize()));
    this->connect(parent, SIGNAL(titleUpdated(const QString &)), SLOT(setSystrayTitle(const QString &)));
    this->connect(parent, SIGNAL(iconUpdated(const QIcon &)), SLOT(setSystrayIcon(const QIcon &)));
}

SystemTrayIcon::~SystemTrayIcon()
{
}

MainWindow *SystemTrayIcon::mainWindow() const
{
    return static_cast<MainWindow *>(this->parent());
}

void SystemTrayIcon::initialize()
{
    this->setUi(this->mainWindow()->ui);
    this->show();
}

void SystemTrayIcon::setUi(Ui::MainWindow *ui)
{
    QMenu *menu = new QMenu(QString("SystrayMenu"), this->mainWindow());
    if (! menu) {
        return;
    }

    menu->addAction(ui->action_configure);
    menu->addSeparator();
    menu->addMenu(this->m_availability);
    menu->addSeparator();
    menu->addAction(ui->action_connect);
    menu->addAction(ui->action_disconnect);
    menu->addSeparator();
    menu->addAction(ui->action_show_window);
    menu->addSeparator();
    menu->addAction(ui->action_to_systray);
    menu->addSeparator();
    menu->addAction(ui->action_quit);
    this->setContextMenu(menu);
}

void SystemTrayIcon::setSystrayTitle(const QString & title)
{
    this->setToolTip(title);
}

void SystemTrayIcon::setSystrayIcon(const QIcon & icon)
{
    this->setIcon(icon);
}

/*! \brief process clicks to the systray icon
 *
 * This slot is connected to the activated() signal of the
 * System Tray icon. It currently toggle the visibility
 * of the MainWidget on a simple left click. */
void SystemTrayIcon::systrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        #ifndef Q_OS_MAC
        if(this->mainWindow()->isVisible()) {
            this->mainWindow()->hideWindow();
        } else {
            this->mainWindow()->showWindow();
        }
        #endif
    }
}

/*!
 * This slot implementation show, activate (and raise) the
 * window.
 */
void SystemTrayIcon::systrayMsgClicked()
{
    this->mainWindow()->showWindow();
}
