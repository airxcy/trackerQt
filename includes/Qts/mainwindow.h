

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QGraphicsView>
#include "streamthread.h"
#include "viewqt.h"
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

signals:
    void clicked(QGraphicsSceneMouseEvent * event);
};
class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(QGraphicsScene *scene) : QGraphicsView(scene)
    {
    }
    GraphicsView(QGraphicsScene *scene, QWidget * parent = 0) : QGraphicsView(scene,parent)
    {
    }
    virtual void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;
};

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    StreamThread* streamThd;
    TrkScene* trkscene;
    GraphicsView* gview;
    DefaultScene* defaultscene;
    virtual void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;

    void makeConns();
public slots:
    void gviewClicked(QGraphicsSceneMouseEvent * event);
    void initUI();
    void updateAFrame();
};
//! [0]

#endif // MAINWINDOW_H
