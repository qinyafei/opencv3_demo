#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
//#include <opencv2/gpu/gpu.hpp>
#include <fstream>
#include <iomanip>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

using namespace std;
using namespace cv;
using namespace cv::ml;


int test_svm()
{
	// Data for visual representation
	int width = 512, height = 512;
	Mat image = Mat::zeros(height, width, CV_8UC3);

	// Set up training data
	int labels[4] = { 1, -1, -1, -1 };
	Mat labelsMat(4, 1, CV_32SC1, labels);

	float trainingData[4][2] = { { 501, 10 },{ 255, 10 },{ 501, 255 },{ 10, 501 } };
	Mat trainingDataMat(4, 2, CV_32FC1, trainingData);

	// Set up SVM's parameters
	Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::LINEAR);
	svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));

	// Train the SVM with given parameters
	Ptr<TrainData> td = TrainData::create(trainingDataMat, ROW_SAMPLE, labelsMat);
	svm->train(td);

	// Or train the SVM with optimal parameters
	//svm->trainAuto(td);

	Vec3b green(0, 255, 0), blue(255, 0, 0);
	// Show the decision regions given by the SVM
	for (int i = 0; i < image.rows; ++i)
		for (int j = 0; j < image.cols; ++j)
		{
			Mat sampleMat = (Mat_<float>(1, 2) << j, i);
			float response = svm->predict(sampleMat);

			if (response == 1)
				image.at<Vec3b>(i, j) = green;
			else if (response == -1)
				image.at<Vec3b>(i, j) = blue;
		}

	// Show the training data
	int thickness = -1;
	int lineType = 8;
	circle(image, Point(501, 10), 5, Scalar(0, 0, 0), thickness, lineType);
	circle(image, Point(255, 10), 5, Scalar(255, 255, 255), thickness, lineType);
	circle(image, Point(501, 255), 5, Scalar(255, 255, 255), thickness, lineType);
	circle(image, Point(10, 501), 5, Scalar(255, 255, 255), thickness, lineType);

	// Show support vectors
	thickness = 2;
	lineType = 8;
	Mat sv = svm->getSupportVectors();

	for (int i = 0; i < sv.rows; ++i)
	{
		const float* v = sv.ptr<float>(i);
		circle(image, Point((int)v[0], (int)v[1]), 6, Scalar(128, 128, 128), thickness, lineType);
	}

	imwrite("result.png", image);        // save the image

	imshow("SVM Simple Example", image); // show it to the user
	waitKey(0);

	return 0;
}

#ifdef SVM__

int test_objdetect()
{
#if 0
	Mat image;
	image = imread("D:\\dataset\\temp\\6.png");
	HOGDescriptor hog;
	vector<float> v_feature;
	hog.compute(image, v_feature, cv::Size(8, 8));
	//hog.compute(image, v_feature, cv::Size(16, 16));

	int featureVecSize = v_feature.size();
	cout << "****************************************haha****************" << endl;
	cout << featureVecSize << endl;
#else
	//***************************************************************************************************
	//                                           初始化    
	//***************************************************************************************************

	//char positivePath[256] = "D:\\dataset\\INRIAPerson\\Train\\pos\\";
	//char negativePath[256] = "D:\\dataset\\INRIAPerson\\Train\\neg\\";
	//char testPath[256] = "D:\\dataset\\INRIAPerson\\Test\\pos\\";
	char positivePath[256] = "D:\\dataset\\INRIAPerson\\train_64x128_H96\\pos\\";
	char negativePath[256] = "D:\\dataset\\INRIAPerson\\train_64x128_H96\\neg\\";
	char testPath[256] = "D:\\dataset\\INRIAPerson\\test_64x128_H96\\pos\\";
	char classifierSavePath[256] = "D:\\dataset\\INRIAPerson\\myResult";

	int positiveSampleCount = 614;
	int negativeSampleCount = 1218;
	//int positiveSampleCount = 100;
	//int negativeSampleCount = 100;

	int totalSampleCount = positiveSampleCount + negativeSampleCount;
	int testSampleCount = 288;


	CvMat *sampleFeaturesMat = cvCreateMat(totalSampleCount, 94500, CV_32FC1);             //参数1764？
	cvSetZero(sampleFeaturesMat);
	CvMat *sampleLabelMat = cvCreateMat(totalSampleCount, 1, CV_32FC1);//样本标识    
	cvSetZero(sampleLabelMat);

	//CvMat *testFeaturesMat = cvCreateMat(testSampleCount, 94500, CV_32FC1);             //参数1764？    正288，负453
	CvMat *testFeaturesMat = cvCreateMat(1, 94500, CV_32FC1);             //参数1764？    正288，负453
	cvSetZero(testFeaturesMat);
	CvMat *testLabelMat = cvCreateMat(testSampleCount, 1, CV_32FC1);//样本标识    
	cvSetZero(testLabelMat);
	//float testLabelMat[288] = {0};
	//Mat sampleFeaturesMat();


	//***************************************************************************************************
	//                                       正例的特征
	//***************************************************************************************************
	//positive文件读入
	//ifstream fin(positivePath, ios::in);
	//if (!fin)
	//{
	//    cout << "positivePath can not open the file!" << endl;
	//    return -1;
	//}
	char positiveImage[256];
	string path;
	string s_positiveImage;
	for (int i = 0; i < positiveSampleCount; i++)
	{
		//图片名修改
		memset(positiveImage, '\0', sizeof(positiveImage));
		sprintf(positiveImage, "%d.png", i + 1);                              //要改成.jpg吗
		s_positiveImage = positiveImage;
		path = positivePath + s_positiveImage;

		Mat image = imread(path);
		if (image.data == NULL)
		{
			cout << "positive image sample load error: " << i << " " << path << endl;
			system("pause");
			continue;
		}
		//hog特征提取
		//gpu::HOGDescriptor hog(Size(64, 64), Size(16, 16), Size(8, 8), Size(8, 8), 9);    //貌似还有一些参数，重载了？
		//HOGDescriptor hog(Size(64, 64), Size(16, 16), Size(8, 8), Size(8, 8), 9);
		HOGDescriptor hog;
		vector<float> v_feature;
		hog.compute(image, v_feature, cv::Size(8, 8));
		//hog.compute(image, v_feature, cv::Size(16, 16));

		int featureVecSize = v_feature.size();
		//cout << "****************************************haha****************" << endl;
		//cout << featureVecSize<<endl;
		//return 0;
		for (int j = 0; j<featureVecSize; j++)
		{
			//sampleFeaturesMat[i][j] = v_feature[j];
			CV_MAT_ELEM(*sampleFeaturesMat, float, i, j) = v_feature[j];                    //CV_MAT_ELEM什么意思
		}
		sampleLabelMat->data.fl[i] = 1;                                                     //.fl[]是什么
		cout << "positive" << i + 1 << "is running..." << endl;
	}
	//fclose(fin);
	//***************************************************************************************************
	//                                       负例的特征
	//***************************************************************************************************
	//negative文件读入
	//ifstream fin(negativePath, ios::in);
	//if (!fin)
	//{
	//    cout << "can not open the file!" << endl;
	//    return -1;
	//}
	char negativeImage[256] = "";                                    //初始化吗
	string s_negativeImage;
	for (int i = 0; i < negativeSampleCount; i++)
	{
		//图片名修改
		//hog特征提取
		memset(negativeImage, '\0', sizeof(negativeImage));
		sprintf(negativeImage, "%d.png", i + 1);                              //要改成.jpg吗
		s_negativeImage = negativeImage;
		path = negativePath + s_negativeImage;
		//cout << "********************************************************"<<endl;
		//cout << path<<endl;
		Mat image = imread(path);
		if (image.data == NULL)
		{
			cout << "positive image sample load error: " << i << " " << path << endl;
			system("pause");
			continue;
		}
		//hog特征提取
		//gpu::HOGDescriptor hog(Size(64, 64), Size(16, 16), Size(8, 8), Size(8, 8), 9);    //貌似还有一些参数，重载了？
		//HOGDescriptor hog(Size(64, 64), Size(16, 16), Size(8, 8), Size(8, 8), 9);
		HOGDescriptor hog;
		vector<float> v_feature;
		hog.compute(image, v_feature, cv::Size(8, 8));

		int featureVecSize = v_feature.size();
		//cout << "***************************lalala*****************************" << endl;
		//cout << featureVecSize;

		for (int j = 0; j<featureVecSize; j++)
		{
			CV_MAT_ELEM(*sampleFeaturesMat, float, positiveSampleCount + i, j) = v_feature[j];                    //CV_MAT_ELEM什么意思
		}
		sampleLabelMat->data.fl[positiveSampleCount + i] = 0;                                                     //.fl[]是什么
		cout << "negative" << i + 1 << "is running..." << endl;
	}

	//**********************************************************************************************
	//                                         Linear SVM训练
	//**********************************************************************************************
	//设置参数
	Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::LINEAR);
	//svm->setGamma(0.01);
	svm->setC(0.01);
	svm->setTermCriteria(TermCriteria(CV_TERMCRIT_ITER, 1000, FLT_EPSILON));
#if 0
	ml::SVM::Params params;
	params.svm_type = SVM::C_SVC;
	params.C = 0.01;
	params.kernel_type = SVM::LINEAR;
	//params.term_crit = TermCriteria(CV_TERMCRIT_ITER, (int)1e7, 1e-6);
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 1000, FLT_EPSILON);
#endif


	//训练
	cout << "Starting training process" << endl;
#if 0
	CvSVM svm;
	svm.train(sampleFeaturesMat, sampleLabelMat, Mat(), Mat(), params);
#endif
	cout << "Finished training process" << endl;
	//**********************************************************************************************
	//                                            结果保存
	//**********************************************************************************************
	//classifierSavePath文件读入
	//ifstream fin(classifierSavePath, ios::in);
	//if (!fin)
	//{
	//    cout << "positivePath can not open the file!" << endl;
	//    return -1;
	//}
	//svm.save(classifierSavePath);
	//cvReleaseMat(&sampleFeaturesMat);
	//cvReleaseMat(&sampleLabelMat);
	//cout << "has saved succeeded! " << endl;

	//**********************************************************************************************
	//                                          测试
	//**********************************************************************************************
	//test文件读入
	//ifstream fin(testPath, ios::in);
	//if (!fin)
	//{
	//    cout << "can not open the file!" << endl;
	//    return -1;
	//}
	char testImage[256] = "";                                    //初始化吗
	string s_testImage;
	for (int i = 0; i < testSampleCount; i++)
	{
		//图片名修改
		//hog特征提取
		memset(testImage, '\0', sizeof(testImage));
		sprintf(testImage, "%d.png", i + 1);                              //要改成.jpg吗
		s_testImage = testImage;
		path = testPath + s_testImage;

		Mat image = imread(path);
		if (image.data == NULL)
		{
			cout << "positive image sample load error: " << i << " " << path << endl;
			system("pause");
			continue;
		}
		//hog特征提取
		//gpu::HOGDescriptor hog(Size(64, 64), Size(16, 16), Size(8, 8), Size(8, 8), 9);    //貌似还有一些参数，重载了？
		//HOGDescriptor hog(Size(64, 64), Size(16, 16), Size(8, 8), Size(8, 8), 9);
		HOGDescriptor hog;
		vector<float> v_feature;
		hog.compute(image, v_feature, cv::Size(8, 8));

		int featureVecSize = v_feature.size();
		//cout << "***************************lalala*****************************" << endl;
		//cout << featureVecSize;
		for (int j = 0; j<featureVecSize; j++)
		{
			//CV_MAT_ELEM(*testFeaturesMat, float, i, j) = v_feature[j];                    //CV_MAT_ELEM什么意思
			CV_MAT_ELEM(*testFeaturesMat, float, 0, j) = v_feature[j];                    //CV_MAT_ELEM什么意思
		}
		float response = svm.predict(testFeaturesMat);
		testLabelMat->data.fl[i] = response;                                                     //.fl[]是什么
																								 //testLabelMat[i] = response;
	}
	float right = 0;
	for (int i = 0; i < testSampleCount; i++)
	{
		if (testLabelMat->data.fl[i] == 1)
			//if (testLabelMat[i] == 1)
		{
			right++;
		}
	}
	float radio = right / testSampleCount;
	cout << "the radio of the train is:" << radio << endl;
#endif
	return 0;
}


#endif