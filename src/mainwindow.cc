#include "mainwindow.hh"
#include "imageviewer.hh"

#include <QVBoxLayout>
#include <QtUiTools>
#include <QMessageBox>

#include <lyucamera.h>
#include <iostream>

static void streamCallback (int index, LyuBuffer *buffer, void *user_data)
{
    int w = lyu_buffer_get_image_width(buffer);
    int h = lyu_buffer_get_image_height(buffer);
    LyuPixelFormat format = lyu_buffer_get_image_pixel_format(buffer);
    MainWindow *mainWindow = static_cast<MainWindow*>(user_data);
#if 1
    double frameRate = lyu_camera_get_frame_rate(index);
	std::cout << "receive stream: " << w << "x" << h << "framerate: " << frameRate
              << std::hex << "format: " << format << std::endl;
#endif
    if (format == LYU_PIXEL_FORMAT_MONO_8) {
        QImage image(w, h, QImage::Format_Grayscale8);
        uchar *p = (uchar*)lyu_buffer_get_data(buffer, NULL);
        for (int k = 0; k < h; k++) {
            memcpy(image.scanLine(k), p, w);
            p += w;
        }
        emit mainWindow->imageComplete(image);
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    QWidget *formWidget = loadUiFile();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(formWidget);
    setLayout(layout);

    loadControls();

    addImageViewer();

    QMetaObject::connectSlotsByName(this);
    connect(this, &MainWindow::imageComplete, ui_imageViewer, &ImageViewer::setImage);
}

MainWindow::~MainWindow()
{
    lyu_camera_stop_acquisition(0);
    lyu_camera_close(0);
}

QWidget* MainWindow::loadUiFile() {
    QString appDir = QCoreApplication::applicationDirPath();

    QFile file(":/forms/mainwindow.ui");
    file.open(QFile::ReadOnly);

    QUiLoader loader;
    QWidget *formWidget = loader.load(&file, NULL);
    file.close();

    return formWidget;
}

void MainWindow::loadControls() {
    ui_startButton = findChild<QPushButton*>("startButton");
    ui_stopButton = findChild<QPushButton*>("stopButton");
    ui_imageWidget = findChild<QWidget*>("imageWidget");
}

void MainWindow::on_startButton_clicked() {
    int n = lyu_count_cameras();
    if (n <= 0) {
        QMessageBox msgBox;
        msgBox.setText("No camera found");
        msgBox.exec();
        return;
    }

    if (!lyu_camera_open(0)) {
        QMessageBox msgBox;
        msgBox.setText("camera open failed");
        msgBox.exec();
        return;
    }

    lyu_camera_set_acquisition_mode(0, LYU_ACQUISITION_MODE_CONTINUOUS);
    lyu_camera_set_pixel_format(0, LYU_PIXEL_FORMAT_MONO_8);

    lyu_camera_start_acquisition(0, streamCallback, this);

    ui_startButton->setEnabled(false);
    ui_stopButton->setEnabled(true);
}

void MainWindow::on_stopButton_clicked() {
    lyu_camera_stop_acquisition(0);
    lyu_camera_close(0);

    ui_startButton->setEnabled(true);
    ui_stopButton->setEnabled(false);
}

void MainWindow::addImageViewer() {
    ui_imageViewer = new ImageViewer("video");
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    ui_imageWidget->setLayout(layout);
    layout->addWidget(ui_imageViewer);
}
