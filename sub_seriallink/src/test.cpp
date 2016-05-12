#include "test.h"



Test::Test(ros::NodeHandle node)
{
    sub = node.subscribe("cameraPos", 1000, &Test::cameraposCallback, this);
};


void Test::run() {
	ros::spin();//must be ros::spin()
};


void Test::cameraposCallback(const use_opencv::move& msg)
{
    Q_EMIT ImageReceived(msg);
}
