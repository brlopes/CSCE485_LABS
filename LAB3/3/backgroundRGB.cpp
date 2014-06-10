/*
CSCE 485 - LAB#3_3
Comparing 2 frames in order to frame differenciate on 3 bands and remove background noise
anything that changes from intial-to-next frame will return a value different than zero, and stored
directly the matResult.
matResult = {P} - {Q}
note that camera shake/focus adds some noise
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
unsigned char framebuff1[1440*2560*3];// 1440rows 2560cols, 3ch 
unsigned char framebuff2[1440*2560*3];// 1440rows 2560cols, 3ch
unsigned char framebuff3[1440*2560*3];// 1440rows 2560cols, 3ch
unsigned char framebuff4[1440*2560*3];// 1440rows 2560cols, 3ch

int TOTALframes = 100;	//# of ppm frames extracted from ffmpeg



//mat function for subtracting the different frame, only changing pixels on next frame should return something other than 0 
Mat get_background(Mat initial, Mat next)
{
	Mat matResult(VRES_ROWS,HRES_COLS, CV_8UC3, framebuff4);
	
	for(int i=0; i<1920;i++)
	{
		for(int j =0; j<1080;j++)
		{

		//printf("FRAME R: %d \n",initial.at<cv::Vec3b>(j,i)[0]);
		//printf("FRAME G: %d \n",initial.at<cv::Vec3b>(j,i)[1]);
		//printf("FRAME B: %d \n",initial.at<cv::Vec3b>(j,i)[2]);
		

		//Here is my attempt to filter noise on the background:
		if(initial.at<cv::Vec3b>(j,i)[0] < 192)
		{
		initial.at<cv::Vec3b>(j,i)[0] = 0;	//R
		next.at<cv::Vec3b>(j,i)[0] = 0;
		}

		if(initial.at<cv::Vec3b>(j,i)[1] < 192)
		{
		initial.at<cv::Vec3b>(j,i)[1] = 0;	//G
		next.at<cv::Vec3b>(j,i)[1] = 0;
		}

		if(initial.at<cv::Vec3b>(j,i)[2] < 192)
		{
		initial.at<cv::Vec3b>(j,i)[2] = 0;	//B
		next.at<cv::Vec3b>(j,i)[2] = 0;
		}
//end of attempt

		matResult.at<cv::Vec3b>(j,i)[0] = initial.at<cv::Vec3b>(j,i)[0] - next.at<cv::Vec3b>(j,i)[0]; //R
		matResult.at<cv::Vec3b>(j,i)[1] = initial.at<cv::Vec3b>(j,i)[1] - next.at<cv::Vec3b>(j,i)[1]; //G
		matResult.at<cv::Vec3b>(j,i)[2] = initial.at<cv::Vec3b>(j,i)[2] - next.at<cv::Vec3b>(j,i)[2]; //B
		}
	}
	
	return matResult; 
}

	
int main(int argc, char **argv)
{ 

	char n[10];
	char m[10];	
	
	Mat inMat(VRES_ROWS,HRES_COLS, CV_8UC3, framebuff1);
	Mat nextMat(VRES_ROWS,HRES_COLS, CV_8UC3, framebuff2);
	Mat outMat(VRES_ROWS,HRES_COLS, CV_8UC3, framebuff3);



	//background comparisson per frame
	for(int i = 0; i < TOTALframes; i++)
	{

	//input/output strings
	sprintf(n,"%d",i);
	string sframe(n);
	string input = "frame" + sframe + ".ppm";
	string output = "output" + sframe + ".ppm";
	
	//load frame rgb
	inMat = imread(input, CV_LOAD_IMAGE_COLOR);

	//load next to compare
	sprintf(m,"%d",i+1);
	string stframe(m);
	string next = "frame" + stframe + ".ppm";
	nextMat = imread(next, CV_LOAD_IMAGE_COLOR);

	//{P} - {Q}: background diff
	outMat = get_background(inMat,nextMat);


	//save frame
	imwrite(output, outMat);

	}
    
}

