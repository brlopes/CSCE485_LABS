/*
CSCE 485 - LAB#3_2
modified from simplecv:
+median filter on G band grayscale
+read frame output with median
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

#define ESC_KEY (27)

using namespace cv;
using namespace std;

//buffers
unsigned char framebuff[1440*2560];
unsigned char framebuff2[1440*2560];
unsigned char framebuff3[1440*2560];


//median filter
int medianFilter(Mat img, int j, int i)
{	
	int Q;
	int mat[9];	//P0 ~ P8
	int hist[256]; 	//histogram

	//zero histogram from 0~255
    	for(int i = 0; i<=255; i++)
	{ 
		hist[i] =  0;
   	}
	
	// around nearest neighbor
	mat[0] = img.at<uchar>(j,i);
	mat[1] = img.at<uchar>(j,i-1);
	mat[2] = img.at<uchar>(j,i+1);
	mat[3] = img.at<uchar>(j-1,i);
	mat[4] = img.at<uchar>(j+1,i);
	mat[5] = img.at<uchar>(j+1,i+1);
	mat[6] = img.at<uchar>(j+1,i-1);
	mat[7] = img.at<uchar>(j-1,i-1);
	mat[8] = img.at<uchar>(j-1,i+1);

	//count:
	for(int i = 0; i <9 ; i++)
	{
		hist[mat[i]]++;
	}
	
	//filter snippet from week5
	int sum = 0;
	int m = 0;
	while ( sum < 5)
	{
		sum = sum + hist[m];	
		m = m + 1;
	}
	Q = m - 1;
	return Q;
}



int main(int argc, char **argv)
{
    int hres = HRES_COLS;
    int vres = VRES_ROWS;
    string output = "frameout.pgm";


    Mat inputframe(vres, hres, CV_8UC1, framebuff);
    Mat outputframe(vres, hres, CV_8UC1, framebuff2);



    namedWindow("MEDIAN FILTER APPLIED ON frame10.ppm", CV_WINDOW_AUTOSIZE);
    inputframe = imread("frame10.ppm",CV_LOAD_IMAGE_GRAYSCALE); //load ppm frame gray scale


    for(int i = 1; i <= 1919; i++)
    { 
	for(int j = 1; j <= 1079; j++)
	{
		outputframe.at<uchar>(j,i) = medianFilter(inputframe,j,i);		
	} 
     }


	//generate output file
	imwrite(output,outputframe);


    //IplImage
    while(1)
    {        
        imshow("Median filter", outputframe);  

        // set pace on shared memory sampling in msecs
        char c = cvWaitKey(10);

        if(c == ESC_KEY)
        {
            exit(1);
        }
    }

 
    return 1;

}
