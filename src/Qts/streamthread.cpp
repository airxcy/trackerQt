#include "Qts/streamthread.h"
#include "trackers/klttracker.h"
#include "Qts/viewqt.h"
#include "Qts/modelsqt.h"

#include <iostream>
#include <fstream>
#include <time.h>

#include <opencv2/opencv.hpp>
#include <QMessageBox>
using namespace cv;

VideoCapture cap;
Mat frame,gray,darker;
clock_t t;
float fps=0;
StreamThread::StreamThread(QObject *parent) : QThread(parent)
{
    restart = false;
    abort = false;
    pause = false;
    gt_N=-1;
    gtInited=false;
    isTracking=false;
    trkscene=NULL;
}
StreamThread::~StreamThread()
{
    /*
    mutex.lock();
    abort = true;
    condition.wakeOne();
    mutex.unlock();
    wait();
    */
    QMutexLocker lock(&mutex);
    abort = true;
    cv.wakeAll();
    wait();
}

bool StreamThread::init()
{
    QMutexLocker lock(&mutex);
    //cap.open("C:/Users/xcy/Documents/CVProject/data/grandcentral/grandcentral.avi");
    cap.open(vidfname);
    std::cout<<"init:"<<cap.isOpened()<<std::endl;
    restart=false;
    abort=false;
    cap>>frame;
    darker=frame*0.5;
    delay=20;
    cvtColor(frame,gray,CV_BGR2GRAY);
    cvtColor(frame,frame,CV_BGR2RGB);
    framewidth=frame.size[1],frameheight=frame.size[0];
    framebuff = new FrameBuff();
    graybuff = new FrameBuff();
    framebuff->init(frame.elemSize(),framewidth,frameheight,delay+1);
    graybuff->init(gray.elemSize(),framewidth,frameheight,delay+1);
    frameByteSize=frame.size[0]*frame.size[1]*frame.elemSize();
    framebuff->updateAFrame(frame.data);
    graybuff->updateAFrame(gray.data);
    frameptr=framebuff->cur_frame_ptr;
    delayedFrameptr=framebuff->headptr;

    tracker=new KLTtracker();
    tracker->init(8,framewidth,frameheight);
    tracker->initBG(0);

    tracker->selfinit(gray.data);
    nFeatures=tracker->nFeatures;
    trackBuff=std::vector<TrackBuff>(nFeatures);
    for(int i=0;i<nFeatures;i++)
    {
        trackBuff[i].init(1,100);
    }

    gtbasedir="C:/Users/xcy/Documents/CVProject/data/label_company/labels/";
    vidid=vidfname.substr(vidfname.length() - 10, 6);
    gtdir = gtbasedir + vidid + "txt/";
    bbxft=NULL;
    return cap.isOpened();
}
void StreamThread::beginTracking()
{
    tracker->selfinit(gray.data);
    isTracking=true;
}
void StreamThread::updateItems()
{
    mutex.lock();
    for(int i=0;i<nFeatures;i++)
    {
        if(tracker->trackBuff[i].lastupdate>0)
        {
            tracker->trackBuff[i].clone(&trackBuff[i]);
            //std::cout<<trackBuff[i].len<<","<<tracker->trackBuff[i].len<<std::endl;
        }
        else if(tracker->trackBuff[i].lastupdate<0)
        {
            tracker->trackBuff[i].clear();
        }
    }
    if(tracker->gt_inited&&bbxft!=NULL)
        memcpy(bbxft,tracker->bbxft,gt_N*nFeatures*sizeof(int));
    if(gt_N>0)
    {
        //tracker->targetLoc.clone(&targetLoc);
        tracker->targetBB.clone(&targetBB);
        if(trkscene!=NULL&&trkscene->bbvec.size()>0)
        {
            REAL* bb=targetBB.cur_frame_ptr;
            for(int bb_i=0;bb_i<gt_N;bb_i++)
            {
                REAL l=bb[bb_i*4+0],t=bb[bb_i*4+1],r=bb[bb_i*4+2],b=bb[bb_i*4+3];
                trkscene->bbvec[bb_i]->updateVtx(l,t,r,b);
            }
        }

    }
    mutex.unlock();
}
void StreamThread::initBB()
{
    gt_N=trkscene->dragBBvec.size();
    targetBB.init(4*gt_N,10);
    vector<REAL> bbVec(4*gt_N),dlyVec(4*gt_N);
    for(int bb_i=0;bb_i<gt_N;bb_i++)
    {
        bbVec[bb_i*4+0]=trkscene->dragBBvec[bb_i]->vtx[0]->coord[0];
        bbVec[bb_i*4+1]=trkscene->dragBBvec[bb_i]->vtx[0]->coord[1];
        bbVec[bb_i*4+2]=trkscene->dragBBvec[bb_i]->vtx[1]->coord[0];
        bbVec[bb_i*4+3]=trkscene->dragBBvec[bb_i]->vtx[1]->coord[1];
        dlyVec[bb_i*4+0]=refscene->dragBBvec[bb_i]->vtx[0]->coord[0];
        dlyVec[bb_i*4+1]=refscene->dragBBvec[bb_i]->vtx[0]->coord[1];
        dlyVec[bb_i*4+2]=refscene->dragBBvec[bb_i]->vtx[1]->coord[0];
        dlyVec[bb_i*4+3]=refscene->dragBBvec[bb_i]->vtx[1]->coord[1];
    }
    targetBB.updateAFrame(bbVec.data());
    bbxft=new int[gt_N*nFeatures];
    tracker->initBB(targetBB,dlyVec,gt_N,delay);
    dirVec=std::vector<REAL>(gt_N*2);
    memcpy(dirVec.data(),tracker->dirVec.data(),gt_N*2*sizeof(REAL));
    emit initBBox();
    gtInited=true;
}
void StreamThread::changeCurBB(std::vector<REAL>& bbVec)
{
    //memcpy(targetBB.cur_frame_ptr,bbVec.data(),targetBB.frame_byte_size);
    tracker->changeBB(bbVec);
    updateItems();
}
void StreamThread::streaming()
{
    forever
    {
        if(init())
        {
            std::cout<<"init success"<<std::endl;
            emit initSig();
            frameidx=0;
            //namedWindow("frame");
            while(cap.isOpened())
            //for(int i=0;i<10000;i++)
            {
                if (restart)
                        break;
                if (abort)
                        return;
                if(pause)
                {
                    mutex.lock();
                    cv.wait(&mutex);
                    mutex.unlock();
                }
                frameidx++;
                /*
                if ((frameidx - 50) % 1500 == 0)
                {
                    mutex.lock();
                    sprintf(strbuff, "%06d.txt\0", frameidx);
                    std::string gtfname = gtdir + strbuff;
                    tracker->initGT(gtfname);
                    gt_N=tracker->gt_N;
                    targetLoc.init( tracker->targetLoc.frame_step_size,tracker->targetLoc.buff_len);
                    targetBB.init(tracker->targetBB.frame_step_size,tracker->targetBB.buff_len);
                    bbxft=new unsigned char[gt_N*nFeatures];
                    tracker->targetLoc.clone(&targetLoc);
                    tracker->targetBB.clone(&targetBB);
                    gtInited=true;
                    mutex.unlock();
                    std::cout <<"mutex end"<<std::endl;
                    emit initBBox();
                    std::cout <<"siged"<<std::endl;
                }
                */
                cap >> frame;
                darker=frame*0.5;
                cvtColor(frame,gray,CV_BGR2GRAY);
                cvtColor(frame,frame,CV_BGR2RGB);
                framebuff->updateAFrame(frame.data);
                graybuff->updateAFrame(gray.data);
                frameptr=framebuff->cur_frame_ptr;
                delayedFrameptr=framebuff->headptr;
                if(isTracking)
                {
                    tracker->updateAframe(gray.data,frameidx);
                    updateItems();
                }
                /*
                if(frameidx>50)
                {
                    imshow("frame",frame);
                    waitKey(0);
                }
                */
                emit aFrameDone();
            }
        }
        else
        {
            //emit debug( "init Failed");
        }
        /*
        mutex.lock();
        if (!restart)
            condition.wait(&mutex);
        restart = false;
        mutex.unlock();
        */
    }
}

void StreamThread::run()
{
    streaming();
}

void StreamThread::streamStart(std::string & filename)
{
    QMutexLocker locker(&mutex);
    //QMessageBox::question(NULL, "Test", "msg",QMessageBox::Ok);
    if (!isRunning()) {
        vidfname=filename;
        start(InheritPriority);
    }
    else
    {
        restart=true;
    }
}
