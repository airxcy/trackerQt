#include "Qts/viewqt.h"
#include <QPainter>
#include <QBrush>
#include <QPixmap>
#include <cmath>
unsigned char feat_colos[6][3] =
{
    {0,255,0},
    {0,0,255},
    {255,255, 0},
    {255,0,255},
    {0,255,255},
    {255,0,0},
};
char viewstrbuff[200];
TrkScene::TrkScene(const QRectF & sceneRect, QObject * parent):QGraphicsScene(sceneRect, parent)
{
    streamThd=NULL;
}
TrkScene::TrkScene(qreal x, qreal y, qreal width, qreal height, QObject * parent):QGraphicsScene( x, y, width, height, parent)
{
    streamThd=NULL;
}
void TrkScene::drawForeground(QPainter * painter, const QRectF & rect)
{
    if(streamThd!=NULL)
    {
        QImage bgimg(streamThd->darkerPtr,streamThd->framewidth, streamThd->frameheight, QImage::Format_RGB888);
        QImage fgimg(streamThd->frameptr,streamThd->framewidth, streamThd->frameheight, QImage::Format_RGB888);
        QBrush bgBrush(bgimg);
        QBrush fgBrush(fgimg);
        fgBrush.setColor(QColor(0,0,0,20));
        //setBackgroundBrush(QImage(streamThd->frameptr,streamThd->framewidth, streamThd->frameheight, QImage::Format_RGB888));
        //bgBrush.setColor(Qt::white);
        setBackgroundBrush(bgBrush);
        painter->setBrush(fgBrush);
        REAL *bb=streamThd->targetBB.cur_frame_ptr;
        for(int i=0;i<streamThd->gt_N;i++)
        {
            REAL left = bb[i*4+0], top = bb[i*4+1], right = bb[i*4+2], bottom = bb[i*4+3];
            if(streamThd->targetBB.len>0)
            {
                REAL* bbt_1=streamThd->targetBB.cur_frame_ptr-streamThd->targetBB.frame_step_size;
                REAL left1 = bbt_1[i*4+0], top1 = bbt_1[i*4+1], right1 = bbt_1[i*4+2], bottom1 = bbt_1[i*4+3];
                REAL displc=abs(left-left1)+abs(top-top1);
                if(displc>0)
                {
                    painter->setBrush(fgBrush);
                }
                else
                {
                    painter->setBrush(QBrush());
                }
            }
            painter->setPen(QColor(feat_colos[i%6][0],feat_colos[i%6][1],feat_colos[i%6][2]));
            painter->drawRect(left,top,right-left,bottom-top);
            //painter->setBrush();
        }
        painter->setPen(Qt::white);
        painter->setFont(QFont("Arial", 11));
        sprintf(viewstrbuff,"%d\0",streamThd->frameidx);
        painter->drawText(QRectF (0,0,width(),height()), Qt::AlignRight|Qt::AlignTop, viewstrbuff);


        for(int i=0;i<streamThd->nFeatures;i++)
        {
            TrackBuff& trk=streamThd->trackBuff[i];
            if (trk.len > 5)
            {
                float opcacity = 1;
                for (int j = 1; j <trk.len; ++j)
                {
                    opcacity+=0.5;
                    painter->setPen(QColor(feat_colos[i%6][0],feat_colos[i%6][1],feat_colos[i%6][2],opcacity));
                    TrkPts *aptr = trk.getPtr(j - 1);
                    TrkPts *bptr = trk.getPtr(j);
                    painter->drawLine(aptr->x,aptr->y,bptr->x,bptr->y);
                }
            }
        }
    }
}
/*
void TrkScene::drawBackground(QPainter * painter, const QRectF & rect)
{    if(streamThd!=NULL)
    {
        painter->setBackground(Qt::black);
    }
}
*/
