#include <cstdint>
#include <fstream>
#include <iostream>
#include <sift.h>
#include <string>
#include <tchar.h>

#include "cv.h"
#include "highgui.h"

#include "common.h"
#include "ctime"
#include "math.h"
#include "time.h"

#include "face_alignment.h"
#include "face_detection.h"
#include "face_identification.h"

// 特征码大小
constexpr auto SeetafaceFeatureNnm = 2048;

using namespace cv;
using namespace seeta;
using namespace std;

typedef void(_stdcall* LogCallBack)(const char* logText);
typedef struct {
	FaceInfo face;
	FacialLandmark landmark[5];
} AlignmentResult;

string MODEL_DIR = "./model/";
LogCallBack logger = NULL;

FaceDetection detector(NULL);
FaceAlignment alignment(NULL);
FaceIdentification identification(NULL);

string int2string(int num)
{
	char str[25];
	itoa(num, str, 10);
	return str;
}

void log(const char* txt)
{
	if (logger != NULL)
		logger(txt);
}

// byte[] -> Mat
Mat ByteToMat(BYTE* imageByte, int width, int height, int stride)
{
	Mat mat = Mat(height, width, CV_8UC3, imageByte, stride).clone();
	return mat;
}

// byte[] -> CvMat
CvMat ByteToCvMat(BYTE* imageByte, int width, int height, int stride) {
	try
	{
		Mat a = ByteToMat(imageByte, width, height, stride);
		//CvMat* b;
		CvMat temp = a; //转化为CvMat类型，而不是复制数据  
		cvCopy(&a.data, &temp.data); //真正复制数据  
		//return *b;
		return temp;
	}
	catch (const exception & ex)
	{
		log(ex.what());
		return cvMat(height, width, CV_8UC1, imageByte);
	}
}


// 注册日志回调函数
extern "C" __declspec(dllexport) void SetDisplayLog(LogCallBack DisplayLog)
{
	logger = DisplayLog;
}

// 设置人脸模型目录
extern "C" __declspec(dllexport) void SetModelDirectory(const char* path)
{
	MODEL_DIR = path;
	log(MODEL_DIR.c_str());
}

// 初始化人脸检测, 人脸对齐, 人脸识别  初始化成功返回1
extern "C" __declspec(dllexport) bool Init()
{
	string tDetectModelPath = MODEL_DIR + "seeta_fd_frontal_v1.0.bin";
	string  tAlignModelPath = MODEL_DIR + "seeta_fa_v1.1.bin";
	string tIdentificationModelPath = MODEL_DIR + "seeta_fr_v1.0.bin";

	log(tDetectModelPath.c_str());
	log(tAlignModelPath.c_str());
	log(tIdentificationModelPath.c_str());

	FILE* fp = fopen(tDetectModelPath.c_str(), "r");
	if (!fp) { // 人脸检测模型不存在
		log("人脸检测模型不存在");
		return false;
	}
	fclose(fp);

	fp = fopen(tAlignModelPath.c_str(), "r");
	if (!fp) { // 人脸对齐模型不存在
		log("人脸对齐模型不存在");
		return false;
	}
	fclose(fp);

	fp = fopen(tIdentificationModelPath.c_str(), "r");
	if (!fp) { // 人脸识别模型不存在
		log("人脸识别模型不存在");
		return false;
	}
	fclose(fp);

	detector.initWithModel(tDetectModelPath.c_str());
	alignment.initWithModel(tAlignModelPath.c_str());
	identification.initWithModel(tIdentificationModelPath.c_str());
	return true;
}

// 人脸检测,返回第一张人脸
extern "C" __declspec(dllexport) int DetectFace(const char* picPath, FaceInfo * face)
{
	//灰度图
	IplImage* img_grayscale = NULL;
	img_grayscale = cvLoadImage(picPath, 0);
	if (img_grayscale == NULL)
	{
		return 0;
	}
	//彩色图
	IplImage* img_color = cvLoadImage(picPath, 1);

	int im_width = img_grayscale->width;
	int im_height = img_grayscale->height;
	unsigned char* data = new unsigned char[im_width * im_height];
	unsigned char* data_ptr = data;
	unsigned char* image_data_ptr = (unsigned char*)img_grayscale->imageData;
	int h = 0;
	for (h = 0; h < im_height; h++) {
		memcpy(data_ptr, image_data_ptr, im_width);
		data_ptr += im_width;
		image_data_ptr += img_grayscale->widthStep;
	}

	ImageData image_data;
	image_data.data = data;
	image_data.width = im_width;
	image_data.height = im_height;
	image_data.num_channels = 1;

	// Detect faces
	vector< FaceInfo> faces = detector.Detect(image_data);
	size_t faceCount = faces.size();

	int32_t face_num = static_cast<int32_t>(faceCount);
	if (face_num == 0)
	{
		delete[]data;
		cvReleaseImage(&img_grayscale);
		cvReleaseImage(&img_color);
		return 0;
	}

	if (!faces.empty()) {
		face->bbox.x = faces[0].bbox.x;
		face->bbox.y = faces[0].bbox.y;
		face->bbox.height = faces[0].bbox.height;
		face->bbox.width = faces[0].bbox.width;
		face->pitch = faces[0].pitch;
		face->roll = faces[0].roll;
		face->score = faces[0].score;
		face->yaw = faces[0].yaw;

		return faceCount;
	}

	return 0;
}

// 人脸检测,返回第一张人脸,通过内存数据
extern "C" __declspec(dllexport) int DetectFaceByte(BYTE * imageByte, int width, int height, int stride, FaceInfo * face)
{

	Mat mat_gray;//彩色图像转化成灰度图  
	cvtColor(ByteToMat(imageByte, width, height, stride), mat_gray, COLOR_BGR2GRAY);

	clock_t start, end = 0;

	ImageData image_data(mat_gray.cols, mat_gray.rows, mat_gray.channels());
	image_data.data = mat_gray.data;

	// Detect faces
	vector< FaceInfo> faces = detector.Detect(image_data);
	size_t faceCount = faces.size();

	int32_t face_num = static_cast<int32_t>(faceCount);
	if (face_num == 0)
	{
		return 0;
	}

	if (!faces.empty()) {
		face->bbox.x = faces[0].bbox.x;
		face->bbox.y = faces[0].bbox.y;
		face->bbox.height = faces[0].bbox.height;
		face->bbox.width = faces[0].bbox.width;
		face->pitch = faces[0].pitch;
		face->roll = faces[0].roll;
		face->score = faces[0].score;
		face->yaw = faces[0].yaw;

		return faceCount;
	}

	return 0;
}

// 人脸检测,返回多张人脸 JSON 数据
extern "C" __declspec(dllexport) int DetectFaces(const char* picPath, char* json)
{
	//灰度图
	IplImage* img_grayscale = NULL;
	img_grayscale = cvLoadImage(picPath, 0);
	if (img_grayscale == NULL)
	{
		return 0;
	}
	//彩色图
	IplImage* img_color = cvLoadImage(picPath, 1);

	int im_width = img_grayscale->width;
	int im_height = img_grayscale->height;
	unsigned char* data = new unsigned char[im_width * im_height];
	unsigned char* data_ptr = data;
	unsigned char* image_data_ptr = (unsigned char*)img_grayscale->imageData;
	int h = 0;
	for (h = 0; h < im_height; h++) {
		memcpy(data_ptr, image_data_ptr, im_width);
		data_ptr += im_width;
		image_data_ptr += img_grayscale->widthStep;
	}

	ImageData image_data;
	image_data.data = data;
	image_data.width = im_width;
	image_data.height = im_height;
	image_data.num_channels = 1;

	// Detect faces
	vector< FaceInfo> faces = detector.Detect(image_data);
	size_t faceCount = faces.size();
	size_t count = 0;
	string result = "[";

	int32_t face_num = static_cast<int32_t>(faceCount);
	if (face_num == 0)
	{
		delete[]data;
		cvReleaseImage(&img_grayscale);
		cvReleaseImage(&img_color);
		return 0;
	}

	if (!faces.empty()) {
		for (auto iter = faces.begin(); iter != faces.end(); iter++, count++)
		{
			result += "{";
			result += "\"bbox\":{";
			result += ("\"x\":" + int2string((*iter).bbox.x) + ",");
			result += ("\"y\":" + int2string((*iter).bbox.y) + ",");
			result += ("\"height\":" + int2string((*iter).bbox.height) + ",");
			result += ("\"width\":" + int2string((*iter).bbox.width) + "}");
			result += (",\"pitch\":" + int2string((*iter).pitch));
			result += (",\"roll\":" + int2string((*iter).roll));
			result += (",\"score\":" + int2string((*iter).score));
			result += (",\"yaw\":" + int2string((*iter).yaw));
			result += "}";

			if (count < faceCount - 1)
				result += ",";

		}

	}
	result += "]";
	log(result.c_str());

	strcpy(json, result.c_str());

	return faceCount;
}

// 人脸检测,返回多张人脸 JSON 数据,通过内存数据
extern "C" __declspec(dllexport) int DetectFacesByte(BYTE * imageByte, int width, int height, int stride, char* json)
{

	Mat mat_gray;//彩色图像转化成灰度图  
	cvtColor(ByteToMat(imageByte, width, height, stride), mat_gray, COLOR_BGR2GRAY);

	clock_t start, end = 0;

	ImageData image_data(mat_gray.cols, mat_gray.rows, mat_gray.channels());
	image_data.data = mat_gray.data;

	// Detect faces
	vector< FaceInfo> faces = detector.Detect(image_data);
	size_t faceCount = faces.size();
	size_t count = 0;
	string result = "[";

	int32_t face_num = static_cast<int32_t>(faceCount);
	if (face_num == 0)
	{
		return 0;
	}

	if (!faces.empty()) {
		for (auto iter = faces.begin(); iter != faces.end(); iter++, count++)
		{
			result += "{";
			result += "\"bbox\":{";
			result += ("\"x\":" + int2string((*iter).bbox.x) + ",");
			result += ("\"y\":" + int2string((*iter).bbox.y) + ",");
			result += ("\"height\":" + int2string((*iter).bbox.height) + ",");
			result += ("\"width\":" + int2string((*iter).bbox.width) + "}");
			result += (",\"pitch\":" + int2string((*iter).pitch));
			result += (",\"roll\":" + int2string((*iter).roll));
			result += (",\"score\":" + int2string((*iter).score));
			result += (",\"yaw\":" + int2string((*iter).yaw));
			result += "}";

			if (count < faceCount - 1)
				result += ",";

		}

	}
	result += "]";
	log(result.c_str());

	strcpy(json, result.c_str());

	return faceCount;
}

// 人脸对齐
extern "C" __declspec(dllexport) int Alignment(const char* picPath, char* json)
{
	clock_t start, end = 0;

	//灰度图
	IplImage* img_grayscale = NULL;
	img_grayscale = cvLoadImage(picPath, 0);
	if (img_grayscale == NULL)
	{
		return 0;
	}
	//彩色图
	IplImage* img_color = cvLoadImage(picPath, 1);

	int im_width = img_grayscale->width;
	int im_height = img_grayscale->height;
	unsigned char* data = new unsigned char[im_width * im_height];
	unsigned char* data_ptr = data;
	unsigned char* image_data_ptr = (unsigned char*)img_grayscale->imageData;
	int h = 0;
	for (h = 0; h < im_height; h++) {
		memcpy(data_ptr, image_data_ptr, im_width);
		data_ptr += im_width;
		image_data_ptr += img_grayscale->widthStep;
	}

	ImageData image_data;
	image_data.data = data;
	image_data.width = im_width;
	image_data.height = im_height;
	image_data.num_channels = 1;

	start = clock();
	// Detect faces
	vector< FaceInfo> faces = detector.Detect(image_data);
	end = clock() - start;
	int haoshi = end;
	log(("检测人脸耗时:" + int2string(haoshi)).c_str());

	size_t faceCount = faces.size();

	int32_t face_num = static_cast<int32_t>(faceCount);
	if (face_num == 0)
	{
		delete[]data;
		cvReleaseImage(&img_grayscale);
		cvReleaseImage(&img_color);
		return 0;
	}

	size_t count = 0;
	string result = "[";

	if (!faces.empty()) {
		start = clock();
		for (auto iter = faces.begin(); iter != faces.end(); iter++, count++)
		{
			// 检测五官坐标
			FacialLandmark points[5];
			alignment.PointDetectLandmarks(image_data, (*iter), points);

			result += "{";
			result += "\"face\":{";
			result += "\"bbox\":{";
			result += ("\"x\":" + int2string((*iter).bbox.x) + ",");
			result += ("\"y\":" + int2string((*iter).bbox.y) + ",");
			result += ("\"height\":" + int2string((*iter).bbox.height) + ",");
			result += ("\"width\":" + int2string((*iter).bbox.width) + "}");
			result += (",\"pitch\":" + int2string((*iter).pitch));
			result += (",\"roll\":" + int2string((*iter).roll));
			result += (",\"score\":" + int2string((*iter).score));
			result += (",\"yaw\":" + int2string((*iter).yaw));
			result += "},";
			result += "\"landmark\":[";
			for (int32_t i = 0; i < 5; ++i) {
				result += ("{\"x\":" + int2string(points[i].x) + ",");
				result += ("\"y\":" + int2string(points[i].y) + "}");
				if (i < 4) result += ",";
			}
			result += "]";
			result += "}";

			if (count < faceCount - 1)
				result += ",";

		}
		end = clock() - start;
		haoshi = end;
		log(("检测五官+序列化JSON 耗时:" + int2string(haoshi)).c_str());
	}
	result += "]";
	log(result.c_str());

	strcpy(json, result.c_str());

	return faceCount;
}

// 人脸对齐,通过内存数据
extern "C" __declspec(dllexport) int AlignmentByte(BYTE * imageByte, int width, int height, int stride, char* json)
{
	Mat mat_gray;//彩色图像转化成灰度图  
	cvtColor(ByteToMat(imageByte, width, height, stride), mat_gray, COLOR_BGR2GRAY);

	clock_t start, end = 0;

	ImageData image_data(mat_gray.cols, mat_gray.rows, mat_gray.channels());
	image_data.data = mat_gray.data;

	start = clock();
	// Detect faces
	vector< FaceInfo> faces = detector.Detect(image_data);
	end = clock() - start;
	int haoshi = end;
	log(("检测人脸耗时:" + int2string(haoshi)).c_str());

	size_t faceCount = faces.size();

	int32_t face_num = static_cast<int32_t>(faceCount);
	log(("人脸数:" + int2string(face_num)).c_str());
	if (face_num == 0)
	{
		return 0;
	}

	size_t count = 0;
	string result = "[";

	if (!faces.empty()) {
		start = clock();
		for (auto iter = faces.begin(); iter != faces.end(); iter++, count++)
		{
			// 检测五官坐标
			FacialLandmark points[5];
			alignment.PointDetectLandmarks(image_data, (*iter), points);

			result += "{";
			result += "\"face\":{";
			result += "\"bbox\":{";
			result += ("\"x\":" + int2string((*iter).bbox.x) + ",");
			result += ("\"y\":" + int2string((*iter).bbox.y) + ",");
			result += ("\"height\":" + int2string((*iter).bbox.height) + ",");
			result += ("\"width\":" + int2string((*iter).bbox.width) + "}");
			result += (",\"pitch\":" + int2string((*iter).pitch));
			result += (",\"roll\":" + int2string((*iter).roll));
			result += (",\"score\":" + int2string((*iter).score));
			result += (",\"yaw\":" + int2string((*iter).yaw));
			result += "},";
			result += "\"landmark\":[";
			for (int32_t i = 0; i < 5; ++i) {
				result += ("{\"x\":" + int2string(points[i].x) + ",");
				result += ("\"y\":" + int2string(points[i].y) + "}");
				if (i < 4) result += ",";
			}
			result += "]";
			result += "}";

			if (count < faceCount - 1)
				result += ",";

		}
		end = clock() - start;
		haoshi = end;
		log(("检测五官+序列化JSON 耗时:" + int2string(haoshi)).c_str());
	}
	result += "]";
	log(result.c_str());

	strcpy(json, result.c_str());

	return faceCount;
}

// 提取人脸特征
extern "C" __declspec(dllexport) bool ExtractFeature(const char* picPath, AlignmentResult * alignResult, float* feat)
{
	FaceIdentification& face_recognizer = identification;
	clock_t start, count = 0;

	//int feat_size = face_recognizer.feature_size();
	Mat src_img_color = imread(picPath, 1);
	// Mat src_img_color = cvLoadImage(picPath, 1);
	if (src_img_color.data == nullptr) {
		log("Load image error!");
		return false;
	}

	ImageData src_img_data_color(src_img_color.cols, src_img_color.rows, src_img_color.channels());
	src_img_data_color.data = src_img_color.data;

	log((int2string(alignResult->landmark[0].x) + "," + int2string(alignResult->landmark[0].y)).c_str());
	//提取特征
	start = clock();
	face_recognizer.ExtractFeatureWithCrop(src_img_data_color, alignResult->landmark, feat);
	count += clock() - start;
	int haoshi = count;
	log(("提取特征耗时:" + int2string(haoshi)).c_str());
	return true;
}

// 提取人脸特征
extern "C" __declspec(dllexport) bool ExtracFeatureByte(BYTE * imageByte, int width, int height, int stride, AlignmentResult * alignmentResult, float* feat)
{
	FaceIdentification& face_recognizer = identification;
	clock_t start, count = 0;

	Mat src_img_color = ByteToMat(imageByte, width, height, stride);
	if (src_img_color.data == nullptr) {
		log("Load image error!");
		return false;
	}

	ImageData src_img_data_color(src_img_color.cols, src_img_color.rows, src_img_color.channels());
	src_img_data_color.data = src_img_color.data;

	log((int2string(alignmentResult->landmark[0].x) + "," + int2string(alignmentResult->landmark[0].y)).c_str());
	//提取特征
	start = clock();
	face_recognizer.ExtractFeatureWithCrop(src_img_data_color, alignmentResult->landmark, feat);
	count += clock() - start;
	int haoshi = count;
	log(("提取特征耗时:" + int2string(haoshi)).c_str());
	return true;
}

// 计算人脸相似度(人脸特征1, 人脸特征2)
extern "C" __declspec(dllexport) double CalcSimilarity(float* feat1, float* feta2)
{
	double tSim = identification.CalcSimilarity(feat1, feta2);
	//保留两位小数
	tSim = int(100 * tSim) / 100.0;
	return tSim;
}
