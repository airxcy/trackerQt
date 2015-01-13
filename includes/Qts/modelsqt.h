#ifndef MODELSQT
#define MODELSQT
#include <QGraphicsItem>
#include <QPen>
#include <QBrush>
#include <QFont>
extern unsigned char feat_colos[6][3];

class BBox : public QGraphicsItem
{
public:
    BBox(double l,double t,double r,double b,QGraphicsItem * parent=0);
    BBox(QGraphicsItem * parent=0);
    QPen linePen;
    QBrush inBrush;
    QFont txtFont;
    int bbid;
    char txt[100];
    double vtx[4];
    QPointF dragStartPos;
    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void updateVtx(double l,double t,double r,double b);
    void clone(BBox * arr);
};
class DragVtx :public QGraphicsItem
{
public:
    DragVtx(QGraphicsItem * parent=0);
    DragVtx(int x,int y,QGraphicsItem * parent=0);
    int coord[2];
    int range;
    QPen dotPen;
    QBrush rgBrush;
    QColor color;
    bool selected;
    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;
    void setCoord(int x,int y);
};


class DragBBox : public QGraphicsItem
{
public:
    DragBBox(QGraphicsItem * paren=0);
    DragBBox(int l,int t,int r,int b,QGraphicsItem * parent=0);
    QPen linePen;
    QBrush inBrush;
    QFont txtFont;
    int bbid;
    char txt[100];
    std::vector<DragVtx*> vtx;
    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;

};

#endif // MODELSQT

