#include "transformation.h"
#include "ui_transformation.h"
#include "imageviewer.h"

Transformation::Transformation(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Transformation)
{
    ui->setupUi(this);

    // Disable buttons
    ui->eraseButton->setEnabled(false);
    ui->changeColorButton->setEnabled(false);
    ui->transformationsBox->setEnabled(false);
    ui->frame->installEventFilter(this);
    // Widget setup
    new ImageViewer(ui->frame->size(), ui->frame);
}

Transformation::~Transformation()
{
    delete ui;
}

ImageViewer* Transformation::getImageViewer()
{
    return static_cast<ImageViewer*>(ui->frame->children()[0]);
}


// Events
bool Transformation::eventFilter(QObject *obj, QEvent *event)
{
    if(obj->objectName() == "frame") {
        return ImageViewerEventFilter(static_cast<ImageViewer*>(obj->children()[0]), event);
    } else {
        return false;
    }
}

bool Transformation::ImageViewerEventFilter(ImageViewer* obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress) {
        return ImageViewerMouseButtonPress(obj, static_cast<QMouseEvent*>(event));
    } else if(event->type() == QEvent::Wheel) {
        return ImageViewerWheel(obj, static_cast<QWheelEvent*>(event));
    } else if(event->type() == QEvent::MouseMove) {
        return ImageViewerMove(obj, static_cast<QMouseEvent*>(event));
    } else if(event->type() == QEvent::MouseButtonRelease) {
        return ImageViewerMouseButtonRelease(obj, static_cast<QMouseEvent*>(event));
    } else {
        return false;
    }
}

bool Transformation::ImageViewerMove(ImageViewer* obj, QMouseEvent* event)
{
    if(!obj->getIfMove())
        return false;
    obj->move(event->pos());
    return true;
}

bool Transformation::ImageViewerMouseButtonPress(ImageViewer* obj, QMouseEvent *event)
{
    if(!ui->drawButton->isEnabled())
    {
        obj->moveBegin(event->pos());
        return true;
    }
    obj->addPoint(event->pos());
    obj->drawPoints();

    return true;
}

bool Transformation::ImageViewerMouseButtonRelease(ImageViewer *obj, QMouseEvent *event)
{
    if(ui->drawButton->isEnabled())
        return false;

    obj->moveEnd();
    return true;
}

bool Transformation::ImageViewerWheel(ImageViewer* obj, QWheelEvent* event)
{
    if(abs(event->angleDelta().ry()) >= 15)
    {
        event->angleDelta().ry() > 0 ? obj->zoomIn() : obj->zoomOut();
        return true;
    } else
        return false;
}


// Slots
void Transformation::on_drawButton_clicked()
{
    if(static_cast<ImageViewer*>(ui->frame->children()[0])->getPointsCount() < 3)
        return;

    ImageViewer* w = getImageViewer();
    w->completePolygon();

    ui->drawButton->setEnabled(false);
    ui->changeColorButton->setEnabled(true);
    ui->eraseButton->setEnabled(true);
    ui->transformationsBox->setEnabled(true);

}

void Transformation::on_eraseButton_clicked()
{
    auto w = getImageViewer();
    w->resetTransform();
    w->clearPoints();
    w->clear();

    ui->drawButton->setEnabled(true);
    ui->eraseButton->setEnabled(false);
    ui->changeColorButton->setEnabled(false);
    ui->transformationsBox->setEnabled(false);
    ui->frame->installEventFilter(this);
}

void Transformation::on_changeColorButton_clicked()
{
    QColor color = QColorDialog::getColor(Qt::black, this);
    ImageViewer* w = getImageViewer();
    w->setColor(color);
    w->clear();
    w->drawPoints();
    w->completePolygon();
}


void Transformation::on_rotate1Button_clicked()
{
    getImageViewer()->rotateClockwise();
}

void Transformation::on_rotate2Button_clicked()
{
    getImageViewer()->rotateCounterClockwise();
}


void Transformation::on_zoomInButton_clicked()
{
    getImageViewer()->zoomIn();
}

void Transformation::on_zoomOutButton_clicked()
{
    getImageViewer()->zoomOut();
}


void Transformation::on_swapButton_clicked()
{
    getImageViewer()->swap();
}


void Transformation::on_shearRightButton_clicked()
{
    getImageViewer()->shearR();
}

void Transformation::on_shearLeftButton_clicked()
{
    getImageViewer()->shearL();
}


void Transformation::on_resetButton_clicked()
{
    getImageViewer()->resetTransform();
}