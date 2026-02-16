#ifndef GALLERYVIEW_H
#define GALLERYVIEW_H

#include <QListWidget>
#include <QMap>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QKeyEvent>
#include <QLabel>

class GalleryView : public QWidget {
    Q_OBJECT
    
public:
    explicit GalleryView(QWidget* parent = nullptr);
    
    void addTexture(const QString& filename, const QImage& thumbnail);
    void clear();
    QString getCurrentFilename() const;
    int getVisibleCount() const;
    void setThumbnailSize(int size);
    void selectNext();
    void selectPrevious();
    void selectFirst();
    void selectLast();
    void focusSearch();
    void focusGalleryList();
    void toggleViewMode();
    
signals:
    void textureSelected(const QString& filename);
    void textureDoubleClicked(const QString& filename);
    void visibleCountChanged(int count);
    
private slots:
    void onItemSelectionChanged();
    void onItemDoubleClicked(QListWidgetItem* item);
    void filterItems(const QString& text);
    void sortItems(int sortIndex);
    
private:
    QListWidget* listWidget_;
    QLineEdit* searchEdit_;
    QComboBox* sortCombo_;
    QPushButton* viewToggleButton_;
    QMap<QListWidgetItem*, QString> itemToFilename_;
    QMap<QListWidgetItem*, qint64> itemToFileSize_;
    QLabel* placeholderLabel_;
    
protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif // GALLERYVIEW_H
