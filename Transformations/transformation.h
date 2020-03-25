#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include <QMainWindow>
#include "imageviewer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Transformation; }
QT_END_NAMESPACE

class Transformation : public QMainWindow
{
    Q_OBJECT

public:
    Transformation(QWidget *parent = nullptr);
    ~Transformation();

private:
    Ui::Transformation *ui;

    // Events
    bool eventFilter(QObject *obj, QEvent *event);
    bool ImageViewerEventFilter(ImageViewer* obj, QEvent *event);

    bool ImageViewerMove(ImageViewer* obj, QMouseEvent* event);
    bool ImageViewerMouseButtonPress(ImageViewer* obj, QMouseEvent *event);
    bool ImageViewerMouseButtonRelease(ImageViewer* obj, QMouseEvent *event);
    bool ImageViewerWheel(ImageViewer* obj, QWheelEvent* event);
    void setRasterizationAlgorithm(int num);

    ImageViewer* getImageViewer();
private slots:
    void on_drawButton_clicked();
    void on_eraseButton_clicked();
    void on_changeColorButton_clicked();

    void on_rotate1Button_clicked();
    void on_rotate2Button_clicked();

    void on_zoomInButton_clicked();
    void on_zoomOutButton_clicked();

    void on_swapButton_clicked();

    void on_shearRightButton_clicked();
    void on_shearLeftButton_clicked();

    void on_resetButton_clicked();

    void on_applyButton_clicked();

    void on_cBeginButton_clicked();
    void on_cEndButton_clicked();
};
#endif // TRANSFORMATION_H
