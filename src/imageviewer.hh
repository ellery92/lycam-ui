#pragma once

#include <QFrame>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>

class QLabel;
class PixmapItem;
class GraphicsView;

/**
 * @brief ImageViewer Affiche des images dans un widget graphique qui propose
 * en complément un certain nombre d'outils pour exploiter les images.
 *
 * Ne pas utiliser directement, faire appel à ImageViewerNode à la place
 */
class ImageViewer : public QFrame {
    Q_OBJECT

public:
    explicit ImageViewer(QString name, QWidget *parent = nullptr);
    GraphicsView *getView() { return m_view; };

public slots:
    void setImage(const QImage &);
    void zoomFit();
    void zoomOriginal();
    void zoomIn(int level = 1);
    void zoomOut(int level = 1);
    void mouseAt(int x, int y);

signals:
    void imageChanged();

private:
    void setMatrix();
    void makeTools();
    QLayout * makeToolbar();

private:
    int m_zoom_level;
    QString m_name;
    QLabel *m_pixel_value;
    GraphicsView *m_view;
    PixmapItem *m_pixmap;
};

class GraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    explicit GraphicsView(ImageViewer *viewer);

protected:
    void wheelEvent(QWheelEvent *) override;
    void enterEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;

private:
    ImageViewer *m_viewer;
};


class PixmapItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    PixmapItem(QGraphicsItem *parent = nullptr);
    const QImage & image() const { return m_image; }

public slots:
    void setImage(QImage im);

signals:
    void imageChanged(const QImage &);
    void sizeChanged(int w, int h);
    void mouseMoved(int x, int y);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *) override;

private:
    QImage m_image;
};
