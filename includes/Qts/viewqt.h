#ifndef VIEWQT
#define VIEWQT

#include <QGraphicsScene>
#include <QGraphicsView>
#include <vector>
class StreamThread;
class BBox;
class RefScene;
class DragVtx;
class DragBBox;
class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(QGraphicsScene *scene) : QGraphicsView(scene)
    {
        setMouseTracking(true);
        setAcceptDrops(true);
    }
    GraphicsView(QGraphicsScene *scene, QWidget * parent = 0) : QGraphicsView(scene,parent)
    {
        setMouseTracking(true);
        setAcceptDrops(true);
    }
    virtual void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;
};
class TrkScene :public QGraphicsScene
{
    Q_OBJECT
public:
    TrkScene(const QRectF & sceneRect, QObject * parent = 0);
    TrkScene(qreal x, qreal y, qreal width, qreal height, QObject * parent = 0);

    StreamThread* streamThd;
    RefScene* refscene;
    int bb_N;
    std::vector<BBox*> bbvec;
    BBox* dragBB;

    QBrush bgBrush;
    QBrush fgBrush;
    bool syncflag;
    virtual void drawBackground(QPainter * painter, const QRectF & rect) Q_DECL_OVERRIDE;
    //virtual void drawBackground(QPainter * painter, const QRectF & rect) Q_DECL_OVERRIDE;
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    /*
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    */
public slots:
    void initBBox();
};

class RefScene :public QGraphicsScene
{
    Q_OBJECT
 public:
    RefScene(const QRectF & sceneRect, QObject * parent = 0);
    RefScene(qreal x, qreal y, qreal width, qreal height, QObject * parent = 0);
    std::vector<BBox*> bbvec;
    QImage bgimg;
    QBrush bgBrush;
    DragVtx* dragvtx;
    DragBBox* dragBB;
    void initBBox(std::vector<BBox*>& arr,int N);
    void initBg(QImage & img);
    virtual void drawBackground(QPainter * painter, const QRectF & rect) Q_DECL_OVERRIDE;
};

#endif // VIEWQT

