/* XiVO Client
 * Copyright (C) 2012-2014 Avencall
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

#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>

#include <baseengine.h>

#include "debug.h"

XletDebug::XletDebug(
    QWidget *parent) :
        XLet(parent), m_text(nullptr), m_send(nullptr)
{
    setTitle(tr("Debug"));
    QHBoxLayout *layout = new QHBoxLayout(this);
    m_text = new QTextEdit();
    m_send = new QPushButton("Send");

    layout->addWidget(m_send);
    layout->addWidget(m_text);

    connect(m_send, SIGNAL(clicked()), this, SLOT(sendJSON()));
}

void XletDebug::sendJSON() const
{
    b_engine->sendCommand(m_text->toPlainText());
}

XletDebug::~XletDebug()
{
}
