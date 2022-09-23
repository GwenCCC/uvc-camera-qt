#include "framesettingdialog.h"
#include "ui_framesettingdialog.h"

FrameSettingDialog::FrameSettingDialog(QGraphicsVideoItem * videoItem,QMediaRecorder* mediarecorder,QWidget *parent) :
    QDialog(parent),
    m_mediaRecorder(mediarecorder),
    m_videoItem(videoItem),
    ui(new Ui::FrameSettingDialog)
{
    qDebug("[%s] Start! \n",__FUNCTION__);
    ui->setupUi(this);
    if(m_mediaRecorder)
    {
        QList<QSize> supportedResolutions = m_mediaRecorder->supportedResolutions();
        foreach(const QSize &resolution, supportedResolutions) {
            ui->FrameSizeCombox->addItem(QString("%1x%2").arg(resolution.width()).arg(resolution.height()),
                                            QVariant(resolution));
        }
        if(supportedResolutions.size()>0)
        {
            QVideoEncoderSettings videosetting = m_mediaRecorder->videoSettings();
            QSize resolution = videosetting.resolution();
            if(resolution.width()>0 &&resolution.height()>0)
            {
                ui->FrameSizeCombox->setCurrentText(QString("%1x%2").arg(resolution.width()).arg(resolution.height()));
            }
            else {
                 ui->FrameSizeCombox->setCurrentIndex(supportedResolutions.size()-1);
            }

        }
    }


    if(m_videoItem)
    {
        ui->FrameoutCodecCombox->addItem("自适应窗口", 2);
        ui->FrameoutCodecCombox->addItem("保持输出比例", 1);
        ui->FrameoutCodecCombox->addItem("保持输出最大并扩展", 3);
        if(m_videoItem->aspectRatioMode()==Qt::IgnoreAspectRatio)
        {
            ui->FrameoutCodecCombox->setCurrentText("保持输出比例");
        }
        else if (m_videoItem->aspectRatioMode()==Qt::IgnoreAspectRatio) {
            ui->FrameoutCodecCombox->setCurrentText("自适应窗口");
        }
        else {
            ui->FrameoutCodecCombox->setCurrentText("保持输出最大并扩展");
        }
    }
    setWindowTitle("画面设置");
}

FrameSettingDialog::~FrameSettingDialog()
{
    qDebug("[%s] Start! \n",__FUNCTION__);
    delete ui;
}

void FrameSettingDialog::on_okBtn_clicked()
{
    qDebug("[%s] Start! \n",__FUNCTION__);
    switch (ui->FrameoutCodecCombox->currentData().toInt()) {
    case 1:
        m_videoItem->setAspectRatioMode(Qt::KeepAspectRatio);
        break;
    case 2:
        m_videoItem->setAspectRatioMode(Qt::IgnoreAspectRatio);
        break;
    case 3:
        m_videoItem->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
        break;
    default:
        break;

    }

    QVideoEncoderSettings settings = m_mediaRecorder->videoSettings();
    if(nullptr != ui->FrameSizeCombox->currentText())
    {
        settings.setResolution(ui->FrameSizeCombox->currentData().toSize());
    }
    m_mediaRecorder->setVideoSettings(settings);
    close();
}

void FrameSettingDialog::on_cancelBtn_clicked()
{
    qDebug("[%s] Start! \n",__FUNCTION__);
    close();
}
