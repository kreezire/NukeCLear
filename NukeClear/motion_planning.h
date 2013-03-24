#pragma once
#include "stdafx.h"
#include "defines.h"
#include "calibration.h"



CvPoint getCentreBF(IplImage *yuv, IplImage *binary);
CvPoint getCentreBB(IplImage *yuv, IplImage *binary);

CvPoint operator+(CvPoint p1, CvPoint p2);
CvPoint operator-(CvPoint p1, CvPoint p2);
CvPoint operator*(CvPoint p, int t);
CvPoint operator/(CvPoint p, double t);