#ifndef KLTTRACKER_H
#define KLTTRACKER_H

#include "trackers/tracker.h"
#include "trackers/klt_c/klt.h"
#include "trackers/klt_c/pnmio.h"
#include "trackers/klt_c/trk.h"
#include <cmath>
#include <fstream>
#include <string>

#include <opencv2/opencv.hpp>
using namespace cv;

using namespace std;

#define PI 3.14159265
#define dirN 8

class KLTtracker : public Tracker
{
public:
	KLT_TrackingContext tc;
	KLT_FeatureList fl;
	KLT_FeatureTable ft;

	int *isTracking, *trkIndex;
	int bgMod;

	int frameidx;
	int nFeatures; /*** get frature number ***/
	int FOREGROUND_THRESHOLD;
	int curMaxTrk;
	int drawW, drawH, scaleW, scaleH;
	int delay;
	unsigned char* preframedata,* bgdata,*curframedata;

	std::vector<TrackBuff> trackBuff;
	TrkPts pttmp;
    Buff<REAL> targetLoc,targetBB;
    unsigned char* bbxft;
	int bbw, bbh;

	int init(int bsize,int w,int h);
	int selfinit(unsigned char* framedata);
	int initBG(int mode,unsigned char* bgptr=NULL);
	int initGT(string & gtfilename);
    int updateAframe(unsigned char* framedata,int fidx);
    void updateBB();
	void updateFGMask(unsigned char* bgptr);
	
	int checkFG(int x,int y);
	bool checkTrackMoving(TrackBuff &strk);
	bool checkCurve(TrackBuff &strk);

	void drawStuff(unsigned char* cfarmeptr);
	void drawStuffScale(unsigned char* cfarmeptr,int w,int h);
	void drawline(unsigned char* cfarmeptr,int x1,int y1,int x2, int y2,unsigned char* rg);
	void drawBox(unsigned char* cfarmeptr, int w, int h);
	int endTraking();
};
#endif
