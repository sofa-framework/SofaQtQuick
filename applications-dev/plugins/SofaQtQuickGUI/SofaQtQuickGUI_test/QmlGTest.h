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
#pragma once
#include <boost/filesystem/path.hpp>
using boost::filesystem::path;

#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlComponent>

#include <sofa/helper/testing/BaseTest.h>
using sofa::helper::testing::BaseTest;

#include <sofa/helper/system/FileSystem.h>
#include <sofa/helper/StringUtils.h>

/// a Qml_test is defined by a python filepath and optional arguments
struct QmlTestData
{
    QmlTestData( const std::string& filepath, const std::string& testgroup,
                 const std::vector<std::string>& arguments );
    std::string filepath;
    std::vector<std::string> arguments;
    std::string testgroup;
};

/// This function is used by gtest to print the content of the struct in a human friendly way
/// eg:
///        test.all_tests/2, where GetParam() = /path/to/file.py with args {1,2,3}
/// instead of the defautl googletest printer that output things like the following:
///        test.all_tests/2, where GetParam() = 56-byte object <10-48 EC-37 18-56 00-00 67-00-00-00>
void PrintTo(const QmlTestData& d, ::std::ostream* os);

class QmlGTest;

/// utility to build a static list of Python_test_data
struct QmlTestList
{
public:
    std::vector<QmlTestData> list;
    std::vector<QmlTestData> getList();

protected:
    virtual void doFillList() = 0;

    /// add a QmlTestData with given path
    void addGroupOfTests(
                 const std::string& filename,
                 const std::string& path="", const std::string &testgroup="",
                 const std::vector<std::string>& arguments=std::vector<std::string>(0));

    /// add all the qml test files in `dir` starting with `prefix`
    void addTestDir(const std::string& dir, const std::string& testgroup = "", const std::string& prefix = "" );

private:
    /// concatenate path and filename
    static std::string filepath( const std::string& path, const std::string& filename )
    {
        if( path!="" )
            return path+"/"+filename;
        else
            return filename;
    }
};

/// A group of tests written in Qml
class QmlGTest :
        public BaseTest,
        public ::testing::WithParamInterface<QmlTestData>
{
public:
    QmlGTest();
    void listAllTests();

    void run( const QmlTestData& );

    /// This function is called by gtest to generate the test from the filename. This is nice
    /// As this allows to do mytest --gtest_filter=*MySomething*
    static std::string getTestName(const testing::TestParamInfo<QmlTestData>& p)
    {
        if(p.param.arguments.size()==0)
            return  std::to_string(p.index)+"_"+p.param.testgroup+path(p.param.filepath).stem().string();
        return  std::to_string(p.index)+"_"+p.param.testgroup+path(p.param.filepath).stem().string()
                                       +"_"+p.param.arguments[0];
    }

private:
    QQmlEngine* m_engine;
    QQmlContext* m_objectContext;
};
