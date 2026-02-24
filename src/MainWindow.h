#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QDockWidget>
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
    void closeCurrent();
    void firstTexture();
    void lastTexture();
    void openContainingDir();
    void copyFilePath();
    void focusGallery();
    void reloadCurrentTexture();
    void quickExportAll();
    void reopenLastDirectory();
    void togglePropertiesPanel();
    void randomTexture();
    void toggleAutoFit();
    void showDirectoryStats();
    void saveCurrentView();
    
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
    QDockWidget* propertiesDock_;
    
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
    QAction* closeCurrentAction_;
    QAction* firstTextureAction_;
    QAction* lastTextureAction_;
    QAction* openContainingDirAction_;
    QAction* copyFilePathAction_;
    QAction* focusGalleryAction_;
    QAction* reloadCurrentTextureAction_;
    QAction* quickExportAllAction_;
    QAction* reopenLastDirAction_;
    QAction* togglePropertiesPanelAction_;
    QAction* randomTextureAction_;
    QAction* autoFitAction_;
    QAction* directoryStatsAction_;
    QAction* saveCurrentViewAction_;
    QMenu* recentMenu_;
    
    // Status bar widgets
    QLabel* zoomLabel_;
    QLabel* textureCountLabel_;
    QLabel* imageDimensionsLabel_;
    QLabel* formatLabel_;
    QLabel* alphaLabel_;
    
    // Data
    QMap<QString, QString> loadedTextures_; // filename -> full path
    QString currentDirectory_;
    VTFReader* currentVTF_;
    QStringList recentDirectories_;
    bool checkerboardEnabled_;
    bool recursiveScan_;
    int thumbnailSize_;
    QString lastExportPath_;
    bool autoFitOnSelect_;
    QString lastExportFormat_;
    
    void updateRecentDirectoriesMenu();
    void addToRecentDirectories(const QString& path);
    void loadSettings();
    void saveSettings();
    
protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    VMTParser* currentVMT_;
};

#endif // MAINWINDOW_H
