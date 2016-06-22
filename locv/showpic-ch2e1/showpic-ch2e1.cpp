// showpic-ch2e1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

/* License:
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
*/


#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include<stdlib.h>
#include<crtdbg.h>
#else //_DEBUG
#endif //_DEBUG

#include <stdio.h>
#include <iostream>
#include <fstream>
#include "cv.h"
#include "highgui.h"

int main_1(int argc, char* argv[])
{
	if (argc < 3) {
		printf("Usage: example-ch2 1 <picture file path>\n");
		return (-1);
	}

	IplImage* img = cvLoadImage(argv[2]);
	cvNamedWindow("Example1", CV_WINDOW_AUTOSIZE);
	cvShowImage("Example1", img);
	cvWaitKey(0);
	cvReleaseImage(&img);
	cvDestroyWindow("Example1");
	
	return (0);
}



// Convert a video to grayscale
// argv[1]: input video file
// argv[2]: name of new output file
//

//#define NOWRITE 1;   //Turn this on (removed the first comment out "//" if you can't write on linux

int main_2(int argc, char* argv[]) {
	if (argc < 4) {
		printf("Usage: example-ch2 2 <video file path> <output filepath>\n");
		return (-1);
	}

	cvNamedWindow("Example2_10", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Log_Polar", CV_WINDOW_AUTOSIZE);
	CvCapture* capture = cvCreateFileCapture(argv[2]);
	if (!capture){
		return -1;
	}
	IplImage* bgr_frame;
	double fps = cvGetCaptureProperty(
		capture,
		CV_CAP_PROP_FPS
		);
	printf("fps=%d\n", (int)fps);

	CvSize size = cvSize(
		(int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH),
		(int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT)
		);

	printf("frame (w, h) = (%d, %d)\n", size.width, size.height);
#ifndef NOWRITE
	CvVideoWriter* writer = cvCreateVideoWriter(  // On linux Will only work if you've installed ffmpeg development files correctly, 
		argv[3],                               // otherwise segmentation fault.  Windows probably better.
		CV_FOURCC('D', 'X', '5', '0'),
		fps,
		size
		);
#endif
	IplImage* logpolar_frame = cvCreateImage(
		size,
		IPL_DEPTH_8U,
		3
		);

	IplImage* gray_frame = cvCreateImage(
		size,
		IPL_DEPTH_8U,
		1
		);

	while ((bgr_frame = cvQueryFrame(capture)) != NULL) {
		cvShowImage("Example2_10", bgr_frame);
		cvConvertImage(   //We never make use of this gray image
			bgr_frame,
			gray_frame,
			CV_RGB2GRAY
			);
		cvLogPolar(bgr_frame, logpolar_frame,  //This is just a fun conversion the mimic's the human visual system
			cvPoint2D32f(bgr_frame->width / 2,
			bgr_frame->height / 2),
			40,
			CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);
		cvShowImage("Log_Polar", logpolar_frame);
		//Sigh, on linux, depending on your ffmpeg, this often won't work ...
#ifndef NOWRITE
		cvWriteToAVI(writer, logpolar_frame);
#endif
		char c = cvWaitKey(10);
		if (c == 27) break;
	}
#ifndef NOWRITE
	cvReleaseVideoWriter(&writer);
#endif
	cvReleaseImage(&gray_frame);
	cvReleaseImage(&logpolar_frame);
	cvReleaseCapture(&capture);

	return (0);
}


int main_3(int argc, char** argv) {
	if (argc < 3) {
		printf("Usage: example-ch2 1 <video path>\n");
		return (-1);
	}

	cvNamedWindow("Example2", CV_WINDOW_AUTOSIZE);
	//CvCapture* capture = cvCaptureFromAVI( argv[1] ); // either one will work
	CvCapture* capture = cvCreateFileCapture(argv[2]);
	IplImage* frame;
	while (1) {
		frame = cvQueryFrame(capture);
		if (!frame) break;
		cvShowImage("Example2", frame);
		char c = cvWaitKey(33);
		if (c == 27) break;
	}
	cvReleaseCapture(&capture);
	cvDestroyWindow("Example2");

	return 0;
}


/*
OK, you caught us.  Video playback under linux is still just bad.  Part of this is due to FFMPEG, part of this
is due to lack of standards in video files.  But the position slider here will often not work. We tried to at least
find number of frames using the "getAVIFrames" hack below.  Terrible.  But, this file shows something of how to
put a slider up and play with it.  Sorry.
*/


using namespace std;

int        g_slider_position = 0;
CvCapture* g_capture = NULL;

void onTrackbarSlide(int pos) {
	cvSetCaptureProperty(
		g_capture,
		CV_CAP_PROP_POS_FRAMES,
		pos
		);
}

//Hack because sometimes the number of frames in a video is not accessible. 
//Probably delete this on Widows
int getAVIFrames(char * fname) {
	char tempSize[4];
	// Trying to open the video file
	ifstream  videoFile(fname, ios::in | ios::binary);
	// Checking the availablity of the file
	if (!videoFile) {
		cout << "Couldn¡¯t open the input file " << fname << endl;
		exit(1);
	}
	// get the number of frames
	videoFile.seekg(0x30, ios::beg);
	videoFile.read(tempSize, 4);
	int frames = (unsigned char)tempSize[0] + 0x100 * (unsigned char)tempSize[1] + 0x10000 * (unsigned char)tempSize[2] + 0x1000000 * (unsigned char)tempSize[3];
	videoFile.close();
	return frames;
}


int main_4(int argc, char** argv) {
	if (argc < 3) {
		printf("Usage: example-ch2 1 <video path>\n");
		return (-1);
	}

	cvNamedWindow("Example2_3", CV_WINDOW_AUTOSIZE);
	g_capture = cvCreateFileCapture(argv[2]);
	IplImage *foo = cvQueryFrame(g_capture);


	int frames = (int)cvGetCaptureProperty(
		g_capture,
		CV_CAP_PROP_FRAME_COUNT
		);

	int tmpw = (int)cvGetCaptureProperty(
		g_capture,
		CV_CAP_PROP_FRAME_WIDTH
		);

	int tmph = (int)cvGetCaptureProperty(
		g_capture,
		CV_CAP_PROP_FRAME_HEIGHT
		);

	printf("opencv frames %d w %d h %d\n", frames, tmpw, tmph);

	frames = getAVIFrames(argv[2]); //This is a hack because on linux, getting number of frames often doesn't work

	printf("hacked frames %d w %d h %d\n", frames, tmpw, tmph);

	cvCreateTrackbar(
		"Position",
		"Example2_3",
		&g_slider_position,
		frames,
		onTrackbarSlide
		);
	IplImage* frame;
	frames = 0;
	while (1) {
		frame = cvQueryFrame(g_capture);
		if (!frame) break;
		//      int frames = cvGetCaptureProperty( g_capture, CV_CAP_PROP_POS_FRAMES);//This should work, sometimes it does not on linux
		frames++; //My cheat
		printf("\nFrame number=%d\n", frames);
		cvSetTrackbarPos("Position", "Example2_3", frames);
		cvShowImage("Example2_3", frame);
		char c = (char)cvWaitKey(10);
		if (c == 27) break;
	}
	cvReleaseCapture(&g_capture);
	cvDestroyWindow("Example2_3");
	return(0);
}



void example2_4(IplImage* image)
{
	// Create some windows to show the input
	// and output images in.
	//
	cvNamedWindow("Example2_4-in", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Example2_4-out", CV_WINDOW_AUTOSIZE);

	// Create a window to show our input image
	//
	cvShowImage("Example2_4-in", image);

	// Create an image to hold the smoothed output
	//
	IplImage* out = cvCreateImage(
		cvGetSize(image),
		IPL_DEPTH_8U,
		3
		);

	// Do the smoothing
	//
	cvSmooth(image, out, CV_GAUSSIAN, 5, 5);
	cvSmooth(out, out, CV_GAUSSIAN, 5, 5);

	// Show the smoothed image in the output window
	//
	cvShowImage("Example2_4-out", out);

	// Be tidy
	//
	cvReleaseImage(&out);

	// Wait for the user to hit a key, then clean up the windows
	//
	cvWaitKey(0);
	cvDestroyWindow("Example2_4-in");
	cvDestroyWindow("Example2_4-out");

}

int main_5(int argc, char** argv)
{
	if (argc < 3) {
		printf("Usage: example-ch2 5 <picture filepath>\n");
		return (-1);
	}

	IplImage* img = cvLoadImage(argv[2]);
	cvNamedWindow("Example1", CV_WINDOW_AUTOSIZE);
	cvShowImage("Example1", img);
	example2_4(img);
	//  cvWaitKey(0);
	cvReleaseImage(&img);
	cvDestroyWindow("Example1");

	return (0);
}


IplImage* doPyrDown(
	IplImage* in,
	int       filter = IPL_GAUSSIAN_5x5)
{

	// Best to make sure input image is divisible by two.
	//
	assert(in->width % 2 == 0 && in->height % 2 == 0);

	IplImage* out = cvCreateImage(
		cvSize(in->width / 2, in->height / 2),
		in->depth,
		in->nChannels
		);
	cvPyrDown(in, out);
	return(out);
};

int main_6(int argc, char** argv)
{
	if (argc < 3) {
		printf("Usage: example-ch2 6 <picture filepath>\n");
		return (-1);
	}

	IplImage* img = cvLoadImage(argv[2]);
	IplImage* img2 = cvCreateImage(cvSize(img->width / 2, img->height / 2), img->depth, img->nChannels);
	cvNamedWindow("Example1", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Example2", CV_WINDOW_AUTOSIZE);
	cvShowImage("Example1", img);
	img2 = doPyrDown(img);
	cvShowImage("Example2", img2);
	cvWaitKey(0);
	cvReleaseImage(&img);
	cvReleaseImage(&img2);
	cvDestroyWindow("Example1");
	cvDestroyWindow("Example2");

	return 0;
}


IplImage* doCanny(
	IplImage* in,
	double    lowThresh,
	double    highThresh,
	double    aperture)
{
	if (in->nChannels != 1)
		return(0); // Canny only handles gray scale images
	IplImage* out = cvCreateImage(
		cvGetSize(in),
		in->depth, //IPL_DEPTH_8U,    
		1);
	cvCanny(in, out, lowThresh, highThresh, aperture);
	return(out);
};

int main_7(int argc, char** argv)
{
	if (argc < 3) {
		printf("Usage: example-ch2 7 <picture filepath>\n");
		return (-1);
	}

	IplImage* img_rgb = cvLoadImage(argv[2]);
	IplImage* img_gry = cvCreateImage(cvSize(img_rgb->width, img_rgb->height), img_rgb->depth, 1);
	cvCvtColor(img_rgb, img_gry, CV_BGR2GRAY);
	cvNamedWindow("Example Gray", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Example Canny", CV_WINDOW_AUTOSIZE);
	cvShowImage("Example Gray", img_gry);
	IplImage* img_cny = doCanny(img_gry, 10, 100, 3);
	cvShowImage("Example Canny", img_cny);
	cvWaitKey(0);
	cvReleaseImage(&img_rgb);
	cvReleaseImage(&img_gry);
	cvReleaseImage(&img_cny);
	cvDestroyWindow("Example Gray");
	cvDestroyWindow("Example Canny");

	return 0;
}


IplImage* doCanny_8(
	IplImage* in,
	double    lowThresh,
	double    highThresh,
	double    aperture)
{
	IplImage* out = cvCreateImage(
		cvGetSize(in),
		in->depth, //IPL_DEPTH_8U,    
		1);
	cvCanny(in, out, lowThresh, highThresh, aperture);
	return(out);
};

IplImage* doPyrDown_8(
	IplImage* in,
	int       filter = IPL_GAUSSIAN_5x5)
{

	// Best to make sure input image is divisible by two.
	//
	assert(in->width % 2 == 0 && in->height % 2 == 0);

	IplImage* out = cvCreateImage(
		cvSize(in->width / 2, in->height / 2),
		in->depth,
		in->nChannels
		);
	cvPyrDown(in, out);
	return(out);
};

int main_8(int argc, char** argv)
{
	if (argc < 3) {
		printf("Usage: example-ch2 8 <picture filepath>\n");
		return (-1);
	}

	IplImage* img_rgb = cvLoadImage(argv[2]);
	IplImage* img_gry = cvCreateImage(cvSize(img_rgb->width, img_rgb->height), img_rgb->depth, 1);
	cvCvtColor(img_rgb, img_gry, CV_BGR2GRAY);
	IplImage* img_pyr = doPyrDown_8(img_gry, IPL_GAUSSIAN_5x5);
	IplImage* img_pyr2 = doPyrDown_8(img_pyr, IPL_GAUSSIAN_5x5);
	IplImage* img_cny = doCanny_8(img_pyr2, 10, 100, 3);

	cvNamedWindow("Example Gray", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Example Pyr", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Example Canny", CV_WINDOW_AUTOSIZE);
	cvShowImage("Example Gray", img_gry);
	cvShowImage("Example Pyr", img_pyr2);
	cvShowImage("Example Canny", img_cny);
	cvWaitKey(0);
	cvReleaseImage(&img_rgb);
	cvReleaseImage(&img_gry);
	cvReleaseImage(&img_pyr);
	cvReleaseImage(&img_pyr2);
	cvReleaseImage(&img_cny);
	cvDestroyWindow("Example Gray");
	cvDestroyWindow("Example Pyr");
	cvDestroyWindow("Example Canny");

	return 0;
}



IplImage* doCanny_9(
	IplImage* in,
	double    lowThresh,
	double    highThresh,
	double    aperture)
{
	IplImage* out = cvCreateImage(
		cvGetSize(in),
		in->depth, //IPL_DEPTH_8U,    
		1);
	cvCanny(in, out, lowThresh, highThresh, aperture);
	return(out);
};

IplImage* doPyrDown_9(
	IplImage* in,
	int       filter = IPL_GAUSSIAN_5x5)
{

	// Best to make sure input image is divisible by two.
	//
	assert(in->width % 2 == 0 && in->height % 2 == 0);

	IplImage* out = cvCreateImage(
		cvSize(in->width / 2, in->height / 2),
		in->depth,
		in->nChannels
		);
	cvPyrDown(in, out);
	return(out);
};

int main_9(int argc, char** argv)
{
	if (argc < 3) {
		printf("Usage: example-ch2 9 <picture filepath>\n");
		return (-1);
	}

	cvNamedWindow("Example Gray", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Example Pyr", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Example Canny", CV_WINDOW_AUTOSIZE);
	IplImage* img_rgb = cvLoadImage(argv[2]);
	IplImage* out;

	out = cvCreateImage(cvSize(img_rgb->width, img_rgb->height), img_rgb->depth, 1);
	cvCvtColor(img_rgb, out, CV_BGR2GRAY);
	cvShowImage("Example Gray", out);
	out = doPyrDown_9(out);
	out = doPyrDown_9(out);
	cvShowImage("Example Pyr", out);
	out = doCanny_9(out, 10, 100, 3);
	cvShowImage("Example Canny", out);

	cvWaitKey(0);
	cvReleaseImage(&out);
	cvDestroyWindow("Example Gray");
	cvDestroyWindow("Example Pyr");
	cvDestroyWindow("Example Canny");

	return 0;
}


int main_10(int argc, char** argv) {
	cvNamedWindow("Example2_9", CV_WINDOW_AUTOSIZE);
	CvCapture* capture;
	if (argc == 1) {
		capture = cvCreateCameraCapture(0);
	}
	else {
		capture = cvCreateFileCapture(argv[1]);
	}
	assert(capture != NULL);

	IplImage* frame;
	while (1) {
		frame = cvQueryFrame(capture);
		if (!frame) break;
		cvShowImage("Example2_9", frame);
		char c = cvWaitKey(10);
		if (c == 27) break;
	}
	cvReleaseCapture(&capture);
	cvDestroyWindow("Example2_9");

	return 0;
}


int main_11(int argc, char* argv[]) {
	cvNamedWindow("Example2_10", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Log_Polar", CV_WINDOW_AUTOSIZE);
	CvCapture* capture = cvCreateFileCapture(argv[1]);
	if (!capture){
		return -1;
	}
	IplImage* bgr_frame;
	double fps = cvGetCaptureProperty(
		capture,
		CV_CAP_PROP_FPS
		);
	printf("fps=%d\n", (int)fps);

	CvSize size = cvSize(
		(int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH),
		(int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT)
		);

	printf("frame (w, h) = (%d, %d)\n", size.width, size.height);
#ifndef NOWRITE
	CvVideoWriter* writer = cvCreateVideoWriter(  // On linux Will only work if you've installed ffmpeg development files correctly, 
		argv[2],                               // otherwise segmentation fault.  Windows probably better.
		CV_FOURCC('D', 'X', '5', '0'),
		fps,
		size
		);
#endif
	IplImage* logpolar_frame = cvCreateImage(
		size,
		IPL_DEPTH_8U,
		3
		);

	IplImage* gray_frame = cvCreateImage(
		size,
		IPL_DEPTH_8U,
		1
		);

	while ((bgr_frame = cvQueryFrame(capture)) != NULL) {
		cvShowImage("Example2_10", bgr_frame);
		cvConvertImage(   //We never make use of this gray image
			bgr_frame,
			gray_frame,
			CV_RGB2GRAY
			);
		cvLogPolar(bgr_frame, logpolar_frame,  //This is just a fun conversion the mimic's the human visual system
			cvPoint2D32f(bgr_frame->width / 2,
			bgr_frame->height / 2),
			40,
			CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);
		cvShowImage("Log_Polar", logpolar_frame);
		//Sigh, on linux, depending on your ffmpeg, this often won't work ...
#ifndef NOWRITE
		cvWriteToAVI(writer, logpolar_frame);
#endif
		char c = cvWaitKey(10);
		if (c == 27) break;
	}
#ifndef NOWRITE
	cvReleaseVideoWriter(&writer);
#endif
	cvReleaseImage(&gray_frame);
	cvReleaseImage(&logpolar_frame);
	cvReleaseCapture(&capture);

	return 0;
}


int main(int argc, char* argv[]) {
#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif //_DEBUG

	if (argc < 2) {
		printf("Usage: example-ch2 <example id> <argv1> <argv2> ....\n");
		return (-1);
	}

	int result = 0;
	switch (atoi(argv[1])) {
	case 1:
		result = main_1(argc, argv);
		break;
	case 2:
		result = main_2(argc, argv);
		break;
	case 3:
		result = main_3(argc, argv);
		break;
	case 4:
		result = main_4(argc, argv);
		break;
	case 5:
		result = main_5(argc, argv);
		break;
	case 6:
		result = main_6(argc, argv);
		break;
	case 7:
		result = main_7(argc, argv);
		break;
	case 8:
		result = main_8(argc, argv);
		break;
	case 9:
		result = main_9(argc, argv);
		break;
	}

	return result;
}

