

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QGraphicsView>
#include <QLayout>
#include <QPushButton>
class StreamThread;
class TrkScene;
class GraphicsView;
class RefScene;
class DefaultScene;
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
    QGridLayout* layout;

    QPushButton* startTag;
    QPushButton* addTag;
    QPushButton* transTag;
    QPushButton* finishTag;
    QPushButton* editTag;
    QPushButton* resumeTag;

    QString btnstyle;
    //virtual void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;
    void setupLayout();
    void makeConns();
public slots:
    void gviewClicked(QGraphicsSceneMouseEvent * event);
    void initUI();
    void startTagging();
    void finishTagging();
    void addATag();
    void transfer();
    void pauseEdit();
    void resume();
};
//! [0]

#endif // MAINWINDOW_H
