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

#include <baseengine.h>

#include "xlet.h"

XLet::XLet(QWidget *parent, const QString &title, const QString &icon_path)
    : QWidget(parent),
      m_ui(NULL),
      m_monitored_ui(NULL),
      m_title(title),
      m_icon_path(icon_path)
{
    connect(this, SIGNAL(ipbxCommand(const QVariantMap &)),
            b_engine, SLOT(ipbxCommand(const QVariantMap &)));
    connect(b_engine, SIGNAL(localUserInfoDefined()),
            this, SLOT(localUserInfoDefined()));
    connect(b_engine, SIGNAL(monitoredUserInfoDefined()),
            this, SLOT(monitoredUserInfoDefined()));
    m_xuserid = b_engine->getFullId();
}

void XLet::localUserInfoDefined()
{
    m_ui = b_engine->getXivoClientUser();
}

void XLet::monitoredUserInfoDefined()
{
    m_monitored_ui = b_engine->getXivoClientMonitored();
}

const QString & XLet::title() const
{
    return m_title;
}

const QString & XLet::iconPath() const
{
    return m_icon_path;
}

void XLet::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
