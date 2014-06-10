/*
LAB#6 "Hand Detection"
CSCE 485 - Bruno Lopes


background elimination done by using opencv codeBook (oppose to framediff from lab#3)
adapted from Bradski, Learning OpenCV Chap#9  281~294
http://goo.gl/Ewwvs6

Many important Sources:
http://docs.opencv.org/doc/tutorials/imgproc/shapedescriptors/hull/hull.html
http://docs.opencv.org/modules/imgproc/doc/structural_analysis_and_shape_descriptors.html
http://docs.opencv.org/doc/tutorials/imgproc/shapedescriptors/find_contours/find_contours.html
http://docs.opencv.org/trunk/doc/py_tutorials/py_imgproc/py_contours/py_contours_more_functions/py_contours_more_functions.html
http://stackoverflow.com/questions/9998195/how-to-find-the-differences-between-frames-using-opencv
http://www.cs.iit.edu/~agam/cs512/lect-notes/opencv-intro/opencv-intro.html
http://opencv-code.com/tutorials/count-and-segment-overlapping-objects-with-watershed-and-distance-transform/
*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "opencv2/core/core.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/legacy/legacy.hpp"


//VARIABLES for CODEBOOK METHOD:
CvBGCodeBookModel* model = 0;
 

void find_hand(IplImage* img_8uc1,IplImage* img_8uc3) 
{	

	CvMemStorage* storage = cvCreateMemStorage();
	CvSeq* first_contour = NULL;
	CvSeq* maxitem=NULL;
	double area=0,areamax=0;
	int maxn=0;

	int Nc = cvFindContours(img_8uc1,storage,&first_contour,sizeof(CvContour),CV_RETR_LIST); 
	int n=0;
	

	char text1[255]; 
	sprintf(text1, "SPACE to reset, ESC to quit"); //reset codebook msg
	CvFont font1;
	double hScale=1.0;
	double vScale=1.0;
	int    lineWidth=1.5;
	cvInitFont(&font1,CV_FONT_HERSHEY_DUPLEX, hScale,vScale,0,lineWidth);



	if(Nc>0)
	{
		for( CvSeq* c=first_contour; c!=NULL; c=c->h_next )
		{
			area=cvContourArea(c,CV_WHOLE_SEQ );

			if(area>areamax)
			{
				areamax=area;
				maxitem=c;
				maxn=n;
			}
			n++;
		}
	
		CvMemStorage* storage3 = cvCreateMemStorage(0);


	if(areamax>5000)
	{
		cvPutText (img_8uc3, text1, cvPoint(20,430), &font1, cvScalar(255,255,255));
		maxitem = cvApproxPoly( maxitem, sizeof(CvContour), storage3, CV_POLY_APPROX_DP, 10, 1 );
		
		CvPoint pt0;

		CvMemStorage* storage1 = cvCreateMemStorage(0);
		CvMemStorage* storage2 = cvCreateMemStorage(0);
		CvSeq* ptseq = cvCreateSeq( CV_SEQ_KIND_GENERIC|CV_32SC2, sizeof(CvContour), sizeof(CvPoint), storage1 );
		CvSeq* hull;
		CvSeq* defects;

		for(int i = 0; i < maxitem->total; i++ )
		{
		    CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, maxitem, i );
		    pt0.x = p->x;
		    pt0.y = p->y;
		    cvSeqPush( ptseq, &pt0 );
		}

		hull = cvConvexHull2( ptseq, 0, CV_CLOCKWISE, 0 );
		int hullcount = hull->total;

		defects= cvConvexityDefects(ptseq,hull,storage2);


	    	CvConvexityDefect* defectArray;

	     	int j=0;
		
		for(;defects;defects = defects->h_next)	// marks defects of contours
		{
			int nomdef = defects->total; // defect amount


			if(nomdef == 0)
			continue;

			defectArray = (CvConvexityDefect*)malloc(sizeof(CvConvexityDefect)*nomdef);

			cvCvtSeqToArray(defects,defectArray, CV_WHOLE_SEQ);

			for(int i=0; i<nomdef; i++)
			{
				printf(" convex hull defect @ %d %f \n",i,defectArray[i].depth);
				cvLine(img_8uc3, *(defectArray[i].start), *(defectArray[i].depth_point),CV_RGB(0,0,255),1, CV_AA, 0 );
				cvCircle( img_8uc3, *(defectArray[i].depth_point), 5, CV_RGB(0,153,0), 3, 8,0);
				cvCircle( img_8uc3, *(defectArray[i].start), 5, CV_RGB(0,153,0), 3, 8,0);
				cvLine(img_8uc3, *(defectArray[i].depth_point), *(defectArray[i].end),CV_RGB(0,0,255),1, CV_AA, 0 );
			}

			char txt[]="0";
			txt[0]='0'+nomdef-1;
			CvFont font;
			cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, 2.0, 2.0, 0, 4, CV_AA);
			cvPutText(img_8uc3, txt, cvPoint(50, 50), &font, cvScalar(0, 255, 255, 0));
			
			j++;

			free(defectArray);
		}


	cvReleaseMemStorage( &storage );
	cvReleaseMemStorage( &storage1 );
	cvReleaseMemStorage( &storage2 );
	cvReleaseMemStorage( &storage3 );
	}
	}
}


int main(int argc, char** argv)
{
	
	const char* filename = 0;
	IplImage* rawImage = 0, *yuvImage = 0; 
	IplImage *ImaskCodeBook = 0,*ImaskCodeBookCC = 0;
	CvCapture* capture = 0;

	int c, n, nframes = 0;
	int nframesToLearnBG = 300;

	model = cvCreateBGCodeBookModel();

	//Set color thresholds to default values
	model->modMin[0] = 3;
	model->modMin[1] = model->modMin[2] = 3;
	model->modMax[0] = 10;
	model->modMax[1] = model->modMax[2] = 10;
	model->cbBounds[0] = model->cbBounds[1] = model->cbBounds[2] = 10;


	char text1[255]; 
	sprintf(text1, "WAIT AT LEAST 10 seconds!");
	CvFont font;
	double hScale=1.0;
	double vScale=1.0;
	int    lineWidth=3;
	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX, hScale,vScale,0,lineWidth);



    	bool pause = false;
    	bool singlestep = false;

	//camera input (change if need)
	capture = cvCreateCameraCapture(1);

   
    for(;;)
    {
        if( !pause )
        {
            rawImage = cvQueryFrame(capture);
            nframes++;
            if(!rawImage)
                break;
        }
        if(singlestep)
            pause = true;

        if( nframes == 1 && rawImage )
        {
            yuvImage = cvCloneImage(rawImage);
            ImaskCodeBook = cvCreateImage( cvGetSize(rawImage), IPL_DEPTH_8U, 1 );
            ImaskCodeBookCC = cvCreateImage( cvGetSize(rawImage), IPL_DEPTH_8U, 1 );
            cvSet(ImaskCodeBook,cvScalar(255));

            cvNamedWindow( "HAND DETECTION", 1 );
//          cvNamedWindow( "codeBook_back",1);
//          cvNamedWindow( "codeBook_front",1);
        }

        // If we've got an rawImage and are good to go:
        if( rawImage )
        {
            cvCvtColor( rawImage, yuvImage, CV_BGR2YCrCb );//YUV For codeBook method
            //This is where we build our background model
            if( !pause && nframes-1 < nframesToLearnBG  )
                cvBGCodeBookUpdate( model, yuvImage );

            if( nframes-1 == nframesToLearnBG  )
                cvBGCodeBookClearStale( model, model->t/2 );

            //Find the foreground if any
            if( nframes-1 >= nframesToLearnBG  )
            {
                //foreground by codeBook method
                cvBGCodeBookDiff( model, yuvImage, ImaskCodeBook );

                cvCopy(ImaskCodeBook,ImaskCodeBookCC);
                cvSegmentFGMask( ImaskCodeBookCC );

//              cvShowImage( "codeBook_back",ImaskCodeBookCC);
                find_hand(ImaskCodeBookCC,rawImage);
            }
		
            //Display
	    cvPutText (rawImage, text1, cvPoint(170,30), &font, cvScalar(0,0,255));
            cvShowImage( "HAND DETECTION", rawImage );
//          cvShowImage( "codeBook_front",ImaskCodeBook);

        }

		
        c = cvWaitKey(7)&0xFF;
        c = tolower(c);

	if(c == 27 || c == 'q')
        break;
	
        switch( c )
        {
		case ' '://reset codeBook
            	cvBGCodeBookClearStale( model, 0 );
            	nframes = 0;
            	break;
        }
    }

    cvReleaseCapture( &capture );
    cvDestroyWindow( "HAND DETECTION" );
//cvDestroyWindow( "codeBook_front");
//cvDestroyWindow( "codeBook_back");
    return 0;
}



