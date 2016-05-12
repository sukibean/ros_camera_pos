#include <QThread>
#include "ros/ros.h"

#include <use_opencv/move.h>

//#include <image_transport/image_transport.h>
//#include <cv_bridge/cv_bridge.h>
//#include <sensor_msgs/image_encodings.h>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/highgui/highgui.hpp>

//using namespace cv;

class MainWindow;

class Test:public QThread
{  
    Q_OBJECT 
Q_SIGNALS:
    void ImageReceived(const use_opencv::move& msg);
public:  

    Test(ros::NodeHandle node);

    ros::Subscriber sub;

    void cameraposCallback(const use_opencv::move& msg);

 


 
public:  
    void run();  
};

