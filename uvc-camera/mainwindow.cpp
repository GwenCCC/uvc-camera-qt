#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "framesettingdialog.h"

#include <QList>
#include <QDebug>
#include <QSize>
#include "settingdialog.h"
#include <QDateTime>
#include <QVideoProbe>
#include <QTimer>
#include <QThread>
#include <QStandardPaths>

bool IsVideoRecording = false;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    qDebug("[%s] Start! \n", __FUNCTION__);
    ui->setupUi(this);

    //获取可用摄像头设备列表 
    //因为main函数里面已经判断了当前存在摄像头，所以这里不用判断摄像头个数了
    m_InfoList = QCameraInfo::availableCameras();
    int index = m_InfoList.size();
    
    for (int i = 0; i < index; i++)
    {
        qDebug() << index << m_InfoList.at(i).description(); //摄像头的设备名称
        ui->CameraChooseCombox->addItem(m_InfoList.at(i).description());
    }
    initSence();
    initCamera(m_InfoList[0]);
    initConnect();    
    setWindowTitle("相机");

    m_picSavePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    m_movSavePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    ui->tabWidget->setCurrentIndex(0);
     
 
}

MainWindow::~MainWindow()
{
    qDebug("[%s] Start! \n", __FUNCTION__);
    m_mediaRecorder->stop();
    delete ui;
}

void MainWindow::initSence()
{
    qDebug("[%s] Start! \n", __FUNCTION__);
    if (!m_graphicsVideoItem)
    {
        m_graphicsVideoItem = new QGraphicsVideoItem();
        m_graphicsVideoItem->setPos(0, 0);
    }
    if (!m_graphicsScene)
    {
        m_graphicsScene = new QGraphicsScene(this);
        m_graphicsScene->addItem(m_graphicsVideoItem);
        ui->graphicsView->setScene(m_graphicsScene);
        m_graphicsScene->setBackgroundBrush(Qt::black);
    }
}
//设置采集目标

//        m_pCamera->setCaptureDestination(QCameraImageCapture::CaptureToFile);
//    m_pCameraImageCapture->setBufferFormat(QVideoFrame::Format_ARGB32_Premultiplied);
//设置采集模式
//    m_pCamera->setCaptureMode(QCamera::CaptureViewfinder);//将其采集为图片
//    m_pCamera->setCaptureMode(QCamera::CaptureMode::CaptureStillImage);//将其采集到取景器中
void MainWindow::initCamera(QCameraInfo cameraInfo)
{
    qDebug("[%s] Start! \n", __FUNCTION__);
    if (m_mediaRecorder)
    {
        m_mediaRecorder->deleteLater();
        m_mediaRecorder = nullptr;
    }
    if (m_pCameraImageCapture)
    {
        m_pCameraImageCapture->deleteLater();
        m_pCameraImageCapture = nullptr;
    }
    if (m_pCamera)
    {
        m_pCamera->deleteLater();
        m_pCamera = nullptr;
    }

    m_pCamera = new QCamera(cameraInfo);
    m_pCameraImageCapture = new QCameraImageCapture(m_pCamera);

    //设置取景器
    m_pCamera->setViewfinder(m_graphicsVideoItem);
    m_pCamera->setCaptureMode(QCamera::CaptureVideo);

    // dosomething about the resolution

    m_mediaRecorder = new QMediaRecorder(m_pCamera);

    QCameraViewfinderSettings *m_finderSetting = new QCameraViewfinderSettings();
    m_finderSetting->setPixelAspectRatio(900, 200);
    m_pCamera->setViewfinderSettings(*m_finderSetting);
    QVideoEncoderSettings videosetting = m_mediaRecorder->videoSettings();
    //设置分辨率
    videosetting.setResolution(QSize(2592, 1944));

    m_widthTheight = float(2592) / float(1944);
    m_mediaRecorder->setVideoSettings(videosetting);

    //设置画面输出方式
    m_graphicsVideoItem->setAspectRatioMode(Qt::KeepAspectRatio);
    //开启相机
    m_pCamera->start();

    ui->statusBar->hide();

    auto *probe = new QVideoProbe(m_pCamera);

    m_InfoList = QCameraInfo::availableCameras();

    connect(m_mediaRecorder, &QMediaRecorder::durationChanged, this, [=](qint64 index)
            {
                QString str = QString("Recorded %1 sec").arg(m_mediaRecorder->duration() / 1000);
                qDebug() << str;
            });
    //    void imageCaptured(int id, const QImage &preview);
    //    void imageMetadataAvailable(int id, const QString &key, const QVariant &value);
    //    void imageAvailable(int id, const QVideoFrame &frame);
    connect(m_pCameraImageCapture, &QCameraImageCapture::imageAvailable, this, [=](int id, const QVideoFrame &frame)
            {
                qDebug() << "1111";
            });

    //一旦有探测到有视频，就触发了ProcessVideoFrame函数
    connect(probe, SIGNAL(videoFrameProbed(QVideoFrame)), this, SLOT(ProcessVideoFrame(QVideoFrame)));

    //    void stateChanged(QCamera::State state);
    //    void captureModeChanged(QCamera::CaptureModes);
     //    void statusChanged(QCamera::Status status);\
    QCamera::CaptureModes;
    connect(m_pCamera, &QCamera::stateChanged, this, [=](QCamera::State state)
            { qDebug() << state; });
    connect(m_pCamera, &QCamera::captureModeChanged, this, [=](QCamera::CaptureModes state)
            { qDebug() << state; });
    connect(m_pCamera, &QCamera::statusChanged, this, [=](QCamera::Status status)
            { qDebug() << status; });
}

void MainWindow::resizeMovieWindow()
{
    qDebug("[%s] Start! \n", __FUNCTION__);
    float width = ui->graphicsView->width();
    float height = ui->graphicsView->height();
    if (m_graphicsVideoItem->aspectRatioMode() == Qt::KeepAspectRatio)
    {
        if (width / height > m_widthTheight)
        {
            width = height * m_widthTheight;
        }
        else
        {
            height = width / m_widthTheight;
        }
    }
    m_graphicsScene->setSceneRect(QRect(0, 0, (width - 2) * m_playMultiple, (height - 2) * m_playMultiple));
    m_graphicsVideoItem->setSize(QSize(m_graphicsScene->width(), m_graphicsScene->height()));
}

bool MainWindow::isFile(const QString &path)
{
}
void MainWindow::initConnect()
{
    qDebug("[%s] Start! \n", __FUNCTION__);
    connect(ui->actionsetting, &QAction::triggered, this, [=]
            {
            settingDialog dialog(m_mediaRecorder);
            dialog.exec(); });
    connect(ui->actionFrameSetting, &QAction::triggered, this, [=]
            {
            FrameSettingDialog dialog(m_graphicsVideoItem,m_mediaRecorder);
            dialog.exec(); });
    //切换拍照和录像的函数
    connect(ui->tabWidget, &QTabWidget::tabBarClicked, this, [=](int index)
            {
       if(0==index)
       {
        ui->graphicsView_2->setVisible(true);
        //    m_pCamera->searchAndLock();

        //    m_pCamera->setCaptureMode(QCamera::CaptureStillImage);

        //    m_pCamera->unlock();
       }
       else if(1==index){
        ui->graphicsView_2->setVisible(false);

        //    m_pCamera->searchAndLock();

        //    m_pCamera->setCaptureMode(QCamera::CaptureVideo);

        //    m_pCamera->unlock();
       }
    });
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    qDebug("[%s] Start! \n", __FUNCTION__);
    resizeMovieWindow();
    return QMainWindow::resizeEvent(event);
}
void MainWindow::showEvent(QShowEvent *event)
{
    qDebug("[%s] Start! \n", __FUNCTION__);
    resizeMovieWindow();
    return QMainWindow::showEvent(event);
}
//开始录像
void MainWindow::on_startBtn_clicked()
{
    qDebug("[%s] Start! \n", __FUNCTION__);
    qDebug() << m_mediaRecorder->availableMetaData();
    qDebug() << m_mediaRecorder->audioSettings().codec();
    qDebug() << m_mediaRecorder->audioSettings().bitRate();
    qDebug() << m_mediaRecorder->audioSettings().channelCount();
    qDebug() << m_mediaRecorder->audioSettings().sampleRate();

    qDebug() << m_mediaRecorder->videoSettings().codec();
    qDebug() << m_mediaRecorder->videoSettings().bitRate();
    qDebug() << m_mediaRecorder->videoSettings().frameRate();
    qDebug() << m_mediaRecorder->videoSettings().resolution();

    qDebug() << m_mediaRecorder->containerFormat();

    //    qDebug()<<m_mediaRecorder->supportedContainers();

    m_mediaRecorder->stop();
    //    QAudioEncoderSettings audioSettings() const;
    //    QVideoEncoderSettings videoSettings() const;
    //    QString containerFormat() const;
    QString path;
    if (m_movSavePath != nullptr)
    {
        path = m_movSavePath + "/video";
    }
    else
    {
        path = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + "/" +
               QDateTime::currentDateTime().toString() + QString::number(QDateTime::currentMSecsSinceEpoch());
    }
    m_mediaRecorder->setOutputLocation(QUrl(path));
    m_mediaRecorder->record();
    IsVideoRecording = true;
}
//暂停录像
void MainWindow::on_pauseBtn_clicked()
{
    qDebug("[%s] Start! \n", __FUNCTION__);
    m_mediaRecorder->pause();
}
//停止录像
void MainWindow::on_stopBtn_clicked()
{
    qDebug("[%s] Start! \n", __FUNCTION__);
    m_mediaRecorder->stop();
    IsVideoRecording = false;
    QMessageBox::information(this, tr("拍摄程序"), tr("录像完成，已保存"));
}
//拍照
void MainWindow::on_picBtn_clicked()
{
    qDebug("[%s] Start! \n", __FUNCTION__);
    ui->picBtn->setChecked(false);
    if (QCamera::CaptureStillImage == m_pCamera->captureMode())
    {
        m_pCameraImageCapture->capture("UOS" + QDateTime::currentDateTime().toString());
    }
    else
    {
        qDebug() << "video";
        QImage image(m_graphicsScene->sceneRect().size().toSize(), QImage::Format_ARGB32);
        QPainter painter(&image);
        m_graphicsScene->render(&painter);
        QString path;
        if (m_picSavePath != nullptr)
        {
            path = m_picSavePath + "/" + "capture.png";
        }
        else
        {
            path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/" +
                   QDateTime::currentDateTime().toString() + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".png";
        }
        qDebug("[%s] path = %s \n", __FUNCTION__,path);

        image.save(path);

        /****************以下代码是将拍好的照片显示在graphicsView_2上*******************/
        //创建显示容器
        QGraphicsScene *scene = new QGraphicsScene;
        scene->setBackgroundBrush(Qt::black);
        //向容器中添加文件路径为fileName（QString类型）的文件
        scene->addPixmap(QPixmap(path));
        //借助graphicsView（QGraphicsView类）控件显示容器的内容
        ui->graphicsView_2->setScene(scene);
        //开始显示
        ui->graphicsView_2->show();
    }
    ui->picBtn->setChecked(true);
}


void MainWindow::ProcessVideoFrame(QVideoFrame frame)
{
    qDebug("[%s] Start! \n", __FUNCTION__);
    qDebug() << 111111;
}

//切换摄像头
void MainWindow::on_CameraChooseCombox_activated(int index)
{
    index = ui->CameraChooseCombox->currentIndex();
    qDebug()<<"Index"<< index <<": "<< ui->CameraChooseCombox->currentText();
    if(m_pCamera->isAvailable())
    {
        m_pCamera->stop();
        delete m_pCamera;
    }
     
    m_pCamera = new QCamera(m_InfoList[index]);
    m_pCameraImageCapture = new QCameraImageCapture(m_pCamera);

    //设置取景器
    m_pCamera->setViewfinder(m_graphicsVideoItem);
    m_pCamera->setCaptureMode(QCamera::CaptureVideo);

    // dosomething about the resolution

    m_mediaRecorder = new QMediaRecorder(m_pCamera);

    QCameraViewfinderSettings *m_finderSetting = new QCameraViewfinderSettings();
    m_finderSetting->setPixelAspectRatio(900, 200);
    m_pCamera->setViewfinderSettings(*m_finderSetting);
    QVideoEncoderSettings videosetting = m_mediaRecorder->videoSettings();
    //设置分辨率
    videosetting.setResolution(QSize(2592, 1944));

    m_widthTheight = float(2592) / float(1944);
    m_mediaRecorder->setVideoSettings(videosetting);
    //设置画面输出方式
    m_graphicsVideoItem->setAspectRatioMode(Qt::KeepAspectRatio);

    //开启相机
    m_pCamera->start();
    
    //设置存储路径
    m_picSavePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    m_movSavePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

}

void MainWindow::on_closeBtn_clicked()
{
    QApplication::exit() ;
}

void MainWindow::on_closeBtn_2_clicked()
{
    if(IsVideoRecording == true)
        QMessageBox::critical(this, tr("拍摄程序"), tr("当前录像未完成，无法退出"));
    else
        QApplication::exit() ;
}
