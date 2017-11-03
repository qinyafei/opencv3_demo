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

	//Canny��Ե���
	Canny(src, edge, 50, 200, 3);
	//����������飬���ڴ洢ֱ�ߵĽǶȺ;�������������
	namedWindow("edge", CV_WINDOW_AUTOSIZE);
	IplImage iplimg;
	iplimg = IplImage(edge);
	cvShowImage("edge", &iplimg);

	vector<Vec2f> lines;
	//����ֱ���Ϊ1���Ƕȷֱ���Ϊ��/180����ֵΪ215
	//��ֵ��ѡȡֱ��Ӱ�쵽���ֱ�ߵ�����
	HoughLines(edge, lines, 1, CV_PI / 180, 170, 0, 0);

	//��ֱ��
	for (size_t i = 0; i < lines.size(); i++)
	{
		//��ȡ������ͽǶ�
		float rho = lines[i][0], theta = lines[i][1];
		//���������㣬����ȷ��һ��ֱ��
		//����õ������������Ϊ����cos��-1000sin�ȣ���sin��+1000cos�ȣ�������cos��+1000sin�ȣ���sin��-1000cos�ȣ�
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		//��ԭͼ�ϻ����Ϊ2�ĺ���
		line(src, pt1, pt2, Scalar(0, 0, 255), 2);
	}

	namedWindow("lines", CV_WINDOW_AUTOSIZE);
	imshow("lines", src);
	waitKey(0);

	return 0;
}
