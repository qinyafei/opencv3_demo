// 26RuQinDetection_BaseTwoDiff.cpp : �������̨Ӧ�ó������ڵ㡣
// �������ּ�⣬������֡��ַ�

#include "stdafx.h"
#include <cv.h>  
#include <cxcore.h>  
#include <highgui.h>  
//CAM����������ͷ�����Ƶelse�ļ�
//#define CAM 

int apos = 30;
CvRect box;
bool Drawing = false;
bool out = false;
int xx1, xx2, yy1, yy2;
int n = 0;
CvVideoWriter* writer;//������Ƶ
CvFont font;  //����

void DrawRect(IplImage*img, CvRect rect)
{
	xx1 = box.x;
	xx2 = box.x + box.width;
	yy1 = box.y;
	yy2 = box.y + box.height;
	cvRectangle(img, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0x00, 0xff, 0x00), 2);

}

//ʵ�ֶԴ���Window_Name�ϵĶ���param��������Ӧ����
void my_mouse_callback(int event, int x, int y, int flags, void* param)
{
	IplImage*img = (IplImage*)param;
	switch (event)
	{
	case CV_EVENT_MOUSEMOVE:
	{
		if (Drawing)
		{
			box.width = x - box.x;
			box.height = y - box.y;
		}
	}
	break;

	case CV_EVENT_LBUTTONDOWN:
	{
		Drawing = true;
		box = cvRect(x, y, 0, 0);
	}
	break;

	case CV_EVENT_LBUTTONUP:
	{
		Drawing = false;
		if (box.width<0)
		{
			box.x += box.width;
			box.width *= -1;
		}
		if (box.height<0)
		{
			box.y += box.height;
			box.height *= -1;
		}
		out = true;
		DrawRect(img, box);

	}
	break;

	}
}


void on_trackbar(int pos)
{
	apos = pos;
}

//
static 	int nFrmNum = 0;
static bool binit = false;
//����IplImageָ��  
//static IplImage* pFrame = NULL; //ԭʼ��Ƶ֡  
static IplImage* pFrImg = NULL; //��ȡ��ǰ��ͼ�񣬼��˶�Ŀ��  
static IplImage* pBkImg = NULL; //����ͼ�� 

static CvMat* pFrameMat = NULL; //ԭʼ��Ƶ���� 
static CvMat* pFrMat = NULL;    //ǰ������
static CvMat* pBkMat = NULL;    //��������


int rectDetect(IplImage* pFrame)
{
	if (!binit)
	{
		//����IplImageָ��   
		pFrImg = NULL; //��ȡ��ǰ��ͼ�񣬼��˶�Ŀ��  
		pBkImg = NULL; //����ͼ�� 

		pFrameMat = NULL; //ԭʼ��Ƶ���� 
		pFrMat = NULL;    //ǰ������
		pBkMat = NULL;    //��������
		nFrmNum = 0;


		box = cvRect(0, 0, -1, -1);
		cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1.0, 1.0, 0, 2, 8);
		//��������  
		cvNamedWindow("video", 1);
		//cvNamedWindow("background",1);  
		cvNamedWindow("foreground", 1);
		//ʹ������������  
		cvMoveWindow("video", 30, 0);
		//cvMoveWindow("background", 360, 0);  
		cvMoveWindow("foreground", 690, 0);
		// ������          
		//int nThreshold = 30;        
		cvCreateTrackbar("��ֵ", "foreground", &apos, 100, on_trackbar);

		IplImage*temp1 = cvCloneImage(pFrame);
		cvSetMouseCallback("video", my_mouse_callback, (void*)pFrame);
		while (out == false)
		{
			cvCopy(pFrame, temp1);
			if (Drawing)
				DrawRect(temp1, box);
			cvShowImage("video", temp1);
			if (cvWaitKey(100) == 27)
				break;
		}

		printf("%d,%d,%d,%d,", xx1, xx2, yy1, yy2);
		//������Ƶ
		writer = cvCreateVideoWriter("123.avi", CV_FOURCC('M', 'J', 'P', 'G'), 27, cvSize(pFrame->width, pFrame->height));

		binit = true;
	}

	//��֡��ȡ��Ƶ  

	{
		CvScalar s;
		int i, j;

		nFrmNum++;

		n = 0;
		//����ǵ�һ֡����Ҫ�����ڴ棬����ʼ��  
		if (nFrmNum == 1)
		{
			pBkImg = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);
			pFrImg = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);

			pBkMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			pFrMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			pFrameMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);

			//ת���ɵ�ͨ��ͼ���ٴ���  
			cvCvtColor(pFrame, pBkImg, CV_BGR2GRAY); //��һ֡��Ϊ������ 
													 //cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY); // 
													 //ת���ɾ���
			cvConvert(pBkImg, pFrameMat);  //��Ȼû�ã��൱�ڸ������˳�ֵ
			cvConvert(pBkImg, pFrMat);	   //��Ȼû�ã��൱�ڸ������˳�ֵ
			cvConvert(pBkImg, pBkMat);
		}
		else
		{
			cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);
			cvConvert(pFrImg, pFrameMat);
			//��˹�˲��ȣ���ƽ��ͼ��  
			//cvSmooth(pFrameMat, pFrameMat, CV_GAUSSIAN, 3, 0, 0);  

			//��ǰ֡������ͼ��� �������������ľ���ֵ 
			cvAbsDiff(pFrameMat, pBkMat, pFrMat);

			//��ֵ��ǰ��ͼ  
			cvThreshold(pFrMat, pFrImg, apos, 255.0, CV_THRESH_BINARY);

			//������̬ѧ�˲���ȥ������  ֻ������ͷʱ�����
#ifdef CAM
			cvErode(pFrImg, pFrImg, 0, 1);
			cvDilate(pFrImg, pFrImg, 0, 1);
			/* cvDilate(pFrImg, pFrImg, 0, 1);
			cvErode(pFrImg, pFrImg, 0, 1);*/
#endif
			//���±���  
			//cvRunningAvg(pFrameMat, pBkMat, 0.003, 0);  
			//������ת��Ϊͼ���ʽ��������ʾ  
			//cvConvert(pBkMat, pBkImg);  
			cvCopy(pFrameMat, pBkMat, NULL);
			//pBkMat=pFrameMat;
			//��ʾͼ��  
			for (int j = xx1; j < xx2; j++)
				for (int i = yy1; i < yy2; i++)
				{
					s = cvGet2D(pFrImg, i, j);
					if (s.val[0] == 0xff)//0xffΪ��ɫ��
					{
						n++;
					}
				}
			printf("n=%d\n", n);
			//û��Ŀ������
			if (n <= 50)
			{
				//��ԭͼ���ϻ���
				cvRectangle(pFrame, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0x00, 0xff, 0x00), 2);
				//�ڶ�ֵ��ǰ��ͼ�ϻ���
				cvRectangle(pFrImg, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0xff, 0xff, 0xff), 2);

			}
			else //��Ŀ������
			{
				//��ԭͼ���ϱ�����������Ƶ
				cvRectangle(pFrame, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0x00, 0x00, 0xff), 2);
				cvPutText(pFrame, "warning!", cvPoint(0, 20), &font, CV_RGB(255, 0, 0));
				cvWriteFrame(writer, pFrame);//��ͼ��д����Ƶ
											 //�ڶ�ֵ��ǰ��ͼ�ϱ���
				cvRectangle(pFrImg, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0xff, 0xff, 0xff), 2);
				cvPutText(pFrImg, "warning!", cvPoint(0, 20), &font, CV_RGB(255, 255, 255));

			}

			cvShowImage("video", pFrame);
			//cvShowImage("background", pBkImg);  
			cvShowImage("foreground", pFrImg);

			//����а����¼���������ѭ��  
			//�˵ȴ�ҲΪcvShowImage�����ṩʱ�������ʾ  
			//�ȴ�ʱ����Ը���CPU�ٶȵ���  
			if (cvWaitKey(100) >= 0)
			{
				printf("into wait if");
				//break;  
			}

		}
	}

	return 0;

	cvWaitKey();

	//���ٴ���  
	cvDestroyWindow("video");
	//cvDestroyWindow("background");  
	cvDestroyWindow("foreground");

	//�ͷ�ͼ��;���  
	cvReleaseImage(&pFrImg);
	cvReleaseImage(&pBkImg);

	cvReleaseMat(&pFrameMat);
	cvReleaseMat(&pFrMat);
	cvReleaseMat(&pBkMat);

	//cvReleaseCapture(&pCapture);
	cvReleaseVideoWriter(&writer);
	return 0;
}



// FFMpeg + OpenCV demo
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

#ifdef __cplusplus
}
#endif

#pragma comment(lib, "opencv_core330d.lib")
#pragma comment(lib, "opencv_highgui330d.lib")

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")	
#pragma comment(lib ,"swscale.lib")




static void CopyData(AVFrame *pFrame, int width, int height, int time);

int test_movedetect(int argc, const char * argv[])
{
	AVFormatContext *pFormatCtx = NULL;
	int             i, videoStream;
	AVCodecContext  *pCodecCtx;
	AVCodec         *pCodec;
	AVFrame         *pFrame;
	AVFrame         *pFrameRGB;
	AVPacket        packet;
	int             frameFinished;
	int             numBytes;
	uint8_t         *buffer;

	// Register all formats and codecs
	av_register_all();

	const char* videoFile = "vtest.avi";

	// Open video file
	if (avformat_open_input(&pFormatCtx, videoFile, NULL, NULL) != 0)
		// if(avformat_open_input(NULL, argv[1], NULL, NULL)!=0)
		return -1; // Couldn't open file

				   // Retrieve stream information
				   //if (av_find_stream_info(pFormatCtx)<0)
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
		return -1; // Couldn't find stream information

				   // Dump information about file onto standard error
	av_dump_format(pFormatCtx, 0, argv[1], false);

	// Find the first video stream
	videoStream = -1;
	for (i = 0; i < pFormatCtx->nb_streams; i++)
	{
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoStream = i;
			break;
		}
	}

	if (videoStream == -1)
		return -1; // Didn't find a video stream

				   // Get a pointer to the codec context for the video stream
	pCodecCtx = pFormatCtx->streams[videoStream]->codec;

	// Find the decoder for the video stream
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL)
		return -1; // Codec not found

				   // Open codec
	if (avcodec_open2(pCodecCtx, pCodec, 0) < 0)
		return -1; // Could not open codec

				   // Hack to correct wrong frame rates that seem to be generated by some codecs
	if (pCodecCtx->time_base.num > 1000 && pCodecCtx->time_base.den == 1)
		pCodecCtx->time_base.den = 1000;

	// Allocate video frame
	//pFrame = avcodec_alloc_frame();
	pFrame = av_frame_alloc();

	// Allocate an AVFrame structure
	//pFrameRGB = avcodec_alloc_frame();
	pFrameRGB = av_frame_alloc();
	if (pFrameRGB == NULL)
		return -1;

	// Determine required buffer size and allocate buffer
	numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width,
		pCodecCtx->height);

	//buffer=malloc(numBytes);
	buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

	// Assign appropriate parts of buffer to image planes in pFrameRGB
	avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24,
		pCodecCtx->width, pCodecCtx->height);

	// Read frames and save first five frames to disk
	i = 0;
	long prepts = 0;
	while (av_read_frame(pFormatCtx, &packet) >= 0)
	{
		// Is this a packet from the video stream?
		if (packet.stream_index == videoStream)
		{
			// Decode video frame
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

			// Did we get a video frame?
			if (frameFinished)
			{
				static struct SwsContext *img_convert_ctx;

#if 0
				// Older removed code
				// Convert the image from its native format to RGB swscale
				img_convert((AVPicture *)pFrameRGB, PIX_FMT_RGB24,
					(AVPicture*)pFrame, pCodecCtx->pix_fmt, pCodecCtx->width,
					pCodecCtx->height);

				// function template, for reference
				int sws_scale(struct SwsContext *context, uint8_t* src[], int srcStride[], int srcSliceY,
					int srcSliceH, uint8_t* dst[], int dstStride[]);
#endif
				// Convert the image into YUV format that SDL uses
				if (img_convert_ctx == NULL) {
					int w = pCodecCtx->width;
					int h = pCodecCtx->height;

					img_convert_ctx = sws_getContext(w, h,
						pCodecCtx->pix_fmt,
						w, h, AV_PIX_FMT_RGB24, SWS_BICUBIC,
						NULL, NULL, NULL);
					if (img_convert_ctx == NULL) {
						fprintf(stderr, "Cannot initialize the conversion context!\n");
						exit(1);
					}
				}
				int ret = sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0,
					pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
#if 0 
				// this use to be true, as of 1/2009, but apparently it is no longer true in 3/2009
				if (ret) {
					fprintf(stderr, "SWS_Scale failed [%d]!\n", ret);
					exit(-1);
				}
#endif

				CopyData(pFrameRGB, pCodecCtx->width, pCodecCtx->height, packet.pts - prepts);
				prepts = packet.pts;
			}
		}

		// Free the packet that was allocated by av_read_frame
		av_free_packet(&packet);
	}

	// Free the RGB image
	//free(buffer);
	av_free(buffer);
	av_free(pFrameRGB);

	// Free the YUV frame
	av_free(pFrame);

	// Close the codec
	avcodec_close(pCodecCtx);

	// Close the video file
	avformat_close_input(&pFormatCtx);

	system("Pause");

	return 0;
}

static void CopyData(AVFrame *pFrame, int width, int height, int time)
{
	if (time <= 0) time = 1;

	int		nChannels;
	int		stepWidth;
	uchar*	pData;
	cv::Mat frameImage(cv::Size(width, height), CV_8UC3, cv::Scalar(0));
	stepWidth = frameImage.step;
	nChannels = frameImage.channels();
	pData = frameImage.data;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			pData[i*stepWidth + j*nChannels + 0] = pFrame->data[0][i*pFrame->linesize[0] + j*nChannels + 2];
			pData[i*stepWidth + j*nChannels + 1] = pFrame->data[0][i*pFrame->linesize[0] + j*nChannels + 1];
			pData[i*stepWidth + j*nChannels + 2] = pFrame->data[0][i*pFrame->linesize[0] + j*nChannels + 0];
		}
	}

	IplImage* rgbFrame;
	rgbFrame = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	*rgbFrame = frameImage;
	rectDetect(rgbFrame);

}


int test_algor1(int argc, char** argv)
{
	//����IplImageָ��  
	IplImage* pFrame = NULL; //ԭʼ��Ƶ֡  
	IplImage* pFrImg = NULL; //��ȡ��ǰ��ͼ�񣬼��˶�Ŀ��  
	IplImage* pBkImg = NULL; //����ͼ�� 

	CvMat* pFrameMat = NULL; //ԭʼ��Ƶ���� 
	CvMat* pFrMat = NULL;    //ǰ������
	CvMat* pBkMat = NULL;    //��������

	CvCapture* pCapture = NULL;
	//֡��
	int nFrmNum = 0;
	box = cvRect(0, 0, -1, -1);
	CvScalar s;
	int i, j;
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1.0, 1.0, 0, 2, 8);
	//��������  
	cvNamedWindow("video", 1);
	//cvNamedWindow("background",1);  
	cvNamedWindow("foreground", 1);
	//ʹ������������  
	cvMoveWindow("video", 30, 0);
	//cvMoveWindow("background", 360, 0);  
	cvMoveWindow("foreground", 690, 0);
	// ������          
	//int nThreshold = 30;        
	cvCreateTrackbar("��ֵ", "foreground", &apos, 100, on_trackbar);

#ifdef CAM
	if (!(pCapture = cvCaptureFromCAM(0)))
	{
		//pCapture = cvCaptureFromCAM(-1))  
		fprintf(stderr, "Can not open CAM .\n");
		return -2;
	}
#else
	char *filename = "driveway-320x240.avi";
	if (!(pCapture = cvCaptureFromAVI(filename)))
	{
		//pCapture = cvCaptureFromCAM(-1))  
		fprintf(stderr, "Can not open file %s.\n", filename);
		return -2;
	}
#endif
	pFrame = cvQueryFrame(pCapture);
	IplImage*temp1 = cvCloneImage(pFrame);
	cvSetMouseCallback("video", my_mouse_callback, (void*)pFrame);
	while (out == false)
	{
		cvCopy(pFrame, temp1);
		if (Drawing)
			DrawRect(temp1, box);
		cvShowImage("video", temp1);
		if (cvWaitKey(100) == 27)
			break;
	}
	printf("%d,%d,%d,%d,", xx1, xx2, yy1, yy2);
	//������Ƶ
	writer = cvCreateVideoWriter("14floor.avi", CV_FOURCC('M', 'J', 'P', 'G'), 27, cvSize(pFrame->width, pFrame->height));
	//��֡��ȡ��Ƶ  
	while (pFrame = cvQueryFrame(pCapture))
	{
		nFrmNum++;

		n = 0;
		//����ǵ�һ֡����Ҫ�����ڴ棬����ʼ��  
		if (nFrmNum == 1)
		{
			pBkImg = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);
			pFrImg = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);

			pBkMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			pFrMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			pFrameMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);

			//ת���ɵ�ͨ��ͼ���ٴ���  
			cvCvtColor(pFrame, pBkImg, CV_BGR2GRAY); //��һ֡��Ϊ������ 
													 //cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY); // 
													 //ת���ɾ���
			cvConvert(pBkImg, pFrameMat);  //��Ȼû�ã��൱�ڸ������˳�ֵ
			cvConvert(pBkImg, pFrMat);	   //��Ȼû�ã��൱�ڸ������˳�ֵ
			cvConvert(pBkImg, pBkMat);
		}
		else
		{
			cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);
			cvConvert(pFrImg, pFrameMat);
			//��˹�˲��ȣ���ƽ��ͼ��  
			//cvSmooth(pFrameMat, pFrameMat, CV_GAUSSIAN, 3, 0, 0);  

			//��ǰ֡������ͼ��� �������������ľ���ֵ 
			cvAbsDiff(pFrameMat, pBkMat, pFrMat);

			//��ֵ��ǰ��ͼ  
			cvThreshold(pFrMat, pFrImg, apos, 255.0, CV_THRESH_BINARY);

			//������̬ѧ�˲���ȥ������  ֻ������ͷʱ�����
#ifdef CAM
			cvErode(pFrImg, pFrImg, 0, 1);
			cvDilate(pFrImg, pFrImg, 0, 1);
			/* cvDilate(pFrImg, pFrImg, 0, 1);
			cvErode(pFrImg, pFrImg, 0, 1);*/
#endif
			//���±���  
			//cvRunningAvg(pFrameMat, pBkMat, 0.003, 0);  
			//������ת��Ϊͼ���ʽ��������ʾ  
			//cvConvert(pBkMat, pBkImg);  
			cvCopy(pFrameMat, pBkMat, NULL);
			//pBkMat=pFrameMat;
			//��ʾͼ��  
			for (j = xx1; j < xx2; j++)
				for (i = yy1; i < yy2; i++)
				{
					s = cvGet2D(pFrImg, i, j);
					if (s.val[0] == 0xff)//0xffΪ��ɫ��
					{
						n++;
					}
				}
			printf("n=%d\n", n);
			//û��Ŀ������
			if (n <= 50)
			{
				//��ԭͼ���ϻ���
				cvRectangle(pFrame, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0x00, 0xff, 0x00), 2);
				//�ڶ�ֵ��ǰ��ͼ�ϻ���
				cvRectangle(pFrImg, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0xff, 0xff, 0xff), 2);

			}
			else //��Ŀ������
			{
				//��ԭͼ���ϱ�����������Ƶ
				cvRectangle(pFrame, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0x00, 0x00, 0xff), 2);
				cvPutText(pFrame, "warning!", cvPoint(0, 20), &font, CV_RGB(255, 0, 0));
				cvWriteFrame(writer, pFrame);//��ͼ��д����Ƶ
											 //�ڶ�ֵ��ǰ��ͼ�ϱ���
				cvRectangle(pFrImg, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0xff, 0xff, 0xff), 2);
				cvPutText(pFrImg, "warning!", cvPoint(0, 20), &font, CV_RGB(255, 255, 255));

			}

			cvShowImage("video", pFrame);
			//cvShowImage("background", pBkImg);  
			cvShowImage("foreground", pFrImg);

			//����а����¼���������ѭ��  
			//�˵ȴ�ҲΪcvShowImage�����ṩʱ�������ʾ  
			//�ȴ�ʱ����Ը���CPU�ٶȵ���  
			if (cvWaitKey(100) >= 0)
			{
				printf("into wait if");
				//break;  
			}

		}
	}
	cvWaitKey();

	//���ٴ���  
	cvDestroyWindow("video");
	//cvDestroyWindow("background");  
	cvDestroyWindow("foreground");

	//�ͷ�ͼ��;���  
	cvReleaseImage(&pFrImg);
	cvReleaseImage(&pBkImg);

	cvReleaseMat(&pFrameMat);
	cvReleaseMat(&pFrMat);
	cvReleaseMat(&pBkMat);

	cvReleaseCapture(&pCapture);
	cvReleaseVideoWriter(&writer);
	return 0;
}



