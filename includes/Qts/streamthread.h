#ifndef STREAMTHREAD
#define STREAMTHREAD

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "trackers/klttracker.h"
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

    //tracking items
    int nFeatures,gt_N;
    std::vector<TrackBuff> trackBuff;
    Buff<REAL> targetLoc,targetBB;
public slots:
    void streamStart(string &filename);
    bool init();
signals:
    void aFrameDone();
    void debug(const char * msg);
    void initSig();
protected:
    void run();
public:
    bool restart;
    bool abort;
};

#endif // STREAMTHREAD

