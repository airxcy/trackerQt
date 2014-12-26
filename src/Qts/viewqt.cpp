#include "Qts/viewqt.h"
#include <QPainter>
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
        setBackgroundBrush(QImage(streamThd->frameptr,streamThd->framewidth, streamThd->frameheight, QImage::Format_RGB888));

        painter->setPen(Qt::blue);
        painter->setFont(QFont("Arial", 30));
        sprintf(viewstrbuff,"%d\0",streamThd->frameidx);
        painter->drawText(QRectF (0,0,width(),height()), Qt::AlignRight|Qt::AlignTop, viewstrbuff);


        for(int i=0;i<streamThd->nFeatures;i++)
        {

            TrackBuff& trk=streamThd->trackBuff[i];
            QColor clrtmp(feat_colos[i%6][0],feat_colos[i%6][1],feat_colos[i%6][2],50);
            painter->setPen(clrtmp);
            if (trk.len > 5)
            {
                for (int j = 1; j <trk.len; ++j)
                {

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
        streamThd->mutex.lock();
        streamThd->condition.wait(&(streamThd->mutex));
        setBackgroundBrush(QImage(streamThd->framedata,streamThd->framewidth, streamThd->frameheight, QImage::Format_RGB888));
        streamThd->mutex.unlock();
    }
}
*/
