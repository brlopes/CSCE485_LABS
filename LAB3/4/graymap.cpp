/*
SUBMITED
CSCE 485 - LAB#3_4
applying grayscale to frame
*/

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/contrib/contrib.hpp"

// Default resolution 1920x1080
#define HRES_COLS (1920)
#define VRES_ROWS (1080)

using namespace cv;
using namespace std;

// Buffer for highest resolution visualization possible
unsigned char framebuff1[1440*2560*3];	// 1440rows 2560 cols/row, 3ch initial input
unsigned char framebuff2[1440*2560]; 	// 1440rows 2560 cols/row, 1ch result
unsigned char framebuff3[1440*2560];	// 1440rows 2560 cols/row, 1ch buffer

int TOTALframes = 100;	//# of ppm frames extracted from ffmpeg


//mat function on input frame, only 1 color band for grayscale
Mat get_grayscale(Mat input)
{
	Mat matResult(VRES_ROWS,HRES_COLS, CV_8UC1, framebuff3);
	
	for(int i = 0; i < 1920; i++)
	{
		for(int j = 0; j < 1080; j++)
		{
		matResult.at<uchar>(j,i) = input.at<cv::Vec3b>(j,i)[1];	
		}
	}
	return matResult; 
}

	 
int main(int argc, char **argv)
{
	
	char n[10];

	Mat inMat(VRES_ROWS,HRES_COLS, CV_8UC3, framebuff1);
	Mat outMat(VRES_ROWS,HRES_COLS, CV_8UC1, framebuff2);
	

	//apply grayscale per frame
	for(int i = 0; i < TOTALframes; i++)
	{

	//input/output strings
	sprintf(n,"%d",i);
	string sframe(n);
	string input = "frame" + sframe + ".ppm";
	string output = "gray_frame" + sframe + ".pgm";

	//load frame rgb from buffer
	inMat = imread(input, CV_LOAD_IMAGE_COLOR);

	//apply grayscale on inMat
	outMat = get_grayscale(inMat);
//	cvtColor(inMat, outMat, CV_RGB2GRAY );	//both work!!


	//save frame
	imwrite(output, outMat);

	}

}
