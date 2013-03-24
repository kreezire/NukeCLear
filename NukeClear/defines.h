#pragma once

#include "stdafx.h"
#include <Windows.h>
#include <algorithm>
#include <string>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <ctype.h>
#include <queue>
#include <vector>
#include <list>
#include "tserial.h"
#include "bot_control.h"


#if (defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__) || (defined(__APPLE__) & defined(__MACH__)))
#include <cv.h>
#include <highgui.h>
#else
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

#include <cvblob.h>
using namespace std;
using namespace cvb;

typedef vector<int> Row;
typedef vector<Row> Mat;
typedef vector<unsigned char> RowC;
typedef vector<RowC> MatC;

typedef pair<int, int> PointIJ;

#define FOR(i,a) for(size_t i=0;i<(size_t)(a);i++)
#define FOR1(i,a) for(size_t i=1;i<=(size_t)a;i++)
#define FORAB(i,a,b) for(size_t i=(size_t)a; i<=(size_t)b;i++)
#define DATA(img, i, j) (uchar*)(img->imageData+(i)*img->widthStep + (j)*img->nChannels) //extracts pixel information
#define YUV2BINARY(src, dst) FOR(i, src->height) { FOR(j, src->width) { *DATA(dst,i,j) = *DATA(src,i,j); } }
#define CHECKWITHINBOUNDS(i, j, ibound, jbound) ((i)>=0 && (i)<(ibound) && (j)>=0 && (j)<(jbound))


#define BLOCKSIZE 3
#define WHITE 255
#define BLACK 0
#define START 's'
#define END 'e'
#define BF_WALL 0
#define BF_NOTINIT -1
#define WALLCLEARANCERATIO 0.5
#define BOTSIZEINPIXELS 50
#define BOTSIZEINBLOCKS (BOTSIZEINPIXELS/BLOCKSIZE)
#define RAYCASTWIDTH (int)((float)BOTSIZEINPIXELS*(1+WALLCLEARANCERATIO*0.5))
#define CLEARANCEVAL ((BOTSIZEINBLOCKS + 1 + (int)((float)BOTSIZEINBLOCKS*WALLCLEARANCERATIO))/2)
#define CLEARANCEVALFORBOMBNEAREST CLEARANCEVAL
#define DRAWPATH_B 0
#define DRAWPATH_G 120
#define DRAWPATH_R 120
#define PI 3.14159265
#define MINDISTBTWNODESINPIXELS 20
#define BOMBDRAWCOLOR CV_RGB(0,255,0)
#define NULLPOINT cvPoint(-1, -1)

#define SOUTH 'S'
#define NORTH 'N'
#define EAST 'E'
#define WEST 'W'

#define SE 'a'
#define NE 'b'
#define NW 'c'
#define SW 'd'

#define BLOCKTOFULLRES(p) cvPoint(p.x*BLOCKSIZE+BLOCKSIZE/2, p.y*BLOCKSIZE+BLOCKSIZE/2)
#define FULLRESTOBLOCK(p) cvPoint((p.x - BLOCKSIZE/2)/BLOCKSIZE, (p.y - BLOCKSIZE/2)/BLOCKSIZE)

#define TOLATURN 0.174532925*1.5 		//tolerance in the angle when rotating. 10 deg in rads
#define TOLAGEN  0.174532925		//tolerance angle in gen. 10 deg in rad
#define TOLRAD 20	//tolerance in the length 


extern CvCapture *gCapture;
#define CAMINDEX 1
#define SIMON 0
