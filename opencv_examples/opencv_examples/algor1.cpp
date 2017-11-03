// 26RuQinDetection_BaseTwoDiff.cpp : 定义控制台应用程序的入口点。
// 区域入侵检测，基于两帧差分法

#include "stdafx.h"
#include <cv.h>  
#include <cxcore.h>  
#include <highgui.h>  
//CAM定义用摄像头获得视频else文件
//#define CAM 

int apos = 30;
CvRect box;
bool Drawing = false;
bool out = false;
int xx1, xx2, yy1, yy2;
int n = 0;
CvVideoWriter* writer;//保存视频
CvFont font;  //字体

void DrawRect(IplImage*img, CvRect rect)
{
	xx1 = box.x;
	xx2 = box.x + box.width;
	yy1 = box.y;
	yy2 = box.y + box.height;
	cvRectangle(img, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0x00, 0xff, 0x00), 2);

}

//实现对窗口Window_Name上的对象param添加鼠标响应操作
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
//声明IplImage指针  
//static IplImage* pFrame = NULL; //原始视频帧  
static IplImage* pFrImg = NULL; //提取的前景图像，即运动目标  
static IplImage* pBkImg = NULL; //背景图像 

static CvMat* pFrameMat = NULL; //原始视频矩阵 
static CvMat* pFrMat = NULL;    //前景矩阵
static CvMat* pBkMat = NULL;    //背景矩阵


int rectDetect(IplImage* pFrame)
{
	if (!binit)
	{
		//声明IplImage指针   
		pFrImg = NULL; //提取的前景图像，即运动目标  
		pBkImg = NULL; //背景图像 

		pFrameMat = NULL; //原始视频矩阵 
		pFrMat = NULL;    //前景矩阵
		pBkMat = NULL;    //背景矩阵
		nFrmNum = 0;


		box = cvRect(0, 0, -1, -1);
		cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1.0, 1.0, 0, 2, 8);
		//创建窗口  
		cvNamedWindow("video", 1);
		//cvNamedWindow("background",1);  
		cvNamedWindow("foreground", 1);
		//使窗口有序排列  
		cvMoveWindow("video", 30, 0);
		//cvMoveWindow("background", 360, 0);  
		cvMoveWindow("foreground", 690, 0);
		// 滑动条          
		//int nThreshold = 30;        
		cvCreateTrackbar("阀值", "foreground", &apos, 100, on_trackbar);

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
		//创建视频
		writer = cvCreateVideoWriter("123.avi", CV_FOURCC('M', 'J', 'P', 'G'), 27, cvSize(pFrame->width, pFrame->height));

		binit = true;
	}

	//逐帧读取视频  

	{
		CvScalar s;
		int i, j;

		nFrmNum++;

		n = 0;
		//如果是第一帧，需要申请内存，并初始化  
		if (nFrmNum == 1)
		{
			pBkImg = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);
			pFrImg = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);

			pBkMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			pFrMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			pFrameMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);

			//转化成单通道图像再处理  
			cvCvtColor(pFrame, pBkImg, CV_BGR2GRAY); //第一帧作为背景了 
													 //cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY); // 
													 //转换成矩阵
			cvConvert(pBkImg, pFrameMat);  //虽然没用，相当于给矩阵赋了初值
			cvConvert(pBkImg, pFrMat);	   //虽然没用，相当于给矩阵赋了初值
			cvConvert(pBkImg, pBkMat);
		}
		else
		{
			cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);
			cvConvert(pFrImg, pFrameMat);
			//高斯滤波先，以平滑图像  
			//cvSmooth(pFrameMat, pFrameMat, CV_GAUSSIAN, 3, 0, 0);  

			//当前帧跟背景图相减 计算两个数组差的绝对值 
			cvAbsDiff(pFrameMat, pBkMat, pFrMat);

			//二值化前景图  
			cvThreshold(pFrMat, pFrImg, apos, 255.0, CV_THRESH_BINARY);

			//进行形态学滤波，去掉噪音  只有摄像头时候会用
#ifdef CAM
			cvErode(pFrImg, pFrImg, 0, 1);
			cvDilate(pFrImg, pFrImg, 0, 1);
			/* cvDilate(pFrImg, pFrImg, 0, 1);
			cvErode(pFrImg, pFrImg, 0, 1);*/
#endif
			//更新背景  
			//cvRunningAvg(pFrameMat, pBkMat, 0.003, 0);  
			//将背景转化为图像格式，用以显示  
			//cvConvert(pBkMat, pBkImg);  
			cvCopy(pFrameMat, pBkMat, NULL);
			//pBkMat=pFrameMat;
			//显示图像  
			for (int j = xx1; j < xx2; j++)
				for (int i = yy1; i < yy2; i++)
				{
					s = cvGet2D(pFrImg, i, j);
					if (s.val[0] == 0xff)//0xff为白色的
					{
						n++;
					}
				}
			printf("n=%d\n", n);
			//没有目标入侵
			if (n <= 50)
			{
				//在原图像上画框
				cvRectangle(pFrame, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0x00, 0xff, 0x00), 2);
				//在二值化前景图上画框
				cvRectangle(pFrImg, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0xff, 0xff, 0xff), 2);

			}
			else //有目标入侵
			{
				//在原图像上报警并保存视频
				cvRectangle(pFrame, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0x00, 0x00, 0xff), 2);
				cvPutText(pFrame, "warning!", cvPoint(0, 20), &font, CV_RGB(255, 0, 0));
				cvWriteFrame(writer, pFrame);//将图像写入视频
											 //在二值化前景图上报警
				cvRectangle(pFrImg, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0xff, 0xff, 0xff), 2);
				cvPutText(pFrImg, "warning!", cvPoint(0, 20), &font, CV_RGB(255, 255, 255));

			}

			cvShowImage("video", pFrame);
			//cvShowImage("background", pBkImg);  
			cvShowImage("foreground", pFrImg);

			//如果有按键事件，则跳出循环  
			//此等待也为cvShowImage函数提供时间完成显示  
			//等待时间可以根据CPU速度调整  
			if (cvWaitKey(100) >= 0)
			{
				printf("into wait if");
				//break;  
			}

		}
	}

	return 0;

	cvWaitKey();

	//销毁窗口  
	cvDestroyWindow("video");
	//cvDestroyWindow("background");  
	cvDestroyWindow("foreground");

	//释放图像和矩阵  
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
	//声明IplImage指针  
	IplImage* pFrame = NULL; //原始视频帧  
	IplImage* pFrImg = NULL; //提取的前景图像，即运动目标  
	IplImage* pBkImg = NULL; //背景图像 

	CvMat* pFrameMat = NULL; //原始视频矩阵 
	CvMat* pFrMat = NULL;    //前景矩阵
	CvMat* pBkMat = NULL;    //背景矩阵

	CvCapture* pCapture = NULL;
	//帧数
	int nFrmNum = 0;
	box = cvRect(0, 0, -1, -1);
	CvScalar s;
	int i, j;
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1.0, 1.0, 0, 2, 8);
	//创建窗口  
	cvNamedWindow("video", 1);
	//cvNamedWindow("background",1);  
	cvNamedWindow("foreground", 1);
	//使窗口有序排列  
	cvMoveWindow("video", 30, 0);
	//cvMoveWindow("background", 360, 0);  
	cvMoveWindow("foreground", 690, 0);
	// 滑动条          
	//int nThreshold = 30;        
	cvCreateTrackbar("阀值", "foreground", &apos, 100, on_trackbar);

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
	//创建视频
	writer = cvCreateVideoWriter("14floor.avi", CV_FOURCC('M', 'J', 'P', 'G'), 27, cvSize(pFrame->width, pFrame->height));
	//逐帧读取视频  
	while (pFrame = cvQueryFrame(pCapture))
	{
		nFrmNum++;

		n = 0;
		//如果是第一帧，需要申请内存，并初始化  
		if (nFrmNum == 1)
		{
			pBkImg = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);
			pFrImg = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);

			pBkMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			pFrMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			pFrameMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);

			//转化成单通道图像再处理  
			cvCvtColor(pFrame, pBkImg, CV_BGR2GRAY); //第一帧作为背景了 
													 //cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY); // 
													 //转换成矩阵
			cvConvert(pBkImg, pFrameMat);  //虽然没用，相当于给矩阵赋了初值
			cvConvert(pBkImg, pFrMat);	   //虽然没用，相当于给矩阵赋了初值
			cvConvert(pBkImg, pBkMat);
		}
		else
		{
			cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);
			cvConvert(pFrImg, pFrameMat);
			//高斯滤波先，以平滑图像  
			//cvSmooth(pFrameMat, pFrameMat, CV_GAUSSIAN, 3, 0, 0);  

			//当前帧跟背景图相减 计算两个数组差的绝对值 
			cvAbsDiff(pFrameMat, pBkMat, pFrMat);

			//二值化前景图  
			cvThreshold(pFrMat, pFrImg, apos, 255.0, CV_THRESH_BINARY);

			//进行形态学滤波，去掉噪音  只有摄像头时候会用
#ifdef CAM
			cvErode(pFrImg, pFrImg, 0, 1);
			cvDilate(pFrImg, pFrImg, 0, 1);
			/* cvDilate(pFrImg, pFrImg, 0, 1);
			cvErode(pFrImg, pFrImg, 0, 1);*/
#endif
			//更新背景  
			//cvRunningAvg(pFrameMat, pBkMat, 0.003, 0);  
			//将背景转化为图像格式，用以显示  
			//cvConvert(pBkMat, pBkImg);  
			cvCopy(pFrameMat, pBkMat, NULL);
			//pBkMat=pFrameMat;
			//显示图像  
			for (j = xx1; j < xx2; j++)
				for (i = yy1; i < yy2; i++)
				{
					s = cvGet2D(pFrImg, i, j);
					if (s.val[0] == 0xff)//0xff为白色的
					{
						n++;
					}
				}
			printf("n=%d\n", n);
			//没有目标入侵
			if (n <= 50)
			{
				//在原图像上画框
				cvRectangle(pFrame, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0x00, 0xff, 0x00), 2);
				//在二值化前景图上画框
				cvRectangle(pFrImg, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0xff, 0xff, 0xff), 2);

			}
			else //有目标入侵
			{
				//在原图像上报警并保存视频
				cvRectangle(pFrame, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0x00, 0x00, 0xff), 2);
				cvPutText(pFrame, "warning!", cvPoint(0, 20), &font, CV_RGB(255, 0, 0));
				cvWriteFrame(writer, pFrame);//将图像写入视频
											 //在二值化前景图上报警
				cvRectangle(pFrImg, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0xff, 0xff, 0xff), 2);
				cvPutText(pFrImg, "warning!", cvPoint(0, 20), &font, CV_RGB(255, 255, 255));

			}

			cvShowImage("video", pFrame);
			//cvShowImage("background", pBkImg);  
			cvShowImage("foreground", pFrImg);

			//如果有按键事件，则跳出循环  
			//此等待也为cvShowImage函数提供时间完成显示  
			//等待时间可以根据CPU速度调整  
			if (cvWaitKey(100) >= 0)
			{
				printf("into wait if");
				//break;  
			}

		}
	}
	cvWaitKey();

	//销毁窗口  
	cvDestroyWindow("video");
	//cvDestroyWindow("background");  
	cvDestroyWindow("foreground");

	//释放图像和矩阵  
	cvReleaseImage(&pFrImg);
	cvReleaseImage(&pBkImg);

	cvReleaseMat(&pFrameMat);
	cvReleaseMat(&pFrMat);
	cvReleaseMat(&pBkMat);

	cvReleaseCapture(&pCapture);
	cvReleaseVideoWriter(&writer);
	return 0;
}



