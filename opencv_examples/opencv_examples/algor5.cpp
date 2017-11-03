// 18FaceDetetion.cpp : �������̨Ӧ�ó������ڵ㡣
//�������

#include "stdafx.h"
#include <opencv2/opencv.hpp>  
#include <cstdio>  
#include <cstdlib>  
#include <Windows.h>  
using namespace std;


int test_facedetect()
{
	// ����Haar������������  
	// haarcascade_frontalface_alt.xmlϵOpenCV�Դ��ķ����� �������һ����ϵ��ļ�·��  
	const char *pstrCascadeFileName = "haarcascade_frontalface_alt.xml";
	CvHaarClassifierCascade *pHaarCascade = NULL;
	pHaarCascade = (CvHaarClassifierCascade*)cvLoad(pstrCascadeFileName);

	// ����ͼ��  
	const char *pstrImageName = "timg.jpg";
	IplImage *pSrcImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_UNCHANGED);

	IplImage *pGrayImage = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);
	cvCvtColor(pSrcImage, pGrayImage, CV_BGR2GRAY);

	// ����ʶ������  
	if (pHaarCascade != NULL)
	{
#if 0
		CvScalar FaceCirclecolors[] =
		{
			{ { 0, 0, 255 } },
			{ { 0, 128, 255 } },
			{ { 0, 255, 255 } },
			{ { 0, 255, 0 } },
			{ { 255, 128, 0 } },
			{ { 255, 255, 0 } },
			{ { 255, 0, 0 } },
			{ { 255, 0, 255 } }
		};
#else
		CvScalar FaceCirclecolors[] =
		{
			{  0, 0, 255 },
			{  0, 128, 255  },
			{  0, 255, 255 },
			{  0, 255, 0 },
			{  255, 128, 0  },
			{  255, 255, 0  },
			{  255, 0, 0  },
			{  255, 0, 255  }
		};
#endif

		CvMemStorage *pcvMStorage = cvCreateMemStorage(0);
		cvClearMemStorage(pcvMStorage);
		// ʶ��  
		DWORD dwTimeBegin, dwTimeEnd;
		dwTimeBegin = GetTickCount();
		CvSeq *pcvSeqFaces = cvHaarDetectObjects(pGrayImage, pHaarCascade, pcvMStorage);
		dwTimeEnd = GetTickCount();

		printf("��������: %d   �����ʱ: %d ms\n", pcvSeqFaces->total, dwTimeEnd - dwTimeBegin);

		// ���  
		for (int i = 0; i <pcvSeqFaces->total; i++)
		{
			CvRect* r = (CvRect*)cvGetSeqElem(pcvSeqFaces, i);
			CvPoint center;
			int radius;
			center.x = cvRound((r->x + r->width * 0.5));
			center.y = cvRound((r->y + r->height * 0.5));
			radius = cvRound((r->width + r->height) * 0.25);
			cvCircle(pSrcImage, center, radius, FaceCirclecolors[i % 8], 2);
		}
		cvReleaseMemStorage(&pcvMStorage);
	}

	const char *pstrWindowsTitle = "������� ";
	cvNamedWindow(pstrWindowsTitle, CV_WINDOW_AUTOSIZE);
	cvShowImage(pstrWindowsTitle, pSrcImage);

	cvWaitKey(0);

	cvDestroyWindow(pstrWindowsTitle);
	cvReleaseImage(&pSrcImage);
	cvReleaseImage(&pGrayImage);
	return 0;
}
