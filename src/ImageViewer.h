#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>
#include <QScrollArea>
#include <QLabel>
#include <QImage>
#include <QPoint>

class ImageViewer : public QWidget {
    Q_OBJECT
    
public:
    explicit ImageViewer(QWidget* parent = nullptr);
    
    void setImage(const QImage& image);
    void clear();
    double getScaleFactor() const { return scaleFactor_; }
    bool isFitToWindow() const { return fitToWindowMode_; }
    
public slots:
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void fitToWindow();
    void setCheckerboardEnabled(bool enabled);
    void rotateClockwise();
    void rotateCounterClockwise();
    
signals:
    void zoomChanged(double factor, bool fitMode);
    
private:
    QScrollArea* scrollArea_;
    QLabel* imageLabel_;
    QImage currentImage_;
    double scaleFactor_;
    bool fitToWindowMode_;
    bool checkerboardEnabled_;
    int rotation_;
    bool dragging_;
    QPoint lastMousePos_;
    
    void updateImage();
    void scaleImage(double factor);

public:
    QImage getRotatedImage() const;
    
protected:
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
};

#endif // IMAGEVIEWER_H
