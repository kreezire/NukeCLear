#include "stdafx.h"
#include "defines.h"
#include "calibration.h"

CvPoint getCentreBF(IplImage *yuv, IplImage *binary)
{
	BFBinary(yuv, binary);
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
	int count =0;
	CvPoint centre = cvPoint(0,0);
	FOR(i, binary->height)
	{
		FOR(j, binary->width)
		{
			if(*DATA(binary, i, j))
			{
				count++;
				centre = cvPoint(centre.x + j, centre.y + i);
			}
		}
	}
	if(!count)
	{
		printf("ShitFront\n");
		return NULLPOINT;
	}
	centre = cvPoint(centre.x/count, centre.y/count);
	return centre;
}


CvPoint getCentreBB(IplImage *yuv, IplImage *binary)
{
	BBBinary(yuv, binary);
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
	int count =0;
	CvPoint centre = cvPoint(0,0);
	FOR(i, binary->height)
	{
		FOR(j, binary->width)
		{
			if(*DATA(binary, i, j))
			{
				count++;
				centre = cvPoint(centre.x + j, centre.y + i);
			}
		}
	}
	if(!count)
	{
		printf("ShitBack\n");
		return NULLPOINT;
	}
	centre = cvPoint(centre.x/count, centre.y/count);
	return centre;
}

CvPoint operator+(CvPoint p1, CvPoint p2)
{
	return cvPoint(p1.x + p2.x, p1.y + p2.y);
}

CvPoint operator-(CvPoint p1, CvPoint p2)
{
	return cvPoint(p1.x-p2.x, p1.y-p2.y);
}

CvPoint operator*(CvPoint p, int t)
{
	return cvPoint(p.x*t, p.y*t);
}

CvPoint operator/(CvPoint p, double t)
{
	return cvPoint((int)p.x/t, (int)p.y/t);
}