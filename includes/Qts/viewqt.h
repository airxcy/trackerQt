#ifndef VIEWQT
#define VIEWQT
#include <QGraphicsScene>
#include "streamthread.h"
class TrkScene :public QGraphicsScene
{
    Q_OBJECT
public:
    TrkScene(const QRectF & sceneRect, QObject * parent = 0);
    TrkScene(qreal x, qreal y, qreal width, qreal height, QObject * parent = 0);

    StreamThread* streamThd;

    virtual void drawForeground(QPainter * painter, const QRectF & rect) Q_DECL_OVERRIDE;
    //virtual void drawBackground(QPainter * painter, const QRectF & rect) Q_DECL_OVERRIDE;
};

#endif // VIEWQT

