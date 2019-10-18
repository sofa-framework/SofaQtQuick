/*********************************************************************
Copyright 2019, Inria, CNRS, University of Lille

This file is part of runSofa2

runSofa2 is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

runSofa2 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
/********************************************************************
 Contributors:
    - damien.marchal@univ-lille.fr
********************************************************************/
#include <QCoreApplication>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlComponent>

#include <sofa/helper/testing/BaseTest.h>
using sofa::helper::testing::BaseTest ;

#include <SofaQtQuickGUI/SofaScene.h>
using sofa::qtquick::SofaScene ;

#include <SofaQtQuickGUI/SofaQtQuickQmlModule.h>
using sofaqtquick::SofaQtQuickQmlModule;

#include "QmlGTest.h"
#include <chrono>
#include <thread>

/// static build of the test list
static struct Bindings_testdata : public QmlTestList
{
    void doFillList()
    {
        addTestDir(std::string(SOFAQTQUICK_TEST_DIR)+"/qmltests/", "");
    }
} qml_tests;

class Bindings_test : public QmlGTest {};

/// run test list using the custom name function getTestName.
/// this allows to do gtest_filter=*FileName*
INSTANTIATE_TEST_CASE_P(SofaQtQuick,
                        Bindings_test,
                        ::testing::ValuesIn(qml_tests.getList()),
                        Bindings_test::getTestName);

TEST_P(Bindings_test, all_tests)
{
    run(GetParam());
}

