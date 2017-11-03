// opencv_examples.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "examples.h"
#include <iostream>
#include <cv.h>  
#include <cxcore.h>  

int main(int argc, char** argv)
{
	cv::String str = cv::getBuildInformation();
	std::cout << str << std::endl;

	//test_algor1(argc, argv);
	//test_algor2(argc, argv);
	//test_algor33(argc, argv);
	//test_algor4(argc, argv);
	//test_facedetect();
	//test_ffmpeg(argc, (const char**)argv);
	//test_outline(argc, (const char**)argv);
	//test_ffmpeg_contours(argc, (const char**)argv);
	test_movedetect(argc, (const char**)argv);
	//test_susan_track(argc, argv);
	//test_canny();
	//test_houghlines(argc, argv);
	//test_img_avi();
	//test_svm();

	return 0;
}

