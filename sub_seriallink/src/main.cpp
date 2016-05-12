/**
 * @file /src/main.cpp
 *
 * @brief Qt based gui.
 *
 * @date November 2010
 **/
/*****************************************************************************
** Includes
*****************************************************************************/

#include <QtGui>
#include <QApplication>
#include <QThread>
#include "test.h"
//#include "mainwindow.h"
#include "ros/ros.h"

/*****************************************************************************
** Main
*****************************************************************************/

#include <use_opencv/move.h>
#include "seriallink_obj.h"

Q_DECLARE_METATYPE(use_opencv::move)
int main(int argc, char **argv) {

    /*********************
    ** Qt
    **********************/
    QApplication app(argc, argv);


    qRegisterMetaType<use_opencv::move>("use_opencv::move");


//    MainWindow w;
//    w.show();
    ros::init(argc, argv, "camera_pos_subscriber");

    seriallink_obj obj;

    ros::NodeHandle node;  
    Test test(node);
    test.start();  
    QObject::connect(&test, SIGNAL(ImageReceived(use_opencv::move)), &obj, SLOT(ImageShow(use_opencv::move)));


    int result = app.exec();

	return result;
}
