#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <fstream>
#define THRESH 25

using namespace cv;
using namespace std;

int main()
{
    system("/home/pi/Desktop/pi_uart");
    fstream myfile;
    myfile.open("/dev/ttyS0", ios::in | ios::out);
    Mat src;
    Mat dst, cdst;

    VideoCapture cap(0);
    if(!cap.isOpened()) {
        //printf("Camera didn't open.\n");
        return -1;
    }
    //namedWindow("line detector", CV_WINDOW_NORMAL);
    //setWindowProperty("line detector", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    
    // set frame size to 200*200 for fast processing
    cap.set(CV_CAP_PROP_FRAME_WIDTH,200);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,200);
    double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

    cout << "Frame Size = " << dWidth << "x" << dHeight << endl;

    Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));
/*
    VideoWriter oVideoWriter ("MyVideo.avi", CV_FOURCC('M','J','P','G'), 20, frameSize, true); //initialize the VideoWriter object 

    if ( !oVideoWriter.isOpened() ) //if not initialize the VideoWriter successfully, exit the program
    {
        cout << "ERROR: Failed to write the video" << endl;
        return -1;
    } */ 
    while(1) {
        cap >> src;

        cdst = src.clone();
        GaussianBlur(src, src, Size(15,15), 2, 2); // smoothens image
        //pyrMeanShiftFiltering( src, src, 20, 45, 3);
        cvtColor(src, src, CV_BGR2GRAY);
        dst.create(src.size(), src.type());
        Canny( src, dst, 15, 15*3, 3, true ); // finds edges in image
        vector<Vec2f> lines;
        // detect lines
        HoughLines(dst, lines, 8, CV_PI/180, 135, 0, 0 ); // finds straight lines in image
        
        // add all rhos and thetas for averaging later
	    float rho1 = 0.0, theta1 = 0.0;
	    int num_lines = 0;
        for( size_t i = 0; i < lines.size(); i++ ) {
            float rho = lines[i][0], theta = lines[i][1];
	        if(theta > 0.1) {
		        rho1 += rho;
	    	    theta1 += theta;
		        num_lines++;
	        }
        }
        
        // center point to compare offset to
    	Point center;
    	center.x = 99;
    	center.y = 99;
        circle(cdst, center, THRESH, Scalar(0,255,0));

    	if(num_lines > 0) {
    	    // average rho and theta
    		rho1 = rho1 / num_lines;
    		theta1 = theta1 / num_lines;
    		// endpoints of the average line
    		Point pt1, pt2;
    		double a = cos(theta1), b = sin(theta1);
    		double x0 = a*rho1, y0 = b*rho1;
    		pt1.x = cvRound(x0 + 1000*(-b));
    		pt1.y = cvRound(y0 + 1000*(a));
    		pt2.x = cvRound(x0 - 1000*(-b));
    		pt2.y = cvRound(y0 - 1000*(a));
    		// draw on screen
    		line( cdst, pt1, pt2, Scalar(255,0,0), 3, CV_AA);
    		// find which one is left and right
    		Point l, r;
    		if (pt1.y > pt2.y) {
    			r = pt1;
    			l = pt2;
    		} else {
    			r = pt2;
    			l = pt1;
    		}
    		// use slope to calculate intercept
    		float slope = ((float) r.x - l.x) / (r.y - l.y);
    		int bi = l.x - slope * l.y;
    		Point onLine;
    		// find the x location of where the line passes through image center
    		onLine.y = 99;
    		onLine.x = 99 * slope + bi;
    		// draw horizontal line from center to line
    		line( cdst, center, onLine, Scalar(0,0,255), 2, CV_AA);
    		// dispatch command to correct offset
    		if(onLine.x > 99 + THRESH) {
    			myfile << "r" << endl;
    			cout << "r" << endl;
    		} else if(onLine.x < 99 - THRESH) {
    			myfile << "l" << endl;
    			cout << "l" << endl;
    		} else {
    			myfile << "f" << endl;
    			cout << "f" << endl;
    		} 
    
    
    
            
    	} else {
    	    // no lines seen so stop
    		myfile << "s" << endl;
    		cout << "s" << endl;
    	}
    	imshow("line detector", cdst);
        //oVideoWriter.write(src);
        waitKey(50);
    } 
    cout << "End" << endl;
    return 0;
}