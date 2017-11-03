// 6CannyAlgorithmforImage.cpp : �������̨Ӧ�ó������ڵ㡣
#include "stdafx.h"
//ͼ���Canny��Ե���  
#include <opencv2/opencv.hpp>  
using namespace std;
//���ؿ���̨����
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")  
IplImage *g_pSrcImage, *g_pCannyImg;
const char *pstrWindowsCannyTitle = "��Ե���ͼ";

//cvCreateTrackbar�Ļص�����  
void on_trackbar5(int threshold)
{
	//canny��Ե���  
	cvCanny(g_pSrcImage, g_pCannyImg, threshold, threshold * 3, 3);
	cvShowImage(pstrWindowsCannyTitle, g_pCannyImg);
}


int test_canny()
{
	const char *pstrImageName = "qqtimg.jpg";
	const char *pstrWindowsSrcTitle = "ԭͼ";
	const char *pstrWindowsToolBar = "Threshold";

	//���ļ�������ͼ��ĻҶ�ͼCV_LOAD_IMAGE_GRAYSCALE - �Ҷ�ͼ  
	g_pSrcImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_GRAYSCALE);
	g_pCannyImg = cvCreateImage(cvGetSize(g_pSrcImage), IPL_DEPTH_8U, 1);

	//��������  
	cvNamedWindow(pstrWindowsSrcTitle, CV_WINDOW_AUTOSIZE);
	cvNamedWindow(pstrWindowsCannyTitle, CV_WINDOW_AUTOSIZE);

	//����������  
	int nThresholdEdge = 1;
	cvCreateTrackbar(pstrWindowsToolBar, pstrWindowsCannyTitle, &nThresholdEdge, 100, on_trackbar5);

	//��ָ����������ʾͼ��  
	cvShowImage(pstrWindowsSrcTitle, g_pSrcImage);
	on_trackbar5(1);

	//�ȴ������¼�  
	cvWaitKey();

	cvDestroyWindow(pstrWindowsSrcTitle);
	cvDestroyWindow(pstrWindowsCannyTitle);
	cvReleaseImage(&g_pSrcImage);
	cvReleaseImage(&g_pCannyImg);
	return 0;
}
