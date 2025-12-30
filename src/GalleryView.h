#ifndef GALLERYVIEW_H
#define GALLERYVIEW_H

#include <QListWidget>
#include <QMap>

class GalleryView : public QListWidget {
    Q_OBJECT
    
public:
    explicit GalleryView(QWidget* parent = nullptr);
    
    void addTexture(const QString& filename, const QImage& thumbnail);
    void clear();
    QString getCurrentFilename() const;
    
signals:
    void textureSelected(const QString& filename);
    void textureDoubleClicked(const QString& filename);
    
private slots:
    void onItemSelectionChanged();
    void onItemDoubleClicked(QListWidgetItem* item);
    
private:
    QMap<QListWidgetItem*, QString> itemToFilename_;
};

#endif // GALLERYVIEW_H
