#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace cv;
using namespace std;

int test_houghlines(int argc, char** argv)
{
	Mat src, edge;
	src = imread("timg2.jpg");
	if (!src.data)
		return -1;

	//Canny边缘检测
	Canny(src, edge, 50, 200, 3);
	//定义输出数组，用于存储直线的角度和距离这两个变量
	namedWindow("edge", CV_WINDOW_AUTOSIZE);
	IplImage iplimg;
	iplimg = IplImage(edge);
	cvShowImage("edge", &iplimg);

	vector<Vec2f> lines;
	//距离分辨率为1，角度分辨率为π/180，阈值为215
	//阈值的选取直接影响到输出直线的数量
	HoughLines(edge, lines, 1, CV_PI / 180, 170, 0, 0);

	//画直线
	for (size_t i = 0; i < lines.size(); i++)
	{
		//提取出距离和角度
		float rho = lines[i][0], theta = lines[i][1];
		//定义两个点，两点确定一条直线
		//计算得到的两点的坐标为（ρcosθ-1000sinθ，ρsinθ+1000cosθ），（ρcosθ+1000sinθ，ρsinθ-1000cosθ）
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		//在原图上画宽带为2的红线
		line(src, pt1, pt2, Scalar(0, 0, 255), 2);
	}

	namedWindow("lines", CV_WINDOW_AUTOSIZE);
	imshow("lines", src);
	waitKey(0);

	return 0;
}
