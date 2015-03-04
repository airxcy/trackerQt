#ifndef STREAMTHREAD
#define STREAMTHREAD

#include "trackers/buffers.h"
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class KLTtracker;
class BBox;
class TrkScene;
class RefScene;
class StreamThread : public QThread
{
    Q_OBJECT
public:
    //StreamThread(){};
    StreamThread(QObject *parent = 0);
    ~StreamThread();
    void streaming();
    void updateItems();
public:
    std::string vidfname,gtdir, gtbasedir, vidid;
    char strbuff[100];
    unsigned char * frameptr;
    int framewidth,frameheight,frameidx,frameByteSize,gtframeidx;
    QMutex mutex;
    QWaitCondition cv;

    KLTtracker* tracker;
    TrkScene* trkscene;
    RefScene* refscene;
    //tracking items
    int nFeatures,gt_N;
    bool gtInited,isTracking;
    std::vector<TrackBuff> trackBuff;
    Buff<REAL> targetLoc,targetBB;
    int* bbxft;

    int delay;
    FrameBuff* framebuff,* graybuff;
    unsigned char * delayedFrameptr;
    std::vector<REAL> dirVec;



    void beginTracking();
public slots:
    void streamStart(std::string &filename);
    bool init();
    void initBB();
    void changeCurBB(std::vector<REAL>& bbVec);

signals:
    void initBBox();
    void aFrameDone();
    void debug(const char * msg);
    void initSig();

protected:
    void run();
public:
    bool restart,abort,pause;
};

#endif // STREAMTHREAD

