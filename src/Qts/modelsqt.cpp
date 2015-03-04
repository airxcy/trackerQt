#include "Qts/modelsqt.h"

#include <iostream>
#include <QPainter>
#include <QGraphicsSceneEvent>
#include <QDrag>
#include <QMimeData>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsWidget>
unsigned char feat_colos[6][3] =
{
    {0,255,0},
    {0,0,255},
    {255,255, 0},
    {255,0,255},
    {0,255,255},
    {255,0,0},
};
BBox::BBox(QGraphicsItem * parent):QGraphicsItem(parent)
{
    setAcceptDrops(true);
    vtx[0]=10, vtx[1]=10, vtx[2]=200, vtx[3]=200;
    setVisible(true);
    bbid=0;
}
BBox::BBox(double l,double t,double r,double b,QGraphicsItem * parent):QGraphicsItem(parent)
{
    vtx[0]=l, vtx[1]=t, vtx[2]=r, vtx[3]=b;
    setVisible(true);
     bbid=0;
}
QRectF BBox::boundingRect() const
{

    return QRectF(vtx[0], vtx[1], vtx[2]-vtx[0], vtx[3]-vtx[1]);
}
void BBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    prepareGeometryChange();

    painter->setPen(linePen);
    painter->setBrush(inBrush);
    //painter->drawRect(vtx[0], vtx[1], vtx[2]-vtx[0], vtx[3]-vtx[1]);
    painter->drawRect(boundingRect());
    painter->setFont(txtFont);
    painter->drawText(QRectF (vtx[0], vtx[1],12,12), Qt::AlignRight|Qt::AlignTop,txt);
}
void BBox::updateVtx(double l,double t,double r,double b)
{

    vtx[0]=l, vtx[1]=t, vtx[2]=r, vtx[3]=b;
    //prepareGeometryChange();
    //setVisible(true);
}
void BBox::clone(BBox * abox)
{
    if(abox==NULL)return;
    memcpy(abox->vtx,vtx,sizeof(double)*4);
    abox->linePen=linePen;
    abox->inBrush=inBrush;
    abox->txtFont=txtFont;
    abox->bbid=bbid;
    memcpy(abox->txt,txt,100);
    abox->dragStartPos=dragStartPos;
}
void BBox::dragEnterEvent(QGraphicsSceneDragDropEvent * event)
{

}
void BBox::mousePressEvent(QGraphicsSceneMouseEvent* event)
{

    if(event->button()==Qt::LeftButton)
    {
        std::cout<<"pressed:"<<bbid<<"->"<<std::endl;
        dragStartPos = event->pos();
    }
}
void BBox::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (!(event->buttons() & Qt::LeftButton))
    return;
    if((event->pos()-dragStartPos).manhattanLength()<2)
    return;
    std::cout<<this->scene()->mouseGrabberItem()<<"|"<<this<<std::endl;
    QDrag *drag = new QDrag(event->widget());
    QMimeData *mimeData = new QMimeData;
    int datarr[3]={event->pos().x(),event->pos().y(),bbid};
    QByteArray iddata((const char *)datarr,sizeof(int)*3);
    mimeData->setData("id",iddata);
    drag->setMimeData(mimeData);
    //QPixmap pixmap(vtx[2]-vtx[0],vtx[3]-vtx[1]);
    //drag->setPixmap(pixmap);
    linePen.setStyle(Qt::DotLine);
    Qt::DropAction dropAction = drag->exec(Qt::MoveAction);
    linePen.setStyle(Qt::SolidLine);
    ungrabMouse();
}
void BBox::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    std::cout<<"released:"<<bbid<<"->"<<std::endl;
    dragStartPos = event->pos();
}
DragVtx::DragVtx(QGraphicsItem * parent):QGraphicsItem(parent)
{
    setFlag(ItemIsMovable);
    selected=false;
    coord[0]=0,coord[1]=0,range=3;
    dotPen=QPen(Qt::white);
    setCursor(Qt::CrossCursor);
    //setZValue(1);
}
DragVtx::DragVtx(int x, int y,QGraphicsItem * parent):QGraphicsItem(parent)
{
    setFlag(ItemIsMovable);
    selected=false;
    coord[0]=x,coord[1]=y,range=3;
    dotPen=QPen(Qt::white);
    setCursor(Qt::CrossCursor);
    //setZValue(1);
}
QRectF DragVtx::boundingRect() const
{
    return QRectF(coord[0]-range,coord[1]-range,2*range+1,2*range+1);
}
void DragVtx::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(255,255,255));
    painter->drawRect(boundingRect());
    painter->setPen(dotPen);
    painter->drawPoint(coord[0],coord[1]);
}
void DragVtx::mousePressEvent(QGraphicsSceneMouseEvent* event)
{

    if(event->button()==Qt::LeftButton)
    {
        selected=true;
        std::cout<<"selected"<<std::endl;
    }
}
void DragVtx::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        selected=false;
        std::cout<<"released"<<std::endl;
        setCoord(event->pos().x(),event->pos().y());
    }
    ungrabMouse();
}
void DragVtx::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (!(event->buttons() & Qt::LeftButton))
    return;
    if(!selected)return;

    //coord[0]=event->pos().x(),coord[1]=event->pos().y();
    setCoord(event->pos().x(),event->pos().y());
}
void DragVtx::setCoord(int x,int y)
{
    prepareGeometryChange();
    coord[0]=x,coord[1]=y;
}


DragBBox::DragBBox(QGraphicsItem * parent):QGraphicsItem(parent)
{
    //setFlag(ItemIsMovable);
    vtx = std::vector<DragVtx*>(2);
    vtx[0]=new DragVtx(this);
    vtx[1]=new DragVtx(this);
    selected=false;
    //vtx[2]=new DragVtx(this);
    //vtx[3]=new DragVtx(this);
}
DragBBox::DragBBox(int l, int t,int r,int b,QGraphicsItem * parent):QGraphicsItem(parent)
{
    //setFlag(ItemIsMovable);
    vtx = std::vector<DragVtx*>(2);
    vtx[0]=new DragVtx(l,t,this);
    //vtx[1]=new DragVtx(r,t,this);
    vtx[1]=new DragVtx(r,b,this);
    selected=false;
    //vtx[3]=new DragVtx(l,b,this);
}
QRectF DragBBox::boundingRect() const
{
    //return childrenBoundingRect();
    return QRectF(QPointF(vtx[0]->coord[0],vtx[0]->coord[1]),QPointF(vtx[1]->coord[0],vtx[1]->coord[1]));
}

void  DragBBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //prepareGeometryChange();
    painter->setBrush(QBrush(QColor(rgb[0],rgb[1],rgb[2],10),Qt::SolidPattern));
    painter->setPen(QColor(rgb[0],rgb[1],rgb[2]));
    painter->drawRect(boundingRect());
    painter->setPen(QColor(rgb[0],rgb[1],rgb[2]));
    painter->setFont(QFont("Times",20));
    //QRectF (vtx[0]->coord[0],vtx[0]->coord[1],12,12)
    painter->drawText(boundingRect(), Qt::AlignCenter|Qt::AlignCenter,txt);
    //painter->drawText(Rectf(vtx[0]->coord[0],vtx[0]->coord[1]), Qt::AlignCenter|Qt::AlignCenter,);
}
void DragBBox::mousePressEvent(QGraphicsSceneMouseEvent* event)
{

    if(event->button()==Qt::LeftButton)
    {
        selected=true;
        x1=event->pos().x()-vtx[0]->coord[0];
        y1=event->pos().y()-vtx[0]->coord[1];
        x2=event->pos().x()-vtx[1]->coord[0];
        y2=event->pos().y()-vtx[1]->coord[1];
    }
}
void DragBBox::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        selected=false;
        vtx[0]->setCoord(event->pos().x()-x1,event->pos().y()-y1);
        vtx[1]->setCoord(event->pos().x()-x2,event->pos().y()-y2);
    }
    ungrabMouse();
}

void DragBBox::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (!(event->buttons() & Qt::LeftButton))
    return;
    if(!selected)return;
    vtx[0]->setCoord(event->pos().x()-x1,event->pos().y()-y1);
    vtx[1]->setCoord(event->pos().x()-x2,event->pos().y()-y2);
}
void DragBBox::clone(DragBBox*& abox)
{
    int l=vtx[0]->coord[0],t=vtx[0]->coord[1],r=vtx[1]->coord[0],b=vtx[1]->coord[1];
    if(abox==NULL)
    {
        std::cout<<"newed:"<<bbid<<"->";
        abox = new DragBBox(l,t,r,b);
    }
    std::cout<<l<<","<<t<<","<<r<<","<<b<<","<<std::endl;
    abox->bbid=bbid;
    memcpy(abox->txt,txt,100);
    memcpy(abox->rgb,rgb,3);
    abox->vtx[0]->setCoord(l,t);
    abox->vtx[1]->setCoord(r,b);
}
void DragBBox::setVtx(int l,int t,int r,int b)
{
    prepareGeometryChange();
    vtx[0]->setCoord(l,t);
    vtx[1]->setCoord(r,b);
}
