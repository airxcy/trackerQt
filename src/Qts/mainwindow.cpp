#include "Qts/mainwindow.h"

#include <QtWidgets>


//! [1]
MainWindow::MainWindow()
{
    defaultscene = new DefaultScene(0, 0, 440, 240);
    gview = new GraphicsView(defaultscene,this);

    defaultscene->setBackgroundBrush(QImage(":/images/default.png"));
    gview->setFixedSize(defaultscene->width()+2,defaultscene->height()+2);
    //gview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //gview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gview->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setMinimumSize(gview->width()+10,gview->height()+20);
    //setWindowFlags(Qt::FramelessWindowHint);
    setStyleSheet("QWidget { background-color: rgb(0, 0, 0); }");
    streamThd = new StreamThread(this);
    makeConns();
}
void MainWindow::makeConns()
{
    connect(defaultscene,SIGNAL(clicked(QGraphicsSceneMouseEvent *)),this,SLOT(gviewClicked(QGraphicsSceneMouseEvent *)));
    connect(streamThd,SIGNAL(initSig()),this,SLOT(initUI()),Qt::BlockingQueuedConnection);
    connect(streamThd,SIGNAL(aFrameDone()),this,SLOT(updateAFrame()));
}
void MainWindow::resizeEvent(QResizeEvent * evt)
{
    gview->resizeEvent(evt);
}

void MainWindow::gviewClicked(QGraphicsSceneMouseEvent * event)
{
    //QMessageBox::question(NULL, "Test", "msg",QMessageBox::Ok);
    //QString fileName = QFileDialog::getOpenFileName(this,tr("Open vid"), "/", tr("Vid Files (*.avi *.mp4 *.mkv)"));
    QString fileName="C:/Users/xcy/Documents/CVProject/data/label_company/200412.avi";
    streamThd->streamStart(fileName.toStdString());
}

void MainWindow::initUI()
{
    int fw=streamThd->framewidth,fh=streamThd->frameheight;
    trkscene = new TrkScene(0, 0, fw, fh);
    //trkscene->setBackgroundBrush(QImage(streamThd->framedata,fw, fh, QImage::Format_RGB888));
    trkscene->streamThd=streamThd;

    gview->setFixedSize(fw+2,fh+2);
    gview->setScene(trkscene);
    setMinimumSize(gview->width()+10,gview->height()+20);
}

void MainWindow::updateAFrame()
{
    //int fw=streamThd->framewidth,fh=streamThd->frameheight;
    //gview->scene()->setBackgroundBrush(QImage(streamThd->framedata,fw, fh, QImage::Format_RGB888));
}
void GraphicsView::resizeEvent(QResizeEvent * evt)
{

}
void DefaultScene::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    emit clicked(event);
}

