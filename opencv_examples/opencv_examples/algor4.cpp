// 222.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include "opencv/cxcore.h"
//#include "cvcam.h"
#include "opencv/cv.h"
#include "opencv2/highgui/highgui.hpp"


int test_algor4(int argc, char ** argv)
{
	CvCapture * capture = cvCreateFileCapture("vtest.avi");  //��ȡ��Ƶ
	if (capture == NULL) {
		printf("NO capture");    //��ȡ���ɹ������ʶ
		return 1;
	};
	double fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);   //��ȡ��Ƶ��֡��
	int vfps = 1000 / fps;                                        //����ÿ֡���ŵ�ʱ��
	printf("%5.1f\t%5d\n", fps, vfps);
	double frames = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);//��ȡ��Ƶ���ж���֡
	printf("frames is %f\n", frames);
	cvNamedWindow("example", CV_WINDOW_AUTOSIZE);                  //���崰��
	IplImage * frame;
	while (1) {
		frame = cvQueryFrame(capture);                          //ץȡ֡
		float ratio = cvGetCaptureProperty(capture, CV_CAP_PROP_POS_AVI_RATIO);     //��ȡ��֡����Ƶ�е����λ��
		printf("%f\n", ratio);
		if (!frame)break;
		cvShowImage("example", frame);                          //��ʾ
		char c = cvWaitKey(vfps);
		if (c == 27)break;
	}
	cvReleaseCapture(&capture);
	cvDestroyWindow("example");
}
