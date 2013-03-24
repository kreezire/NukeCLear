#pragma once
#include "stdafx.h"
#include "defines.h"
#include "square_detection.h"

extern int gWhiteThreshold;
extern int gDilateIter, gErodeIter, gDilateFirst;
extern int gDilateIterBomb, gErodeIterBomb, gDilateFirstBomb;
extern CvPoint gStartPoint, gEndPoint;
extern char gWhichPoint;
extern uchar gBombY, gBombU, gBombV;
extern int gUToleranceBomb, gVToleranceBomb;

void makeBinaryArray(IplImage* grayscale, IplImage *dest, IplImage *end, IplImage *squares);
IplImage* calibWhiteImg(IplImage *img, IplImage *end, IplImage *squares);
IplImage *getBinImageFullRes(IplImage* img, IplImage* end, IplImage *squares);
IplImage *getBinary(IplImage *img);

void bombBinary(IplImage *yuv, IplImage* bin);
IplImage* squareCalibration(IplImage *img);
void squareCalibration2(IplImage *img, IplImage* bombBinary, vector<CvPoint> &listOfCentres, vector<CircleWConf> &listOfCircles, vector<SquareWConf> &listOfSquares);
void BBBinary(IplImage *yuv, IplImage* bin);
void BFBinary(IplImage *yuv, IplImage* bin);
void botFrontCalib();
void botBackCalib();
IplImage* endCalib(IplImage *img);
void defusalCalibration();

extern CvScalar gBotFront, gBotBack, gEnd;
extern int gUToleranceBF , gVToleranceBF , gDilateFirstBF, gErodeIterBF , gDilateIterBF;
extern int gUToleranceBB , gVToleranceBB , gDilateFirstBB, gErodeIterBB , gDilateIterBB;
extern int gUToleranceEnd , gVToleranceEnd , gDilateFirstEnd , gErodeIterEnd , gDilateIterEnd ;
