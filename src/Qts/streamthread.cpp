#include "Qts/StreamThread.h"
#include <QMessageBox>
#include <iostream>

#include <opencv2/opencv.hpp>
#include <fstream>
#include <time.h>
using namespace cv;

VideoCapture cap;
KLTtracker* tracker;
Mat frame,gray;
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
    cap.open("C:/Users/xcy/Documents/CVProject/data/grandcentral/grandcentral.avi");
    std::cout<<"init:"<<cap.isOpened()<<std::endl;
    restart=false;
    abort=false;
    cap>>frame;
    framewidth=frame.size[1],frameheight=frame.size[0];
    frameByteSize=frame.size[0]*frame.size[1]*frame.elemSize();
    frameptr=new unsigned char[frameByteSize];
    memcpy(frameptr,frame.data,frameByteSize);

    tracker=new KLTtracker();
    tracker->init(8,framewidth,frameheight);
    tracker->initBG(0);
    cvtColor(frame,gray,CV_BGR2GRAY);
    tracker->selfinit(gray.data);

    nFeatures=tracker->nFeatures;
    trackBuff=std::vector<TrackBuff>(nFeatures);
    for(int i=0;i<nFeatures;i++)
    {
        trackBuff[i].init(1,100);
    }

    return cap.isOpened();
}
void StreamThread::updateItems()
{
    mutex.lock();
    memcpy(frameptr,frame.data,frameByteSize);
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

                cap >> frame;
                cvtColor(frame,gray,CV_BGR2GRAY);
                tracker->updateAframe(gray.data);
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

void StreamThread::streamStart()
{
    QMutexLocker locker(&mutex);
    //QMessageBox::question(NULL, "Test", "msg",QMessageBox::Ok);
    if (!isRunning()) {
        start(InheritPriority);
    }
    else
    {
        restart=true;
    }
}
