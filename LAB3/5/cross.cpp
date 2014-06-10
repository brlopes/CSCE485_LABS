/*
SUBMITED
CSCE 485 - LAB#3_4
applying grayscale to frame
*/


#include <cmath>  //absolute value
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
unsigned char framebuff1[1440*2560];	// 1440rows 2560 cols/row, 1ch 
unsigned char framebuff2[1440*2560]; 	// 1440rows 2560 cols/row, 1ch 
unsigned char framebuff3[1440*2560]; 	// 1440rows 2560 cols/row, 1ch 

unsigned char framebuff4[1440*2560];	// 1440rows 2560cols, 1ch
unsigned char framebuff5[1440*2560];	// 1440rows 2560cols, 1ch
unsigned char framebuff6[1440*2560];	// 1440rows 2560cols, 1ch

int TOTALframes = 100;	//# of ppm frames extracted from ffmpeg


///FILTER BACKGROUND G band
Mat get_background(Mat initial, Mat next)
{
	Mat matResult(VRES_ROWS,HRES_COLS, CV_8UC1, framebuff4);
	
	for(int i=0; i<1920;i++)
	{
		for(int j =0; j<1080;j++)
		{

		int limit = initial.at<uchar>(j,i);

		//Here is my attempt to filter noise on the background:
		if(limit < 230)
		{
		initial.at<uchar>(j,i) = 0;	
		next.at<uchar>(j,i) = 0;
		}
		//end of attempt

		matResult.at<uchar>(j,i) = initial.at<uchar>(j,i) - next.at<uchar>(j,i); //
		
		}
	}
	
	return matResult; 
}



/////
Mat get_cross(Mat input)
{
	Mat matResult(VRES_ROWS,HRES_COLS, CV_8UC1, framebuff2);
	int posx[1920];
	int posy[1080];

	int x = 0;
	int y = 0;
	int countx = 0;
	int county = 0;
	int comx, comy;

	int temp;
	
///FIND DOT, MARK ON position x/y
	for(int i = 0; i < 1920; i++)
	{
		for(int j = 0; j < 1080; j++)
		{
		matResult.at<uchar>(j,i) = input.at<uchar>(j,i);
		temp = matResult.at<uchar>(j,i);
		
			if(temp > 240) //for all values on histogram != 0 (noise free)
			{
				posx[i] = 1;
				posy[j] = 1;
			}

		}
	}




//SUMM ALL POSITIONS to find AVG = CENTER OF MASS
	for(int i = 0; i < 1920; i++)
	{
		for(int j = 0; j < 1080; j++)
		{

			if(posx[i] == 1)
			{
			x += i;
			countx++;
			}

			if(posy[j] == 1)
			{
			y += j;
			county++;			
			}				
		
		
		}
	}


	//divide the summ of each index found by the # of hits
	comx = abs(x/countx);
	comy = abs(y/county);

	//printf("comx: %d \n", comx);
	//printf("comy: %d \n", comy);
	
	//draw the cross-hair using the avg for center of mass
	for(int i = 0; i < 1920; i++)
	{
		matResult.at<uchar>(comx,i) = 255;
	}

	for(int i = 0; i < 1080; i++)
	{
		matResult.at<uchar>(i,comy) = 255;
	}	

	return matResult; 

}


	 
int main(int argc, char **argv)
{
	
	char n[10];
	char m[10];
	

	Mat inMat(VRES_ROWS,HRES_COLS, CV_8UC1, framebuff1);	//input image

	Mat nextMat(VRES_ROWS,HRES_COLS, CV_8UC1, framebuff5);  //to filter background
	Mat tempMat(VRES_ROWS,HRES_COLS, CV_8UC1, framebuff6);  //temp

	Mat outMat(VRES_ROWS,HRES_COLS, CV_8UC1, framebuff3);	//out frame, single G channel


	//apply grayscale per frame
	for(int i = 0; i < TOTALframes; i++)
	{

	//input/output strings
	sprintf(n,"%d",i);
	string sframe(n);
	string input = "frame" + sframe + ".pgm";
	string output = "cross_frame" + sframe + ".pgm";

	
	//load next to compare
	sprintf(m,"%d",i+1);
	string stframe(m);
	string next = "frame" + stframe + ".pgm";
	
	
	//load frames gray scale
	inMat = imread(input, CV_LOAD_IMAGE_GRAYSCALE);
	nextMat = imread(next, CV_LOAD_IMAGE_GRAYSCALE);

	//{P} - {Q}: background diff
	tempMat = get_background(inMat, nextMat);
	
	//get cross-hair for G band:
	outMat = get_cross(tempMat);


	//save frame
	imwrite(output, outMat);

	}

}
