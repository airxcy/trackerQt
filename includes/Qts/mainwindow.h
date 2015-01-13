

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QGraphicsView>
#include <QLayout>
class StreamThread;
class TrkScene;
class GraphicsView;
class RefScene;
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


//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    StreamThread* streamThd;
    QWidget* cWidget;
    TrkScene* trkscene;
    RefScene* refscene;
    GraphicsView* gview;
    GraphicsView* refview;
    QGraphicsScene * ascene;
    DefaultScene* defaultscene;
    QBoxLayout* layout;
    //virtual void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;
    void setupLayout();
    void makeConns();
public slots:
    void gviewClicked(QGraphicsSceneMouseEvent * event);
    void initUI();
    void updateAFrame();
};
//! [0]

#endif // MAINWINDOW_H
