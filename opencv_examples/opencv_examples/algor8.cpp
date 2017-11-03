
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace cv;
using namespace std;


int test_img_avi()
{
	int i = 0;
	IplImage* img = 0;
	char image_name[25];

	CvVideoWriter *writer = 0;
	int isColor = 1;
	int fps = 30; // or 25  

	writer = cvCreateVideoWriter("out.avi", CV_FOURCC('X', 'V', 'I', 'D'), fps, cvSize(640, 480), isColor);

	cvNamedWindow("vivi");
	while (1)
	{
		sprintf(image_name, "%s%d%s", ".\\pic\\", ++i, ".jpg");
		img = cvLoadImage(image_name, -1);
		if (img == NULL)
			break;
		cvShowImage("vivi", img);
		char key = cvWaitKey(20);
		cvWriteFrame(writer, img);
		img = NULL;

	}
	cvReleaseVideoWriter(&writer);
	cvDestroyWindow("vivi");

	return 0;
}