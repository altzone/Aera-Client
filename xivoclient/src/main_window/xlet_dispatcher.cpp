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

#include <QMap>

#include <baseengine.h>
#include <xletfactory.h>
#include <QPushButton>

#include "main_window.h"
#include "main_widget.h"
#include "tabber.h"
#include "xlet_dispatcher.h"

XletDispatcher::XletDispatcher(MainWindow *main_window, MainWidget *main_widget, QObject *parent)
    : QObject(parent),
      m_main_window(main_window),
      m_main_widget(main_widget),
      m_dock_container(NULL),
      m_xlets_dock_widget(),
      m_xlets_dock(),
      m_grid_container(NULL),
      m_xlets_grid_widget(),
      m_xlets_grid(),
      m_xlets_tab_widget(),
      m_xlets_tab(),
      m_tabber(NULL),
      m_fold_signal_mapper(new QSignalMapper(this)),
      m_unfold_signal_mapper(new QSignalMapper(this))
{
    connect(b_engine, SIGNAL(logged()),
            this, SLOT(setStatusLogged()));
    connect(b_engine, SIGNAL(delogged()),
            this, SLOT(setStatusNotLogged()));
    connect(this->m_fold_signal_mapper, SIGNAL(mapped(const QString &)),
            this, SLOT(showOneXlet(const QString &)));
    connect(this->m_unfold_signal_mapper, SIGNAL(mapped(const QString &)),
            this, SLOT(showAllXlets()));
    connect(b_engine, SIGNAL(aboutToBeDelogged()),
            this, SLOT(setAboutToBeNotLogged()));
}

void XletDispatcher::showOneXlet(const QString &xlet_name)
{
    XLet *identity = m_xlets[xlet_name];
    QMap<QString, XLet *> xlets_to_hide = this->m_xlets_grid_widget;
    xlets_to_hide.remove(xlet_name);

    this->hideXletsDock();

    if (this->m_tabber) {
        int current_index = this->m_tabber->tabWidget()->currentIndex();
        b_engine->getSettings()->setValue("display/lastfocusedtab", current_index);
        m_tabber->hide();
    }

    foreach (QWidget *widget, xlets_to_hide.values()) {
        widget->hide();
    }

    m_main_window->setFolded(identity->size());
}

void XletDispatcher::showAllXlets()
{

    foreach (QWidget *widget, this->m_xlets_grid_widget.values()) {
        widget->show();
    }

    if (this->m_tabber) {
        this->m_tabber->show();
        int new_index = b_engine->getSettings()->value("display/lastfocusedtab").toInt();
        this->m_tabber->tabWidget()->setCurrentIndex(new_index);
    }

    m_main_window->restoreFolded();
    this->showXletsDock();
    m_main_window->restoreStateFromConfigFile();
}

XletDispatcher::~XletDispatcher()
{
}

void XletDispatcher::setStatusLogged()
{
    this->prepareAppearance();
    this->prepareXletsGrid();
    this->prepareXletsTab();
    this->prepareXletsDock();
}

void XletDispatcher::setStatusNotLogged()
{
    this->cleanXletsDock();
    this->cleanXletsTab();
    this->cleanXletsGrid();
    this->clearAppearance();
}

void XletDispatcher::setAboutToBeNotLogged()
{
    if (m_main_window->isFolded()) {
        this->showAllXlets();
    }
    m_main_window->saveStateToConfigFile();
}

bool XletDispatcher::hasWidget()
{
    return this->m_xlets_grid.size() != 0;
}

XLet *XletDispatcher::xletFactory(const QString &xlet_id)
{
    XLet *xlet = XLetFactory::spawn(xlet_id, this->m_main_widget);
    if (xlet) {
        xlet->doGUIConnects(this->m_main_window);
        this->m_xlets.insert(xlet_id, xlet);
    } else {
        qDebug() << Q_FUNC_INFO << "cannot instantiate XLet" << xlet_id;
    }
    return xlet;
}

void XletDispatcher::prepareXletsGrid()
{
    if (!this->hasWidget()) {
        return;
    }

    this->m_grid_container = new QVBoxLayout(this->m_main_widget);
    this->m_grid_container->setContentsMargins(0,0,0,0);
    this->m_grid_container->setSpacing(0);

    foreach (const XletAndOption &xlet_and_option, this->m_xlets_grid) {
        const QString &name = xlet_and_option.first;
        const QString &options = xlet_and_option.second;
        if (name == "tabber") {
            if (this->m_xlets_tab.size() > 0) {
                this->m_tabber = new Tabber(this->m_main_widget);
                this->m_grid_container->insertWidget(options.toInt(), this->m_tabber);
            }
        } else {
            XLet *xlet = this->xletFactory(name);
            if (xlet) {
                connect(xlet, SIGNAL(showOnlyMeRequested()),
                        m_fold_signal_mapper, SLOT(map()));
                m_fold_signal_mapper->setMapping(xlet, name);
                connect(xlet, SIGNAL(showOthersRequested()),
                        m_unfold_signal_mapper, SLOT(map()));
                m_unfold_signal_mapper->setMapping(xlet, name);
                this->m_grid_container->insertWidget(options.toInt(), xlet);
                this->m_xlets_grid_widget.insert(name, xlet);
            }
        }
    }
}

void XletDispatcher::cleanXletsGrid()
{
    if (!this->hasWidget()) {
        return;
    }

    foreach (QWidget *widget, this->m_xlets_grid_widget.values()) {
        this->m_grid_container->removeWidget(widget);
        widget->deleteLater();
    }

    this->m_grid_container->deleteLater();
    this->m_xlets_grid_widget.clear();
}

void XletDispatcher::prepareXletsTab()
{
    if (! this->m_tabber) {
        return;
    }

    QTabWidget *tab_widget = this->m_tabber->tabWidget();

    foreach (const XletAndOption &xlet_and_option, this->m_xlets_tab) {
        const QString &name = xlet_and_option.first;
        XLet *xlet = this->xletFactory(name);
        if (xlet) {
            int tabIndex = tab_widget->addTab(xlet, QIcon(xlet->iconPath()), "");
            tab_widget->setTabToolTip(tabIndex, xlet->title());
            this->m_xlets_tab_widget.insert(name, xlet);
        }
    }

    int new_index = b_engine->getSettings()->value("display/lastfocusedtab").toInt();
    tab_widget->setCurrentIndex(new_index);
}

void XletDispatcher::cleanXletsTab()
{
    if (! this->m_tabber) {
        return;
    }

    this->m_grid_container->removeWidget(this->m_tabber);

    int current_index = this->m_tabber->tabWidget()->currentIndex();
    b_engine->getSettings()->setValue("display/lastfocusedtab", current_index);

    foreach (QWidget *widget, this->m_xlets_tab_widget.values()) {
        widget->deleteLater();
    }
    this->m_xlets_tab_widget.clear();

    this->m_tabber->deleteLater();
    this->m_tabber = NULL;
}

void XletDispatcher::prepareXletsDock()
{
    if (this->m_xlets_dock.size() == 0) {
        return;
    }

    foreach (const XletAndOption &xlet_and_option, this->m_xlets_dock) {
        const QString &name = xlet_and_option.first;
        const QString &options = xlet_and_option.second;

        XLet *xlet = this->xletFactory(name);
        if (! xlet) {
            qDebug() << Q_FUNC_INFO << "Failed to instanciate xlet" << name;
            continue;
        }

        QDockWidget::DockWidgetFeatures features = this->getXletsDockFeatures(options);
        QDockWidget *dockWidget = new QDockWidget(xlet->title(), this->m_main_widget);
        dockWidget->setFeatures(features);
        dockWidget->setAllowedAreas(Qt::BottomDockWidgetArea);
        dockWidget->setObjectName(name);
        if (options.contains("s")) { // with scrollbar ?
            QScrollArea *dockWidgetContents = new QScrollArea(this->m_main_widget);
            dockWidgetContents->setWidget(xlet);
            dockWidgetContents->setWidgetResizable(true);
            dockWidget->setWidget(dockWidgetContents);
        } else {
            dockWidget->setWidget(xlet);
        }
        this->m_main_window->addDockWidget(Qt::BottomDockWidgetArea, dockWidget);
        this->m_xlets_dock_widget.insert(name, dockWidget);
        dockWidget->hide();
    }
}

QDockWidget::DockWidgetFeatures XletDispatcher::getXletsDockFeatures(const QString &options)
{
    QDockWidget::DockWidgetFeatures features = QDockWidget::NoDockWidgetFeatures;
    if (options.contains("c")) {
        features |= QDockWidget::DockWidgetClosable;
    }
    if (options.contains("f")) {
        features |= QDockWidget::DockWidgetFloatable;
    }
    if (options.contains("m")) {
        features |= QDockWidget::DockWidgetMovable;
    }
    return features;
}

void XletDispatcher::cleanXletsDock()
{
    foreach (QDockWidget *widget, this->m_xlets_dock_widget.values()) {
        this->m_main_window->removeDockWidget(widget);
        widget->deleteLater();
    }
    this->m_xlets_dock_widget.clear();
}

void XletDispatcher::hideXletsDock()
{
    this->m_main_window->saveStateToConfigFile();
    foreach (QDockWidget *widget, this->m_xlets_dock_widget.values()) {
        widget->hide();
    }
}

void XletDispatcher::showXletsDock()
{
    foreach (QDockWidget *widget, this->m_xlets_dock_widget.values()) {
        widget->show();
    }
}

/*! \brief show this XLet on top of others
 *
 * This slot Works when tabbed view is selected.
 */
void XletDispatcher::showWidgetOnTop(QWidget *widget)
{
    if (this->m_tabber) {
        this->m_tabber->tabWidget()->setCurrentWidget(widget);
    }
}

void XletDispatcher::prepareAppearance()
{
    const QVariantList &xlets_infos = b_engine->getCapaXlets();
    foreach (const QVariant &qvariant_xlet_infos, xlets_infos) {
        const QStringList &xlet_infos_raw = qvariant_xlet_infos.toStringList();

        if (xlet_infos_raw.size() < 2) {
            continue;
        }

        const QString &name = xlet_infos_raw[0];
        const QString &type = xlet_infos_raw[1];
        QString options = "";
        if ((name == "customerinfo") && (! b_engine->checkedFunction(name))) {
                continue;
        }
        if (xlet_infos_raw.size() > 2) {
            options = xlet_infos_raw[2];
        }

        XletAndOption pair = XletAndOption(name, options);
        if (type == "dock" && name != "tabber") {
            this->m_xlets_dock << pair;
        } else if (type == "grid") {
            this->m_xlets_grid << pair;
        } else if (type == "tab" && name != "tabber") {
            this->m_xlets_tab << pair;
        }
    }
}

void XletDispatcher::clearAppearance()
{
    m_xlets_dock.clear();
    m_xlets_grid.clear();
    m_xlets_tab.clear();
}
