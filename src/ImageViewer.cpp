#include "ImageViewer.h"
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QTransform>

ImageViewer::ImageViewer(QWidget* parent) 
    : QWidget(parent), scaleFactor_(1.0), fitToWindowMode_(false), 
      checkerboardEnabled_(false), rotation_(0) {
    
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
    rotation_ = 0;
    updateImage();
    emit zoomChanged(scaleFactor_, fitToWindowMode_);
}

void ImageViewer::clear() {
    currentImage_ = QImage();
    imageLabel_->clear();
    rotation_ = 0;
}

QImage ImageViewer::getRotatedImage() const {
    if (rotation_ == 0) {
        return currentImage_;
    }
    QTransform transform;
    transform.rotate(rotation_);
    return currentImage_.transformed(transform, Qt::SmoothTransformation);
}

void ImageViewer::updateImage() {
    if (currentImage_.isNull()) {
        return;
    }
    
    QImage displayImage = getRotatedImage();
    
    if (fitToWindowMode_) {
        QSize availableSize = scrollArea_->viewport()->size();
        QPixmap pixmap = QPixmap::fromImage(displayImage);
        pixmap = pixmap.scaled(availableSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel_->setPixmap(pixmap);
        imageLabel_->adjustSize();
    } else {
        QPixmap pixmap = QPixmap::fromImage(displayImage);
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
    emit zoomChanged(scaleFactor_, fitToWindowMode_);
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
    emit zoomChanged(scaleFactor_, fitToWindowMode_);
}

void ImageViewer::fitToWindow() {
    fitToWindowMode_ = true;
    updateImage();
    emit zoomChanged(scaleFactor_, fitToWindowMode_);
}

void ImageViewer::rotateClockwise() {
    rotation_ = (rotation_ + 90) % 360;
    updateImage();
}

void ImageViewer::rotateCounterClockwise() {
    rotation_ = (rotation_ - 90 + 360) % 360;
    updateImage();
}

void ImageViewer::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (fitToWindowMode_) {
        updateImage();
    }
}

void ImageViewer::wheelEvent(QWheelEvent* event) {
    if (currentImage_.isNull()) {
        QWidget::wheelEvent(event);
        return;
    }
    
    int delta = event->angleDelta().y();
    if (delta > 0) {
        scaleImage(1.15);
    } else if (delta < 0) {
        scaleImage(1.0 / 1.15);
    }
    
    event->accept();
}

void ImageViewer::setCheckerboardEnabled(bool enabled) {
    checkerboardEnabled_ = enabled;
    if (enabled) {
        scrollArea_->setStyleSheet(
            "QScrollArea { background-image: "
            "url('data:image/svg+xml;utf8,<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"16\" height=\"16\"><rect width=\"8\" height=\"8\" fill=\"%23404040\"/><rect x=\"8\" y=\"8\" width=\"8\" height=\"8\" fill=\"%23404040\"/><rect x=\"8\" width=\"8\" height=\"8\" fill=\"%23606060\"/><rect y=\"8\" width=\"8\" height=\"8\" fill=\"%23606060\"/></svg>'); }"
        );
        scrollArea_->widget()->setStyleSheet("background: transparent;");
    } else {
        scrollArea_->setStyleSheet("");
        scrollArea_->widget()->setStyleSheet("");
    }
}

void ImageViewer::mouseDoubleClickEvent(QMouseEvent* event) {
    if (currentImage_.isNull()) {
        QWidget::mouseDoubleClickEvent(event);
        return;
    }
    
    if (fitToWindowMode_) {
        resetZoom();
    } else {
        fitToWindow();
    }
    event->accept();
}
