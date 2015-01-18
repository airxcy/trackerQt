#include "Qts/mainwindow.h"
#include "Qts/streamthread.h"
#include "Qts/viewqt.h"
#include <QtWidgets>
#include <QSizePolicy>
#include <iostream>
#include <QPalette>
#include <QKeySequence>
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
    cWidget->setStyleSheet("QWidget { background-color: rgb(105,210,231); }");
    setupLayout();
    makeConns();
    //setWindowFlags(Qt::FramelessWindowHint);
    setStyleSheet(" QPushButton:disabled {background: rgba(0,0,0,100)}");
    setFixedSize(cWidget->minimumSize());
}
void MainWindow::setupLayout()
{


    layout=new QGridLayout(cWidget);
    btnstyle = "QPushButton { background: rgba(243,134,48,100); color:rgba(243,134,48);} QPushButton:disabled{background: rgba(0,0,0,100)}";
    //defaultscene->setBackgroundBrush(QImage(":/images/default.png"));
    gview->setFixedSize(defaultscene->width()+2,defaultscene->height()+2);
    //gview->setMinimumSize(defaultscene->width()+2,defaultscene->height()+2);
    //gview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //gview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gview->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    refview->setFixedSize(defaultscene->width()+2,defaultscene->height()+2);
    layout->addWidget(refview,0,0,1,3,Qt::AlignLeft);
    layout->addWidget(gview,0,3,1,3,Qt::AlignLeft);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    cWidget->setLayout(layout);
    cWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    startTag = new QPushButton(cWidget);
    startTag->setStyleSheet(btnstyle);
    startTag->setFixedHeight(50);
    startTag->setText("Start Tagging");
    startTag->setFont(QFont("Times",20));
    startTag->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    startTag->setShortcut(QKeySequence(" "));
    layout->addWidget(startTag,1,0,Qt::AlignLeft);
    addTag = new QPushButton(cWidget);
    addTag->setStyleSheet(btnstyle);
    addTag->setFixedHeight(50);
    addTag->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    addTag->setFont(QFont("Times",20));
    addTag->setText("Add");
    addTag->setShortcut(QKeySequence("A"));
    layout->addWidget(addTag,1,1,Qt::AlignLeft);
    transTag = new QPushButton(cWidget);
    transTag->setStyleSheet(btnstyle);
    transTag->setFixedHeight(50);
    transTag->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    transTag->setFont(QFont("Times",20));
    transTag->setText("Transfer");
    transTag->setShortcut(QKeySequence("A"));
    layout->addWidget(transTag,1,2,Qt::AlignLeft);
    finishTag = new QPushButton(cWidget);
    finishTag->setStyleSheet(btnstyle);
    finishTag->setFixedHeight(50);
    finishTag->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    finishTag->setText("Finish Tagging");
    finishTag->setFont(QFont("Times",20));
    layout->addWidget(finishTag,1,3,Qt::AlignLeft);
    editTag = new QPushButton(cWidget);
    editTag->setStyleSheet(btnstyle);
    editTag->setFixedHeight(50);
    editTag->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    editTag->setText("Pause&Edit");
    editTag->setFont(QFont("Times",20));
    layout->addWidget(editTag,1,4,Qt::AlignLeft);
    resumeTag = new QPushButton(cWidget);
    resumeTag->setStyleSheet(btnstyle);
    resumeTag->setFixedHeight(50);
    resumeTag->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    resumeTag->setText("Resume");
    resumeTag->setFont(QFont("Times",20));
    layout->addWidget(resumeTag,1,5,Qt::AlignLeft);
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
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open vid"), "/Users/xcy/Documents/CVProject/data/Street", tr("Vid Files (*.avi *.mp4 *.mkv *.mts)"));
    //QString fileName="C:/Users/xcy/Documents/CVProject/data/label_company/200412.avi";
    if(!fileName.isEmpty())
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
    streamThd->refscene=refscene;
    refscene->streamThd=streamThd;
    refscene->trkscene=trkscene;
    gview->setFixedSize(fw+2,fh+2);
    gview->setScene(trkscene);

    refview->setFixedSize(fw+2,fh+2);
    refview->setScene(refscene);
    //setMinimumSize(gview->width()+10,gview->height()+20);
    connect(streamThd,SIGNAL(initBBox()),trkscene,SLOT(initBBox()));//,Qt::BlockingQueuedConnection);

    connect(startTag,SIGNAL(clicked()),this,SLOT(startTagging()));
    connect(finishTag,SIGNAL(clicked()),this,SLOT(finishTagging()));
    connect(addTag,SIGNAL(clicked()),this,SLOT(addATag()));
    connect(transTag,SIGNAL(clicked()),this,SLOT(transfer()));
    connect(editTag,SIGNAL(clicked()),this,SLOT(pauseEdit()));
    connect(resumeTag,SIGNAL(clicked()),this,SLOT(resume()));

    finishTag->setEnabled(false);
    addTag->setEnabled(false);
    transTag->setEnabled(false);
    editTag->setEnabled(false);
    resumeTag->setEnabled(false);
}
void MainWindow::startTagging()
{
    if(streamThd!=NULL)
    {
        streamThd->pause=true;
        refscene->isTagging=true;
        std::cout<<"start"<<std::endl;
        startTag->setEnabled(false);
        addTag->setEnabled(true);
        transTag->setEnabled(true);
    }
}
void MainWindow::addATag()
{
    if(refscene->isTagging)
    {
        refscene->addADragBB();
    }
}
void MainWindow::transfer()
{
    if(refscene->isTagging)
    {
        refscene->startTrans();
        addTag->setEnabled(false);
        transTag->setEnabled(false);
        finishTag->setEnabled(true);
    }
}
void MainWindow::finishTagging()
{
    if(streamThd!=NULL)
    {
        streamThd->pause=false;
        refscene->isTagging=false;
        refscene->endTrans();
        streamThd->initBB();
        streamThd->cv.wakeAll();
        finishTag->setEnabled(false);
        editTag->setEnabled(true);
        std::cout<<"finish"<<std::endl;
    }
}
void MainWindow::pauseEdit()
{

    if(streamThd->gtInited)
    {
        std::cout<<"pause Edit"<<std::endl;
        streamThd->pause=true;
        trkscene->startEdit();
        editTag->setEnabled(false);
        resumeTag->setEnabled(true);
    }
}
void MainWindow::resume()
{
    std::cout<<"resume"<<std::endl;
    if(streamThd->gtInited)
    {
        streamThd->pause=false;
        trkscene->endEdit();
        streamThd->cv.wakeAll();
        resumeTag->setEnabled(false);
        editTag->setEnabled(true);
    }
}
