#include "imageviewer.hh"
#include <cmath>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneHoverEvent>
#include <QWheelEvent>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>

ImageViewer::ImageViewer(QString name, QWidget *parent) :
    QFrame(parent), m_zoom_level(0), m_name(name)
{
    auto scene = new QGraphicsScene(this);
    m_view = new GraphicsView(this);
    m_view->setScene(scene);

    // graphic object holding the image buffer
    m_pixmap = new PixmapItem;
    scene->addItem(m_pixmap);
    connect(m_pixmap, SIGNAL(mouseMoved(int,int)), SLOT(mouseAt(int,int)));

    auto layout = makeToolbar();

    auto box = new QVBoxLayout;
    box->addLayout(layout);
    box->setContentsMargins(5,0,5,0);
    box->addWidget(m_view, 1);

    setLayout(box);
}

// toolbar with a few quick actions and display information
QLayout * ImageViewer::makeToolbar()
{
    // name and value at pixel
    auto label = new QLabel(m_name, this);
    label->setStyleSheet(QString("QLabel { font-weight: bold; }"));
    m_pixel_value = new QLabel(this);

    auto fit = new QToolButton(this);
    fit->setToolTip(tr("Fit image to window"));
    fit->setIcon(QIcon(":/icons/zoom-fit-best.png"));
    connect(fit, SIGNAL(clicked()), SLOT(zoomFit()));

    auto orig = new QToolButton(this);
    orig->setToolTip(tr("Resize image to its original size"));
    orig->setIcon(QIcon(":/icons/zoom-original.png"));
    connect(orig, SIGNAL(clicked()), SLOT(zoomOriginal()));

    auto hbox = new QHBoxLayout;
    hbox->setContentsMargins(0,0,0,0);
    hbox->addWidget(label);
    hbox->addStretch(1);
    hbox->addWidget(m_pixel_value);
    hbox->addWidget(fit);
    hbox->addWidget(orig);
    return hbox;
}

void ImageViewer::setImage(const QImage &im) {
    m_pixmap->setImage(im);
    emit imageChanged();
}

void ImageViewer::setMatrix() {
    qreal scale = std::pow(2.0, m_zoom_level / 10.0);

    QMatrix matrix;
    matrix.scale(scale, scale);

    m_view->setMatrix(matrix);
}

void ImageViewer::zoomFit() {
    m_view->fitInView(m_pixmap, Qt::KeepAspectRatio);
    m_zoom_level = 10* std::log2f(m_view->matrix().m11());
}

void ImageViewer::zoomOriginal() {
    m_zoom_level = 0;
    setMatrix();
}

void ImageViewer::zoomIn(int level) {
    m_zoom_level += level;
    setMatrix();
}

void ImageViewer::zoomOut(int level) {
    m_zoom_level -= level;
    setMatrix();
}

void ImageViewer::mouseAt(int x, int y) {
    if (m_pixmap->image().valid(x,y)) {
        QRgb rgb = m_pixmap->image().pixel(x, y);
        auto s = QString("[%1, %2] (%3, %4, %5)").arg(x).arg(y)
                    .arg(qRed(rgb)).arg(qGreen(rgb)).arg(qBlue(rgb));
        m_pixel_value->setText(s);
    }
    else
        m_pixel_value->setText(QString());
}


// derive QGraphics View for better mouse events handling
GraphicsView::GraphicsView(ImageViewer *viewer) :
    QGraphicsView(), m_viewer(viewer)
{
    // no antialiasing or filtering, we want to see the exact image content
    setRenderHint(QPainter::Antialiasing, false);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse); // zoom at cursor position
    setInteractive(true);
    setMouseTracking(true);
}

void GraphicsView::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() == Qt::NoModifier) {
        if (event->delta() > 0)
            m_viewer->zoomIn(3);
        else if (event->delta() < 0)
            m_viewer->zoomOut(3);
        event->accept();
    }
    else
        QGraphicsView::wheelEvent(event);
}

void GraphicsView::enterEvent(QEvent *event) {
    QGraphicsView::enterEvent(event);
    viewport()->setCursor(Qt::CrossCursor);
}

void GraphicsView::mousePressEvent(QMouseEvent *event) {
    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    QGraphicsView::mouseReleaseEvent(event);
    viewport()->setCursor(Qt::CrossCursor);
}


PixmapItem::PixmapItem(QGraphicsItem *parent) :
    QObject(), QGraphicsPixmapItem(parent)
{
    setAcceptHoverEvents(true);
}

void PixmapItem::setImage(QImage im) {
    if (im.isNull()) {
        m_image.fill(Qt::white);
        im=m_image.copy();
    }
    std::swap(m_image, im);

    setPixmap(QPixmap::fromImage(m_image));

    if (m_image.size() != im.size())
        emit sizeChanged(m_image.width(), m_image.height());

    emit imageChanged(m_image);
}

void PixmapItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mousePressEvent(event);
}

void PixmapItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent(event);
}

void PixmapItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    auto pos = event->pos();
    emit mouseMoved(pos.x(), pos.y());
    QGraphicsItem::hoverMoveEvent(event);
}
