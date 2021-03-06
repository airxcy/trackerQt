#include "Qts/viewqt.h"
#include "Qts/modelsqt.h"
#include "Qts/streamthread.h"
#include "trackers/klttracker.h"
#include <QPainter>
#include <QBrush>
#include <QPixmap>
#include <cmath>
#include <QGraphicsSceneEvent>
#include <QMimeData>
#include <QByteArray>
#include <QFont>
char viewstrbuff[200];
void GraphicsView::resizeEvent(QResizeEvent * evt)
{
}
void DefaultScene::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    emit clicked(event);
}
void DefaultScene::drawBackground(QPainter * painter, const QRectF & rect)
{
    QPen pen;
    QFont txtfont("Roman",40);
    txtfont.setBold(true);
    //txtfont.setItalic(true);
    pen.setColor(QColor(255,255,255));
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setWidth(10);
    painter->setPen(QColor(243,134,48,150));
    painter->setFont(txtfont);
    painter->drawText(rect, Qt::AlignCenter,"打开文件\nOpen File");
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
    bb_N=0;

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
    bb_N=0;
}
void TrkScene::drawBackground(QPainter * painter, const QRectF & rect)
{
    if(streamThd!=NULL)
    {
        QImage bgimg(streamThd->frameptr,streamThd->framewidth, streamThd->frameheight, QImage::Format_RGB888);
        bgBrush.setTextureImage(bgimg);
        //setBackgroundBrush(bgBrush);
        painter->setBrush(bgBrush);
        painter->drawRect(rect);

        painter->setPen(Qt::white);
        painter->setFont(QFont("Arial", 11));
        sprintf(viewstrbuff,"%d\0",streamThd->frameidx);
        painter->drawText(QRectF (0,0,width(),height()), Qt::AlignLeft|Qt::AlignTop, viewstrbuff);
        //int maxcount=0;
        drawFPts(painter,rect);
        if(bb_N>0&&streamThd->gtInited)
        {
            QPen apen;
            apen.setWidth(3);

            for(int bb_i=0;bb_i<bb_N;bb_i++)
            {
                QColor ac(dragBBvec[bb_i]->rgb[0],dragBBvec[bb_i]->rgb[1],dragBBvec[bb_i]->rgb[2]);
                apen.setColor(ac);
                painter->setPen(apen);
                int xref=((streamThd->tracker->refpoint[bb_i])%2)*2,
                    yref=((streamThd->tracker->refpoint[bb_i])>2)*2+1;
                REAL x0=bbvec[bb_i]->vtx[xref],y0=bbvec[bb_i]->vtx[yref],
                    x1=streamThd->dirVec[bb_i*2]*20+x0,y1=streamThd->dirVec[bb_i*2+1]*20+y0;
                painter->drawLine(x0,y0,x1,y1);
                QPen bpen(QColor(dragBBvec[bb_i]->rgb[0],dragBBvec[bb_i]->rgb[1],dragBBvec[bb_i]->rgb[2]));
                bpen.setWidth(4);
                painter->setPen(bpen);
                TrkPts* aptr=streamThd->tracker->targetLoc[bb_i].cur_frame_ptr;
                painter->drawPoint(aptr->x,aptr->y);
            }
        }
        painter->setPen(QPen());
        views().at(0)->update();
        if(syncflag)
        {
            if(refscene!=NULL)
            {
                //refscene->initBg(bgimg);
            }
            syncflag=!syncflag;
        }
    }
}
void TrkScene::drawFPts(QPainter *painter, const QRectF &rect)
{
    for(int i=0;i<streamThd->nFeatures;i++)
    {
        TrackBuff& trk=streamThd->trackBuff[i];
        QPen linepen;
        float opcacity = 255,rgb[3]={0,0,0},opincre=1;
        bool interested= false;
        TrkPts *aptr=NULL,*bptr=NULL;
        if (trk.len > 5)
        {
            //float opcacity = 1;
            int coounttmp=0;
            for(int bb_i=0;bb_i<streamThd->gt_N;bb_i++)
            {
                if(streamThd->bbxft[bb_i*streamThd->nFeatures+i]>0)
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
                for (int j = trk.len-1; j <trk.len; ++j)
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
            //bbvec[bb_i]->txt[0]=(bb_i-0)+'0';
            //bbvec[bb_i]->txt[1]='\0';
            sprintf(bbvec[bb_i]->txt,"%d,%d\0",bb_i,streamThd->tracker->refpoint[bb_i]);
            bbvec[bb_i]->txtFont.setPixelSize(10);
            addItem(bbvec[bb_i]);
            std::cout << bb_i<<":"<<bb[4*bb_i+0]<<" "<<bb[4*bb_i+1]<<" "<<bb[4*bb_i+2]<<" "<<bb[4*bb_i+3]<<std::endl;;
        }
        for(int bb_i=0;bb_i<dragBBvec.size();bb_i++)
            dragBBvec[bb_i]->setVisible(false);
        update();
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
void  TrkScene::addDragBBvec(std::vector<DragBBox*>& avec)
{
    dragBBvec=std::vector<DragBBox*>(0);
    for(int i =0;i<avec.size();i++)
    {
        DragBBox * abox= NULL;
        avec[i]->clone(abox);
        dragBBvec.push_back(abox);
        std::cout<<abox<<std::endl;
        addItem(abox);
    }
}
void TrkScene::startEdit()
{
    if(streamThd!=NULL)
    {
        for(int bb_i=0;bb_i<bb_N;bb_i++)
        {
            dragBBvec[bb_i]->setVtx(bbvec[bb_i]->vtx[0],bbvec[bb_i]->vtx[1],bbvec[bb_i]->vtx[2],bbvec[bb_i]->vtx[3]);
        }
        for(int bb_i=0;bb_i<bb_N;bb_i++)
        {
            dragBBvec[bb_i]->setVisible(true);
            bbvec[bb_i]->setVisible(false);
        }
        update();
    }
}
void TrkScene::endEdit()
{
    if(streamThd!=NULL)
    {
        std::vector<REAL> bbVec(4*bb_N);
        for(int bb_i=0;bb_i<bb_N;bb_i++)
        {
            int l=dragBBvec[bb_i]->vtx[0]->coord[0],
            t=dragBBvec[bb_i]->vtx[0]->coord[1],
            r=dragBBvec[bb_i]->vtx[1]->coord[0],
            b=dragBBvec[bb_i]->vtx[1]->coord[1];
            bbVec[bb_i*4+0]=l,
            bbVec[bb_i*4+1]=t,
            bbVec[bb_i*4+2]=r,
            bbVec[bb_i*4+3]=b;
            streamThd->changeCurBB(bbVec);
        }
        for(int bb_i=0;bb_i<bb_N;bb_i++)
        {
            dragBBvec[bb_i]->setVisible(false);
            bbvec[bb_i]->setVisible(true);
        }
        update();
    }
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
    dragBBvec=std::vector<DragBBox*>(0);
    isTagging=false,isTransFering=false,tranfered=false;
    streamThd=NULL;
}
RefScene::RefScene(qreal x, qreal y, qreal width, qreal height, QObject * parent):QGraphicsScene( x, y, width, height, parent)
{
    //dragvtx = new DragVtx(100,100);
    dragBBvec=std::vector<DragBBox*>(0);
    isTagging=false,isTransFering=false,tranfered=false;
    streamThd=NULL;
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
    QImage bgimg=img.copy();
    bgBrush=QBrush(bgimg);
}

void RefScene::drawBackground(QPainter * painter, const QRectF & rect)
{
    if(!tranfered)
    {
        QImage bgimg(streamThd->delayedFrameptr,streamThd->framewidth, streamThd->frameheight, QImage::Format_RGB888);
        bgBrush.setTextureImage(bgimg);
    }
    painter->setBrush(bgBrush);
    painter->drawRect(rect);
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 11));
    sprintf(viewstrbuff,"%d\0",streamThd->frameidx-streamThd->delay);
    painter->drawText(QRectF (0,0,width(),height()), Qt::AlignLeft|Qt::AlignTop, viewstrbuff);
    views().at(0)->update();
}
void RefScene::addADragBB()
{
    DragBBox* abox=new DragBBox(50,50,150,150);
    int N=dragBBvec.size();
    abox->rgb[0]=feat_colos[N%6][0],
    abox->rgb[1]=feat_colos[N%6][1],
    abox->rgb[2]=feat_colos[N%6][2];
    abox->bbid=N;
    sprintf(abox->txt,"%d\0",N);
    dragBBvec.push_back(abox);
    addItem(abox);
}
void RefScene::startTrans()
{
    if(!isTagging)return;
    isTransFering=true;
    if(trkscene!=NULL)
    {
        trkscene->addDragBBvec(dragBBvec);
        //streamThd->initBB();
    }
}
void RefScene::endTrans()
{
    QImage curimg(streamThd->delayedFrameptr,streamThd->framewidth, streamThd->frameheight, QImage::Format_RGB888);
    QImage bgimg=curimg.copy();
    bgBrush=QBrush(bgimg);
    tranfered=true;
}
