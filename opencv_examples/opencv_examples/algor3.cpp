// 21VideoContours.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <cv.h>    
#include <cxcore.h>    
#include <highgui.h>  
int apos3 = 20;
void on_trackbar3(int pos)
{
	apos3 = pos;
}



//声明IplImage指针  
static IplImage* image_gray = NULL;
static IplImage* image_erzhi = NULL;
static IplImage* image_qualize = NULL;
static IplImage *image_contours = NULL;

static 	int _levels = 5;
static bool binit = false;
static CvSeq *g_pcvSeq = NULL;
static CvMemStorage* cvMStorage = NULL;
static int nThreshold = 30;

const char *pstrWindowsSrcTitle = "原图";
const char *pstrWindowsGrayTitle = "灰度图";
const char *pstrWindowsErZhiTitle = "二值图";
const char *pstrWindowsContoursTitle = "轮廓图";
const char *pstrWindowsContoursEqualizeTitle = "轮廓均衡图";

int test_contours(IplImage* pFrame)
{
	if (!binit)
	{
		cvNamedWindow(pstrWindowsSrcTitle, 1);
		cvNamedWindow(pstrWindowsGrayTitle, 1);
		cvNamedWindow(pstrWindowsErZhiTitle, 1);
		cvNamedWindow(pstrWindowsContoursTitle, 1);


		// 滑动条        
		cvCreateTrackbar("erzhihua", pstrWindowsErZhiTitle, &nThreshold, 254, on_trackbar3);

		image_gray = cvCreateImage(cvGetSize(pFrame), IPL_DEPTH_8U, 1);
		image_erzhi = cvCreateImage(cvGetSize(pFrame), IPL_DEPTH_8U, 1);
		image_qualize = cvCreateImage(cvGetSize(pFrame), IPL_DEPTH_8U, 1);
		image_contours = cvCreateImage(cvGetSize(pFrame), IPL_DEPTH_8U, 3);

		cvMStorage = cvCreateMemStorage();

		binit = true;
	}

	{
		cvShowImage(pstrWindowsSrcTitle, pFrame);
		// 转为灰度图            
		cvCvtColor(pFrame, image_gray, CV_BGR2GRAY);
		cvShowImage(pstrWindowsGrayTitle, image_gray);
		// 均衡化      
		cvEqualizeHist(image_gray, image_qualize);
		// 转为二值图           
		cvThreshold(image_qualize, image_erzhi, apos3, 255, CV_THRESH_BINARY);
		cvShowImage(pstrWindowsErZhiTitle, image_erzhi);

		// 检索轮廓并返回检测到的轮廓的个数      
		cvFindContours(image_erzhi, cvMStorage, &g_pcvSeq);


		int cnt = 0;
		cvZero(image_contours);
		for (; g_pcvSeq != 0; g_pcvSeq = g_pcvSeq->h_next)
		{
			cnt++;
			CvScalar color = CV_RGB(rand() & 255, rand() & 255, rand() & 255);
			cvDrawContours(image_contours, g_pcvSeq, color, color, 0, 2, CV_FILLED, cvPoint(0, 0));
			CvRect rect = cvBoundingRect(g_pcvSeq, 0);
			cvRectangle(image_contours, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height), CV_RGB(255, 0, 0), 1, 8, 0);
		}
		printf("the num of contours : %d\n", cnt);


		//cvZero(image_contours);
		//cvDrawContours(image_contours, g_pcvSeq, CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), _levels);
		cvShowImage(pstrWindowsContoursTitle, image_contours);
		if (cvWaitKey(10) >= 0)
			return 0;
	}

	return 0;

	cvDestroyWindow(pstrWindowsSrcTitle);
	cvDestroyWindow(pstrWindowsGrayTitle);
	cvDestroyWindow(pstrWindowsErZhiTitle);
	cvDestroyWindow(pstrWindowsContoursTitle);
	cvReleaseImage(&image_gray);
	cvReleaseImage(&image_erzhi);
	cvReleaseImage(&image_contours);


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
int diffFrame(IplImage* pFrame, int width, int height);

int test_ffmpeg_contours(int argc, const char * argv[])
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
	test_contours(rgbFrame);

}


//-----------------------------------------


int test_algor33(int argc, char* argv[])
{
	const char *pstrWindowsSrcTitle = "原图";
	const char *pstrWindowsGrayTitle = "灰度图";
	const char *pstrWindowsErZhiTitle = "二值图";
	const char *pstrWindowsContoursTitle = "轮廓图";
	const char *pstrWindowsContoursEqualizeTitle = "轮廓均衡图";
	IplImage* motion = NULL;
	CvCapture* capture = NULL; //视频获取结构   

	CvSeq *g_pcvSeq = NULL;
	//轮廓最大层数    
	int _levels = 5;
	capture = cvCaptureFromCAM(0);
	//capture = cvCaptureFromAVI( "1.avi" );  
	if (capture)
	{
		cvNamedWindow(pstrWindowsSrcTitle, 1);
		cvNamedWindow(pstrWindowsGrayTitle, 1);
		cvNamedWindow(pstrWindowsErZhiTitle, 1);
		cvNamedWindow(pstrWindowsContoursTitle, 1);

		cvGrabFrame(capture); //从摄像头或者视频文件中抓取帧        
		IplImage* image = cvRetrieveFrame(capture); //取回由函数cvGrabFrame抓取的图像,返回由函数cvGrabFrame 抓取的图像的指针
		IplImage* image_gray = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
		IplImage* image_erzhi = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
		IplImage* image_qualize = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
		IplImage *image_contours = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
		// 滑动条        
		int nThreshold = 0;
		cvCreateTrackbar("erzhihua", pstrWindowsErZhiTitle, &nThreshold, 254, on_trackbar3);
		CvMemStorage* cvMStorage = cvCreateMemStorage();
		for (;;)
		{

			if (!cvGrabFrame(capture)) //从摄像头或者视频文件中抓取帧  
				break;
			image = cvRetrieveFrame(capture); //取回由函数cvGrabFrame抓取的图像,返回由函数cvGrabFrame 抓取的图像的指针  
			cvShowImage(pstrWindowsSrcTitle, image);
			// 转为灰度图            
			cvCvtColor(image, image_gray, CV_BGR2GRAY);
			cvShowImage(pstrWindowsGrayTitle, image_gray);
			// 均衡化      
			cvEqualizeHist(image_gray, image_qualize);
			// 转为二值图           
			cvThreshold(image_qualize, image_erzhi, apos3, 255, CV_THRESH_BINARY);
			cvShowImage(pstrWindowsErZhiTitle, image_erzhi);

			// 检索轮廓并返回检测到的轮廓的个数      
			cvFindContours(image_erzhi, cvMStorage, &g_pcvSeq);

			int cnt = 0;
			cvZero(image_contours);
			for (; g_pcvSeq != 0; g_pcvSeq = g_pcvSeq->h_next)
			{
				cnt++;
				CvScalar color = CV_RGB(rand() & 255, rand() & 255, rand() & 255);
				cvDrawContours(image_contours, g_pcvSeq, color, color, 0, 2, CV_FILLED, cvPoint(0, 0));
				CvRect rect = cvBoundingRect(g_pcvSeq, 0);
				cvRectangle(image_contours, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height), CV_RGB(255, 0, 0), 1, 8, 0);
			}

			printf("the num of contours : %d\n", cnt);

			//cvZero(image_contours);
			//cvDrawContours(image_contours, g_pcvSeq, CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), _levels);
			cvShowImage(pstrWindowsContoursTitle, image_contours);
			if (cvWaitKey(10) >= 0)
				break;
		}
		cvDestroyWindow(pstrWindowsSrcTitle);
		cvDestroyWindow(pstrWindowsGrayTitle);
		cvDestroyWindow(pstrWindowsErZhiTitle);
		cvDestroyWindow(pstrWindowsContoursTitle);
		cvReleaseCapture(&capture);
		cvReleaseImage(&image);
		cvReleaseImage(&image_gray);
		cvReleaseImage(&image_erzhi);
		cvReleaseImage(&image_contours);

	}

	return 0;
}
