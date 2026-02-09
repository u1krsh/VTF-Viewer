#include "GalleryView.h"
#include <QFileInfo>

GalleryView::GalleryView(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);
    
    // Search bar
    searchEdit_ = new QLineEdit;
    searchEdit_->setPlaceholderText("🔍 Filter textures...");
    searchEdit_->setClearButtonEnabled(true);
    layout->addWidget(searchEdit_);
    
    // List widget
    listWidget_ = new QListWidget;
    listWidget_->setViewMode(QListView::IconMode);
    listWidget_->setIconSize(QSize(128, 128));
    listWidget_->setResizeMode(QListView::Adjust);
    listWidget_->setMovement(QListView::Static);
    listWidget_->setSpacing(10);
    listWidget_->setUniformItemSizes(true);
    listWidget_->setSelectionMode(QAbstractItemView::SingleSelection);
    listWidget_->setFocusPolicy(Qt::StrongFocus);
    layout->addWidget(listWidget_);
    
    connect(listWidget_, &QListWidget::itemSelectionChanged, 
            this, &GalleryView::onItemSelectionChanged);
    connect(listWidget_, &QListWidget::itemDoubleClicked,
            this, &GalleryView::onItemDoubleClicked);
    connect(searchEdit_, &QLineEdit::textChanged,
            this, &GalleryView::filterItems);
}

void GalleryView::addTexture(const QString& filename, const QImage& thumbnail) {
    QFileInfo fileInfo(filename);
    QString displayName = fileInfo.fileName();
    
    QListWidgetItem* item = new QListWidgetItem(QIcon(QPixmap::fromImage(thumbnail)), displayName);
    item->setToolTip(filename);
    listWidget_->addItem(item);
    
    itemToFilename_[item] = filename;
}

void GalleryView::clear() {
    itemToFilename_.clear();
    listWidget_->clear();
    searchEdit_->clear();
}

QString GalleryView::getCurrentFilename() const {
    QListWidgetItem* item = listWidget_->currentItem();
    if (item && itemToFilename_.contains(item)) {
        return itemToFilename_[item];
    }
    return QString();
}

int GalleryView::getVisibleCount() const {
    int count = 0;
    for (int i = 0; i < listWidget_->count(); ++i) {
        if (!listWidget_->item(i)->isHidden()) {
            count++;
        }
    }
    return count;
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

void GalleryView::filterItems(const QString& text) {
    for (int i = 0; i < listWidget_->count(); ++i) {
        QListWidgetItem* item = listWidget_->item(i);
        bool matches = text.isEmpty() || 
                      item->text().contains(text, Qt::CaseInsensitive);
        item->setHidden(!matches);
    }
    emit visibleCountChanged(getVisibleCount());
}

void GalleryView::setThumbnailSize(int size) {
    listWidget_->setIconSize(QSize(size, size));
}
