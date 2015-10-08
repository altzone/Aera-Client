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

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHash>
#include <QLabel>
#include <QPixmap>
#include <QMouseEvent>
#include <QApplication>
#include <QMenu>
#include <QDebug>
#include <QFontMetrics>

#include <baseengine.h>
#include <xivoconsts.h>
#include <storage/agentinfo.h>
#include <storage/userinfo.h>
#include <storage/phoneinfo.h>
#include <storage/queueinfo.h>
#include <storage/queuememberinfo.h>
#include <dao/queuememberdao.h>

#include "peerwidget.h"

/*! \brief Constructor
 */
PeerWidget::PeerWidget(const UserInfo * ui)
    : BasePeerWidget(ui), m_user_status(NULL), m_agentlbl(NULL),
      m_mobilelbl(0), m_hLayout(0), m_peer(0)
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    setLayout(vLayout);
    vLayout->setSpacing(0);
    vLayout->setMargin(0);

    m_hLayout = new QHBoxLayout;
    m_hLayout->setSpacing(0);
    m_peer = new QWidget(this);
    vLayout->addWidget(m_peer);
    m_peer->setStyleSheet(".QWidget {"
                            "border-style: dotted;"
                            "border-left-width: 1px;"
                            "border-color: #000000;"
                        "}");

    QGridLayout *layout = new QGridLayout(m_peer);
    m_peer->setLayout(layout);
    layout->setMargin(3);
    layout->setSpacing(0);

    m_textlbl = new QLabel(m_peer);
    m_textlbl->setMinimumWidth(m_maxWidthWanted);
    setName(m_ui_remote->fullname());
    updateChitChatButton();

    foreach (QString xphoneid, ui->phonelist()) {
        const PhoneInfo * phoneinfo = b_engine->phone(xphoneid);
        if (phoneinfo == NULL)
            continue;
        QString iduserfeatures = phoneinfo->iduserfeatures();
        if ((iduserfeatures.isEmpty()) || (iduserfeatures == "0"))
            continue;

        m_lblphones[xphoneid] = new QLabel(m_peer);
        m_lblphones[xphoneid]->setAlignment(Qt::AlignCenter);
        m_lblphones[xphoneid]->setMinimumSize(m_iconsize, m_iconsize);
        m_lblphones[xphoneid]->setProperty("kind", "term");
        m_hLayout->addWidget(m_lblphones[xphoneid]);
    }

    if (! m_ui_remote->mobileNumber().isEmpty()) {
        m_mobilelbl = new QLabel(m_peer);
        m_mobilelbl->setPixmap(QPixmap(":/images/mobile-grey.png"));
        m_mobilelbl->setAlignment(Qt::AlignCenter);
        m_mobilelbl->setMinimumSize(m_iconsize, m_iconsize);
        m_mobilelbl->setProperty("kind", "mobile");
        setMobileState("grey");

        m_hLayout->addWidget(m_mobilelbl);
    }

    m_agentlbl = new QLabel(m_peer);
    m_agentlbl->hide();
    m_hLayout->addWidget(m_agentlbl);
    m_hLayout->addStretch(1);
    connect(b_engine, SIGNAL(updateAgentConfig(const QString &)),
            this, SLOT(updateAgentConfig(const QString &)));
    connect(b_engine, SIGNAL(updateAgentStatus(const QString &)),
            this, SLOT(updateAgentStatus(const QString &)));
    connect(b_engine, SIGNAL(updateQueueMemberConfig(const QString &)),
            this, SLOT(updateQueueMemberConfig(const QString &)));
    connect(b_engine, SIGNAL(postRemoveQueueMemberConfig(const QString &)),
            this, SLOT(removeQueueMemberConfig(const QString &)));
    layout->addWidget(m_textlbl, 0, 2, 1, 1, Qt::AlignLeft);
    layout->addLayout(m_hLayout, 1, 2);
    setMaximumWidth(PeerWidget::max_width);

    reloadSavedName();
}

void PeerWidget::updateChitChatButton()
{
    if (m_ui_remote->enableclient() && ! m_user_status) {
        m_user_status = new ChitchatButton(m_peer, &m_ui_remote);
        m_user_status->setProperty("xuserid", m_ui_remote->xid());
        m_user_status->setIconSize(QSize(m_iconsize, m_iconsize));
        m_user_status->setFixedWidth(m_iconsize);
        m_user_status->setFlat(true);
        m_user_status->setFocusPolicy(Qt::NoFocus);

        connect(m_user_status, SIGNAL(pressed()),
                ChitChatWindow::chitchat_instance, SLOT(writeMessageTo()));
        // Delete spacing
        if (! m_hLayout->isEmpty()) {
            QLayoutItem * item = m_hLayout->itemAt(0);
            if (item) {
                m_hLayout->removeItem(item);
                delete item;
            }
        }
        m_hLayout->insertWidget(0, m_user_status);
    } else if (! m_ui_remote->enableclient() && m_user_status) {
        delete m_user_status;
        m_user_status = 0;
        m_hLayout->insertSpacing(0 , m_iconsize);
    } else if (m_hLayout->isEmpty()) {
        m_hLayout->insertSpacing(0, m_iconsize);
    }
}

void PeerWidget::updateAgentConfig(const QString & xagentid)
{
    if (m_ui_remote && m_ui_remote->xagentid() == xagentid) {
        m_xagentid = xagentid;
        if (m_xagentid.isEmpty())
            return;
        const AgentInfo * agentinfo = b_engine->agent(xagentid);
        if (agentinfo == NULL)
            return;
        m_agentlbl->setAlignment(Qt::AlignCenter);
        m_agentlbl->setMinimumSize(m_iconsize, m_iconsize);
        m_agentlbl->setProperty("kind", "agent");
        m_agentlbl->show();
        updateAgentToolTip();
    }
}

void PeerWidget::updateAgentStatus(const QString & xagentid)
{
    if (xagentid != m_xagentid)
        return;
    const AgentInfo * agentinfo = b_engine->agent(xagentid);
    if (agentinfo == NULL)
        return;

    QString color = "grey";

    if (agentinfo->logged()) {
        color = "green";
    }

    if (agentinfo->isCallingOrBusy()) {
        color = "red";
    }

    m_agentlbl->setPixmap(TaintedPixmap(
       QString(":/images/agent-trans.png"), QColor(color)).getPixmap());

    this->updateAgentToolTip();
}

void PeerWidget::updateQueueMemberConfig(const QString & queuemember_xid)
{
    const QueueMemberInfo *queuememberinfo = b_engine->queuemember(queuemember_xid);
    if (queuememberinfo == NULL)
        return;
    QString updated_agent_number = queuememberinfo->agentNumber();

    const AgentInfo * current_agent_info = b_engine->agent(m_xagentid);
    if (current_agent_info == NULL) {
        return;
    }
    QString current_agent_number = current_agent_info->agentNumber();
    if (updated_agent_number == current_agent_number) {
        updateAgentToolTip();
    }
}

void PeerWidget::removeQueueMemberConfig(const QString &)
{
    updateAgentToolTip();
}

void PeerWidget::updateAgentToolTip()
{
    if (m_ui_remote == NULL)
        return;
    QString agent_xid = m_ui_remote->xagentid();
    if (! agent_xid.isEmpty()) {
        if (const AgentInfo * agentinfo = b_engine->agent(agent_xid)) {
            QString agentnumber = agentinfo->agentNumber();
            QStringList queue_xid_list = QueueMemberDAO::queueListFromAgentId(agent_xid);
            QStringList queue_display_name_list;
            foreach (const QString & queue_xid, queue_xid_list) {
                const QueueInfo * queueinfo = b_engine->queue(queue_xid);
                if (queueinfo != NULL) {
                    queue_display_name_list << queueinfo->queueDisplayName();
                }
            }
            m_agentlbl->setToolTip(tr("Agent Number : %1\n"
                                      "In Queues : %2")
                                   .arg(agentnumber)
                                   .arg(queue_display_name_list.join(" ")));
        }
    }
}

void PeerWidget::setMobileState(const QString &/* color*/)
{
    if (m_mobilelbl) {
        m_mobilelbl->setToolTip(tr("Mobile number : %1").arg(m_ui_remote->mobileNumber()));
    }
}

void PeerWidget::updatePresence()
{
    updateChitChatButton();
    if (m_user_status) {
        QString availstate = m_ui_remote->availstate();
        QVariantMap presencedetails = b_engine->getOptionsUserStatus().value(availstate).toMap();
        QString colorstring = presencedetails.value("color").toString();
        QColor c = QColor(colorstring);
        m_user_status->setIcon(TaintedPixmap( \
            QString(":/images/personal-trans.png"), c).getPixmap());
        m_user_status->setToolTip(tr("User Name: %1\n"
                                     "IPBXid: %2\n"
                                     "Status: %3")
                                  .arg(m_ui_remote->fullname())
                                  .arg(m_ui_remote->ipbxid())
                                  .arg(presencedetails.value("longname").toString()));
        // to add some day in the tooltip : entity name
    }
}

void PeerWidget::updatePhoneConfig(const QString & xphoneid)
{
    // qDebug() << Q_FUNC_INFO;

    const PhoneInfo * phoneinfo = b_engine->phone(xphoneid);
    if (phoneinfo == NULL)
        return;

    // Add a new phone if it does not exist
    if (! m_lblphones.contains(xphoneid)) {
        m_lblphones[xphoneid] = new QLabel(m_peer);
        m_lblphones[xphoneid]->setAlignment(Qt::AlignCenter);
        m_lblphones[xphoneid]->setMinimumSize(m_iconsize, m_iconsize);
        m_lblphones[xphoneid]->setProperty("kind", "term");
        m_hLayout->insertWidget(1, m_lblphones[xphoneid]);
    }
}

void PeerWidget::updatePhoneStatus(const QString & xphoneid)
{
    const PhoneInfo * phoneinfo = b_engine->phone(xphoneid);
    if (phoneinfo == NULL) {
        qDebug() << Q_FUNC_INFO << "unknown phone xid :" << xphoneid;
        return;
    }
    if (m_ui_remote->id() != phoneinfo->iduserfeatures())
        return;

    QString iduserfeatures = phoneinfo->iduserfeatures();
    if ((iduserfeatures.isEmpty()) || (iduserfeatures == "0"))
        return;

    if (! phoneinfo->xchannels().size()) {
        m_transferred = false;
    }

    QString hintstatus = phoneinfo->hintstatus();
    QString phonenumber = phoneinfo->number();
    QString color = "black";
    QString longname;
    if (b_engine->getOptionsPhoneStatus().contains(hintstatus)) {
        QVariantMap qvmop = b_engine->getOptionsPhoneStatus().value(hintstatus).toMap();
        color = qvmop.value("color").toString();
        longname = qvmop.value("longname").toString();
    } else {
        longname = tr("Status:%1").arg(hintstatus);
    }
    if (phonenumber.isEmpty())
        longname = tr("No status (no phone number)");
    QColor c = QColor(color);
    QLabel * phone_label = m_lblphones.value(xphoneid, NULL);
    if (phone_label == NULL) {
        qDebug() << Q_FUNC_INFO << "unknown phone" << xphoneid << "for user" << iduserfeatures;
        return;
    }
    phone_label->setPixmap( \
              TaintedPixmap(QString(":/images/phone-trans.png"), c).getPixmap());
    if (phonenumber.isEmpty())
        phonenumber = tr("<EMPTY>");
    phone_label->setToolTip(tr("Phone Number: %1\n"
                                         "Status: %2\n"
                                         "Busy lines: %3")
                                      .arg(phonenumber)
                                      .arg(longname)
                                      .arg(phoneinfo->channels().count())
                                      );
}

void PeerWidget::removePhoneConfig(const QString & xphoneid) {
    if (m_lblphones.contains(xphoneid)) {
        m_hLayout->removeWidget(m_lblphones[xphoneid]);
        delete m_lblphones[xphoneid];
        m_lblphones.remove(xphoneid);
    }
}

/*! \brief change displayed name
 */
void PeerWidget::setName(const QString &/*name*/)
{
    QString text = tr("(No callerid yet)");

    // no more phonenumber XXX
    if (! m_ui_remote->fullname().isEmpty())
        text = m_ui_remote->fullname();

    QFontMetrics fm(m_textlbl->font());
    if (m_textlbl->width() < fm.width(text)) {
        m_textlbl->setToolTip(text);
        m_textlbl->setText(m_textlbl->fontMetrics().elidedText(text, Qt::ElideRight, m_textlbl->width()));
    } else {
        m_textlbl->setText(text);
    }
}

ChitchatButton::ChitchatButton(QWidget *parent, const UserInfo * * peerUi)
    : QPushButton(parent), m_ui(peerUi)
{
}

bool ChitchatButton::event(QEvent *e)
{
    /* Don't allow an user to open a chitchat windows
     * with someone non connected or with himself */
    if ((e->type() == QEvent::MouseButtonPress) ||
        (e->type() == QEvent::MouseButtonDblClick)) {
        if ((! (*m_ui)->connected()) ||
            (b_engine->getFullId() == (*m_ui)->xid())) {
            return true;
        }
    }

    return QPushButton::event(e);
}
