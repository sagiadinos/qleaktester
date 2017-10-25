#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->AllocateFileLoad, SIGNAL(clicked(bool)), this, SLOT(clickAllocateFileLoad()));
    connect(ui->DeAllocateFileLoad, SIGNAL(clicked(bool)), this, SLOT(clickDeAllocateFileLoad()));

    connect(ui->AutoplayQtMultiMedia, SIGNAL(clicked(bool)), this, SLOT(clickAutoplayQtMultimedia()));
    connect(ui->AllocateQtMultimedia, SIGNAL(clicked(bool)), this, SLOT(clickAllocateQtMultimedia()));
    connect(ui->DeAllocateQtMultimedia, SIGNAL(clicked(bool)), this, SLOT(clickDeAllocateQtMultimedia()));

    connect(ui->AutoplayQtAV, SIGNAL(clicked(bool)), this, SLOT(clickAutoplayQtAV()));
    connect(ui->AllocateQtAV, SIGNAL(clicked(bool)), this, SLOT(clickAllocateQtAV()));
    connect(ui->DeAllocateQtAV, SIGNAL(clicked(bool)), this, SLOT(clickDeAllocateQtAV()));

    ui->RepeatsQtMultimedia->setText(QString::number(repeats_qtmultimedia)+"x");
    ui->RepeatsQtAV->setText(QString::number(repeats_qtav)+"x");
    startTimer(250);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief classic memory allocation
 */
void MainWindow::clickAllocateFileLoad()
{
    // QFile do not like ressources qrc
    QString file_path = "";
    if (FILE_PATH.contains("qrc"))
        file_path =FILE_PATH.mid(3);
    else
        file_path = FILE_PATH;
    file = new QFile(file_path);
    if (!file->open(QIODevice::ReadOnly))
    {
        qDebug() << "File open error" << file->errorString();
        return;
    }
    int size = file->size();
    memory = new char[size];
    for (int i = 0; i < 10; i++)
    {
        file->read(memory, size);
    }
    file->close();
    ui->DeAllocateFileLoad->setEnabled(true);
    ui->AllocateFileLoad->setEnabled(false);
}

void MainWindow::clickDeAllocateFileLoad()
{
    delete[] memory;
    delete file;
    memory = NULL;
    ui->DeAllocateFileLoad->setEnabled(false);
    ui->AllocateFileLoad->setEnabled(true);
}

void MainWindow::clickAutoplayQtMultimedia()
{
    if (ui->AutoplayQtMultiMedia->isChecked())
        clickAllocateQtMultimedia();
}

void MainWindow::clickAllocateQtMultimedia()
{
     QtVideoWidget.reset(new QVideoWidget);
     QtMediaDecoder.reset(new QMediaPlayer);
     connect(QtMediaDecoder.data(), SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(doStateChangedQtMultimedia(QMediaPlayer::State)));
     QtMediaDecoder.data()->setVideoOutput(QtVideoWidget.data());
     ui->video_widget_layout->addWidget(QtVideoWidget.data());
     QtMediaDecoder.data()->setMedia(QUrl(FILE_PATH));
     QtMediaDecoder.data()->play();
     ui->DeAllocateQtMultimedia->setEnabled(true);
     ui->AllocateQtMultimedia->setEnabled(false);
     repeats_qtmultimedia++;
     ui->RepeatsQtMultimedia->setText(QString::number(repeats_qtmultimedia)+"x");
}

void MainWindow::clickDeAllocateQtMultimedia()
{
    QtMediaDecoder.data()->stop();
    QtMediaDecoder.data()->setMedia(QMediaContent());
    ui->video_widget_layout->removeWidget(QtVideoWidget.data());

    ui->DeAllocateQtMultimedia->setEnabled(false);
    ui->AllocateQtMultimedia->setEnabled(true);
    if (ui->AutoplayQtMultiMedia->isChecked())
        QTimer::singleShot(2000, this, SLOT(clickAllocateQtMultimedia()));

}

void MainWindow::restartQtMultimedia()
{

}

void MainWindow::doStateChangedQtMultimedia(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState)
        clickDeAllocateQtMultimedia();
}


void MainWindow::clickAutoplayQtAV()
{
    if (ui->AutoplayQtAV->isChecked())
        clickAllocateQtAV();
}

void MainWindow::clickAllocateQtAV()
{
     QtAVVideoWidget.reset(new QtAV::WidgetRenderer);
     QtAVMediaDecoder.reset(new QtAV::AVPlayer);
     connect(QtAVMediaDecoder.data(), SIGNAL(mediaStatusChanged(QtAV::MediaStatus)), this, SLOT(doMediaStatusChangedQtAV(QtAV::MediaStatus)));
     QtAVMediaDecoder.data()->addVideoRenderer(QtAVVideoWidget.data());
     ui->video_widget_layout->addWidget(QtAVVideoWidget.data());
     QtAVMediaDecoder.data()->setAsyncLoad(false);
     QtAVMediaDecoder.data()->setFile(FILE_PATH);
     if (!QtAVMediaDecoder.data()->load())
     {
         qDebug() << "Error loading ile";
         return;
     }
     QtAVMediaDecoder.data()->play();
     ui->DeAllocateQtAV->setEnabled(true);
     ui->AllocateQtAV->setEnabled(false);
     repeats_qtav++;
     ui->RepeatsQtAV->setText(QString::number(repeats_qtav)+"x");
}

void MainWindow::clickDeAllocateQtAV()
{
    QtAVMediaDecoder.data()->stop();
    QtAVMediaDecoder.data()->removeVideoRenderer(QtAVVideoWidget.data());
    ui->video_widget_layout->removeWidget(QtAVVideoWidget.data());
    QtAVMediaDecoder.data()->setFile("");
    ui->DeAllocateQtAV->setEnabled(false);
    ui->AllocateQtAV->setEnabled(true);
    if (ui->AutoplayQtAV->isChecked())
        QTimer::singleShot(2000, this, SLOT(clickAllocateQtAV()));
}


void MainWindow::doMediaStatusChangedQtAV(QtAV::MediaStatus state)
{
    if (state == QtAV::EndOfMedia)
        clickDeAllocateQtAV();
}



void MainWindow::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    outputMemoryUsage();
}

void MainWindow::outputMemoryUsage()
{
    qint64  actual = getCurrentRSS();
    if (actual > max_memory_used)
        max_memory_used = actual;
    double d_actual = (double)actual / (double)1048576;
    double d_max = (double)max_memory_used / (double)1048576;
    ui->OutputMemoryUse->setText(QString("Now: %1" ).arg(d_actual, 0, 'f', 2) + " MiB");
    ui->MaxMemoryUsed->setText(QString("Max: %1" ).arg(d_max, 0, 'f', 2) + " MiB");
}


qint64 MainWindow::getCurrentRSS()
{
#if defined Q_OS_WIN32
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo( GetCurrentProcess( ), &info, sizeof(info) );
    return (qint64)info.WorkingSetSize;

#elif defined Q_OS_DARWIN
    struct mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
    if ( task_info( mach_task_self( ), MACH_TASK_BASIC_INFO,
        (task_info_t)&info, &infoCount ) != KERN_SUCCESS )
        return (qint64)0L;		/* Can't access? */
    return (qint64)info.resident_size;

#elif defined Q_OS_LINUX // work on Android too
    QFile fp("/proc/self/statm");
    if (!fp.open(QIODevice::ReadOnly))
        return 0;		/* Can't open? */
    QTextStream in(&fp);
    QStringList fields = in.readLine().split(" ");
    fp.close();
    return fields.at(1).toLong() * (qint64)sysconf(_SC_PAGESIZE);
#else
    /* AIX, BSD, Solaris, and Unknown OS ------------------------ */
    return (qint64)0L;			/* Unsupported. */
#endif
}

