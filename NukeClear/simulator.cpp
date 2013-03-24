#include "stdafx.h"
#include "defines.h"
#include "simulator.h"
#include "defines.h"


double sAngle = 0;
CvPoint sCentre;

void sInitSimBot(CvPoint centre)
{
	sCentre = centre;
}

void sDisplayBot(IplImage* img)
{
	CvPoint p1, p2;
	//printf("%lf, %lf",(double)SRADIALDIST*cos(sAngle)+sCentre.x, (double)SRADIALDIST*sin(sAngle) + sCentre.y); 
	p1 = cvPoint((int)(double)SRADIALDIST*cos(sAngle)+sCentre.x, (int)(double)SRADIALDIST*sin(sAngle) + sCentre.y);
	p2 = cvPoint(-(double)SRADIALDIST*cos(sAngle)+sCentre.x, -(double)SRADIALDIST*sin(sAngle) + sCentre.y);
	cvCircle(img,p1,SCIRCLERADIUS,SCOLORFRONT,-1);
	cvCircle(img,p2,SCIRCLERADIUS,SCOLORBACK,-1);
}

void sForward()
{
	sCentre = cvPoint(sCentre.x + (double)SBOTFORWARD*cos(sAngle), sCentre.y + (double)SBOTFORWARD*sin(sAngle));
}
void sRotateCC()
{
	sAngle += SBOTANGLE;
	if(sAngle>2*PI)
		sAngle-=2*PI;
}

void sRotateC()
{
	sAngle -= SBOTANGLE;
	if(sAngle<0)
		sAngle+=2*PI;
}