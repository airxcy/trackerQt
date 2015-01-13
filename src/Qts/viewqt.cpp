#include "Qts/viewqt.h"
#include "Qts/modelsqt.h"
#include "Qts/streamthread.h"

#include <QPainter>
#include <QBrush>
#include <QPixmap>
#include <cmath>
#include <QGraphicsSceneEvent>
#include <QMimeData>
#include <QByteArray>
char viewstrbuff[200];
void GraphicsView::resizeEvent(QResizeEvent * evt)
{

}
TrkScene::TrkScene(const QRectF & sceneRect, QObject * parent):QGraphicsScene(sceneRect, parent)
{
    //setAcceptDrops(true);
    streamThd=NULL;
    //bbarr=NULL;
    syncflag=false;
    dragBB=new BBox();
    addItem(dragBB);
    dragBB->setVisible(false);

}
TrkScene::TrkScene(qreal x, qreal y, qreal width, qreal height, QObject * parent):QGraphicsScene( x, y, width, height, parent)
{
    streamThd=NULL;
    //bbarr=NULL;
    syncflag=false;
    //setAcceptDrops(true);
    //bb = BBox(0,0,100,100);
    //bb.Vertex[0]=0, bb.Vertex[1]=0, bb.Vertex[2]=100, bb.Vertex[3]=200;
    //addItem(&bb);
    dragBB=new BBox();
    addItem(dragBB);
    dragBB->setVisible(false);
}
void TrkScene::drawBackground(QPainter * painter, const QRectF & rect)
{
    if(streamThd!=NULL)
    {
        QImage bgimg(streamThd->darkerPtr,streamThd->framewidth, streamThd->frameheight, QImage::Format_RGB888);
        //QImage fgimg(streamThd->frameptr,streamThd->framewidth, streamThd->frameheight, QImage::Format_RGB888);
        bgBrush.setTextureImage(bgimg);
        //setBackgroundBrush(bgBrush);
        painter->setBrush(bgBrush);
        painter->drawRect(rect);

        painter->setPen(Qt::white);
        painter->setFont(QFont("Arial", 11));
        sprintf(viewstrbuff,"%d\0",streamThd->frameidx);
        painter->drawText(QRectF (0,0,width(),height()), Qt::AlignLeft|Qt::AlignTop, viewstrbuff);
        //int maxcount=0;
        for(int i=0;i<streamThd->nFeatures;i++)
        {
            TrackBuff& trk=streamThd->trackBuff[i];
            QPen linepen;
            float opcacity = 1,rgb[3]={0,0,0},opincre=1;
            bool interested= false;
            TrkPts *aptr=NULL,*bptr=NULL;
            if (trk.len > 5)
            {
                //float opcacity = 1;
                int coounttmp=0;
                for(int bb_i=0;bb_i<streamThd->gt_N;bb_i++)
                {
                    if(streamThd->bbxft[bb_i*streamThd->nFeatures+i])
                    {
                        rgb[0]+=feat_colos[bb_i%6][0],rgb[1]+=feat_colos[bb_i%6][1],rgb[2]+=feat_colos[bb_i%6][2];
                        coounttmp++;
                    }
                }
                //maxcount=(maxcount>coounttmp)?maxcount:coounttmp;
                if(coounttmp)
                {
                    rgb[0]=(rgb[0]/coounttmp>255)?255:(rgb[0]/coounttmp),
                    rgb[1]=(rgb[1]/coounttmp>255)?255:(rgb[1]/coounttmp),
                    rgb[2]=(rgb[2]/coounttmp>255)?255:(rgb[2]/coounttmp);
                }
                else
                    rgb[0]=255,rgb[1]=255,rgb[2]=255,opincre=0.5;
                //if(coounttmp>1)
                    for (int j = 1; j <trk.len; ++j)
                    {
                        opcacity+=opincre;
                        opcacity=(opcacity>255)?255:opcacity;
                        linepen.setColor(QColor(rgb[0],rgb[1],rgb[2],opcacity));
                        painter->setPen(linepen);
                        aptr = trk.getPtr(j - 1);
                        bptr = trk.getPtr(j);
                        painter->drawLine(aptr->x,aptr->y,bptr->x,bptr->y);

                    }
            }
        }
        views().at(0)->update();
        if(syncflag)
        {
            if(refscene!=NULL)
            {
                refscene->initBg(bgimg);
            }
            syncflag=!syncflag;
        }
    }
}
void TrkScene::initBBox()
{
    std::cout<<"check thd NULL"<<std::endl;
    if(streamThd!=NULL)
    {
        std::cout<<"scene init BBox"<<std::endl;
        bb_N=streamThd->gt_N;
        bbvec=std::vector<BBox *>(bb_N);
        REAL* bb=streamThd->targetBB.cur_frame_ptr;
        for(int bb_i=0;bb_i<bb_N;bb_i++)
        {
            bbvec[bb_i] = new BBox();
            bbvec[bb_i]->bbid=bb_i;
            bbvec[bb_i]->updateVtx(bb[4*bb_i+0],bb[4*bb_i+1],bb[4*bb_i+2],bb[4*bb_i+3]);
            bbvec[bb_i]->linePen.setColor(QColor(feat_colos[bb_i%6][0],feat_colos[bb_i%6][1],feat_colos[bb_i%6][2],100));
            bbvec[bb_i]->txt[0]=(bb_i-0)+'0';
            bbvec[bb_i]->txt[1]='\0';
            bbvec[bb_i]->txtFont.setPixelSize(10);
            addItem(bbvec[bb_i]);
            std::cout << bb_i<<" ";
        }
        std::cout<<"inited"<<std::endl;
        //if(refscene!=NULL)refscene->initBBox(bbvec,bb_N);
        syncflag=true;
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
void TrkScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
  if(event->mimeData()->hasFormat("id"))
  {
    event->setAccepted(true);
    QByteArray dataByte = event->mimeData()->data("id");
    std::string stdbytearr=dataByte.toStdString();
    int* info = (int *)stdbytearr.data();
    int id=info[2],x=event->scenePos().x(),y=event->scenePos().y();
    /*
    if(bbvec.size()>0)
    {
        int w=bbarr[id].vtx[2]-bbarr[id].vtx[0];
        int h=bbarr[id].vtx[3]-bbarr[id].vtx[1];
        dragBB->updateVtx(x,y,x+w,y+h);
        QColor linec= bbarr[id].linePen.color();
        linec.setAlpha(255);
        dragBB->linePen.setColor(linec);
        dragBB->setVisible(true);
    }
    */
  }
  else
  {
    event->setAccepted(false);
  }
}

void TrkScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    std::cout<<"droped"<<":";
    if(event->mimeData()->hasFormat("id"))
    {
        event->acceptProposedAction();
        QByteArray dataByte = event->mimeData()->data("id");
        std::string stdbytearr=dataByte.toStdString();
        int* info = (int *)stdbytearr.data();
    }
    dragBB->setVisible(false);
    //mouseReleaseEvent(event);
}

void TrkScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
    return;
    if(event->mimeData()->hasFormat("id"))
    {
        QByteArray dataByte = event->mimeData()->data("id");
        std::string stdbytearr=dataByte.toStdString();
        int* info = (int *)stdbytearr.data();
        int x=event->scenePos().x(),y=event->scenePos().y(),id=info[2];
        /*
        if(bbarr!=NULL)
        {
            int w=bbarr[id].vtx[2]-bbarr[id].vtx[0];
            int h=bbarr[id].vtx[3]-bbarr[id].vtx[1];
            dragBB->updateVtx(x,y,x+w,y+h);
        }
        */
    }

    //std::cout<<mouseGrabberItem()<<std::endl;
}
/*
void TrkScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
std::cout<<"Scene Leave->"<<std::endl;
}
void TrkScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

 std::cout<<"Scene Release->"<<std::endl;
 QGraphicsScene::mouseReleaseEvent(event);
}
void TrkScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
std::cout<<"Scene Press->";
}
*/

RefScene::RefScene(const QRectF & sceneRect, QObject * parent):QGraphicsScene(sceneRect, parent)
{
}
RefScene::RefScene(qreal x, qreal y, qreal width, qreal height, QObject * parent):QGraphicsScene( x, y, width, height, parent)
{
    //dragvtx = new DragVtx(100,100);
    dragBB = new DragBBox(100,100,200,200);
    dragBB->linePen.setColor(QColor(255,255,255));
    dragBB->inBrush.setStyle(Qt::SolidPattern);
    dragBB->inBrush.setColor(QColor(255,0,0,100));
    //addItem(dragvtx);
    addItem(dragBB);

}
void RefScene::initBBox(std::vector<BBox*>& arr, int N)
{
    bbvec =std::vector<BBox*>(N);
    for(int i=0;i<N;i++)
    {
        bbvec[i]=new BBox();
        arr[i]->clone(bbvec[i]);
        addItem(bbvec[i]);
    }
}
void RefScene::initBg(QImage &img)
{
    bgimg=img.copy();
    bgBrush=QBrush(bgimg);
}

void RefScene::drawBackground(QPainter * painter, const QRectF & rect)
{
    painter->setBrush(bgBrush);
    painter->drawRect(rect);
    views().at(0)->update();
}

