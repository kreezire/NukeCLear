
#include "stdafx.h"
#include "defines.h"
#include "square_detection.h"

int gSquareConfidenceThreshold  = 545;
int gCircleConfidenceThreshold = 50;


void blobs(IplImage *source, vector<CvPoint>& listOfCentres, vector <CircleWConf> &listOfCircles,vector <SquareWConf> &listOfSquares)//assuming source has size <= visited
{	
	Mat visited(source->height, Row(source->width, 0));
	vector <int> hCircle(source->width + source->height,0);	
	vector<vector<int>> hSquare(max(source->width,source->height), vector<int> (90,0));
	IplImage* imgEdgesOfBlob = cvCreateImage(cvSize(source->width, source->height), 8, 1);		
	queue <CvPoint> q;
	int label=2;
	int gi=0, gj=0, flag=0, i=0, j=0, countOfPixels=0, sumI =0, sumJ =0 ;
	while(1)
	{		
		flag=0;
		for(;gi<source->height;gi++)
		{
			for(gj = gj%source->width;gj<source->width;gj++)
			{
				if(visited[gi][gj])
					continue;
				if(*DATA(source,gi,gj))
				{
					flag=1;
					break;
				}
				visited[gi][gj]=1;
			}
			if(flag)
				break;
		}
		if(gi==source->height)
			break;
		q.push(cvPoint(gj,gi));
		i=gi;j=gj;
		countOfPixels=0;
		sumI =0 ; sumJ =0 ;
		visited[i][j]=1;
		vector <CvPoint> edgePoints;
		cvZero(imgEdgesOfBlob);
		while(!q.empty())
		{
			i=q.front().y;
			j=q.front().x;
			q.pop();			
			sumI+=i;
			sumJ+=j;
			countOfPixels++;
			flag=0;
			if(ISVALID(i-1,j,source->height, source->width) && !visited[i-1][j] && *DATA(source, i-1, j)) {q.push(cvPoint(j,i-1)); visited[i-1][j]=1;} 
			if(ISVALID(i+1,j,source->height, source->width) && !visited[i+1][j] && *DATA(source, i+1, j)) {q.push(cvPoint(j,i+1)); visited[i+1][j]=1;}
			if(ISVALID(i,j-1,source->height, source->width) && !visited[i][j-1] && *DATA(source, i, j-1)) {q.push(cvPoint(j-1,i)); visited[i][j-1]=1;}
			if(ISVALID(i,j+1,source->height, source->width) && !visited[i][j+1] && *DATA(source, i, j+1)) {q.push(cvPoint(j+1,i)); visited[i][j+1]=1;}

			if(ISVALID(i-1,j-1,source->height, source->width) && !visited[i-1][j-1] && *DATA(source, i-1, j-1)) {q.push(cvPoint(j-1,i-1)); visited[i-1][j-1]=1;} 
			if(ISVALID(i+1,j+1,source->height, source->width) && !visited[i+1][j+1] && *DATA(source, i+1, j+1)) {q.push(cvPoint(j+1,i+1)); visited[i+1][j+1]=1;}
			if(ISVALID(i+1,j-1,source->height, source->width) && !visited[i+1][j-1] && *DATA(source, i+1, j-1)) {q.push(cvPoint(j-1,i+1)); visited[i+1][j-1]=1;}
			if(ISVALID(i-1,j+1,source->height, source->width) && !visited[i-1][j+1] && *DATA(source, i-1, j+1)) {q.push(cvPoint(j+1,i-1)); visited[i-1][j+1]=1;}

			if(ISVALID(i-1,j,source->height, source->width)) visited[i-1][j]=1;
			if(ISVALID(i+1,j,source->height, source->width)) visited[i+1][j]=1;
			if(ISVALID(i,j-1,source->height, source->width)) visited[i][j-1]=1;
			if(ISVALID(i,j+1,source->height, source->width)) visited[i][j+1]=1;

			if(ISVALID(i-1,j-1,source->height, source->width)) visited[i-1][j-1]=1;
			if(ISVALID(i+1,j+1,source->height, source->width)) visited[i+1][j+1]=1;
			if(ISVALID(i+1,j-1,source->height, source->width)) visited[i+1][j-1]=1;
			if(ISVALID(i-1,j+1,source->height, source->width)) visited[i-1][j+1]=1;

			if(ISVALID(i-1,j,source->height, source->width) && !*DATA(source, i-1, j)) flag=1;
			else if(ISVALID(i+1,j,source->height, source->width) && !*DATA(source, i+1, j)) flag=1;
			else if(ISVALID(i,j-1,source->height, source->width) && !*DATA(source, i, j-1)) flag=1;
			else if(ISVALID(i,j+1,source->height, source->width) && !*DATA(source, i, j+1)) flag=1;

			else if(ISVALID(i-1,j-1,source->height, source->width) && !*DATA(source, i-1, j-1)) flag=1;
			else if(ISVALID(i+1,j+1,source->height, source->width) && !*DATA(source, i+1, j+1)) flag=1;
			else if(ISVALID(i+1,j-1,source->height, source->width) && !*DATA(source, i+1, j-1)) flag=1;
			else if(ISVALID(i-1,j+1,source->height, source->width) && !*DATA(source, i-1, j+1)) flag=1;
			if(flag)
			{
			//	*DATA(imgEdgesOfBlob,i,j) = 255;
				edgePoints.push_back(cvPoint(j,i));
			}
		//	arr[i][j]=label;
		}
		label++;		
		listOfCentres.push_back(cvPoint(sumJ/countOfPixels, sumI/countOfPixels));		
		hCircle.assign(source->width + source->height,0);
		FOR(i,hSquare.size())
			FOR(j,hSquare[0].size())
				hSquare[i][j]=0;
//		hSquare.assign(max(source->width, source->height), vector<int>(90,0));
		hough(source,edgePoints, listOfCentres.back(), hCircle, listOfCircles, hSquare, listOfSquares);
		
	//	printf("Label %d: %d %d\n", label-1, sumI/countOfPixels, sumJ/countOfPixels);

	}
}


void hough(IplImage* source,  vector<CvPoint> &edgePoints, CvPoint centre, vector <int> &hCircle, vector<CircleWConf> &listOfCircles, vector<vector<int>> &hSquare, vector<SquareWConf> &listOfSquares)
{
		
	
	CvPoint temp, relative, temp3;
	int noOfEdgePoints = edgePoints.size();
	int circleConf = 10, squareConf = 0;
	double angle=0;
	while(edgePoints.size())
	{
		temp = edgePoints.back();
		relative = cvPoint(temp.x-centre.x, temp.y - centre.y);
		edgePoints.pop_back();	
		hCircle[HOUGHCIRCLE(temp, centre)]++;
		FOR(theta,90)
		{
			if(QUAD1(relative))
				temp3 = relative;
			else if(QUAD2(relative))
				temp3 = cvPoint(relative.y, -relative.x);
			else if(QUAD3(relative))
				temp3 = cvPoint(-relative.x, -relative.y);
			else if(QUAD4(relative))
				temp3 = cvPoint(-relative.y, relative.x);			
			angle = DEGTORAD(theta);
			hSquare[HOUGHSQUARE(temp3, angle)][theta]++;
		}
	}
	int maxCoordCircle=0;
	CvPoint maxCoordSquare = cvPoint(0,0);
	FOR(i,hCircle.size()-2)
	{
		if(hCircle[maxCoordCircle]+hCircle[maxCoordCircle+1] + hCircle[maxCoordCircle+2]<hCircle[i] + hCircle[i+1] + hCircle[i+2])
			maxCoordCircle = i+1;
	}
	int sumSquare=0, maxsumSquare=0;
	FOR(r,hSquare.size()-5)
	{
		FOR(theta, hSquare[0].size()-5)
		{
			sumSquare=0;
			FOR(i,5)
				FOR(j,5)
					sumSquare+=hSquare[r+i][theta+j];
			if(maxsumSquare<sumSquare)
			{
				maxCoordSquare = cvPoint(r+2, theta+2);
				maxsumSquare=sumSquare;
			}
		}
	}
	pair<int,int> mCSquareSorry = make_pair(maxCoordSquare.x, (maxCoordSquare.y));
	circleConf = ((hCircle[maxCoordCircle]+hCircle[maxCoordCircle+1]+hCircle[maxCoordCircle+2])*CONFIDENCEPERCENTRATIO)/noOfEdgePoints;
	squareConf = ((maxsumSquare*CONFIDENCEPERCENTRATIO*SQUARECONFIDENCESCALEFACTOR)/noOfEdgePoints)/90;
//	printf("%d\n",squareConf);
	if(circleConf > gCircleConfidenceThreshold) listOfCircles.push_back(make_pair(make_pair(centre, maxCoordCircle), circleConf ));
	if(squareConf > gSquareConfidenceThreshold) listOfSquares.push_back(make_pair(make_pair(centre, mCSquareSorry), squareConf));
	return ;
	
} 


void drawSquare(IplImage *img, Square sq)
{
	int &r = sq.second.first;
	int &theta = sq.second.second;
//  int theta  = 20;
	int &x = sq.first.x;
	int &y = sq.first.y;
	int rCosPlusSin =  (int)((float)r*((float)cos(DEGTORAD(theta)) + (float)sin(DEGTORAD(theta))));
	int rCosMinusSin=  (int)((float)r*((float)cos(DEGTORAD(theta)) - (float)sin(DEGTORAD(theta))));
	cvLine(img, cvPoint(rCosMinusSin+x,rCosPlusSin+y), cvPoint(rCosPlusSin+x, -rCosMinusSin+y), BOMBDRAWCOLOR);
	cvLine(img, cvPoint(rCosMinusSin+x,rCosPlusSin+y), cvPoint(-rCosPlusSin+x, rCosMinusSin+y), BOMBDRAWCOLOR);
	cvLine(img, cvPoint(-rCosMinusSin+x,-rCosPlusSin+y), cvPoint(rCosPlusSin+x, -rCosMinusSin+y), BOMBDRAWCOLOR);
	cvLine(img, cvPoint(-rCosMinusSin+x,-rCosPlusSin+y), cvPoint(-rCosPlusSin+x, rCosMinusSin+y), BOMBDRAWCOLOR);
}

