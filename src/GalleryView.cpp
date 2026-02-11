#include "GalleryView.h"
#include <QFileInfo>
#include <QHBoxLayout>

GalleryView::GalleryView(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);
    
    // Search bar and sort combo in a horizontal layout
    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->setSpacing(4);
    
    searchEdit_ = new QLineEdit;
    searchEdit_->setPlaceholderText("🔍 Filter textures...");
    searchEdit_->setClearButtonEnabled(true);
    topLayout->addWidget(searchEdit_, 1);
    
    sortCombo_ = new QComboBox;
    sortCombo_->addItem("Name ↑");
    sortCombo_->addItem("Name ↓");
    sortCombo_->addItem("Size ↑");
    sortCombo_->addItem("Size ↓");
    sortCombo_->setToolTip("Sort gallery items");
    sortCombo_->setMinimumWidth(80);
    topLayout->addWidget(sortCombo_);
    
    layout->addLayout(topLayout);
    
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
    connect(sortCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GalleryView::sortItems);
}

void GalleryView::addTexture(const QString& filename, const QImage& thumbnail) {
    QFileInfo fileInfo(filename);
    QString displayName = fileInfo.fileName();
    
    QListWidgetItem* item = new QListWidgetItem(QIcon(QPixmap::fromImage(thumbnail)), displayName);
    item->setToolTip(filename);
    listWidget_->addItem(item);
    
    itemToFilename_[item] = filename;
    itemToFileSize_[item] = fileInfo.size();
}

void GalleryView::clear() {
    itemToFilename_.clear();
    itemToFileSize_.clear();
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

void GalleryView::selectNext() {
    int current = listWidget_->currentRow();
    for (int i = current + 1; i < listWidget_->count(); ++i) {
        if (!listWidget_->item(i)->isHidden()) {
            listWidget_->setCurrentRow(i);
            return;
        }
    }
}

void GalleryView::selectPrevious() {
    int current = listWidget_->currentRow();
    for (int i = current - 1; i >= 0; --i) {
        if (!listWidget_->item(i)->isHidden()) {
            listWidget_->setCurrentRow(i);
            return;
        }
    }
}

void GalleryView::focusSearch() {
    searchEdit_->setFocus();
    searchEdit_->selectAll();
}

void GalleryView::sortItems(int sortIndex) {
    listWidget_->setSortingEnabled(false);
    
    // Collect all items with their data
    struct ItemData {
        QString name;
        QIcon icon;
        QString tooltip;
        QString filename;
        qint64 fileSize;
    };
    
    QList<ItemData> items;
    for (int i = 0; i < listWidget_->count(); ++i) {
        QListWidgetItem* item = listWidget_->item(i);
        ItemData data;
        data.name = item->text();
        data.icon = item->icon();
        data.tooltip = item->toolTip();
        data.filename = itemToFilename_.value(item);
        data.fileSize = itemToFileSize_.value(item, 0);
        items.append(data);
    }
    
    // Sort based on selected option
    switch (sortIndex) {
        case 0: // Name ascending
            std::sort(items.begin(), items.end(), [](const ItemData& a, const ItemData& b) {
                return a.name.toLower() < b.name.toLower();
            });
            break;
        case 1: // Name descending
            std::sort(items.begin(), items.end(), [](const ItemData& a, const ItemData& b) {
                return a.name.toLower() > b.name.toLower();
            });
            break;
        case 2: // Size ascending
            std::sort(items.begin(), items.end(), [](const ItemData& a, const ItemData& b) {
                return a.fileSize < b.fileSize;
            });
            break;
        case 3: // Size descending
            std::sort(items.begin(), items.end(), [](const ItemData& a, const ItemData& b) {
                return a.fileSize > b.fileSize;
            });
            break;
    }
    
    // Rebuild the list
    itemToFilename_.clear();
    itemToFileSize_.clear();
    listWidget_->clear();
    
    for (const ItemData& data : items) {
        QListWidgetItem* item = new QListWidgetItem(data.icon, data.name);
        item->setToolTip(data.tooltip);
        listWidget_->addItem(item);
        itemToFilename_[item] = data.filename;
        itemToFileSize_[item] = data.fileSize;
    }
    
    // Re-apply filter
    filterItems(searchEdit_->text());
}
