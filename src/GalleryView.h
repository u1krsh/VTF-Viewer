#ifndef GALLERYVIEW_H
#define GALLERYVIEW_H

#include <QListWidget>
#include <QMap>
#include <QLineEdit>
#include <QVBoxLayout>

class GalleryView : public QWidget {
    Q_OBJECT
    
public:
    explicit GalleryView(QWidget* parent = nullptr);
    
    void addTexture(const QString& filename, const QImage& thumbnail);
    void clear();
    QString getCurrentFilename() const;
    int getVisibleCount() const;
    void setThumbnailSize(int size);
    
signals:
    void textureSelected(const QString& filename);
    void textureDoubleClicked(const QString& filename);
    void visibleCountChanged(int count);
    
private slots:
    void onItemSelectionChanged();
    void onItemDoubleClicked(QListWidgetItem* item);
    void filterItems(const QString& text);
    
private:
    QListWidget* listWidget_;
    QLineEdit* searchEdit_;
    QMap<QListWidgetItem*, QString> itemToFilename_;
};

#endif // GALLERYVIEW_H
