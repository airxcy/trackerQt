#ifndef STREAMTHREAD
#define STREAMTHREAD

#include "trackers/buffers.h"
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class KLTtracker;
class BBox;
class TrkScene;

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
    unsigned char * darkerPtr;
    int framewidth,frameheight,frameidx,frameByteSize,gtframeidx;
    QMutex mutex;
    QWaitCondition drawcv;

    KLTtracker* tracker;
    TrkScene* trkscene;
    //tracking items
    int nFeatures,gt_N;
    bool gtInited;
    std::vector<TrackBuff> trackBuff;
    Buff<REAL> targetLoc,targetBB;
    unsigned char* bbxft;
public slots:
    void streamStart(std::string &filename);
    bool init();
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

