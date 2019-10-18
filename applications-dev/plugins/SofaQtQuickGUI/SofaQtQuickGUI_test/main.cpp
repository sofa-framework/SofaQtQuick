
#include <QtQuickTest/QtQuickTest>
#include <QQmlEngine>
#include <QQmlContext>
#include <QUrl>
#include <QCoreApplication>

#include <sofa/helper/Utils.h>
#include <sofa/helper/system/FileRepository.h>
#include <sofa/helper/system/FileSystem.h>
#include <sofa/simulation/config.h> // #defines SOFA_HAVE_DAG (or not)
#include <SofaSimulationGraph/init.h>

#include <sofa/helper/logging/Messaging.h>

#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    QCoreApplication application(argc, argv);

    /// application specific settings
    application.setOrganizationName("Sofa Consortium");
    application.setApplicationName("SofaQtQuickGUI_test");
    application.setApplicationVersion("v1.0");

    testing::InitGoogleTest(&argc, argv);

    sofa::simulation::graph::init();

    int ret =  RUN_ALL_TESTS();

    sofa::simulation::graph::cleanup();

    return ret;
}

