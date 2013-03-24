// NukeClear.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "defines.h"
#include "tserial.h"
#include "bot_control.h"
#include "path.h"
#include "calibration.h"
#include "square_detection.h"
#include "motion_planning.h"
#include "simulator.h"
#include "defusion.h"

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <core/core.hpp>
#include <highgui/highgui.hpp>
#include <time.h>
#include <cv.h>
#include <conio.h>


#define IMAGE "image3.jpg"

using namespace std;
CvCapture *gCapture;


int main()
{ 
//	freopen("out.txt", "w", stdout);
//	serial temp;
	
	cvNamedWindow("One");
	IplImage *img, *img2;
	serial comm;
	//comm.startDevice("\\\\.\\COM21",9600);
	//defusalCalibration();	
	//defuse_bomb(comm);
	if(!SIMON)
	{		
		comm.startDevice("\\\\.\\COM21",9600);			
		gCapture = cvCaptureFromCAM(CAMINDEX);
		IplImage *frame; 
		while(cvWaitKey(5)==-1)
		{		
			frame = cvQueryFrame(gCapture);
			cvShowImage("One", frame);
		}
		img = cvCloneImage(frame);
		img2 = cvCloneImage(frame);
		cvReleaseCapture(&gCapture);
		cvSaveImage("image3.jpg", img);
	}
	else
	{
		img = cvLoadImage(IMAGE);
		img2 = cvLoadImage(IMAGE);
	}
	cvDestroyWindow("One");
	IplImage *bombBinary = squareCalibration(img);
	vector<CvPoint> listOfCentres;
	vector<CircleWConf> listOfCircles;
	vector<SquareWConf> listOfSquares;
	squareCalibration2(img, bombBinary, listOfCentres, listOfCircles, listOfSquares);
	defusalCalibration();	
	IplImage* endBinary= endCalib(img);
	IplImage *finalImg = calibWhiteImg(img, endBinary, bombBinary);
	Mat arrBF(finalImg->height, Row(finalImg->width, 0));
	brushfire(finalImg, arrBF);		
	
	vector<NodeWFlag> listOfNodes;
	/*vector<char> path;
	getPathP1P2(gStartPoint, gEndPoint, arrBF, path);
	if(!path.size())
	{
		printf("No");
		cvWaitKey(0);
	}*/
	justDoIt(img2, BLOCKTOFULLRES(gStartPoint), BLOCKTOFULLRES(gEndPoint), listOfSquares, arrBF, listOfNodes, endBinary, bombBinary);
	sInitSimBot(listOfNodes[0].first);
	botFrontCalib();
	botBackCalib();
	/////////////// motion planning:::
	cvNamedWindow("Bot");
	gCapture = cvCaptureFromCAM(CAMINDEX);
	img = cvQueryFrame(gCapture);
	IplImage *binary = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	IplImage * yuv = cvCreateImage(cvSize(img->width, img->height), 8, 3);
	CvPoint botCentre, botFront, botBack, gotoVector, botVector;
	double angleBot, angleDest, angleDiff;
	CvPoint curCoord = listOfNodes[0].first, destCoord;
	listOfNodes.erase(listOfNodes.begin());
	char charCode = 0;
	
	FOR(i, listOfNodes.size())
	{
		destCoord = listOfNodes[i].first;
		charCode = listOfNodes[i].second;
		printf("NODE %d\n", i);
		while(cvWaitKey(5)!=27)
		{
			img = cvQueryFrame(gCapture);
		//	cvFlip(img, img, 1);
			if(SIMON)
				sDisplayBot(img);

			cvCvtColor(img, yuv, CV_BGR2YUV);
			botFront = getCentreBF(yuv, binary);
			botBack = getCentreBB(yuv, binary);			
			botCentre = (botFront + botBack)/2;
			
			cvCircle(img, destCoord, 10, CV_RGB(255,255,100));
			cvCircle(img, botFront, 2, CV_RGB(255,255,255));
			cvCircle(img, botBack, 2, CV_RGB(255,255,255));
			cvCircle(img, botCentre, 2, CV_RGB(255,255,255));

			curCoord = botCentre;
			gotoVector = destCoord - curCoord;
			botVector = botFront - botBack;
			angleBot = atan2((double)botVector.y, (double) botVector.x);
			angleDest = atan2((double)gotoVector.y, (double)gotoVector.x);
			cvShowImage("Bot", img);

			if(abs(angleDest-angleBot ) < TOLATURN)
			{				
				if(distanceSqr(curCoord, destCoord) < TOLRAD*TOLRAD)
				{			
					if(charCode == TYPE_END)
					{
						printf("o\n");
						if(!SIMON)
							comm.send_data('o');
					}
					//if(charCode == TYPE_BOMB)
					//{
					//	printf("o\nl\n");
					//	if(!SIMON)
					//	{
					//		comm.send_data('o');
					//		comm.send_data('l');
					//		cvWaitKey(200);
					//	}
					////	defuse_bomb(comm);
					//}
					break;
					printf("SHITes\n");
				}
				else
				{
					if(charCode == TYPE_BOMBACTUAL)
					{
						if(!SIMON)
						{
							comm.send_data('o');
							comm.send_data('l');
							cvWaitKey(200);
							defuse_bomb(comm);
						}
						break;
					}
					/*forward(angleBot, BOTFORWARD, circle1, circle2);*/
					printf("w\n");
					if(SIMON)
						sForward();
					else
						comm.send_data('w');
				}
			}
			else
			{
				//	printf("%f\n", angleDest - angleBot);
				if(angleBot<0)
					angleBot+=2*PI;
				if(angleDest<0)
					angleDest+=2*PI;
				angleDiff = angleBot - angleDest;
				if((angleDiff > 0 && abs(angleDiff) < PI) || (angleDiff < 0 && abs(angleDiff) > PI))
				{
					printf("d\n");
					if(SIMON)
						sRotateC();
					else
						comm.send_data('a');
				}
				else
				{
					printf("a\n");
					if(SIMON)
						sRotateCC();						
					else
						comm.send_data('d');
					
				}
			}			
		}
	}
	if(!SIMON)
		comm.send_data('o');
	
	return 0;
}
