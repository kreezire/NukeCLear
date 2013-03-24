#pragma once

#include "stdafx.h"
#include "defines.h"
#include "tserial.h"
#include "bot_control.h"



#define CHARGETIME 1000
#define BACKTIME 1000
#define CAMINDEXDEFUSAL 2


extern int CENTERWIDTH ;
extern int UTOLERANCE, VTOLERANCE;
extern int EROSION, DILATION;
extern int BOMB_U, BOMB_V;
extern CvCapture *gCaptureDefusal;

void forward(serial &comm);
void backward(serial &comm);
void rotate_left(serial &comm);
void rotate_right(serial &comm);

void convert2binary (IplImage *yuv, IplImage *binary, int U, int V);
IplImage *imgrotate (IplImage *frame);
void defuse_bomb(serial &comm);
void mouse_callback_bomb(int event, int x, int y, int flags, void* param);
