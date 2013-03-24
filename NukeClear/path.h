#pragma once

#include "stdafx.h"
#include "defines.h"
#include "square_detection.h"

void brushfire(IplImage* finalImg, ::Mat &arr);
void getPathP1P2(CvPoint p1, CvPoint p2, Mat &arrBF, vector<char> &path);//WORKS IN BLOCKS!!
void getPathP1P2FullRes(CvPoint p1, CvPoint p2, Mat &arrBF, vector<char> &path);
void drawPath(IplImage* img, vector<char> &path, CvPoint startPoint);//img = original image size, startPoint = wrt BlockImage
void lineBresenham(CvPoint p1, CvPoint p2, IplImage* img);
void getOffsetPoints(CvPoint src, CvPoint dest, int rad, CvPoint &s1, CvPoint &d1, CvPoint &s2, CvPoint &d2);
int checkIntersection(CvPoint p1, CvPoint p2, IplImage *bin);
void rayCasting(IplImage *img, vector<char> &path, CvPoint startPoint, vector<CvPoint> &nodes, IplImage *end, IplImage *squares);
CvPoint searchForNearestTraversable(Mat &arrBF, CvPoint p);//PERFORMS IN BLOCKSIZE!!
void justDoIt(IplImage *img, CvPoint startPoint, CvPoint endPoint, vector<SquareWConf> &listOfSquares, Mat &arrBF, vector<NodeWFlag> &listOfNodes, IplImage *end, IplImage *squares);//start-end =  FUll RES!

bool operator==(CvPoint p1, CvPoint p2);
bool operator!=(CvPoint p1, CvPoint p2);
int distanceSqr(CvPoint p1, CvPoint p2);