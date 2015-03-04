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
class DefaultScene : public QGraphicsScene
{
    Q_OBJECT
public:
    DefaultScene(const QRectF & sceneRect, QObject * parent = 0):QGraphicsScene(sceneRect, parent)
    {

    }
    DefaultScene(qreal x, qreal y, qreal width, qreal height, QObject * parent = 0):QGraphicsScene( x, y, width, height, parent)
    {

    }
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event ) Q_DECL_OVERRIDE;
    virtual void drawBackground(QPainter * painter, const QRectF & rect) Q_DECL_OVERRIDE;
signals:
    void clicked(QGraphicsSceneMouseEvent * event);
};
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
    std::vector<DragBBox*> dragBBvec;
    QBrush bgBrush;
    QBrush fgBrush;
    bool syncflag;
    virtual void drawBackground(QPainter * painter, const QRectF & rect) Q_DECL_OVERRIDE;
    //virtual void drawBackground(QPainter * painter, const QRectF & rect) Q_DECL_OVERRIDE;
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    void drawFPts(QPainter * painter, const QRectF & rect);
    void addDragBBvec(std::vector<DragBBox*>& avec);
    void startEdit();
    void endEdit();

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

    QBrush bgBrush;
    std::vector<DragBBox*> dragBBvec;
    StreamThread* streamThd;
    TrkScene* trkscene;
    bool isTagging,isTransFering,tranfered;
    void initBBox(std::vector<BBox*>& arr,int N);
    void initBg(QImage & img);
    virtual void drawBackground(QPainter * painter, const QRectF & rect) Q_DECL_OVERRIDE;

    void addADragBB();
    void startTrans();
    void endTrans();
};

#endif // VIEWQT

