#include "stdafx.h"
#include <stdio.h>
#include <core/core.hpp>
#include <highgui/highgui.hpp>
#include <cv.h>
#include <cvblob.h>
#include "tserial.h"
#include "bot_control.h"
#include "defines.h"
#include <math.h>
#include "defusion.h"

#define IMGDATA(image,i,j,k) ((uchar *)&image->imageData[(i)*(image->widthStep) + (j)*(image->nChannels) + (k)])	//extracts pixel information

CvCapture *gCaptureDefusal;

using namespace std;
using namespace cvb;

int CENTERWIDTH = 10;

int UTOLERANCE=10, VTOLERANCE=10;
int EROSION = 4, DILATION = 7;
int BOMB_U = 256, BOMB_V = 256;

void forward(serial &comm) {printf("Go Forward\n"); comm.send_data('w');}
void backward(serial &comm) {printf("Go Backward\n"); comm.send_data('s');}
void rotate_left(serial &comm) {printf("Rotate Left\n"); comm.send_data('a'); cvWaitKey(10); comm.send_data('o');cvWaitKey(30);}
void rotate_right(serial &comm) {printf("Rotate Right\n"); comm.send_data('d'); cvWaitKey(10); comm.send_data('o');cvWaitKey(30);}

void mouse_callback_bomb(int event, int x, int y, int flags, void* param)	//callback function for mouse events
{
	IplImage* image = (IplImage*) param;
	if (event == CV_EVENT_LBUTTONDOWN) 
		{
			BOMB_U = *IMGDATA(image,y,x,1);
			BOMB_V = *IMGDATA(image,y,x,2);
		}
}

void convert2binary (IplImage *yuv, IplImage *binary, int U, int V)
{
	for(int i=0; i<yuv->height; i++)
	{
		for(int j=0; j<yuv->width; j++)
		{
			if (*IMGDATA(yuv,i,j,1)>(U-UTOLERANCE) && *IMGDATA(yuv,i,j,1)<(U+UTOLERANCE) && *IMGDATA(yuv,i,j,2)>(V-VTOLERANCE) && *IMGDATA(yuv,i,j,2)<(V+VTOLERANCE))
				*IMGDATA(binary,i,j,0)=255;
			else
				*IMGDATA(binary,i,j,0)=0;
		}
	}
}

IplImage *imgrotate (IplImage *frame)
{
	
	int angle = -45;
	float m[6];
    CvMat M = cvMat(2, 3, CV_32F, m);
    
	int width = frame->width;
    int height = frame->height;
	IplImage *rotated = cvCreateImage(cvSize(height, width), IPL_DEPTH_8U, 3);	

	    m[0] = (float)(cos(-angle*2*CV_PI/180.));
        m[1] = (float)(sin(-angle*2*CV_PI/180.));
        m[3] = -m[1];
        m[4] = m[0];
        m[2] = width*0.5f;  
        m[5] = height*0.5f;  
    
	cvGetQuadrangleSubPix( frame, rotated, &M);
	return rotated;
}
void defuse_bomb(serial &comm)
{
	gCaptureDefusal = cvCaptureFromCAM(CAMINDEXDEFUSAL);					//gets webcam feed 
	IplImage *origframe = cvQueryFrame(gCaptureDefusal);
	IplImage *frame = imgrotate(origframe);
	
	int height, width, step, channels;
	height = frame->height;				// height of image
	width = frame->width;				// width of image
	step = frame->widthStep;		    // no. of array element for one row of image data    
	channels = frame->nChannels;		// no. of channels....BGR
	CvSize imgSize = cvGetSize(frame);  // size of image

	IplConvKernel* morphKernel = cvCreateStructuringElementEx(5, 5, 1, 1, CV_SHAPE_RECT, NULL); //no idea what this does :P

	IplImage *yuv = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);		
	IplImage *binary = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	IplImage *labelImg = cvCreateImage(cvGetSize(frame), IPL_DEPTH_LABEL, 1);	//image of blobs
	CvBlobs blobs;				//stores blobs 

	cvNamedWindow("Original Video", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("Original Video", 10, 10);
	
	
	int bomb_x, bomb_y, areamax;

	while(1) {

		bomb_x=0, bomb_y=0, areamax=0;		
		origframe = cvQueryFrame(gCaptureDefusal);						//grabs one frame and stores in the IplImage
		frame = imgrotate(origframe);
		//cvFlip( frame, frame, 1 );							//flips the image for a natural feed
		
		cvCvtColor(frame,yuv,CV_BGR2YUV);
		convert2binary(yuv,binary,BOMB_U,BOMB_V);			
		cvErode(binary,binary,NULL,EROSION);
		cvDilate(binary,binary,NULL,DILATION);
		cvSmooth(binary,binary,CV_MEDIAN,7,7);
		
		unsigned int result = cvLabel(binary, labelImg, blobs); //finds blobs
		//cvFilterByArea(blobs,AREAMIN,AREAMAX);				//blob size thresholding
		cvRenderBlobs(labelImg,blobs,frame,frame);				//renders blobs in the image
			
		//finding blob of maximum size
		for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
		{
			if (it->second->area > areamax)
			{
				areamax = it->second->area;
				bomb_x=it->second->centroid.x;
				bomb_y=it->second->centroid.y;
			}
		}
		
		if(bomb_x!=0)
			{
			printf("Centroid=(%d,%d) ",bomb_x,bomb_y);
			if (bomb_x < (width-CENTERWIDTH)/2)
				rotate_left(comm);
			else if(bomb_x > (width+CENTERWIDTH)/2)
				rotate_right(comm);
			else break;
			}
		else(rotate_left(comm));

		cvShowImage("Original Video", frame);
		/*char c = cvWaitKey(33);
        if( c == 27 ) break;*/
	}

	forward(comm);
	cvWaitKey(CHARGETIME);
	backward(comm);
	cvWaitKey(BACKTIME);
	printf("Stopping\n"); comm.send_data('o');

	cvReleaseImage(&yuv);
	cvReleaseImage(&binary);
}

