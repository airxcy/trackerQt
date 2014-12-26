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
    char strbuff[100];
    unsigned char * frameptr;
    int framewidth,frameheight,frameidx,frameByteSize;
    QMutex mutex;
    QWaitCondition drawcv;

    //tracking items
    int nFeatures;
    std::vector<TrackBuff> trackBuff;
public slots:
    void streamStart();
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

