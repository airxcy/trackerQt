#ifndef KLTTRACKER_H
#define KLTTRACKER_H
#include "trackers/tracker.h"
#include "trackers/klt_c/klt.h"
//#include "trackers/klt_c/pnmio.h"
//#include "trackers/klt_c/trk.h"
#include <vector>
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
    Buff<REAL> targetBB;
    std::vector<TrackBuff> targetLoc;
    std::vector<REAL> dlyBB,dirVec,comVec;
    int* refpoint;
    int* bbxft;
	int bbw, bbh;
    Map3D<REAL> relPos;

	int init(int bsize,int w,int h);
	int selfinit(unsigned char* framedata);
	int initBG(int mode,unsigned char* bgptr=NULL);
    int initGT(std::string & gtfilename);
    int initBB(Buff<REAL>& bbbuff,std::vector<REAL>& dlyvec,int bbN,int dly);
    void changeBB(std::vector<REAL>& bbVec);
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
