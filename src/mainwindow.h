#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>

#include <QFile>
#include <QTimer>
#include <QtAV>
#include <QtAVWidgets>
#include <qmediaplayer.h>
#include <QtGui/QMovie>
#include <qvideowidget.h>

#include <unistd.h>

class QPushButton;
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT
        // testfiles from http://techslides.com/sample-webm-ogg-and-mp4-video-files-for-html5
        const QString        FILE_PATH = "qrc:/small.webm";

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();
        char *memory;
        QByteArray *memory2;

    protected:
        Ui::MainWindow *ui;
        QFile *file;
        QScopedPointer<QtAV::AVPlayer> QtAVMediaDecoder;
        QScopedPointer<QtAV::WidgetRenderer> QtAVVideoWidget;
        QScopedPointer<QMediaPlayer> QtMediaDecoder;
        QScopedPointer<QVideoWidget> QtVideoWidget;

        int           repeats_qtmultimedia = 0;
        int           repeats_qtav = 0;
        qint64        max_memory_used = 0;
        void          timerEvent(QTimerEvent *event);
        qint64        getCurrentRSS();
    public slots:

    public slots:
         void clickAllocateFileLoad();
         void clickDeAllocateFileLoad();
         void clickAutoplayQtMultimedia();
         void clickAllocateQtMultimedia();
         void clickDeAllocateQtMultimedia();
         void restartQtMultimedia();
         void doStateChangedQtMultimedia(QMediaPlayer::State state);
         void doMediaStatusChangedQtAV(QtAV::MediaStatus state);
         void clickAutoplayQtAV();
         void clickAllocateQtAV();
         void clickDeAllocateQtAV();
         void outputMemoryUsage();
};

#endif // MAINWINDOW_H
