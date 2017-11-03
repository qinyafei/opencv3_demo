// 6CannyAlgorithmforImage.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
//图像的Canny边缘检测  
#include <opencv2/opencv.hpp>  
using namespace std;
//隐藏控制台窗口
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")  
IplImage *g_pSrcImage, *g_pCannyImg;
const char *pstrWindowsCannyTitle = "边缘检测图";

//cvCreateTrackbar的回调函数  
void on_trackbar5(int threshold)
{
	//canny边缘检测  
	cvCanny(g_pSrcImage, g_pCannyImg, threshold, threshold * 3, 3);
	cvShowImage(pstrWindowsCannyTitle, g_pCannyImg);
}


int test_canny()
{
	const char *pstrImageName = "qqtimg.jpg";
	const char *pstrWindowsSrcTitle = "原图";
	const char *pstrWindowsToolBar = "Threshold";

	//从文件中载入图像的灰度图CV_LOAD_IMAGE_GRAYSCALE - 灰度图  
	g_pSrcImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_GRAYSCALE);
	g_pCannyImg = cvCreateImage(cvGetSize(g_pSrcImage), IPL_DEPTH_8U, 1);

	//创建窗口  
	cvNamedWindow(pstrWindowsSrcTitle, CV_WINDOW_AUTOSIZE);
	cvNamedWindow(pstrWindowsCannyTitle, CV_WINDOW_AUTOSIZE);

	//创建滑动条  
	int nThresholdEdge = 1;
	cvCreateTrackbar(pstrWindowsToolBar, pstrWindowsCannyTitle, &nThresholdEdge, 100, on_trackbar5);

	//在指定窗口中显示图像  
	cvShowImage(pstrWindowsSrcTitle, g_pSrcImage);
	on_trackbar5(1);

	//等待按键事件  
	cvWaitKey();

	cvDestroyWindow(pstrWindowsSrcTitle);
	cvDestroyWindow(pstrWindowsCannyTitle);
	cvReleaseImage(&g_pSrcImage);
	cvReleaseImage(&g_pCannyImg);
	return 0;
}
