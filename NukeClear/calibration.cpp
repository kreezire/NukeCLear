#include "stdafx.h"
#include "defines.h"
#include "square_detection.h"
#include "simulator.h"
#include "defusion.h"
#include <algorithm>

using namespace std;
using namespace cv;

int gWhiteThreshold = 64;
int gDilateIter = 1, gDilateIterBomb=1;
int gErodeIter = 1, gErodeIterBomb=1;
int gDilateFirst = 1, gDilateFirstBomb=0;
CvPoint gStartPoint = cvPoint(28, 127), gEndPoint = cvPoint(190, 23);
char gWhichPoint;
uchar gBombY=97, gBombU=115, gBombV=197;
int gUToleranceBomb = 20, gVToleranceBomb = 20;
CvScalar gBotFront = cvScalar(69, 134, 173), gBotBack = cvScalar(47, 157, 129);
CvScalar gEnd = cvScalar(86, 107, 164);
int gUToleranceEnd = 13, gVToleranceEnd = 13, gDilateFirstEnd = 0, gErodeIterEnd = 2, gDilateIterEnd = 5;
int gUToleranceBF = 9, gVToleranceBF = 10, gDilateFirstBF = 0, gErodeIterBF = 1, gDilateIterBF = 1;
int gUToleranceBB = 10, gVToleranceBB = 9, gDilateFirstBB = 0, gErodeIterBB = 1, gDilateIterBB = 1;

void mouseCallbackBF(int event, int x, int y, int flags, void* param)
{
	IplImage *img = (IplImage*) param;
//	printf("YUV BotFront= %d %d %d\n", *DATA(img, y, x), *(DATA(img, y, x)+1), *(DATA(img, y, x)+2));
	if(event == CV_EVENT_LBUTTONDOWN)
	{
		gBotFront.val[0] = *DATA(img, y, x);
		gBotFront.val[1]= *(DATA(img, y, x)+1);
		gBotFront.val[2]= *(DATA(img, y, x)+2);
		printf("YUV BotFront= %d %d %d\n", *DATA(img, y, x), *(DATA(img, y, x)+1), *(DATA(img, y, x)+2));
		printf("YES");
	}
}


void mouseCallbackBB(int event, int x, int y, int flags, void* param)
{
	IplImage *img = (IplImage*) param;
	if(event == CV_EVENT_LBUTTONDOWN)
	{
		gBotBack.val[0] = *DATA(img, y, x);
		gBotBack.val[1]= *(DATA(img, y, x)+1);
		gBotBack.val[2]= *(DATA(img, y, x)+2);
		printf("YUV BotBack= %d %d %d\n", *DATA(img, y, x), *(DATA(img, y, x)+1), *(DATA(img, y, x)+2));
	}
}

void mouseCallbackEnd(int event, int x, int y, int flags, void* param)
{
	IplImage *img = (IplImage*) param;
	if(event == CV_EVENT_LBUTTONDOWN)
	{
		gEnd.val[0] = *DATA(img, y, x);
		gEnd.val[1]= *(DATA(img, y, x)+1);
		gEnd.val[2]= *(DATA(img, y, x)+2);
		printf("YUV End= %d %d %d\n", *DATA(img, y, x), *(DATA(img, y, x)+1), *(DATA(img, y, x)+2));
	}
}
void mouseCallbackTerminalPoints(int event, int x, int y, int flags, void* param)
{
	if(event == CV_EVENT_LBUTTONDOWN)
	{
		switch(gWhichPoint)
		{			
		case START:
			gStartPoint = cvPoint(x/BLOCKSIZE, y/BLOCKSIZE);
			printf("Start = %d, %d\n", x/BLOCKSIZE, y/BLOCKSIZE);
			break;
		case END:
			gEndPoint = cvPoint(x/BLOCKSIZE, y/BLOCKSIZE);
			printf("End = %d, %d\n", x/BLOCKSIZE, y/BLOCKSIZE);
			break;
		}
	}
}

void mouseCallbackBombColor(int event, int x, int y, int flags, void* param)
{
	IplImage *img = (IplImage*) param;
	if(event == CV_EVENT_LBUTTONDOWN)
	{
		gBombY = *DATA(img, y, x);
		gBombU = *(DATA(img, y, x)+1);
		gBombV = *(DATA(img, y, x)+2);
		printf("YUV = %d %d %d\n", gBombY, gBombU, gBombV);
	}
}
void makeBinaryArray(IplImage* grayscale, IplImage *dest, IplImage *end, IplImage *squares)
{
	IplImage *grayTemp = cvCloneImage(grayscale);
	RowC sortArr(BLOCKSIZE*BLOCKSIZE, 0);	
	FOR(i, grayTemp->height)
	{
		FOR(j, grayTemp->width)
		{
			if(*DATA(end,i,j) == WHITE)
				*DATA(grayTemp, i, j) = WHITE;
			 if(*DATA(squares,i,j) == WHITE)
				 *DATA(grayTemp, i, j) = BLACK;
		}
	}
	for(int i=0; i<=grayTemp->height - BLOCKSIZE; i+=BLOCKSIZE)
	{
		for(int j=0; j<=grayTemp->width-BLOCKSIZE; j+=BLOCKSIZE)
		{
			sortArr.clear();
			FORAB(itemp, i, i+BLOCKSIZE-1)
			{
				FORAB(jtemp, j, j+BLOCKSIZE-1)
				{
					sortArr.push_back(*DATA(grayTemp, itemp, jtemp));							
				}				
			}
			sort(sortArr.begin(), sortArr.end());
			*DATA(dest, i/BLOCKSIZE, j/BLOCKSIZE) = (sortArr[BLOCKSIZE*BLOCKSIZE/2] > gWhiteThreshold ? WHITE: BLACK);
			
			
		}
	}
	
	
}


IplImage* calibWhiteImg(IplImage *img, IplImage *end, IplImage *squares)
{
	IplImage * imgTemp = cvCreateImage(cvSize(img->width, img->height), 8, 3); 
	IplImage *binary = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	IplImage *finalImg = cvCreateImage(cvSize(img->width/BLOCKSIZE, img->height/BLOCKSIZE), 8, 1);
	IplImage *imgToDisplay  = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	cvCvtColor(img, imgTemp, CV_BGR2YUV);
	cvNamedWindow("Original");
	cvNamedWindow("YUV");
	cvNamedWindow("Binary");
	cvNamedWindow("Control Box");
	cvCreateTrackbar("White Threshold", "Control Box", &gWhiteThreshold, 255, NULL);
	cvCreateTrackbar("Dilate Iter", "Control Box", &gDilateIter, 10, NULL);
	cvCreateTrackbar("Erode Iter", "Control Box", &gErodeIter, 10, NULL);
	cvCreateTrackbar("Dilate First", "Control Box", &gDilateFirst, 1, NULL);

	cvSetMouseCallback("Binary", mouseCallbackTerminalPoints, NULL);

	cvShowImage("Original", img);
	cvShowImage("YUV", imgTemp);
	YUV2BINARY(imgTemp, binary);
	MatC arr(binary->height, RowC(binary->width, 0));
	char ch = 0;
	while((ch=cvWaitKey(5))!=27)
	{		
		if(ch!=-1)
			gWhichPoint = ch;
		makeBinaryArray(binary, finalImg, end, squares);
		if(!gDilateFirst)
		{
			cvErode(finalImg, finalImg, (IplConvKernel*)0, gErodeIter);
			cvDilate(finalImg, finalImg, (IplConvKernel*)0, gDilateIter);
		}
		else
		{
			cvDilate(finalImg, finalImg, (IplConvKernel*)0, gDilateIter);
			cvErode(finalImg, finalImg, (IplConvKernel*)0, gErodeIter);		
		}
		FOR(i, imgToDisplay->height)
		{
			FOR(j, imgToDisplay->width)
			{
				*DATA(imgToDisplay, i, j) = *DATA(finalImg, i/BLOCKSIZE, j/BLOCKSIZE);
			}
		}
		cvShowImage("Binary", imgToDisplay);
	}
	cvDestroyWindow("Original");
	cvDestroyWindow("YUV");
	cvDestroyWindow("Binary");
	cvDestroyWindow("Control Box");
	return finalImg;
}


IplImage *getBinImageFullRes(IplImage* img, IplImage* end, IplImage *squares)
{
	IplImage *imgTemp = cvCreateImage(cvSize(img->width, img->height), 8, 3); 
	IplImage *binary = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	IplImage *finalImg = cvCreateImage(cvSize(img->width/BLOCKSIZE, img->height/BLOCKSIZE), 8, 1);
	IplImage *imgToDisplay  = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	cvCvtColor(img, imgTemp, CV_BGR2YUV);
	YUV2BINARY(imgTemp, binary);	
	makeBinaryArray(binary, finalImg, end, squares);
	if(!gDilateFirst)
	{
		cvErode(finalImg, finalImg, (IplConvKernel*)0, gErodeIter);
		cvDilate(finalImg, finalImg, (IplConvKernel*)0, gDilateIter);
	}
	else
	{
		cvDilate(finalImg, finalImg, (IplConvKernel*)0, gDilateIter);
		cvErode(finalImg, finalImg, (IplConvKernel*)0, gErodeIter);		
	}
	FOR(i, imgToDisplay->height)
	{
		FOR(j, imgToDisplay->width)
		{
			*DATA(imgToDisplay, i, j) = *DATA(finalImg, i/BLOCKSIZE, j/BLOCKSIZE);
		}
	}
	return imgToDisplay;
}

IplImage *getBinary(IplImage *img)
{
	IplImage *imgTemp = cvCreateImage(cvSize(img->width, img->height), 8, 3); 
	IplImage *binary = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	cvCvtColor(img, imgTemp, CV_BGR2YUV);
	YUV2BINARY(imgTemp, binary);	
	FOR(i, binary->height)
	{
		FOR(j, binary->width)
		{
			*DATA(binary, i, j) = (*DATA(binary, i, j) > gWhiteThreshold? WHITE : BLACK);
		}
	}
	return binary;
}

void bombBinary(IplImage *yuv, IplImage* bin)
{
	int uVal=0, vVal=0;
	FOR(i, yuv->height)
	{
		FOR(j, yuv->width)
		{
			uVal = *(DATA(yuv, i, j)+1);
			vVal = *(DATA(yuv, i, j)+2);
			if(uVal<= (gBombU + (uchar)gUToleranceBomb) && uVal >= (gBombU - (uchar)gUToleranceBomb) && vVal <= (gBombV + (uchar)gVToleranceBomb) && vVal >= (gBombV - (uchar)gVToleranceBomb))
				*DATA(bin, i, j)=WHITE;
			else
				*DATA(bin, i, j)=BLACK;
		}
	}
}
IplImage* squareCalibration(IplImage *img)
{
	IplImage *imgTemp = cvCreateImage(cvSize(img->width, img->height), 8, 3); 
	IplImage *binary = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	cvCvtColor(img, imgTemp, CV_BGR2YUV);
	cvNamedWindow("Original");
	cvNamedWindow("Binary");
	cvNamedWindow("Control Box");

	cvCreateTrackbar("Dilate Iter", "Control Box", &gDilateIterBomb, 10, NULL);
	cvCreateTrackbar("Erode Iter", "Control Box", &gErodeIterBomb, 10, NULL);
	cvCreateTrackbar("Dilate First", "Control Box", &gDilateFirstBomb, 1, NULL);
	cvCreateTrackbar("U Tolerance", "Control Box", &gUToleranceBomb, 200, NULL);
	cvCreateTrackbar("V Tolerance", "Control Box", &gVToleranceBomb, 200, NULL);
	
	cvSetMouseCallback("Original", mouseCallbackBombColor, imgTemp);
	cvShowImage("Original", img);
	while(cvWaitKey(5)!=27)
	{			
		bombBinary(imgTemp, binary);
		if(!gDilateFirstBomb)
		{
			cvErode(binary, binary, (IplConvKernel*)0, gErodeIterBomb);
			cvDilate(binary, binary, (IplConvKernel*)0, gDilateIterBomb);
		}
		else
		{
			cvDilate(binary, binary, (IplConvKernel*)0, gDilateIterBomb);
			cvErode(binary, binary, (IplConvKernel*)0, gErodeIterBomb);		
		}		
		cvShowImage("Binary", binary);
	}
	cvDestroyWindow("Original");
	cvDestroyWindow("Binary");
	cvDestroyWindow("Control Box");
	return binary;
}


void squareCalibration2(IplImage *img, IplImage* bombBinary, vector<CvPoint> &listOfCentres, vector<CircleWConf> &listOfCircles, vector<SquareWConf> &listOfSquares)
{	
	IplImage *img2 = cvCloneImage(img);
	cvNamedWindow("Bombs");
	cvNamedWindow("Control Box");
	
	cvCreateTrackbar("Square Conf", "Control Box", &gSquareConfidenceThreshold, 600, NULL);
	CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, CV_AA);
	char buf[20];
	while(cvWaitKey(5)!=27)
	{
		listOfCentres.clear(); listOfCircles.clear(); listOfSquares.clear();
		blobs(bombBinary, listOfCentres, listOfCircles, listOfSquares);  
		FOR(i, listOfSquares.size())
		{
			sprintf(buf, "%d", listOfSquares[i].second);
			drawSquare(img2, listOfSquares[i].first);
		//	printf("Centre  = %d %d\n", listOfSquares[i].first.first.x, listOfSquares[i].first.first.y);
			cvPutText(img2, buf, cvPoint(listOfSquares[i].first.first.x, listOfSquares[i].first.first.y+30), &font, cvScalar(255, 255, 0, 0));
		}
		/*FOR(i, listOfCircles.size())
		{
			cvCircle(img, listOfCircles[i].first.first, listOfCircles[i].first.second, CV_RGB(0,200,200));
		}*/
		cvShowImage("Bombs", img2);
		FOR(i,img->height)
		{
			FOR(j, img->width)
			{
				*DATA(img2,i,j) = *DATA(img,i,j);
				*(DATA(img2,i,j)+1) = *(DATA(img,i,j)+1);
				*(DATA(img2,i,j)+2) = *(DATA(img,i,j)+2);
			}
		}
	}
	
	cvDestroyWindow("Bombs");
	cvDestroyWindow("Control Box");

}


void endBinary(IplImage *yuv, IplImage* bin)
{
	int uVal=0, vVal=0;
	FOR(i, yuv->height)
	{
		FOR(j, yuv->width)
		{
			uVal = *(DATA(yuv, i, j)+1);
			vVal = *(DATA(yuv, i, j)+2);
			if(uVal<= (gEnd.val[1] + (uchar)gUToleranceEnd) && uVal >= (gEnd.val[1] - (uchar)gUToleranceEnd) && vVal <= (gEnd.val[2] + (uchar)gVToleranceEnd) && vVal >= (gEnd.val[2] - (uchar)gVToleranceEnd))
				*DATA(bin, i, j)=WHITE;
			else
				*DATA(bin, i, j)=BLACK;
		}
	}
}


IplImage* endCalib(IplImage *img)
{
	IplImage *imgTemp = cvCreateImage(cvSize(img->width, img->height), 8, 3); 
	IplImage *binary = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	cvCvtColor(img, imgTemp, CV_BGR2YUV);
	cvNamedWindow("End Calib");
	cvNamedWindow("Binary");
	cvNamedWindow("Control Box");

	cvCreateTrackbar("Dilate Iter", "Control Box", &gDilateIterEnd, 10, NULL);
	cvCreateTrackbar("Erode Iter", "Control Box", &gErodeIterEnd, 10, NULL);
	cvCreateTrackbar("Dilate First", "Control Box", &gDilateFirstEnd, 1, NULL);
	cvCreateTrackbar("U Tolerance", "Control Box", &gUToleranceEnd, 200, NULL);
	cvCreateTrackbar("V Tolerance", "Control Box", &gVToleranceEnd, 200, NULL);
	
	cvSetMouseCallback("End Calib", mouseCallbackEnd, imgTemp);	
	cvShowImage("End Calib", img);
	while(cvWaitKey(5)!=27)
	{			
		endBinary(imgTemp, binary);
		if(!gDilateFirstEnd)
		{
			cvErode(binary, binary, (IplConvKernel*)0, gErodeIterEnd);
			cvDilate(binary, binary, (IplConvKernel*)0, gDilateIterEnd);
		}
		else
		{
			cvDilate(binary, binary, (IplConvKernel*)0, gDilateIterEnd);
			cvErode(binary, binary, (IplConvKernel*)0, gErodeIterEnd);		
		}		
		cvShowImage("Binary", binary);
	}
	cvDestroyWindow("End Calib");
	cvDestroyWindow("Binary");
	cvDestroyWindow("Control Box");	
	return binary;
}

void BFBinary(IplImage *yuv, IplImage* bin)
{
	int uVal=0, vVal=0;
	FOR(i, yuv->height)
	{
		FOR(j, yuv->width)
		{
			uVal = *(DATA(yuv, i, j)+1);
			vVal = *(DATA(yuv, i, j)+2);
			if(uVal<= (gBotFront.val[1] + (uchar)gUToleranceBF) && uVal >= (gBotFront.val[1] - (uchar)gUToleranceBF) && vVal <= (gBotFront.val[2] + (uchar)gVToleranceBF) && vVal >= (gBotFront.val[2] - (uchar)gVToleranceBF))
				*DATA(bin, i, j)=WHITE;
			else
				*DATA(bin, i, j)=BLACK;
		}
	}
}
void botFrontCalib()
{
	
	gCapture = cvCaptureFromCAM(CAMINDEX);
	IplImage *img = cvQueryFrame(gCapture);		
	IplImage *imgTemp = cvCreateImage(cvSize(img->width, img->height), 8, 3); 
	IplImage *binary = cvCreateImage(cvSize(img->width, img->height), 8, 1);	
	
	cvNamedWindow("Original");
	cvNamedWindow("Front Binary");	
	cvNamedWindow("Control Box");

	cvCreateTrackbar("Dilate Iter", "Control Box", &gDilateIterBF, 10, NULL);
	cvCreateTrackbar("Erode Iter", "Control Box", &gErodeIterBF, 10, NULL);
	cvCreateTrackbar("Dilate First", "Control Box", &gDilateFirstBF, 1, NULL);
	cvCreateTrackbar("U Tolerance", "Control Box", &gUToleranceBF, 200, NULL);
	cvCreateTrackbar("V Tolerance", "Control Box", &gVToleranceBF, 200, NULL);

	
	cvSetMouseCallback("Original", mouseCallbackBF, imgTemp);
	
		
	while(cvWaitKey(5)!=27)
	{
		img = cvQueryFrame(gCapture);/*img = cvQueryFrame(gCapture);img = cvQueryFrame(gCapture);img = cvQueryFrame(gCapture);*/
	//	cvFlip(img, img, 1);
		if(SIMON)
			sDisplayBot(img);
		cvShowImage("Original", img);		
		cvCvtColor(img, imgTemp, CV_BGR2YUV);
		BFBinary(imgTemp, binary);
		if(!gDilateFirstBF)
		{
			cvErode(binary, binary, (IplConvKernel*)0, gErodeIterBF);
			cvDilate(binary, binary, (IplConvKernel*)0, gDilateIterBF);
		}
		else
		{
			cvDilate(binary, binary, (IplConvKernel*)0, gDilateIterBF);
			cvErode(binary, binary, (IplConvKernel*)0, gErodeIterBF);		
		}		
		cvShowImage("Front Binary", binary);
		cvShowImage("Original", img);
		
		
	}

	cvReleaseCapture(&gCapture);
	cvDestroyWindow("Original");
	cvDestroyWindow("Front Binary");
	cvDestroyWindow("Control Box");
	
}

void BBBinary(IplImage *yuv, IplImage* bin)
{
	int uVal=0, vVal=0;
	FOR(i, yuv->height)
	{
		FOR(j, yuv->width)
		{
			uVal = *(DATA(yuv, i, j)+1);
			vVal = *(DATA(yuv, i, j)+2);
			if(uVal<= (gBotBack.val[1] + (uchar)gUToleranceBB) && uVal >= (gBotBack.val[1] - (uchar)gUToleranceBB) && vVal <= (gBotBack.val[2] + (uchar)gVToleranceBB) && vVal >= (gBotBack.val[2] - (uchar)gVToleranceBB))
				*DATA(bin, i, j)=WHITE;
			else
				*DATA(bin, i, j)=BLACK;
		}
	}
}

void botBackCalib()
{
	IplImage *img;

	gCapture = cvCaptureFromCAM(CAMINDEX);
	img = cvQueryFrame(gCapture);	
	IplImage *imgTemp = cvCreateImage(cvSize(img->width, img->height), 8, 3); 
	IplImage *binary = cvCreateImage(cvSize(img->width, img->height), 8, 1);	
	
	cvNamedWindow("Original");
	cvNamedWindow("Back Binary");	
	cvNamedWindow("Control Box");

	cvCreateTrackbar("Dilate Iter", "Control Box", &gDilateIterBB, 10, NULL);
	cvCreateTrackbar("Erode Iter", "Control Box", &gErodeIterBB, 10, NULL);
	cvCreateTrackbar("Dilate First", "Control Box", &gDilateFirstBB, 1, NULL);
	cvCreateTrackbar("U Tolerance", "Control Box", &gUToleranceBB, 200, NULL);
	cvCreateTrackbar("V Tolerance", "Control Box", &gVToleranceBB, 200, NULL);

	cvSetMouseCallback("Original", mouseCallbackBB, imgTemp);
	while(cvWaitKey(5)!=27)
	{		
		img = cvQueryFrame(gCapture);/*img = cvQueryFrame(gCapture);img = cvQueryFrame(gCapture);img = cvQueryFrame(gCapture);*/
	//	cvFlip(img, img, 1);
		if(SIMON)
			sDisplayBot(img);
		cvCvtColor(img, imgTemp, CV_BGR2YUV);
		BBBinary(imgTemp, binary);
		if(!gDilateFirstBB)
		{
			cvErode(binary, binary, (IplConvKernel*)0, gErodeIterBB);
			cvDilate(binary, binary, (IplConvKernel*)0, gDilateIterBB);
		}
		else
		{
			cvDilate(binary, binary, (IplConvKernel*)0, gDilateIterBB);
			cvErode(binary, binary, (IplConvKernel*)0, gErodeIterBB);		
		}		
		cvShowImage("Back Binary", binary);
		cvShowImage("Original", img);
	}
	cvReleaseCapture(&gCapture);
	cvDestroyWindow("Original");
	cvDestroyWindow("Back Binary");
	cvDestroyWindow("Control Box");
	
}

void defusalCalibration()
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
	cvNamedWindow("Control Box", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("Control Box", 50, 650);
	
	cvCreateTrackbar("UTolerance", "Control Box", &UTOLERANCE, 100, NULL);
	cvCreateTrackbar("VTolerance", "Control Box", &VTOLERANCE, 100, NULL);
	cvCreateTrackbar("Erosion", "Control Box", &EROSION, 15, NULL);
	cvCreateTrackbar("Dilation", "Control Box", &DILATION, 15, NULL);
	cvSetMouseCallback("Original Video", mouse_callback_bomb, (void*)yuv);

	
	int bomb_x, bomb_y, areamax;

	while(cvWaitKey(5)!=27) {

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
		
		cvShowImage("Original Video", frame);
		
	}

	cvDestroyWindow("Original Video");
	cvDestroyWindow("Control Box");

	cvReleaseImage(&yuv);
	cvReleaseImage(&binary);
	cvReleaseCapture(&gCaptureDefusal);
}