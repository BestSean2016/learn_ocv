#include "stdafx.h"
//
//Example 5-1. Doing something with each element in the sequence of connected components returned
//             by cvPyrSegmentation(
//
/* *************** License:**************************
Oct. 3, 2008
Right to use this code in any way you want without warrenty, support or any guarentee of it working.

BOOK: It would be nice if you cited it:
Learning OpenCV: Computer Vision with the OpenCV Library
by Gary Bradski and Adrian Kaehler
Published by O'Reilly Media, October 3, 2008

AVAILABLE AT:
http://www.amazon.com/Learning-OpenCV-Computer-Vision-Library/dp/0596516134
Or: http://oreilly.com/catalog/9780596516130/
ISBN-10: 0596516134 or: ISBN-13: 978-0596516130

OTHER OPENCV SITES:
* The source code is on sourceforge at:
http://sourceforge.net/projects/opencvlibrary/
* The OpenCV wiki page (As of Oct 1, 2008 this is down for changing over servers, but should come back):
http://opencvlibrary.sourceforge.net/
* An active user group is at:
http://tech.groups.yahoo.com/group/OpenCV/
* The minutes of weekly OpenCV development meetings are at:
http://pr.willowgarage.com/wiki/OpenCV
************************************************** */

void f(
	IplImage* src,
	IplImage* dst
	) {
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* comp = NULL;

	//cvPyrSegmentation(src, dst, storage, &comp, 4, 200, 50);
	int n_comp = comp->total;

	for (int i = 0; i<n_comp; i++) {
		CvConnectedComp* cc = (CvConnectedComp*)cvGetSeqElem(comp, i);
		// do_something_with( cc );
	}
	cvReleaseMemStorage(&storage);
}

int main_1(int argc, char** argv)
{
	if (argc < 2) {
		printf("Usage ch5ex5 <image path>\n");
		return (-1);
	}

	const char* imgpath = argv[1];
	// Create a named window with a the name of the file.
	cvNamedWindow(imgpath, 1);
	// Load the image from the given file name.
	IplImage* src = cvLoadImage(imgpath);
	if (!src) { printf("Couldn't seem to Open %s, sorry\n", imgpath); return -1; }
	IplImage* dst = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	f(src, dst);

	// Show the image in the named window
	cvShowImage(imgpath, dst);

	// Idle until the user hits the "Esc" key.
	while (1) { if (cvWaitKey(100) == 27) break; }

	// Clean up and don¡¯t be piggies
	cvDestroyWindow(imgpath);
	cvReleaseImage(&src);
	cvReleaseImage(&dst);

	return (0);
}



void sum_rgb(IplImage* src, IplImage* dst) {
	// Allocate individual image planes.
	IplImage* r = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	IplImage* g = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	IplImage* b = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

	// Temporary storage.
	IplImage* s = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

	// Split image onto the color planes.
	cvSplit(src, r, g, b, NULL);

	// Add equally weighted rgb values.
	cvAddWeighted(r, 1. / 3., g, 1. / 3., 0.0, s);
	cvAddWeighted(s, 2. / 3., b, 1. / 3., 0.0, s);

	// Truncate values above 100.
	cvThreshold(s, dst, 200, 200, CV_THRESH_TRUNC);

	cvReleaseImage(&r);
	cvReleaseImage(&g);
	cvReleaseImage(&b);
	cvReleaseImage(&s);
}

int main_2(int argc, char** argv)
{
	if (argc < 2) {
		printf("Usage ch5ex5 <image path>\n");
		return (-1);
	}

	const char* imgpath = argv[1];

	// Create a named window with a the name of the file.
	cvNamedWindow(imgpath, 1);

	// Load the image from the given file name.
	IplImage* src = cvLoadImage(imgpath);
	IplImage* dst = cvCreateImage(cvGetSize(src), src->depth, 1);
	sum_rgb(src, dst);

	// Show the image in the named window
	cvShowImage(imgpath, dst);

	// Idle until the user hits the "Esc" key.
	while (1) { if ((cvWaitKey(10) & 0x7f) == 27) break; }

	// Clean up and don¡¯t be piggies
	cvDestroyWindow(imgpath);
	cvReleaseImage(&src);
	cvReleaseImage(&dst);

	return 0;
}


IplImage *Igray = 0, *It = 0, *Iat;
int main_3(int argc, char** argv)
{
	if (argc != 7){
		//15 1 1 71 15 C:\vm\1\033\Image023_proc.jpg
		printf("Usage: ch5ex5 <threshold> <threshold type> <adaptive method> <block size> <offset> <image path>\n"
			"threshold type : 0 for CV_THRESH_BINARY_INV, 1 for CV_THRESH_BINARY\n"
			"adaptive method : 0 for CV_ADAPTIVE_THRESH_GAUSSIAN_C, 1 for CV_ADAPTIVE_THRESH_MEAN_C\n");

		return -1;
	}
	//Command line
	double threshold = (double)atof(argv[1]);
	int threshold_type = atoi(argv[2]) ?
	CV_THRESH_BINARY : CV_THRESH_BINARY_INV;
	int adaptive_method = atoi(argv[3]) ?
	CV_ADAPTIVE_THRESH_MEAN_C : CV_ADAPTIVE_THRESH_GAUSSIAN_C;
	int block_size = atoi(argv[4]);
	double offset = (double)atof(argv[5]);
	//Read in gray image
	if ((Igray = cvLoadImage(argv[6], CV_LOAD_IMAGE_GRAYSCALE)) == 0){
		return     -1;
	}
	// Create the grayscale output images
	It = cvCreateImage(cvSize(Igray->width, Igray->height),
		IPL_DEPTH_8U, 1);
	Iat = cvCreateImage(cvSize(Igray->width, Igray->height),
		IPL_DEPTH_8U, 1);
	//Threshold
	cvThreshold(Igray, It, threshold, 255, threshold_type);
	cvAdaptiveThreshold(Igray, Iat, 255, adaptive_method,
		threshold_type, block_size, offset);
	//PUT UP 2 WINDOWS
	cvNamedWindow("Raw", 1);
	cvNamedWindow("Threshold", 1);
	cvNamedWindow("Adaptive Threshold", 1);
	//Show the results
	cvShowImage("Raw", Igray);
	cvShowImage("Threshold", It);
	cvShowImage("Adaptive Threshold", Iat);
	cvWaitKey(0);
	//Clean up
	cvReleaseImage(&Igray);
	cvReleaseImage(&It);
	cvReleaseImage(&Iat);
	cvDestroyWindow("Raw");
	cvDestroyWindow("Threshold");
	cvDestroyWindow("Adaptive Threshold");
	return(0);
}

int exercises_1(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage ch5ex5 <image path>\n");
		return (-1);
	}

	const char* imgpath = argv[1];
	// Create a named window with a the name of the file.
	cvNamedWindow(imgpath, 1);
	cvNamedWindow("smooth criminal 5x5");
	cvNamedWindow("smooth criminal 11x11");
	// Load the image from the given file name.
	IplImage* src = cvLoadImage(imgpath);
	if (!src) { printf("Couldn't seem to Open %s, sorry\n", imgpath); return -1; }

	IplImage* dst_5x5 = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	cvSmooth(src, dst_5x5, CV_GAUSSIAN, 5, 5);
	IplImage* dst_11x11 = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	cvSmooth(src, dst_11x11, CV_GAUSSIAN, 11, 11);

	// Show the image in the named window
	cvShowImage(imgpath, src);
	cvShowImage("smooth criminal 5x5", dst_5x5);
	cvShowImage("smooth criminal 11x11", dst_11x11);

	// Idle until the user hits the "Esc" key.
	while (1) { if (cvWaitKey(100) == 27) break; }

	// Clean up and don¡¯t be piggies
	cvDestroyWindow(imgpath);
	cvDestroyWindow("smooth criminal");
	cvReleaseImage(&src);
	cvReleaseImage(&dst_5x5);
	cvReleaseImage(&dst_11x11);

	return (0);
}


int exercises_2(void) {
	int width = 200;
	int height = 200;
	char wname[256];
	_snprintf_s(wname, 256, _TRUNCATE, "%dx%d", width, height);
	cvNamedWindow(wname, CV_WINDOW_AUTOSIZE);
	CvSize size = cvSize(width, height);
	IplImage* img = cvCreateImage(size, IPL_DEPTH_8U, 1);
	//memset(img->imageData, 0, width * height);
	for (int h = 0; h < height; ++h)
		memset(img->imageData + h * img->widthStep, 0, img->widthStep);
	img->imageData[(height / 2 - 1) * img->widthStep + width / 2] = 255;
	cvShowImage(wname, img);


	cvNamedWindow("Gaussian5x5");
	IplImage* img_5x5 = cvCreateImage(size, IPL_DEPTH_8U, 1);
	cvSmooth(img, img_5x5, 2, 5, 5);
	cvShowImage("Gaussian5x5", img_5x5);


	cvNamedWindow("Gaussian11x11");
	IplImage* img_11x11 = cvCreateImage(size, IPL_DEPTH_8U, 1);
	cvSmooth(img, img_11x11, 2, 11, 11);
	cvShowImage("Gaussian11x11", img_11x11);

	while (1) { if (cvWaitKey(100) == 27) break; }

	cvReleaseImage(&img_11x11);
	cvDestroyWindow("Gaussian11x11");
	cvReleaseImage(&img_5x5);
	cvDestroyWindow("Gaussian5x5");
	cvReleaseImage(&img);
	cvDestroyWindow(wname);
	return (0);
}

int main(int argc, char** argv)
{
	//return main_1(argc, argv);
	//return main_2(argc, argv);
	//return main_3(argc, argv);
	//return exercises_1(argc, argv);
	return exercises_2();
}
