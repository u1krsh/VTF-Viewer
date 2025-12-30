#include "ImageViewer.h"
#include <QVBoxLayout>
#include <QResizeEvent>

ImageViewer::ImageViewer(QWidget* parent) 
    : QWidget(parent), scaleFactor_(1.0), fitToWindowMode_(false) {
    
    imageLabel_ = new QLabel;
    imageLabel_->setBackgroundRole(QPalette::Base);
    imageLabel_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel_->setScaledContents(false);
    imageLabel_->setAlignment(Qt::AlignCenter);
    
    scrollArea_ = new QScrollArea;
    scrollArea_->setBackgroundRole(QPalette::Dark);
    scrollArea_->setWidget(imageLabel_);
    scrollArea_->setWidgetResizable(false);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(scrollArea_);
}

void ImageViewer::setImage(const QImage& image) {
    currentImage_ = image;
    scaleFactor_ = 1.0;
    fitToWindowMode_ = false;
    updateImage();
}

void ImageViewer::clear() {
    currentImage_ = QImage();
    imageLabel_->clear();
}

void ImageViewer::updateImage() {
    if (currentImage_.isNull()) {
        return;
    }
    
    if (fitToWindowMode_) {
        QSize availableSize = scrollArea_->viewport()->size();
        QPixmap pixmap = QPixmap::fromImage(currentImage_);
        pixmap = pixmap.scaled(availableSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel_->setPixmap(pixmap);
        imageLabel_->adjustSize();
    } else {
        QPixmap pixmap = QPixmap::fromImage(currentImage_);
        QSize scaledSize = pixmap.size() * scaleFactor_;
        imageLabel_->setPixmap(pixmap.scaled(scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imageLabel_->adjustSize();
    }
}

void ImageViewer::scaleImage(double factor) {
    scaleFactor_ *= factor;
    scaleFactor_ = qBound(0.1, scaleFactor_, 10.0);
    fitToWindowMode_ = false;
    updateImage();
}

void ImageViewer::zoomIn() {
    scaleImage(1.25);
}

void ImageViewer::zoomOut() {
    scaleImage(0.8);
}

void ImageViewer::resetZoom() {
    scaleFactor_ = 1.0;
    fitToWindowMode_ = false;
    updateImage();
}

void ImageViewer::fitToWindow() {
    fitToWindowMode_ = true;
    updateImage();
}

void ImageViewer::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (fitToWindowMode_) {
        updateImage();
    }
}
