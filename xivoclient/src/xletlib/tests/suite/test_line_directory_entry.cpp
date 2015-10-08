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

#include <QtTest/QtTest>
#include <gmock/gmock.h>
#include <QPixmap>

#include "test_line_directory_entry.h"

#include <storage/phoneinfo.h>
#include <dao/userdao.h>
#include <dao/phonedao.h>
#include <xletlib/line_directory_entry.h>
#include <xletlib/taintedpixmap.h>

using namespace testing;

class MockPhoneInfo: public PhoneInfo
{
    public:
        MockPhoneInfo(const QString &ipbxid, const QString &id) :PhoneInfo(ipbxid, id) {}
        MOCK_CONST_METHOD0(number, const QString&());
};

class MockUserDAO: public UserDAO
{
    public:
        MockUserDAO() {}
        ~MockUserDAO() {};
        MOCK_CONST_METHOD1(findUserFromPhone, const UserInfo*(const PhoneInfo *));
        MOCK_CONST_METHOD1(findNameByPhone, QString(const PhoneInfo *));
        MOCK_CONST_METHOD1(findByXId, const UserInfo *(const QString &));
};

class MockPhoneDAO: public PhoneDAO
{
    public:
        MockPhoneDAO() {}
        ~MockPhoneDAO() {};
        MOCK_CONST_METHOD1(getStatusColor, QColor(const PhoneInfo *));
        MOCK_CONST_METHOD1(getStatusName, QString(const PhoneInfo *));
        MOCK_CONST_METHOD1(getPhoneStatusConfig, QVariantMap(const PhoneInfo *));
        MOCK_CONST_METHOD1(findByIdentity, const PhoneInfo *(const QString &));
        MOCK_CONST_METHOD1(findByXId, const PhoneInfo *(const QString &));
};

void TestLineDirectoryEntry::testNumber()
{
    MockPhoneInfo phone("xivo", "1");
    MockUserDAO mock_user_dao;
    MockPhoneDAO mock_phone_dao;
    QString number = "1234";

    EXPECT_CALL(phone, number())
        .WillRepeatedly(ReturnRef(number));

    LineDirectoryEntry line_directory_entry(phone, mock_user_dao, mock_phone_dao);

    QString result = line_directory_entry.number();

    QCOMPARE(result, number);
}

void TestLineDirectoryEntry::testName()
{
    MockPhoneInfo mock_phone("xivo", "1");
    MockUserDAO mock_user_dao;
    MockPhoneDAO mock_phone_dao;

    QString name = "Alice Tremblay";

    EXPECT_CALL(mock_user_dao, findNameByPhone(&mock_phone))
        .WillRepeatedly(Return(name));

    LineDirectoryEntry line_directory_entry(mock_phone, mock_user_dao, mock_phone_dao);

    QString result = line_directory_entry.name();

    QCOMPARE(result, name);
}

void TestLineDirectoryEntry::testSearchList()
{
    MockPhoneInfo mock_phone("xivo", "1");
    MockUserDAO mock_user_dao;
    MockPhoneDAO mock_phone_dao;

    QString name = "Alice Tremblay";
    QString number = "1234";

    QStringList search;
    search.append(name);
    search.append(number);

    EXPECT_CALL(mock_phone, number())
        .WillRepeatedly(ReturnRef(number));

    EXPECT_CALL(mock_user_dao, findNameByPhone(&mock_phone))
        .WillRepeatedly(Return(name));

    LineDirectoryEntry line_directory_entry(mock_phone, mock_user_dao, mock_phone_dao);

    QStringList result = line_directory_entry.searchList();

    QCOMPARE(result, search);
}
