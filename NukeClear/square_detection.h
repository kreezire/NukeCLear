#pragma once
#include "stdafx.h"
#include "defines.h"


#define MAXPTR(a,b) ((*a)>(*b)? (a):(b))
#define VISITED(arr,i,j) (arr[i][j])
#define ISVALID(i,j,ilen, jlen) (((i)<(ilen) && (i)>=0 && (j)<(jlen) && (j)>=0)? 1:0)
#define HOUGHCIRCLE(P,C) (int)(sqrt((float)((P).x - (C).x)*((P).x - (C).x) + (float)((P).y - (C).y)*((P).y - (C).y)))
#define HOUGHSQUARE(p,theta) (int)(max((p).x*cos(theta) + (p).y*sin(theta), (p).y*cos(theta) - (p).x*sin(theta)))

typedef unsigned char uchar;
typedef unsigned int uint;
typedef pair<int,int> _Point;
typedef pair<CvPoint,int> Circle;
typedef pair<Circle, int> CircleWConf;
typedef pair <int,int> RTheta;
typedef pair <CvPoint, RTheta> Square;
typedef pair<Square, int> SquareWConf;
typedef CvPoint Node;
typedef pair<Node, char> NodeWFlag;

#define QUAD1(p) ((p).x >=0 && (p).y>=0 ?1:0)
#define QUAD2(p) ((p).x<0 && (p).y>=0 ? 1:0)
#define QUAD3(p) ((p).x<0 && (p).y<0 ? 1:0)
#define QUAD4(p) ((p).x>=0 && (p).y<0 ? 1:0)
#define DEGTORAD(x) (((float)(x)*PI)/(float)180)
#define ISINRADIUS(x,y,stdx,stdy,r) (((int)(x-stdx)*(int)(x-stdx) + (int)(y-stdy)*(int)(y-stdy) > (int)(r)*(int)(r))? 0:1)
#define CONFIDENCEPERCENTRATIO 100
#define SQUARECONFIDENCESCALEFACTOR 100
#define TYPE_NORMAL 'n'
#define TYPE_BOMB 'b'
#define TYPE_BOMBACTUAL 'B'
#define TYPE_END 'e'

extern int gSquareConfidenceThreshold, gCircleConfidenceThreshold;

void blobs(IplImage *source, vector<CvPoint>& listOfCentres, vector <CircleWConf> &listOfCircles,vector <SquareWConf> &listOfSquares);//assuming source has size <= visited
void drawSquare(IplImage *img, Square sq);
void hough(IplImage* source,  vector<CvPoint> &edgePoints, CvPoint centre, vector <int> &hCircle, vector<CircleWConf> &listOfCircles, vector<vector<int>> &hSquare, vector<SquareWConf> &listOfSquares);
