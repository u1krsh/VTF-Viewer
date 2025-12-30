#include "GalleryView.h"
#include <QFileInfo>

GalleryView::GalleryView(QWidget* parent) : QListWidget(parent) {
    setViewMode(QListView::IconMode);
    setIconSize(QSize(128, 128));
    setResizeMode(QListView::Adjust);
    setMovement(QListView::Static);
    setSpacing(10);
    setUniformItemSizes(true);
    
    connect(this, &QListWidget::itemSelectionChanged, 
            this, &GalleryView::onItemSelectionChanged);
    connect(this, &QListWidget::itemDoubleClicked,
            this, &GalleryView::onItemDoubleClicked);
}

void GalleryView::addTexture(const QString& filename, const QImage& thumbnail) {
    QFileInfo fileInfo(filename);
    QString displayName = fileInfo.fileName();
    
    QListWidgetItem* item = new QListWidgetItem(QIcon(QPixmap::fromImage(thumbnail)), displayName);
    item->setToolTip(filename);
    addItem(item);
    
    itemToFilename_[item] = filename;
}

void GalleryView::clear() {
    itemToFilename_.clear();
    QListWidget::clear();
}

QString GalleryView::getCurrentFilename() const {
    QListWidgetItem* item = currentItem();
    if (item && itemToFilename_.contains(item)) {
        return itemToFilename_[item];
    }
    return QString();
}

void GalleryView::onItemSelectionChanged() {
    QString filename = getCurrentFilename();
    if (!filename.isEmpty()) {
        emit textureSelected(filename);
    }
}

void GalleryView::onItemDoubleClicked(QListWidgetItem* item) {
    if (item && itemToFilename_.contains(item)) {
        emit textureDoubleClicked(itemToFilename_[item]);
    }
}
