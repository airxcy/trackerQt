#include "Qts/mainwindow.h"
#include "Qts/streamthread.h"
#include "Qts/viewqt.h"
#include <QtWidgets>
#include <QSizePolicy>
#include <iostream>
//! [1]
MainWindow::MainWindow()
{
    cWidget = new QWidget(this);
    setCentralWidget(cWidget);
    defaultscene = new DefaultScene(0, 0, 440, 240);
    ascene = new QGraphicsScene(0, 0, 440, 240);
    gview = new GraphicsView(defaultscene,this);
    refview = new GraphicsView(ascene,this);
    streamThd = new StreamThread(this);
    setupLayout();
    makeConns();
}
void MainWindow::setupLayout()
{
    layout=new QBoxLayout(QBoxLayout::LeftToRight,cWidget);
    defaultscene->setBackgroundBrush(QImage(":/images/default.png"));
    gview->setFixedSize(defaultscene->width()+2,defaultscene->height()+2);
    //gview->setMinimumSize(defaultscene->width()+2,defaultscene->height()+2);
    //gview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //gview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gview->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    refview->setFixedSize(defaultscene->width()+2,defaultscene->height()+2);
    layout->addWidget(refview,0,Qt::AlignLeft);
    layout->addWidget(gview,0,Qt::AlignLeft);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    cWidget->setLayout(layout);
    cWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    //setMinimumSize(gview->width()+10,gview->height()+20);

    //setWindowFlags(Qt::FramelessWindowHint);
    //setStyleSheet("MainWindow { background-color: rgb(0, 0, 0); }");
    setStyleSheet("QWidget { background-color: #E46A6B; }");
    cWidget->setStyleSheet("QWidget { background-color: rgb(10, 20, 30); }");
    setFixedSize(cWidget->minimumSize());
}
void MainWindow::makeConns()
{
    connect(defaultscene,SIGNAL(clicked(QGraphicsSceneMouseEvent *)),this,SLOT(gviewClicked(QGraphicsSceneMouseEvent *)));
    connect(streamThd,SIGNAL(initSig()),this,SLOT(initUI()),Qt::BlockingQueuedConnection);
    connect(streamThd,SIGNAL(aFrameDone()),this,SLOT(updateAFrame()));

}
/*
void MainWindow::resizeEvent(QResizeEvent * evt)
{
    gview->resizeEvent(evt);
}
*/
void MainWindow::gviewClicked(QGraphicsSceneMouseEvent * event)
{
    //QMessageBox::question(NULL, "Test", "msg",QMessageBox::Ok);
    //QString fileName = QFileDialog::getOpenFileName(this,tr("Open vid"), "/", tr("Vid Files (*.avi *.mp4 *.mkv)"));
    QString fileName="C:/Users/xcy/Documents/CVProject/data/label_company/200412.avi";
    streamThd->streamStart(fileName.toStdString());
}

void MainWindow::initUI()
{
    std::cout<<"initUI begin"<<std::endl;
    int fw=streamThd->framewidth,fh=streamThd->frameheight;
    trkscene = new TrkScene(0, 0, fw, fh);
    refscene = new RefScene(0,0,fw,fh);
    //trkscene->setBackgroundBrush(QImage(streamThd->framedata,fw, fh, QImage::Format_RGB888));
    trkscene->streamThd=streamThd;
    trkscene->refscene=refscene;
    streamThd->trkscene=trkscene;
    gview->setFixedSize(fw+2,fh+2);
    gview->setScene(trkscene);

    refview->setFixedSize(fw+2,fh+2);
    refview->setScene(refscene);
    //setMinimumSize(gview->width()+10,gview->height()+20);
    connect(streamThd,SIGNAL(initBBox()),trkscene,SLOT(initBBox()));//,Qt::BlockingQueuedConnection);
}

void MainWindow::updateAFrame()
{
    //int fw=streamThd->framewidth,fh=streamThd->frameheight;
    //gview->scene()->setBackgroundBrush(QImage(streamThd->framedata,fw, fh, QImage::Format_RGB888));
}
void DefaultScene::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    emit clicked(event);
}

