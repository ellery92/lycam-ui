#pragma once
#include <QWidget>

class QPushButton;
class ImageViewer;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();

signals:
    void imageComplete(QImage);

private:
    QWidget *loadUiFile();
    void loadControls();
    void addImageViewer();

    QPushButton *ui_startButton;
    QPushButton *ui_stopButton;
    QWidget *ui_imageWidget;
    ImageViewer *ui_imageViewer;
};
