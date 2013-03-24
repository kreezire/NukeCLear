#pragma once
#include "stdafx.h"
#include "defines.h"
using namespace std;
extern double sAngle;
extern CvPoint sCentre;


#define SRADIALDIST 10
#define SCIRCLERADIUS 8
#define SCOLORFRONT CV_RGB(250,0,250)
#define SCOLORBACK CV_RGB(0,250,250)
#define SBOTFORWARD 3
#define SBOTANGLE 0.0872664626


void sInitSimBot(CvPoint centre);
void sDisplayBot(IplImage* img);
void sForward();
void sRotateCC();
void sRotateC();