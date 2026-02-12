#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QTreeWidget>
#include <QMap>
#include <QStringList>
#include <QSettings>
#include <QLabel>
#include <QSlider>

class GalleryView;
class ImageViewer;
class PropertiesPanel;
class VTFReader;
class VMTParser;

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    
private slots:
    void openDirectory();
    void exportCurrent();
    void exportAll();
    void onTextureSelected(const QString& filename);
    void onTextureDoubleClicked(const QString& filename);
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void fitToWindow();
    void about();
    void openRecentDirectory();
    void clearRecentDirectories();
    void copyImageToClipboard();
    void toggleCheckerboardBackground();
    void updateZoomDisplay(double factor, bool fitMode);
    void toggleRecursiveScan();
    void rotateImageCW();
    void rotateImageCCW();
    void updateTextureCount();
    void nextTexture();
    void previousTexture();
    void toggleFullScreen();
    void focusSearch();
    void reloadDirectory();
    
private:
    void createActions();
    void createMenus();
    void createToolBar();
    void createStatusBar();
    void createDockWidgets();
    
    void loadDirectory(const QString& path);
    void loadTexture(const QString& filename);
    void exportTexture(const QString& filename, const QString& outputPath, 
                      const QString& format, int quality);
    
    // UI Components
    QSplitter* mainSplitter_;
    GalleryView* galleryView_;
    ImageViewer* imageViewer_;
    PropertiesPanel* propertiesPanel_;
    
    // Actions
    QAction* openDirAction_;
    QAction* exportCurrentAction_;
    QAction* exportAllAction_;
    QAction* exitAction_;
    QAction* zoomInAction_;
    QAction* zoomOutAction_;
    QAction* resetZoomAction_;
    QAction* fitToWindowAction_;
    QAction* aboutAction_;
    QAction* copyToClipboardAction_;
    QAction* checkerboardAction_;
    QAction* recursiveScanAction_;
    QAction* rotateCWAction_;
    QAction* rotateCCWAction_;
    QAction* nextTextureAction_;
    QAction* prevTextureAction_;
    QAction* fullScreenAction_;
    QAction* focusSearchAction_;
    QAction* reloadAction_;
    QMenu* recentMenu_;
    
    // Status bar widgets
    QLabel* zoomLabel_;
    QLabel* textureCountLabel_;
    
    // Data
    QMap<QString, QString> loadedTextures_; // filename -> full path
    QString currentDirectory_;
    VTFReader* currentVTF_;
    QStringList recentDirectories_;
    bool checkerboardEnabled_;
    bool recursiveScan_;
    int thumbnailSize_;
    QString lastExportPath_;
    
    void updateRecentDirectoriesMenu();
    void addToRecentDirectories(const QString& path);
    void loadSettings();
    void saveSettings();
    
protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    VMTParser* currentVMT_;
};

#endif // MAINWINDOW_H
