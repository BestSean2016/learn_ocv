// ch3ex3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main_1()
{
	// Create an OpenCV Matrix containing some fixed data.
	//
	float vals[] = { 0.866025, -0.500000, 0.500000, 0.866025 };

	CvMat rotmat;

	cvInitMatHeader(
		&rotmat,
		2,
		2,
		CV_32FC1,
		vals
		);
	printf("Ex 3_3 matrix initialized\n");

	return 0;
}


int main_2()
{
	CvMat* mat = cvCreateMat(5, 5, CV_32FC1);
	float element_3_2 = CV_MAT_ELEM(*mat, float, 3, 2);
	printf("Exercise 3_4, matrix created and accessed [3,2]=%f\n", element_3_2);

	return (0);
}

int main_3()
{
	CvMat* mat = cvCreateMat(5, 5, CV_32FC1);
	float element_3_2 = 7.7;
	*((float*)CV_MAT_ELEM_PTR(*mat, 3, 2)) = element_3_2;

	// below from example ch3_ex3_8.txt
	cvmSet(mat, 2, 2, 0.5000);
	cvSetReal2D(mat, 3, 3, 0.3300);


	printf("Exercise 3_5, matrix created and accessed [3,2]=%f, [2,2]=%f, [3,3]=%f\n", CV_MAT_ELEM(*mat, float, 3, 2), CV_MAT_ELEM(*mat, float, 2, 2), CV_MAT_ELEM(*mat, float, 3, 3));

	return 0;
}



float sum(CvMat* mat) {
	float s = 0.0f;
	for (int row = 0; row<mat->height; row++) {
		float* ptr = mat->data.fl + row * mat->step / 4;
		for (int col = 0; col<mat->width; col++) {
			s += *ptr++;
		}
	}
	return(s);
};

int main_4()
{
	CvMat *mat = cvCreateMat(5, 5, CV_32FC1);
	float element_3_2 = 7.7;
	*((float*)CV_MAT_ELEM_PTR(*mat, 3, 2)) = element_3_2;
	cvmSet(mat, 4, 4, 0.5000);
	cvSetReal2D(mat, 3, 3, 0.5000);
	float s = sum(mat);
	printf("%f\n", s);
	return 0;
}


void saturate_sv(IplImage* img) {

	for (int y = 0; y<img->height; y++) {
		uchar* ptr = (uchar*)(
			img->imageData + y * img->widthStep
			);
		for (int x = 0; x<img->width; x++) {
			ptr[3 * x + 1] = 255;
			ptr[3 * x + 2] = 255;
		}
	}
}

int main_5(const char* picpath)
{
	IplImage* img = cvLoadImage(picpath);
	cvNamedWindow("Example1", CV_WINDOW_AUTOSIZE);
	saturate_sv(img);
	cvShowImage("Example1", img);
	cvWaitKey(0);
	cvReleaseImage(&img);
	cvDestroyWindow("Example1");

	return 0;
}



int main_6(const char* picpath, int x, int y, int width, int height, int add)
{
	IplImage* src;
	cvNamedWindow("Example3_12_pre", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Example3_12_post", CV_WINDOW_AUTOSIZE);
	if ((src = cvLoadImage(picpath, 1)) != 0) {
		cvShowImage("Example3_12_pre", src);
		cvSetImageROI(src, cvRect(x, y, width, height));
		cvAddS(src, cvScalar(add), src);
		cvResetImageROI(src);
		cvShowImage("Example3_12_post", src);
		cvWaitKey();
	}
	cvReleaseImage(&src);
	cvDestroyWindow("Example3_12_pre");
	cvDestroyWindow("Example3_12_post");
	return 0;
}



int main_7(const char* picpath, int x, int y, int width, int height, int add)
{
	IplImage* interest_img;
	CvRect interest_rect;
	if ((interest_img = cvLoadImage(picpath, 1)) != 0) {
		interest_rect.x = x;
		interest_rect.y = y;
		interest_rect.width = width;
		interest_rect.height = height;

		// Assuming IplImage *interest_img; and 
		//  CvRect interest_rect;
		//  Use widthStep to get a region of interest
		//
		// (Alternate method)
		//
		IplImage *sub_img = cvCreateImageHeader(
			cvSize(
			interest_rect.width,
			interest_rect.height
			),
			interest_img->depth,
			interest_img->nChannels
			);

		sub_img->origin = interest_img->origin;

		sub_img->widthStep = interest_img->widthStep;

		sub_img->imageData = interest_img->imageData +
			interest_rect.y * interest_img->widthStep +
			interest_rect.x * interest_img->nChannels;

		cvAddS(sub_img, cvScalar(add), sub_img);

		cvReleaseImageHeader(&sub_img);

		cvNamedWindow("Roi_Add", CV_WINDOW_AUTOSIZE);
		cvShowImage("Roi_Add", interest_img);
		cvWaitKey();
	}
	return 0;
}


int main_8(const char* picpath_1, const char* picpath_2, int x, int y, int width, int height, double alpha, double beta)
{
	IplImage *src1, *src2;
	if (((src1 = cvLoadImage(picpath_1, 1)) != 0) && ((src2 = cvLoadImage(picpath_2, 1)) != 0)) {
		cvSetImageROI(src1, cvRect(x, y, width, height));
		cvSetImageROI(src2, cvRect(0, 0, width, height));
		cvAddWeighted(src1, alpha, src2, beta, 0.0, src1);
		cvResetImageROI(src1);
		cvNamedWindow("Alpha_blend", 1);
		cvShowImage("Alpha_blend", src1);
		cvWaitKey();
	}
	return 0;
}

int main_9()
{
	CvMat *the_matrix_data = cvCreateMat(5, 5, CV_32FC1);
	float element_3_2 = 7.7;
	*((float*)CV_MAT_ELEM_PTR(*the_matrix_data, 3, 2)) = element_3_2;
	cvmSet(the_matrix_data, 4, 4, 0.5000);
	cvSetReal2D(the_matrix_data, 3, 3, 0.5000);

	CvMat A = cvMat(5, 5, CV_32F, the_matrix_data);

	printf("Example 3_15:\nSaving my_matrix.xml\n");
	cvSave("my_matrix.xml", &A);
	//. . .
	// to load it then in some other program use бн
	printf("Loading my_matrix.xml\n");
	CvMat* A1 = (CvMat*)cvLoad("my_matrix.xml");

	return 0;
}

int main_10()
{
	CvMat *cmatrix = cvCreateMat(5, 5, CV_32FC1);
	float element_3_2 = 7.7;
	*((float*)CV_MAT_ELEM_PTR(*cmatrix, 3, 2)) = element_3_2;
	cvmSet(cmatrix, 4, 4, 0.5000);
	cvSetReal2D(cmatrix, 3, 3, 0.5000);
	printf("Example 3_17, writing cfg.xml\n");
	CvFileStorage* fs = cvOpenFileStorage(
		"cfg.xml",
		0,
		CV_STORAGE_WRITE
		);
	cvWriteInt(fs, "frame_count", 10);
	cvStartWriteStruct(fs, "frame_size", CV_NODE_SEQ);
	cvWriteInt(fs, 0, 320);
	cvWriteInt(fs, 0, 200);
	cvEndWriteStruct(fs);
	cvWrite(fs, "color_cvt_matrix", cmatrix);
	cvReleaseFileStorage(&fs);

	return 0;
}

int main_11()
{
	printf("Example 3_19 Reading in cfg.xml\n");
	CvFileStorage* fs = cvOpenFileStorage(
		"cfg.xml",
		0,
		CV_STORAGE_READ
		);

	int frame_count = cvReadIntByName(
		fs,
		0,
		"frame_count",
		5 /* default value */
		);

	CvSeq* s = cvGetFileNodeByName(fs, 0, "frame_size")->data.seq;

	int frame_width = cvReadInt(
		(CvFileNode*)cvGetSeqElem(s, 0)
		);

	int frame_height = cvReadInt(
		(CvFileNode*)cvGetSeqElem(s, 1)
		);

	CvMat* color_cvt_matrix = (CvMat*)cvRead(
		fs,
		0
		);
	printf("frame_count=%d, frame_width=%d, frame_height=%d\n", frame_count, frame_width, frame_height);

	cvReleaseFileStorage(&fs);

	return 0;
}

int main_12()
{
	const char* libraries;
	const char* modules;
	cvGetModuleInfo(0, &libraries, &modules);
	printf("Libraries: %s\nModules: %s\n", libraries, modules);

	return 0;
}

int main()
{
	main_5("C:\\vm\\1\\033\\image023_proc.jpg");
	main_6("C:\\vm\\1\\033\\image023_proc.jpg", 500, 150, 160, 100, 100);
	main_7("C:\\vm\\1\\033\\image023_proc.jpg", 500, 150, 160, 100, 100);
	main_8("C:\\vm\\1\\033\\image023_proc.jpg", "C:\\vm\\1\\033\\image017_proc.jpg", 500, 150, 160, 100, 0.5, 0.5);
	
	main_9();
	main_10();
	main_11();
	main_12();

	return 0;
}

