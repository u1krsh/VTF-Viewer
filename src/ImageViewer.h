#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>
#include <QScrollArea>
#include <QLabel>
#include <QImage>

class ImageViewer : public QWidget {
    Q_OBJECT
    
public:
    explicit ImageViewer(QWidget* parent = nullptr);
    
    void setImage(const QImage& image);
    void clear();
    
public slots:
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void fitToWindow();
    
private:
    QScrollArea* scrollArea_;
    QLabel* imageLabel_;
    QImage currentImage_;
    double scaleFactor_;
    bool fitToWindowMode_;
    
    void updateImage();
    void scaleImage(double factor);
    
protected:
    void resizeEvent(QResizeEvent* event) override;
};

#endif // IMAGEVIEWER_H
