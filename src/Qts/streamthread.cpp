#include "Qts/StreamThread.h"
#include <QMessageBox>
#include <iostream>

#include <opencv2/opencv.hpp>
#include <fstream>
#include <time.h>
using namespace cv;

VideoCapture cap;
KLTtracker* tracker;
Mat frame,gray,darker;
clock_t t;
float fps=0;
StreamThread::StreamThread(QObject *parent) : QThread(parent)
{
    restart = false;
    abort = false;
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
    drawcv.wakeAll();
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
    cvtColor(frame,gray,CV_BGR2GRAY);

    framewidth=frame.size[1],frameheight=frame.size[0];
    frameByteSize=frame.size[0]*frame.size[1]*frame.elemSize();
    frameptr=new unsigned char[frameByteSize];
    darkerPtr=new unsigned char[frameByteSize];
    memcpy(frameptr,frame.data,frameByteSize);
    memcpy(darkerPtr,darker.data,frameByteSize);
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

    return cap.isOpened();
}
void StreamThread::updateItems()
{
    mutex.lock();
    memcpy(frameptr,frame.data,frameByteSize);
    memcpy(darkerPtr,darker.data,frameByteSize);
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
    for(int i=0;i<gt_N;i++)
    {
        tracker->targetLoc.clone(&targetLoc);
        tracker->targetBB.clone(&targetBB);
    }
    mutex.unlock();
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
                frameidx++;
                if ((frameidx - 50) % 1500 == 0)
                {

                    sprintf(strbuff, "%06d.txt\0", frameidx);
                    std::string gtfname = gtdir + strbuff;
                    tracker->initGT(gtfname);
                    gt_N=tracker->gt_N;
                    targetLoc.init( tracker->targetLoc.frame_step_size,tracker->targetLoc.buff_len);
                    targetBB.init(tracker->targetBB.frame_step_size,tracker->targetBB.buff_len);
                    tracker->targetLoc.clone(&targetLoc);
                    tracker->targetBB.clone(&targetBB);
                }
                cap >> frame;
                darker=frame*0.5;
                cvtColor(frame,gray,CV_BGR2GRAY);
                tracker->updateAframe(gray.data,frameidx);
                updateItems();
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
