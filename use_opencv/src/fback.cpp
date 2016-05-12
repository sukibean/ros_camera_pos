
#include <opencv2/opencv.hpp>

#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>


#include "ros/ros.h"

#include "use_opencv/move.h"

using namespace cv;
using namespace std;






struct Item
{
    Item(){

    };
    Item(double val) : val_(val), no_(sno++){};
    bool operator< (const Item& r) const { return val_ < r.val_; }

    double val_;
    int no_;
    static int sno;
};

int Item::sno = 0;

static Point2f drawOptFlowMap(const Mat& flow, Mat& cflowmap, int iStepSize,
                              double, const Scalar& color)
{
    Item::sno = 0;
    int head = 0, tail = 0;
    int iStepX = cflowmap.cols/iStepSize;
    int iStepY = cflowmap.rows/iStepSize;
    const int TOTAL = iStepX*iStepY;
    const double pi = 3.1415926;
    const double dRange = 15 * pi/180/*what ever precision you choose*/;
    int peak_index = 0;
    int max_peak_count  = 0;

    vector<Item> timestamp(TOTAL);

    int iIdx = 0;
    for(int y = 0; y < cflowmap.rows; y += iStepSize){
        for(int x = 0; x < cflowmap.cols; x += iStepSize){
            const Point2f& fxy = flow.at<Point2f>(y, x);
            // int iX = cvRound(fxy.x);
            // int iY = cvRound(fxy.x);

            // 			cout << "angle:"<< acos(fxy.x/sqrt(fxy.x*fxy.x+fxy.y*fxy.y))*180/3.1415926<< endl;
            timestamp[iIdx] = Item(acos(fxy.x/sqrt(fxy.x*fxy.x+fxy.y*fxy.y)));
            // timestamp[iIdx] = Item(acos(iX/sqrt(iX*iX+iY*iY)));
            iIdx++;
        }
    }

    sort(timestamp.begin(), timestamp.end());

    while(tail < TOTAL){
        if (tail + 1 - head > max_peak_count) {
            max_peak_count = tail + 1 - head;
            peak_index = head;
        }
        ++tail;
        if(tail < TOTAL){
            while (timestamp[tail].val_ - timestamp[head].val_ > dRange) {
                ++head;
            }
        }
    }

    // int iTmp = peak_index;
    // while(iTmp < TOTAL && fabsf(timestamp[peak_index].val_-timestamp[iTmp].val_) < dRange) {
    //     iTmp++;
    //     /* code */
    // }

    // int ypeak_index = timestamp[peak_index].no_/iStepX*iStepSize;
    // int xpeak_index = timestamp[peak_index].no_%iStepX*iStepSize;
    // cout << "peak_index:"<<peak_index<<endl;
    // cout << "xpeak_index:"<< flow.at<Point2f>(ypeak_index, xpeak_index).x <<endl;
    // cout << "ypeak_index:"<< flow.at<Point2f>(ypeak_index, xpeak_index).y <<endl;

    // cout << endl;

    // int yTmp = timestamp[iTmp].no_/iStepX*iStepSize;
    // int xTmp = timestamp[iTmp].no_%iStepX*iStepSize;
    // cout << "iTmp:"<<iTmp<<endl;
    // cout << "xTmp:"<<flow.at<Point2f>(yTmp, xTmp).x<<endl;
    // cout << "yTmp:"<<flow.at<Point2f>(yTmp, xTmp).y<<endl;

    // cout << endl;

    // cout << "timestamp[peak_index].val_-timestamp[iTmp].val_:"<<(timestamp[peak_index].val_-timestamp[iTmp].val_)*180/pi<<endl;



    int iIdxBegin = peak_index;
    float fSumX = 0.f;
    float fSumY = 0.f;
    int iDenseNum = 0;
    while (iIdxBegin < TOTAL && fabsf(timestamp[iIdxBegin].val_-timestamp[peak_index].val_) < dRange ){

        int y = timestamp[iIdxBegin].no_/iStepX*iStepSize;
        int x = timestamp[iIdxBegin].no_%iStepX*iStepSize;

        const Point2f& fxy = flow.at<Point2f>(y, x);
        // int iX = cvRound(fxy.x);
        // int iY = cvRound(fxy.x);
        // fSumX += iX;
        // fSumY += iY;
        fSumX += fxy.x;
        fSumY += fxy.y;
        iDenseNum++;
        iIdxBegin++;

    }

    if (iDenseNum == 0)
    {
        return Point2f(0, 0);
    }

    return Point2f(fSumX/iDenseNum, fSumY/iDenseNum);


}

static void drawOptFlowMapOrigin(const Mat& flow, Mat& cflowmap, int step,
                                 double, const Scalar& color)
{
    for(int y = 0; y < cflowmap.rows; y += step){
        for(int x = 0; x < cflowmap.cols; x += step){
            const Point2f& fxy = flow.at<Point2f>(y, x);
            line(cflowmap, Point(x,y), Point(cvRound(x+fxy.x), cvRound(y+fxy.y)),
                 color);
            circle(cflowmap, Point(x,y), 2, color, -1);
        }
    }

}

int main(int argc, char** argv)
{
    ros::init(argc,argv,"fback");
    ros::NodeHandle n;
    ros::Publisher chatter_pub = n.advertise<use_opencv::move>("/use_opencv/move", 1000);

    VideoCapture cap(-1);


    if( !cap.isOpened() )
    {
        cout << "can't open VideoCapture! by xqh";

        return -1;
    }

    Mat flow, cflow, frame;
    Mat flow1, cflow1;
    Mat gray, prevgray, uflow;
    // namedWindow("flow", 1);

    //    ros::Rate loop_rate(10);

    for(;;)
    {
        ROS_INFO_STREAM("before cin frame");
        cap >> frame;
        ROS_INFO_STREAM("after cin frame");
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        ROS_INFO_STREAM("after cvtColor");
        if( !prevgray.empty() && ros::ok())
        {
            calcOpticalFlowFarneback(prevgray, gray, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);
            ROS_WARN_STREAM("after calcOpticalFlowFarneback");

            cvtColor(prevgray, cflow, COLOR_GRAY2BGR);
            cflow.copyTo(cflow1);
            uflow.copyTo(flow);
            uflow.copyTo(flow1);

            int iStepSize = 32;
            Point2f f = drawOptFlowMap(flow, cflow, iStepSize, 1.5, Scalar(0, 0,255));
            // drawOptFlowMapOrigin(flow1, cflow1, iStepSize, 1.5, Scalar(0, 255, 0));
            //          imshow("flow", cflow);
            // imshow("flow1", cflow1);

            use_opencv::move msg;
            msg.iX = f.x;
            msg.iY = f.y;

            chatter_pub.publish(msg);

            ros::spinOnce();
            //            loop_rate.sleep();

        }
        if(waitKey(30)>=0)
            break;
        std::swap(prevgray, gray);
    }
    return 0;
}
