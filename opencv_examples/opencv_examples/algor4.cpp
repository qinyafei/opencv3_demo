// 222.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include "opencv/cxcore.h"
//#include "cvcam.h"
#include "opencv/cv.h"
#include "opencv2/highgui/highgui.hpp"


int test_algor4(int argc, char ** argv)
{
	CvCapture * capture = cvCreateFileCapture("vtest.avi");  //读取视频
	if (capture == NULL) {
		printf("NO capture");    //读取不成功，则标识
		return 1;
	};
	double fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);   //读取视频的帧率
	int vfps = 1000 / fps;                                        //计算每帧播放的时间
	printf("%5.1f\t%5d\n", fps, vfps);
	double frames = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);//读取视频中有多少帧
	printf("frames is %f\n", frames);
	cvNamedWindow("example", CV_WINDOW_AUTOSIZE);                  //定义窗口
	IplImage * frame;
	while (1) {
		frame = cvQueryFrame(capture);                          //抓取帧
		float ratio = cvGetCaptureProperty(capture, CV_CAP_PROP_POS_AVI_RATIO);     //读取该帧在视频中的相对位置
		printf("%f\n", ratio);
		if (!frame)break;
		cvShowImage("example", frame);                          //显示
		char c = cvWaitKey(vfps);
		if (c == 27)break;
	}
	cvReleaseCapture(&capture);
	cvDestroyWindow("example");
}
