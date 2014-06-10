/* Sukjin Yoon
   CSCE 485 - Computer Machine Vision
	Exercise #3-2
	 Applying median filter to grayscale of the frame.
	 This code is modified version of 'simplecv.cpp' from exercise1.
*/


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

using namespace cv;
using namespace std;

#define VRES_ROWS (1080)
#define HRES_COLS (1920)

#define ESC_KEY (27)

unsigned char imagebuffer[1440*2560*3];
unsigned char imagebuffer2[1440*2560*3];
unsigned char imagebuffer3[1440*2560];
unsigned char imagebuffer4[1440*2560];
unsigned char imagebuffer5[1440*2560*3];

void draw_crosshair(Mat *image,int x,int y){

	
	for(int i = 0; i < HRES_COLS; i++){
		image->at<cv::Vec3b>(y,i)[0] = 255;
		image->at<cv::Vec3b>(y,i)[1] = 255;
		image->at<cv::Vec3b>(y,i)[2] = 255;
	}

	for(int i = 0; i < VRES_ROWS; i++){
		image->at<cv::Vec3b>(i,x)[0] = 255;
		image->at<cv::Vec3b>(i,x)[1] = 255;
		image->at<cv::Vec3b>(i,x)[2] = 255;
	}	
	
}

int apply_median_filter_on_pixel(Mat *img, int y, int x){
	
	int Q;
	int mat[9];
	
 	int hist[256]; //takes it 0-255
    	for(int i = 0; i<=255; i++){ //initialize hist to all 0.
		hist[i] =  0;
   	}

	mat[0] = img->at<uchar>(y,x);
	mat[1] = img->at<uchar>(y,x-1);
	mat[2] = img->at<uchar>(y,x+1);
	mat[3] = img->at<uchar>(y-1,x);
	mat[4] = img->at<uchar>(y+1,x);
	mat[5] = img->at<uchar>(y+1,x+1);
	mat[6] = img->at<uchar>(y+1,x-1);
	mat[7] = img->at<uchar>(y-1,x-1);
	mat[8] = img->at<uchar>(y-1,x+1);
	
	for(int i = 0; i <9 ; i++){
		hist[mat[i]]++;
		
	}
	
	
	 // code from the class ppt.
	int sum = 0;
	int m = 0;
	while ( sum < 5){
		sum = sum + hist[m];	
		m = m + 1;
	}
	Q = m - 1;
	return Q; 
}


void apply_median_filter(Mat *img){
   
	for(int i=1; i <=HRES_COLS-1;i++){ // for all pixel except edge.
		for(int k =1; k<=VRES_ROWS-1;k++){
			img->at<uchar>(k,i) = apply_median_filter_on_pixel(img,k,i);		
		
	}
     }
}


void apply_frame_differencing(Mat *first, Mat *second){
	int one,two,three;
	 for(int i=0; i<HRES_COLS;i++){ // for all pixel except edge.
		for(int k =0; k<VRES_ROWS;k++){
			one = first->at<cv::Vec3b>(k,i)[0] - second->at<cv::Vec3b>(k,i)[0] ;
			two = first->at<cv::Vec3b>(k,i)[1] - second->at<cv::Vec3b>(k,i)[1] ;
			three = first->at<cv::Vec3b>(k,i)[3] - second->at<cv::Vec3b>(k,i)[3];
		if(one >20 || one < -20)
			first->at<cv::Vec3b>(k,i)[0] = one;
		else
			first->at<cv::Vec3b>(k,i)[0] = 0;
		if(two >20 || two < -20)
			first->at<cv::Vec3b>(k,i)[1] = two;
		else
			first->at<cv::Vec3b>(k,i)[1] = 0;
		if(three >20 || three < -20)
			first->at<cv::Vec3b>(k,i)[2] = three;
		else
			first->at<cv::Vec3b>(k,i)[2] = 0;
	
	
		}
     	}

}

void apply_threshold(Mat *before,int *xvalue,int *yvalue){
	int left_top[2],right_top[2],left_bot[2],right_bot[2];
	int hist[256]; //initialize hist to all 0;
	for(int i=0; i<256; i++){
		hist[i] = 0;
	}
	
	for(int i=0; i<1920;i++){ // for all pixel except edge.
		for(int k =0; k<1080;k++){
		
		hist[before->at<uchar>(k,i)]++;
	
		}
     	}

	int max = 100;

	for(int i = 100; i<256; i++){
		if (hist[i] >= hist[max]){
			max = i;
		}
	}

	printf("max: %d\n", max);

	for(int i=0; i<1920;i++){ // left to right top to bottom left_top
		for(int k =0; k<1080;k++){

			if(before->at<uchar>(k,i)==max){
				left_top[0] = k;
				left_top[1] = i;
			
			}
	
		}
     	}
	
	for(int i=1919; i>=0;i--){ // right to left top to bottom, right_top
		for(int k =0; k<1080;k++){

			if(before->at<uchar>(k,i)==max){
				right_top[0] = k;
				right_top[1] = i;
			}
	
		}
     	}

	for(int i=0; i<1920;i++){ // left to right bottom to top, left_bottom
		for(int k =1079; k>=0;k--){

			if(before->at<uchar>(k,i)==max){
				left_bot[0] = k;
				left_bot[1] = i;
			}
	
		}
     	}
	
	for(int i=1919; i>=0;i--){ // right to left bottom to top, right_bottom
		for(int k =1079; k>=0;k--){

			if(before->at<uchar>(k,i)==max){
				right_bot[0] = k;
				right_bot[1] = i;
			}
	
		}
     	}



	/*
	printf("left_top x,y : %d, %d\n",left_top[1],left_top[0]);
	printf("left_bot x,y : %d, %d\n",left_bot[1],left_bot[0]);
	printf("right_top x,y : %d, %d\n",right_top[1],right_top[0]);
	printf("right_bot x,y : %d, %d\n",right_bot[1],right_bot[0]);
	*/
	int x[4],y[4];
	x[0] = left_top[1];
	x[1] = right_top[1];
	x[2] = right_bot[1];
	x[3] = left_bot[1];
	
	y[0] = left_top[0];
	y[1] = right_top[0];
	y[2] = right_bot[0];
	y[3] = left_bot[0];

	int xmin = 9999, ymin = 9999, xmax = 0, ymax = 0;
	for (int i = 0; i<4;i++){
		if(xmin >= x[i])
			xmin = x[i];
		if(xmax <= x[i])
			xmax = x[i];
		if(ymax <= y[i])
			ymax = y[i];
		if(ymin >= y[i])
			ymin = y[i];		
	}

	//printf("min x,y : %d, %d\n",xmin,ymin);
	//printf("max x,y : %d, %d\n",xmax,ymax);
	
	*yvalue = ymin+(ymax - ymin)/2;
	*xvalue = xmin+(xmax - xmin)/2;
	
}

Mat G_to_gray(Mat *before){
	 Mat returned(VRES_ROWS,HRES_COLS, CV_8UC1, imagebuffer4);

	 for(int i=0; i<1920;i++){ // for all pixel except edge.
		for(int k =0; k<1080;k++){

	returned.at<uchar>(k,i) = before->at<cv::Vec3b>(k,i)[1];
	
		}
     	}

	return returned; 
}

//char pgm[5] = ".pgm";
char ppm[5] = ".ppm";
char before[10] = "before";
char after[7] = "after";
char load_str[20];
char save_str[20];
char num[5];

int main(int argc, char **argv)
{
    int hres = HRES_COLS;
    int vres = VRES_ROWS;
    Mat firstimg(vres,hres, CV_8UC3, imagebuffer);//image buffer.
    Mat secondimg(vres,hres, CV_8UC3, imagebuffer2);
    Mat gscale(vres,hres, CV_8UC1, imagebuffer3);
    Mat output(vres,hres, CV_8UC3, imagebuffer5);
    printf("hres=%d, vres=%d\n", hres, vres);


	int nframes = 13;
	

for(int i = 1; i<=nframes-1; i++){
   sprintf(num,"%d",i);
   strcat(save_str,after);
   strcat(save_str,num);
   strcat(save_str,ppm);

   strcat(load_str,before);
   strcat(load_str,num);
   strcat(load_str,ppm);

   firstimg = imread(load_str,CV_LOAD_IMAGE_COLOR);
   output = imread(load_str,CV_LOAD_IMAGE_COLOR);
   strcpy(load_str,"");	

   sprintf(num,"%d",i+1);
   strcat(load_str,before);
   strcat(load_str,num);
   strcat(load_str,ppm);

   secondimg = imread(load_str,CV_LOAD_IMAGE_COLOR);
   strcpy(load_str,"");	
	
   Mat * fimg;
   Mat * simg;
   Mat * gimg;

   fimg = &firstimg;
   simg = &secondimg;
   gimg = &gscale;
   //Assign pointers


   apply_frame_differencing(fimg,simg); 
   //Frame after applying fame 

   *gimg = G_to_gray(fimg);	
   //make copy in grayscale
	

   apply_median_filter(gimg);
   //apply median filter
   int x,y;
	
   apply_threshold(gimg,&x,&y);
   //track COM

   draw_crosshair(&output,x,y);
   //draw_crosshair(fimg,x,y);
   //create overlay on rgb

   //save output
   imwrite(save_str, output);
  // imwrite(save_str, *fimg);
   strcpy(save_str,"");


   }
    
 
    return 1;

}
