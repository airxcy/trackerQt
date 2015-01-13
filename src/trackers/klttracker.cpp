#include "trackers/klttracker.h"

#include <cmath>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
#define REPLACE
#define MAX_FEATURE_NUMBER  5000
#define PI 3.14159265
#define dirN 8

unsigned char _line_colos[6][3] =
{
	{0,255,0},
	{0,0,255},
	{255,255, 0},
	{255,0,255},
	{0,255,255},
	{255,0,0},
};
double directions[dirN][2];
char strbuff[100];

int KLTtracker::init(int bsize,int w,int h)
{
	
	nFeatures = 1000;
	FOREGROUND_THRESHOLD = 0;
	isTracking = (int*)malloc(sizeof(int) * nFeatures);
	trkIndex = (int*)malloc(sizeof(int) * nFeatures);
	trackBuff=std::vector<TrackBuff>(nFeatures);
	for (int i=0;i<nFeatures;i++)
	{
		isTracking[i]=0;
		trkIndex[i]=0;
		//trackBuff[i].init(3,10);
		trackBuff[i].init(1,100);
	}
	frame_width = w;
	frame_height = h;
	frameidx=0;
	tc = KLTCreateTrackingContext();
	fl = KLTCreateFeatureList(nFeatures);
	tc->sequentialMode = TRUE;
	tc->writeInternalImages = FALSE;
	tc->affineConsistencyCheck = -1;  /* set this to 2 to turn on affine consistency check */
	//tc->min_determinant=0.01;
	tc->window_width=7;
	tc->window_height=7;
	//tc->lighting_insensitive=TRUE;
	KLTSetVerbosity(0);
	drawW=frame_width,drawH=frame_height;
	bgMod=0;
	delay=0;
	gt_inited = false;
    for(int di=0;di<dirN;di++)
    {
        double rad=((double)di)/((double)dirN)*PI*2;
        directions[di][0]=cos(rad);
        directions[di][1]=sin(rad);
    }
    gt_N=-1;
	return 1;
}
int KLTtracker::selfinit(unsigned char* framedata)
{
	curframedata=framedata;
	bgdata = new unsigned char[frame_width*frame_height];
	memset(bgdata,0,frame_width*frame_height);
	preframedata = new unsigned char[frame_width*frame_height];
	memcpy(preframedata,framedata,frame_width*frame_height);
	KLTSelectGoodFeatures(tc, framedata, frame_width, frame_height, fl);
	KLTUpdateTCBorder(tc);
	for (int k = 0; k < nFeatures; k++)
	{
		if(true)// (checkFG( x, y))
		{
            pttmp.x = (PntT)(fl->feature[k]->x + 0.5);
            pttmp.y = (PntT)(fl->feature[k]->y + 0.5);
            pttmp.t = frameidx;
			trackBuff[k].updateAFrame(&pttmp);
		}
	}

	return true;
}
int checkinBB(REAL x,REAL y,REAL left,REAL top,REAL right,REAL bottom)
{
    //std::cout<<x<<","<<y<<","<<top<<","<<left<<","<<right<<","<<bottom<<"|";
    return (x<=right&&x>=left&&y<=bottom&&y>=top);
}
int KLTtracker::initGT(string & gtfilename)
{
	std::cout << gtfilename << std::endl;
	if (gtfilename.length()>1)
	{
		//goodFeaturesToTrack(gray, points[0], MAX_COUNT, 0.01, 0, Mat(), 10, 0, 0.04);
		char strbuf[100];
		int x, y;
        int pos[2], bb[4];
		std::ifstream txtfile(gtfilename);
		if (txtfile.is_open())
		{
			txtfile.getline(strbuf, 100);
			bbw = 40, bbh = 100;
			sscanf_s(strbuf, "%d", &gt_N);
			std::cout << gtfilename << "gt:" << gt_N << std::endl;
            targetLoc.init(2* gt_N,10);
            targetBB.init(4*gt_N,10);
            vector<REAL> locVec(2* gt_N), bbVec(4* gt_N);
			for (int gt_i = 0; gt_i < gt_N; gt_i++)
			{
				txtfile.getline(strbuf, 100);
				sscanf_s(strbuf, "%d,%d", pos, pos + 1);

				bb[0] = pos[0] - bbw / 2, bb[1] = pos[1] - bbh / 10, bb[2] = pos[0] + bbw / 2, bb[3] = pos[1] + bbh * 9 / 10;
                //std::cout << pos[0] <<","<<pos[1]<< std::endl;
                locVec[gt_i*2]=pos[0],locVec[gt_i*2+1]=pos[1];
                bbVec[gt_i*4]=bb[0],bbVec[gt_i*4+1]=bb[1],bbVec[gt_i*4+2]=bb[2],bbVec[gt_i*4+3]=bb[3];
			}
            targetLoc.updateAFrame(locVec.data());
            targetBB.updateAFrame(bbVec.data());
            bbxft=new unsigned char[gt_N*nFeatures];
            relPos=Map3D<REAL>(gt_N,nFeatures,2);
            for(int bb_i=0;bb_i<gt_N;bb_i++)
            {
                for(int ft_i=0;ft_i<nFeatures;ft_i++)
                {
                    //bb[0]=bbVec[gt_i*4],bb[1]=bbVec[gt_i*4+1],bb[2]=bbVec[gt_i*4+2],bb[3]=bbVec[gt_i*4+3];
                    PntT x=trackBuff[ft_i].cur_frame_ptr->x;
                    PntT y=trackBuff[ft_i].cur_frame_ptr->y;
                    if(checkinBB(x, y, bbVec[bb_i*4],bbVec[bb_i*4+1],bbVec[bb_i*4+2],bbVec[bb_i*4+3]))
                    {
                        bbxft[bb_i*nFeatures+ft_i]=1;
                        relPos(bb_i,ft_i,0)=x-bbVec[bb_i*4];
                        relPos(bb_i,ft_i,1)=y-bbVec[bb_i*4+1];
                    }
                    else
                        bbxft[bb_i*nFeatures+ft_i]=0;
                }
            }
			gt_inited = true;
		}
	}
	return 1;
}

int KLTtracker::initBG(int mode,unsigned char* bgptr)
{
	bgMod=mode;
	if(mode==1&&bgptr!=NULL)
	{
		bgdata=bgptr;
	}
	return true;
}
int KLTtracker::checkFG(int x,int y)
{
	if(bgMod==1)
	{
		return (abs(curframedata[y*frame_width+x]-bgdata[y*frame_width+x])>=FOREGROUND_THRESHOLD);
	}
	else if(bgMod==2)
	{
		return (int)bgdata[y*frame_width+x];
	}
	else 
		return true;
}
void KLTtracker::updateBB()
{

    REAL* bbVec=targetBB.cur_frame_ptr;
    REAL* newbb=(REAL *)malloc(targetBB.frame_byte_size);

    for(int bb_i=0;bb_i<gt_N;bb_i++)
    {
        int dircount[dirN]={};
        double dirscore[dirN]={};
        REAL dx=0,dy=0;
        int dcount=0;
        double relx=0,rely=0;
        //REAL left=bbVec[bb_i*4],top=bbVec[bb_i*4+1],right=bbVec[bb_i*4+2],bottom=bbVec[bb_i*4+3];
        REAL left=frame_width,top=frame_height,right=0,bottom=0;
        REAL maxdx=0,maxdy=0;
        for(int ft_i=0;ft_i<nFeatures;ft_i++)
        {
            if(trackBuff[ft_i].len>1)
            {
                PntT xt=trackBuff[ft_i].cur_frame_ptr->x;
                PntT yt=trackBuff[ft_i].cur_frame_ptr->y;
                PntT xt_1=(trackBuff[ft_i].cur_frame_ptr-1)->x;
                PntT yt_1=(trackBuff[ft_i].cur_frame_ptr-1)->y;
                PntT ddx=xt-xt_1,ddy=yt-yt_1;

                if(bbxft[bb_i*nFeatures+ft_i])//&&ddx!=0&&ddy!=0)
                {
                    /*
                    double maxsc=-99999;
                    int maxidx=0;
                    for(int di=0;di<dirN;di++)
                    {
                        double sc=ddx*directions[di][0]+ddy*directions[di][1];
                        dirscore[di]+=sc;
                        if(sc>maxsc)maxsc=sc,maxidx=di;
                    }
                    dircount[maxidx]++;
                    */
                    if(xt<left)left=xt;
                    if(xt>right)right=xt;
                    if(yt<top)top=yt;
                    if(yt>bottom)bottom=yt;
                    if(sqrt(maxdx*maxdx+maxdy*maxdy)<ddx){maxdx=ddx;maxdy=ddy;}
                    relx=relx+(xt-relPos(bb_i,ft_i,0));
                    rely=rely+(yt-relPos(bb_i,ft_i,1));

                    dx+=ddx,dy+=ddy;
                    dcount++;
                }
            }
        }
        if(dcount>0)
        {
            /*
            int maxcount=-9999,maxidx=0;
            for(int di=0;di<dirN;di++)
            {
                if(dircountr[di]>maxcount)maxcount=dircount[di],maxidx=di;
            }
            dx=directions[maxidx][0],dy=directions[maxidx][1];
            */
            dx=dx/dcount,dy=dy/dcount;
            //std::cout<<dx<<","<<dy<<"|";
            relx/=dcount,rely/=dcount;
            //dx=relx-bbVec[bb_i*4],dy=rely-bbVec[bb_i*4+1];
            //std::cout<<dx<<","<<dy<<std::endl;
        }
        //dx=maxdx,dy=maxdy;
        //newbb[bb_i*4]=relx,newbb[bb_i*4+1]=rely,newbb[bb_i*4+2]=relx+bbw,newbb[bb_i*4+3]=rely+bbh;
        newbb[bb_i*4]=left-1,newbb[bb_i*4+1]=top-1,newbb[bb_i*4+2]=right+1,newbb[bb_i*4+3]=bottom+1;

        newbb[bb_i*4]=bbVec[bb_i*4]+dx,
        newbb[bb_i*4+1]=bbVec[bb_i*4+1]+dy,
        newbb[bb_i*4+2]=bbVec[bb_i*4+2]+dx,
        newbb[bb_i*4+3]=bbVec[bb_i*4+3]+dy;

        //memcpy(newbb,bbVec,targetBB.frame_byte_size);
        //newbb[bb_i*4]+=dx,newbb[bb_i*4+1]+=dy,newbb[bb_i*4+2]+=dx,newbb[bb_i*4+3]+=dy;
    }
    targetBB.updateAFrame(newbb);
    free(newbb);

    for(int bb_i=0;bb_i<gt_N;bb_i++)
    {
        for(int ft_i=0;ft_i<nFeatures;ft_i++)
        {
            //bb[0]=bbVec[gt_i*4],bb[1]=bbVec[gt_i*4+1],bb[2]=bbVec[gt_i*4+2],bb[3]=bbVec[gt_i*4+3];
            PntT x=trackBuff[ft_i].cur_frame_ptr->x;
            PntT y=trackBuff[ft_i].cur_frame_ptr->y;
            if(checkinBB(x, y, bbVec[bb_i*4],bbVec[bb_i*4+1],bbVec[bb_i*4+2],bbVec[bb_i*4+3]))
            {
                //std::cout<<"1,";
                bbxft[bb_i*nFeatures+ft_i]=1;
                //if(trackBuff[ft_i].lastupdate<0)
                    relPos(bb_i,ft_i,0)=((x-bbVec[bb_i*4])+relPos(bb_i,ft_i,0))/2.0
                    ,relPos(bb_i,ft_i,1)=((y-bbVec[bb_i*4+1])+relPos(bb_i,ft_i,0))/2.0;
            }
            else
            {
                //bbxft[bb_i*nFeatures+ft_i]=0;
                //std::cout<<"0,";
            }
            //std::cout<<std::endl;
        }
    }
}
void KLTtracker::updateFGMask(unsigned char* bgptr)
{
	bgdata=bgptr;
}

bool KLTtracker::checkTrackMoving(TrackBuff &strk)
{
	bool isTrkValid = true;
	int Movelen=15,minlen=5,startidx=max(strk.len-Movelen,0);
	double maxdist = .0, dtmp = .0,totlen=.0;

	TrkPts* aptr = strk.getPtr(0), *bptr = aptr;
	//int xa=(*aptr),ya=(*(aptr+1)),xb=*(strk.cur_frame_ptr),yb=*(strk.cur_frame_ptr+1);
    PntT xa=aptr->x,ya=aptr->y,xb=strk.cur_frame_ptr->x,yb=strk.cur_frame_ptr->y;
	double displc=sqrt( pow(xb-xa, 2.0) + pow(yb-ya, 2.0));
	for(int posi=0;posi<strk.len;posi++)
	{
		bptr=strk.getPtr(posi);
		xb=bptr->x,yb=bptr->y;
		dtmp = sqrt( pow(xb-xa, 2.0) + pow(yb-ya, 2.0));
		totlen+=dtmp;
		if (dtmp > maxdist&&posi>=startidx) maxdist = dtmp;
		xa=xb,ya=yb;
	}
	
    if(strk.len>100&&totlen*0.5>displc){strk.isCurved=true;}
	if (maxdist < 1.4 && strk.len>30){isTrkValid = false;}
	if (maxdist <=0.1 && strk.len>=minlen){isTrkValid = false;}

	return isTrkValid;
}

int KLTtracker::updateAframe(unsigned char* framedata,int fidx)
{
    frameidx=fidx;
	curframedata=framedata;
	KLTTrackFeatures(tc, preframedata, framedata, frame_width, frame_height, fl);
	KLTReplaceLostFeatures(tc, framedata, frame_width, frame_height, fl);

	for (int k = 0; k < nFeatures; k++)
	{

		if ( fl->feature[k]->val == 0)
		{
            pttmp.x = (PntT)(fl->feature[k]->x + 0.5);
            pttmp.y = (PntT)(fl->feature[k]->y + 0.5);
            pttmp.t = frameidx;
			trackBuff[k].updateAFrame(&pttmp);
			bool isMoving=checkTrackMoving(trackBuff[k]);
			if (!isMoving) 
			{
				isTracking[k]=0;
				fl->feature[k]->val=-2;
			}
		}

		if ( fl->feature[k]->val > 0)
		{
			trackBuff[k].clear();
            pttmp.x = (PntT)(fl->feature[k]->x + 0.5);
            pttmp.y = (PntT)(fl->feature[k]->y + 0.5);
            pttmp.t = frameidx;
			//trackBuff[k].updateAFrame(x,y,frameidx);
			trackBuff[k].updateAFrame(&pttmp);
			isTracking[k]=1;

            for(int bb_i=0;bb_i<gt_N;bb_i++)
            {
                bbxft[bb_i*nFeatures+k]=0;
            }
            //std::cout<<k<<std::endl;
		}

		if ( fl->feature[k]->val < 0)
		{
			trackBuff[k].clear();
            for(int bb_i=0;bb_i<gt_N;bb_i++)
            {
                bbxft[bb_i*nFeatures+k]=0;
            }
		}
	}

    if(gt_inited)
        updateBB();
	return 1;
}

void KLTtracker::drawline(unsigned char* cfarmeptr,int x1,int y1,int x2, int y2,unsigned char* rgb)
{

	int A[2]={x1,y1};
	int B[2]={x2,y2};
	int x,y,offset;
	int* x_idx=NULL, *y_idx=NULL;
	//std::cout << x1 << "," << y1 << "," << x2 << "," << y2 << std::endl;
	int len=getLineIdx(&x_idx,&y_idx,A,B);
	for(int i=0;i<len;i++)
	{
		x=x_idx[i];
		y=y_idx[i];
		offset=(y*drawW+x);
		//if(x>=0&&y>=0&&x<frame_width&&y<frame_height)
		//{
		cfarmeptr[offset*3]=rgb[0];
		cfarmeptr[offset*3+1]=rgb[1];
		cfarmeptr[offset*3+2]=rgb[2];
		//}
	}
    free(x_idx);
    free(y_idx);
}

void KLTtracker::drawStuff(unsigned char* cfarmeptr)
{
	for (int i = 0; i < nFeatures; ++i)
	{
		if (trackBuff[i].len > 5)
		{
			for (int j = 1; j <trackBuff[i].len; ++j)
			{
				TrkPts *aptr = trackBuff[i].getPtr(j - 1);
				TrkPts *bptr = trackBuff[i].getPtr(j);
				drawline(cfarmeptr,aptr->x,aptr->y,bptr->x,bptr->y,_line_colos[i%6]);
			}
		}
	}
}

int checkidx=720, checkx=0,checky=0,checkt=0;
vector<int> idxvec;
bool KLTtracker::checkCurve(TrackBuff &strk)
{
	double maxdist = .0, dtmp = .0, totlen = .0;
	TrkPts * aptr = strk.getPtr(0), *bptr = aptr;
    PntT xa = aptr->x, ya = aptr->y, xb = strk.cur_frame_ptr->x, yb = strk.cur_frame_ptr->y;
	double displc = sqrt(pow(xb - xa, 2.0) + pow(yb - ya, 2.0));
	for (int j = 1; j <strk.len; ++j)
	{
		aptr = strk.getPtr(j - 1);
		bptr = strk.getPtr(j);
		dtmp = sqrt(pow(bptr->x - aptr->x, 2.0) + pow(bptr->y - aptr->y, 2.0));
		totlen += dtmp;
		if (dtmp>maxdist)maxdist = dtmp;
	}
	sprintf(strbuff, "%.1f,%.1f\0", displc, totlen);
	return (strk.len > 20 && totlen*0.5 > displc);
}
void KLTtracker::drawStuffScale(unsigned char* cfarmeptr,int w,int h)
{
	drawW=w,drawH=h;
	scaleW=((double)drawW)/((double)frame_width);
	scaleH=((double)drawH)/((double)frame_height);
	Mat	 frameMat(drawH, drawW, CV_8UC3, cfarmeptr);
	TrkPts * aptr, *bptr;
    PntT xa, ya, xb, yb;
	for (int i = 0; i < nFeatures; ++i)
	{
		if (trackBuff[i].len > 5 )//&& isTracking[i])
		{
			unsigned char rgb[3]={_line_colos[i%6][0],_line_colos[i%6][1],_line_colos[i%6][2]};
			//if(trackBuff[i].len>20 && totlen*0.5>displc)
			if(!checkCurve(trackBuff[i]))
			{
				rgb[0]=((float)rgb[0]/3);
				rgb[1]=((float)rgb[1]/3);
				rgb[2]=((float)rgb[2]/3);
			}
			for (int j = 1; j <trackBuff[i].len; ++j)
			{
				aptr=trackBuff[i].getPtr(j-1);
				bptr=trackBuff[i].getPtr(j);
				xa=aptr->x*scaleW,xb=bptr->x*scaleW,ya=aptr->y*scaleH,yb=bptr->y*scaleH;
				drawline(cfarmeptr,xa,ya,xb,yb,rgb);
			}
			putText(frameMat,strbuff, Point(xb,yb), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(rgb[0],rgb[1],rgb[2]));
		}
	}
	drawBox(cfarmeptr, w, h);
}
void KLTtracker::drawBox(unsigned char* cfarmeptr, int w, int h)
{
    /*
	if (gt_inited)
	{
		Mat	 frameMat(drawH, drawW, CV_8UC3, cfarmeptr);
		for (int i = 0; i < gt_N; ++i)
		{
			int* bb = targetBB.getPtr(i);
			int left = bb[0] * scaleW, top = bb[1] * scaleH, right = bb[2] * scaleW, bottom = bb[3] * scaleH;
			unsigned char rgb[3] = { _line_colos[i % 6][0], _line_colos[i % 6][1], _line_colos[i % 6][2] };
			for (int x = left; x < right; x++)
				for (int y = top; y < bottom; y++)
				{
					if (x >= 0 && x < drawW&&y >= 0 && y < drawH)
					{
						int offset = (y*drawW + x);
						cfarmeptr[offset * 3] = (cfarmeptr[offset * 3] * 225 + rgb[0]*30) / 255;
						cfarmeptr[offset * 3 + 1] = (cfarmeptr[offset * 3 + 1] * 225 + rgb[1]*30) / 255;
						cfarmeptr[offset * 3 + 2] = (cfarmeptr[offset * 3 + 2] * 225 + rgb[2]*30) / 255;
					}
				}
		}
		waitKey(0);
	}
    */
}
int KLTtracker::endTraking()
{
  return 0;
}
