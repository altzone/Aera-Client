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

#include "assembler.h"
#include "main_window/main_window.h"
#include "main_window/central_widget.h"
#include "main_window/main_widget.h"
#include "main_window/statusbar.h"
#include "main_window/system_tray_icon.h"
#include "login_widget/login_widget.h"
#include <stdlib.h>

Assembler *assembler = NULL;

Assembler::Assembler()
{
    this->m_main_window = new MainWindow();
    this->m_central_widget = new CentralWidget(this->m_main_window);
    this->m_login_widget = new LoginWidget(this->m_main_window, this->m_central_widget);
    this->m_main_widget = new MainWidget(this->m_central_widget);
    this->m_statusbar = new Statusbar(this->m_main_window);
    this->m_system_tray_icon = new SystemTrayIcon(this->m_main_window);
    this->m_xlet_dispatcher = new XletDispatcher(this->m_main_window, this->m_main_widget, qApp);
    this->m_config_widget = new ConfigWidget(this->m_main_window);

    if (this->m_main_window == NULL
        || this->m_central_widget == NULL
        || this->m_login_widget == NULL
        || this->m_main_widget == NULL
        || this->m_statusbar == NULL
        || this->m_system_tray_icon == NULL
        || this->m_xlet_dispatcher == NULL
        || this->m_config_widget == NULL) {
        qDebug() << Q_FUNC_INFO << "Failed to instanciate the GUI";
        exit(EXIT_FAILURE);
    }
}

Assembler::~Assembler()
{
    if (this->m_main_window) {
        delete m_main_window;
        this->m_main_window = NULL;
    }
}

MainWindow *Assembler::mainWindow()
{
    return m_main_window;
}

CentralWidget *Assembler::centralWidget()
{
    return m_central_widget;
}

MainWidget *Assembler::mainWidget()
{
    return m_main_widget;
}

Statusbar *Assembler::statusbar()
{
    return m_statusbar;
}

SystemTrayIcon *Assembler::systemTrayIcon()
{
    return m_system_tray_icon;
}

LoginWidget *Assembler::loginWidget()
{
    return m_login_widget;
}

ConfigWidget *Assembler::configWidget()
{
    return m_config_widget;
}

XletDispatcher *Assembler::xletDispatcher()
{
    return m_xlet_dispatcher;
}
