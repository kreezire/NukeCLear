
#include "stdafx.h"
#include "defines.h"
#include "calibration.h"
#include "square_detection.h"
#include <cv.h>

using namespace std;

bool operator==(CvPoint p1, CvPoint p2)
{
	if(p1.x==p2.x && p1.y==p2.y)
		return true;
	return false;
}
bool operator!=(CvPoint p1, CvPoint p2)
{
	if(p1==p2)
		return 0;
	return 1;
}
int distanceSqr(CvPoint p1, CvPoint p2)
{
	return ((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));
}

inline int _brushfire(int i, int j, int ibound, int jbound, int curValue, ::Mat &arr)
{
	if(CHECKWITHINBOUNDS(i, j, ibound, jbound))
	{
		if(arr[i][j] == BF_NOTINIT)
		{
			arr[i][j] = curValue+1;
			return 1;
		}		
	}
	return 0;
}
void brushfire(IplImage* finalImg, Mat &arr) // BF_WALL needs to be 0!
{
	FOR(i, finalImg->height)
	{
		FOR(j, finalImg->width)
		{
			if(*DATA(finalImg, i, j) == WHITE)
				arr[i][j] = BF_NOTINIT;
			else
				arr[i][j] = BF_WALL;
			if(i==0 || j==0 || i==finalImg->height-1 || j==finalImg->width-1)
			{
				arr[i][j] = BF_WALL;
			}
		}
	}
	int curValue = BF_WALL;
	int flag=0;
	while(1)
	{
		flag=0;
		FOR(i, finalImg->height)
		{
			FOR(j, finalImg->width)
			{
				if(arr[i][j] == curValue)
				{
					flag|=_brushfire(i-1, j, finalImg->height, finalImg->width, curValue, arr);
					flag|=_brushfire(i+1, j, finalImg->height, finalImg->width, curValue, arr);
					flag|=_brushfire(i, j-1, finalImg->height, finalImg->width, curValue, arr);
					flag|=_brushfire(i, j+1, finalImg->height, finalImg->width, curValue, arr);
					flag|=_brushfire(i-1, j-1, finalImg->height, finalImg->width, curValue, arr);
					flag|=_brushfire(i+1, j+1, finalImg->height, finalImg->width, curValue, arr);
					flag|=_brushfire(i+1, j-1, finalImg->height, finalImg->width, curValue, arr);
					flag|=_brushfire(i-1, j+1, finalImg->height, finalImg->width, curValue, arr);
				}
			}
		}
		if(!flag)
			break;
		curValue++;
	}
}
void backtrack(Mat &visitedArr, int gx, int gy, vector<char> &path)
{
	int value = visitedArr[gy][gx]-1;	
	vector <char> tempPath;
	char pushArr[3][3] = {{SE, SOUTH, SW},{EAST, 0, WEST},{NE, NORTH, NW}};
	int contFlag = 0;
	while(value!=0)
	{		
		contFlag=0;
		FOR(i,3)
		{
			FOR(j,3)
			{
				if(i==j && j==1)
					continue;
				if(CHECKWITHINBOUNDS(gx-1+j, gy-1+i, visitedArr[0].size(), visitedArr.size()))
				{				
				//	printf("%c %d shitty\n", pushArr[i][j], value);
					if(visitedArr[gy-1+i][gx-1+j] == value)
					{				
						printf("%c %d\n", pushArr[i][j], value);
						tempPath.push_back(pushArr[i][j]);
						gy=gy-1+i;
						gx= gx-1+j;
						contFlag=1;
						value--;
						break;
					}
				}				
			}
			if(contFlag==1)
				break;
		}
	}
	reverse(tempPath.begin(), tempPath.end());
	path.insert(path.end(), tempPath.begin(), tempPath.end());
}
inline void _checkConnectivity(CvPoint p, Mat &arrBF, Mat &visitedArr, int sourceVal, queue<CvPoint> &q)
{	
	if(CHECKWITHINBOUNDS(p.x, p.y, arrBF[0].size(), arrBF.size()))
	{		
		if(visitedArr[p.y][p.x] == -1 && arrBF[p.y][p.x] >= CLEARANCEVAL)
		{
			q.push(cvPoint(p.x, p.y));
			visitedArr[p.y][p.x] = sourceVal + 1;
			return;
		} 
		if(visitedArr[p.y][p.x] == -1 )
		{
			visitedArr[p.y][p.x] = -2;
		}
	}
}
void getPathP1P2(CvPoint p1, CvPoint p2, Mat &arrBF, vector<char> &path)//WORKS IN BLOCK!!!
{
	queue<CvPoint> q;
	Mat visitedArr(arrBF.size(), Row(arrBF[0].size(), -1));	
	visitedArr[p1.y][p1.x] =0;
	q.push(p1);
	int foundFlag=0;
	CvPoint curPoint;
	int gx, gy;
	while(q.size())
	{
		curPoint = q.front();
		q.pop();
		
		if(curPoint == p2)
		{
			foundFlag=1;
			break;
		}
		gx = curPoint.x;
		gy = curPoint.y;
		_checkConnectivity(cvPoint(gx-1, gy), arrBF, visitedArr, visitedArr[gy][gx], q);
		_checkConnectivity(cvPoint(gx+1, gy), arrBF, visitedArr, visitedArr[gy][gx], q);
		_checkConnectivity(cvPoint(gx, gy-1), arrBF, visitedArr, visitedArr[gy][gx], q);
		_checkConnectivity(cvPoint(gx, gy+1), arrBF, visitedArr, visitedArr[gy][gx], q);

		_checkConnectivity(cvPoint(gx-1, gy-1), arrBF, visitedArr, visitedArr[gy][gx], q);
		_checkConnectivity(cvPoint(gx+1, gy+1), arrBF, visitedArr, visitedArr[gy][gx], q);
		_checkConnectivity(cvPoint(gx+1, gy-1), arrBF, visitedArr, visitedArr[gy][gx], q);
		_checkConnectivity(cvPoint(gx-1, gy+1), arrBF, visitedArr, visitedArr[gy][gx], q);
	}
	//
	//FOR(i, visitedArr.size())
	//{
	//	FOR(j, visitedArr[i].size())
	//	{
	//		printf("%d ", visitedArr[i][j]);
	//	}
	//	printf("\n");
	//}
	if(!foundFlag)
		return;
	backtrack(visitedArr, p2.x, p2.y, path);
	return;
}
void getPathP1P2FullRes(CvPoint p1, CvPoint p2, Mat &arrBF, vector<char> &path)
{
	getPathP1P2(FULLRESTOBLOCK(p1), FULLRESTOBLOCK(p2), arrBF, path);
}
void drawPath(IplImage* img, vector<char> &path, CvPoint startPoint)//img = original image size, startPoint = wrt BlockImage
{	
	CvPoint curPoint = cvPoint(startPoint.x*BLOCKSIZE, startPoint.y*BLOCKSIZE); //curPoint  = topleft corner of cell
	FOR(i, path.size())
	{
		FOR(k, BLOCKSIZE)
		{
			FOR(j, BLOCKSIZE)
			{
				*DATA(img, curPoint.y+k, curPoint.x+j) = DRAWPATH_B;
				*(DATA(img, curPoint.y+k, curPoint.x+j)+1) = DRAWPATH_G;
				*(DATA(img, curPoint.y+k, curPoint.x+j)+2) = DRAWPATH_R;
			}
		}
		switch (path[i])
		{
		case SOUTH:curPoint.y+=BLOCKSIZE;break;
		case NORTH:curPoint.y-=BLOCKSIZE;break;
		case EAST:curPoint.x+=BLOCKSIZE;break;
		case WEST:curPoint.x-=BLOCKSIZE;break;

		case SE:curPoint.x+=BLOCKSIZE;curPoint.y+=BLOCKSIZE;break;
		case NE:curPoint.x+=BLOCKSIZE;curPoint.y-=BLOCKSIZE;break;
		case NW:curPoint.x-=BLOCKSIZE;curPoint.y-=BLOCKSIZE;break;
		case SW:curPoint.x-=BLOCKSIZE;curPoint.y+=BLOCKSIZE;break;
		}
	}
}
/**
 * Draws a line between two points p1(p1x,p1y) and p2(p2x,p2y).
 * This function is based on the Bresenham's line algorithm and is highly 
 * optimized to be able to draw lines very quickly. There is no floating point 
 * arithmetic nor multiplications and divisions involved. Only addition, 
 * subtraction and bit shifting are used. 
 *
 * Note that you have to define your own customized setPixel(x,y) function, 
 * which essentially lights a pixel on the screen.
 */
void lineBresenham(CvPoint p1, CvPoint p2, IplImage* img)
{
    int F, x, y;
	int &p1x = p1.x;
	int &p1y = p1.y;
	int &p2x = p2.x;
	int &p2y = p2.y;
//	p1x*=BLOCKSIZE;p1y*=BLOCKSIZE;p2x*=BLOCKSIZE;p2y*=BLOCKSIZE;
    if (p1x > p2x)  // Swap points if p1 is on the right of p2
    {
        swap(p1x, p2x);
        swap(p1y, p2y);
    }

    // Handle trivial cases separately for algorithm speed up.
    // Trivial case 1: m = +/-INF (Vertical line)
    if (p1x == p2x)
    {
        if (p1y > p2y)  // Swap y-coordinates if p1 is above p2
        {
            swap(p1y, p2y);
        }

        x = p1x;
        y = p1y;
        while (y <= p2y)
        {
           /* setPixel(x, y);*/
			*DATA(img, y, x) = 0;
            y++;
        }
        return;
    }
    // Trivial case 2: m = 0 (Horizontal line)
    else if (p1y == p2y)
    {
        x = p1x;
        y = p1y;

        while (x <= p2x)
        {
            /* setPixel(x, y);*/
			*DATA(img, y, x) = 0;
            x++;
        }
        return;
    }


    int dy            = p2y - p1y;  // y-increment from p1 to p2
    int dx            = p2x - p1x;  // x-increment from p1 to p2
    int dy2           = (dy << 1);  // dy << 1 == 2*dy
    int dx2           = (dx << 1);
    int dy2_minus_dx2 = dy2 - dx2;  // precompute constant for speed up
    int dy2_plus_dx2  = dy2 + dx2;


    if (dy >= 0)    // m >= 0
    {
        // Case 1: 0 <= m <= 1 (Original case)
        if (dy <= dx)   
        {
            F = dy2 - dx;    // initial F

            x = p1x;
            y = p1y;
            while (x <= p2x)
            {
                /* setPixel(x, y);*/
				*DATA(img, y, x) = 0;
                if (F <= 0)
                {
                    F += dy2;
                }
                else
                {
                    y++;
                    F += dy2_minus_dx2;
                }
                x++;
            }
        }
        // Case 2: 1 < m < INF (Mirror about y=x line
        // replace all dy by dx and dx by dy)
        else
        {
            F = dx2 - dy;    // initial F

            y = p1y;
            x = p1x;
            while (y <= p2y)
            {
                /* setPixel(x, y);*/
				*DATA(img, y, x) = 0;
                if (F <= 0)
                {
                    F += dx2;
                }
                else
                {
                    x++;
                    F -= dy2_minus_dx2;
                }
                y++;
            }
        }
    }
    else    // m < 0
    {
        // Case 3: -1 <= m < 0 (Mirror about x-axis, replace all dy by -dy)
        if (dx >= -dy)
        {
            F = -dy2 - dx;    // initial F

            x = p1x;
            y = p1y;
            while (x <= p2x)
            {
                /* setPixel(x, y);*/
				*DATA(img, y, x) = 0;
                if (F <= 0)
                {
                    F -= dy2;
                }
                else
                {
                    y--;
                    F -= dy2_plus_dx2;
                }
                x++;
            }
        }
        // Case 4: -INF < m < -1 (Mirror about x-axis and mirror 
        // about y=x line, replace all dx by -dy and dy by dx)
        else    
        {
            F = dx2 + dy;    // initial F

            y = p1y;
            x = p1x;
            while (y >= p2y)
            {
                /* setPixel(x, y);*/
				*DATA(img, y, x) = 0;
                if (F <= 0)
                {
                    F += dx2;
                }
                else
                {
                    x++;
                    F += dy2_plus_dx2;
                }
                y--;
            }
        }
    }
}

int checkIntersection(CvPoint p1, CvPoint p2, IplImage *bin)
{
    int F, x, y;
	int &p1x = p1.x;
	int &p1y = p1.y;
	int &p2x = p2.x;
	int &p2y = p2.y;
//	p1x*=BLOCKSIZE;p1y*=BLOCKSIZE;p2x*=BLOCKSIZE;p2y*=BLOCKSIZE;
    if (p1x > p2x)  // Swap points if p1 is on the right of p2
    {
        swap(p1x, p2x);
        swap(p1y, p2y);
    }

    // Handle trivial cases separately for algorithm speed up.
    // Trivial case 1: m = +/-INF (Vertical line)
    if (p1x == p2x)
    {
        if (p1y > p2y)  // Swap y-coordinates if p1 is above p2
        {
            swap(p1y, p2y);
        }

        x = p1x;
        y = p1y;
        while (y <= p2y)
        {
			if(*DATA(bin, y, x) == BLACK)
				return 1;
            y++;
        }
        return 0;
    }
    // Trivial case 2: m = 0 (Horizontal line)
    else if (p1y == p2y)
    {
        x = p1x;
        y = p1y;

        while (x <= p2x)
        {
            if(*DATA(bin, y, x) == BLACK)
				return 1;
            x++;
        }
        return 0;
    }


    int dy            = p2y - p1y;  // y-increment from p1 to p2
    int dx            = p2x - p1x;  // x-increment from p1 to p2
    int dy2           = (dy << 1);  // dy << 1 == 2*dy
    int dx2           = (dx << 1);
    int dy2_minus_dx2 = dy2 - dx2;  // precompute constant for speed up
    int dy2_plus_dx2  = dy2 + dx2;


    if (dy >= 0)    // m >= 0
    {
        // Case 1: 0 <= m <= 1 (Original case)
        if (dy <= dx)   
        {
            F = dy2 - dx;    // initial F

            x = p1x;
            y = p1y;
            while (x <= p2x)
            {
				if(*DATA(bin, y, x) == BLACK)
					return 1;
                if (F <= 0)
                {
                    F += dy2;
                }
                else
                {
                    y++;
                    F += dy2_minus_dx2;
                }
                x++;
            }
        }
        // Case 2: 1 < m < INF (Mirror about y=x line
        // replace all dy by dx and dx by dy)
        else
        {
            F = dx2 - dy;    // initial F

            y = p1y;
            x = p1x;
            while (y <= p2y)
            {
                if(*DATA(bin, y, x) == BLACK)
					return 1;
                if (F <= 0)
                {
                    F += dx2;
                }
                else
                {
                    x++;
                    F -= dy2_minus_dx2;
                }
                y++;
            }
        }
    }
    else    // m < 0
    {
        // Case 3: -1 <= m < 0 (Mirror about x-axis, replace all dy by -dy)
        if (dx >= -dy)
        {
            F = -dy2 - dx;    // initial F

            x = p1x;
            y = p1y;
            while (x <= p2x)
            {
                if(*DATA(bin, y, x) == BLACK)
					return 1;
                if (F <= 0)
                {
                    F -= dy2;
                }
                else
                {
                    y--;
                    F -= dy2_plus_dx2;
                }
                x++;
            }
        }
        // Case 4: -INF < m < -1 (Mirror about x-axis and mirror 
        // about y=x line, replace all dx by -dy and dy by dx)
        else    
        {
            F = dx2 + dy;    // initial F

            y = p1y;
            x = p1x;
            while (y >= p2y)
            {
                if(*DATA(bin, y, x) == BLACK)
					return 1;
                if (F <= 0)
                {
                    F += dx2;
                }
                else
                {
                    x++;
                    F += dy2_plus_dx2;
                }
                y--;
            }
        }
    }
	return 0;
}


void getOffsetPoints(CvPoint src, CvPoint dest, int rad, CvPoint &s1, CvPoint &d1, CvPoint &s2, CvPoint &d2) //
{
	rad/=2;
	if(dest.y == src.y)
	{
		s1 = cvPoint(src.x, src.y+rad);
		s2 = cvPoint(src.x, src.y-rad);
		d1 = cvPoint(dest.x, dest.y+rad);
		d2 = cvPoint(dest.x, dest.y -rad);
		return;
	}
	double angle= atan2(((double)dest.y - (double)src.y), ((double)dest.x - (double)src.x));
	angle+=PI/2;
	s1 = cvPoint(src.x+rad*cos(angle), src.y +rad*sin(angle));
	d1 = cvPoint(dest.x +rad*cos(angle), dest.y +rad*sin(angle));
	angle-=PI;
	s2 = cvPoint(src.x+rad*cos(angle), src.y +rad*sin(angle));
	d2 = cvPoint(dest.x +rad*cos(angle), dest.y +rad*sin(angle));
	return;
}

void rayCasting(IplImage *img, vector<char> &path, CvPoint startPoint, vector<CvPoint> &nodes, IplImage *end, IplImage *squares)//Pass startPoint in FULL res!!!!!!
{
	 CvPoint curPoint = startPoint, destPoint = startPoint, prevPoint = curPoint, endPoint = startPoint;
	 IplImage *bin = getBinImageFullRes(img, end, squares);
	 nodes.push_back(startPoint);
	 FOR(i, path.size())
	 {
		switch(path[i])
		{
		case SOUTH:endPoint.y+=BLOCKSIZE;break;
		case NORTH:endPoint.y-=BLOCKSIZE;break;
		case EAST:endPoint.x+=BLOCKSIZE;break;
		case WEST:endPoint.x-=BLOCKSIZE;break;

		case SE:endPoint.x+=BLOCKSIZE;endPoint.y+=BLOCKSIZE;break;
		case NE:endPoint.x+=BLOCKSIZE;endPoint.y-=BLOCKSIZE;break;
		case NW:endPoint.x-=BLOCKSIZE;endPoint.y-=BLOCKSIZE;break;
		case SW:endPoint.x-=BLOCKSIZE;endPoint.y+=BLOCKSIZE;break;
		}
	 }
	 FOR(i, path.size())
	 {
		switch(path[i])
		{
		case SOUTH:destPoint.y+=BLOCKSIZE;break;
		case NORTH:destPoint.y-=BLOCKSIZE;break;
		case EAST:destPoint.x+=BLOCKSIZE;break;
		case WEST:destPoint.x-=BLOCKSIZE;break;

		case SE:destPoint.x+=BLOCKSIZE;destPoint.y+=BLOCKSIZE;break;
		case NE:destPoint.x+=BLOCKSIZE;destPoint.y-=BLOCKSIZE;break;
		case NW:destPoint.x-=BLOCKSIZE;destPoint.y-=BLOCKSIZE;break;
		case SW:destPoint.x-=BLOCKSIZE;destPoint.y+=BLOCKSIZE;break;
		}
		CvPoint s1,s2,d1,d2;
		getOffsetPoints(curPoint, endPoint, RAYCASTWIDTH, s1, d1, s2, d2);		
		if(!(checkIntersection(curPoint, destPoint, bin) || checkIntersection(s1, d1, bin) || checkIntersection(s2, d2, bin)))
		{
			//lineBresenham(curPoint, endPoint, img);////
			//lineBresenham(s1, d1, img);///
			//lineBresenham(s2, d2, img);////
			nodes.push_back(endPoint);
			return;
		}
		getOffsetPoints(curPoint, destPoint, RAYCASTWIDTH, s1, d1, s2, d2);	
		if(checkIntersection(curPoint, destPoint, bin) || checkIntersection(s1, d1, bin) || checkIntersection(s2, d2, bin))
		{
			//lineBresenham(curPoint, destPoint, img);//
			//lineBresenham(s1, d1, img);///
			//lineBresenham(s2, d2, img);////
			nodes.push_back(prevPoint);
			curPoint = destPoint;
			prevPoint = destPoint;
			continue;
		}
		prevPoint = destPoint;
	 }
	/* if(distanceSqr(nodes.back(), destPoint)>MINDISTBTWNODESINPIXELS*MINDISTBTWNODESINPIXELS)
		 nodes.push_back(destPoint);*/
}

inline void _searchForNearestTraversable(CvPoint p, Mat &arrBF, MatC &visitedArr, queue<CvPoint> &q)
{
	if(CHECKWITHINBOUNDS(p.x,p.y,arrBF[0].size(),arrBF.size()))
	{		
		if(!visitedArr[p.y][p.x])
		{
			q.push(p);			
		}
		visitedArr[p.y][p.x] = 1;			
	}
}

CvPoint searchForNearestTraversable(Mat &arrBF, CvPoint p)//Performs in BLOCKSIZE!!!!
{
	CvPoint curPoint = p;
	MatC visitedArr(arrBF.size(), RowC(arrBF[0].size(), 0));
	visitedArr[p.y][p.x] = 1;
	queue<CvPoint> q;
	q.push(p);
	/*FOR(i,arrBF.size())
	{
		FOR(j, arrBF[0].size())
		{
			printf("%d ", arrBF[i][j]);
		}
		printf("\n");
	}*/
	while(q.size())
	{
		curPoint = q.front();
//		printf("Point: %d %d. Value= %d.\n", curPoint.x, curPoint.y, arrBF[curPoint.y][curPoint.x]);
		if(arrBF[curPoint.y][curPoint.x] >=CLEARANCEVALFORBOMBNEAREST)
			return curPoint;
		
		q.pop();
		_searchForNearestTraversable(cvPoint(curPoint.x-1, curPoint.y), arrBF, visitedArr, q);
		_searchForNearestTraversable(cvPoint(curPoint.x+1, curPoint.y), arrBF, visitedArr, q);
		_searchForNearestTraversable(cvPoint(curPoint.x, curPoint.y-1), arrBF, visitedArr, q);
		_searchForNearestTraversable(cvPoint(curPoint.x, curPoint.y+1), arrBF, visitedArr, q);

		_searchForNearestTraversable(cvPoint(curPoint.x-1, curPoint.y-1), arrBF, visitedArr, q);
		_searchForNearestTraversable(cvPoint(curPoint.x+1, curPoint.y+1), arrBF, visitedArr, q);
		_searchForNearestTraversable(cvPoint(curPoint.x+1, curPoint.y-1), arrBF, visitedArr, q);
		_searchForNearestTraversable(cvPoint(curPoint.x-1, curPoint.y+1), arrBF, visitedArr, q);
	}
	return NULLPOINT;
}

void justDoIt(IplImage *img, CvPoint startPoint, CvPoint endPoint, vector<SquareWConf> &listOfSquares, Mat &arrBF, vector<NodeWFlag> &listOfNodes, IplImage *end, IplImage *squares)//start-end =  FUll RES! 
{
	vector<NodeWFlag> listOfBombNearests;
	
	FOR(i, listOfSquares.size())
	{
		listOfBombNearests.push_back(NodeWFlag(BLOCKTOFULLRES(searchForNearestTraversable(arrBF, FULLRESTOBLOCK(listOfSquares[i].first.first))), TYPE_BOMB));
	}
//	listOfNodes.push_back(startPoint, TYPE_NORMAL);
	NodeWFlag destNode, destBombNode;
	destNode = listOfBombNearests.front();
	NodeWFlag curNode = NodeWFlag(startPoint, TYPE_NORMAL);
	vector <CvPoint> justNodes;
	vector<NodeWFlag> tempListOfNodes;
	vector<char> path;
	int curMinDistance = 0;
	while(listOfBombNearests.size())
	{
		destNode = listOfBombNearests[0];		
		curMinDistance = distanceSqr(curNode.first, destNode.first);
		int minIndex = 0;
		FOR(i, listOfBombNearests.size())
		{
			if(distanceSqr(curNode.first, listOfBombNearests[i].first) < curMinDistance)
			{
				minIndex = i;				
				curMinDistance = distanceSqr(curNode.first, listOfBombNearests[i].first);
			}
		}
		destNode = listOfBombNearests[minIndex];
		destBombNode = NodeWFlag(listOfSquares[minIndex].first.first, TYPE_BOMBACTUAL);
		listOfSquares.erase(listOfSquares.begin() + minIndex);
		listOfBombNearests.erase(listOfBombNearests.begin() + minIndex);
		getPathP1P2FullRes(curNode.first, destNode.first, arrBF, path);
		rayCasting(img, path, curNode.first, justNodes, end, squares);
		tempListOfNodes.push_back(curNode);
		justNodes.erase(justNodes.begin());
		FOR(i, justNodes.size()-1)
		{
			tempListOfNodes.push_back(NodeWFlag(justNodes[i], TYPE_NORMAL));
		}
		tempListOfNodes.push_back(destNode);
		tempListOfNodes.push_back(destBombNode);
		listOfNodes.insert(listOfNodes.end(), tempListOfNodes.begin(), tempListOfNodes.end());
		curNode = destNode;
		path.clear();
		justNodes.clear();
		tempListOfNodes.clear();
	}
	getPathP1P2FullRes(curNode.first, endPoint, arrBF, path);	
	rayCasting(img, path, curNode.first, justNodes, end, squares);

	tempListOfNodes.push_back(curNode);
	justNodes.erase(justNodes.begin());	
	FOR(i, justNodes.size()-1)
	{
		tempListOfNodes.push_back(NodeWFlag(justNodes[i], TYPE_NORMAL));
	}
	tempListOfNodes.push_back(NodeWFlag(endPoint, TYPE_END));
	listOfNodes.insert(listOfNodes.end(), tempListOfNodes.begin(), tempListOfNodes.end());
	cvNamedWindow("Path");
	FOR(i, listOfNodes.size())
	{
		switch(listOfNodes[i].second)
		{
		case TYPE_NORMAL:cvCircle(img, listOfNodes[i].first, 5, CV_RGB(200,0,0));break;
		case TYPE_BOMB:cvCircle(img, listOfNodes[i].first, 5, CV_RGB(200,200,0));break;
		case TYPE_END:cvCircle(img, listOfNodes[i].first, 5, CV_RGB(0,0,200));break;
		case TYPE_BOMBACTUAL:cvCircle(img, listOfNodes[i].first, 5, CV_RGB(0,200,200));break;
		}
	}
	cvShowImage("Path", img);
	cvWaitKey(0);
	
}
