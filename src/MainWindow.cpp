#include "MainWindow.h"
#include "GalleryView.h"
#include "ImageViewer.h"
#include "PropertiesPanel.h"
#include "ExportDialog.h"
#include "VTFReader.h"
#include "VMTParser.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDir>
#include <QFileInfo>
#include <QDirIterator>
#include <QApplication>
#include <QScreen>
#include <QStyle>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QClipboard>
#include <QSettings>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QSlider>
#include <QDesktopServices>
#include <QUrl>
#include <QElapsedTimer>

MainWindow::MainWindow(QWidget* parent) 
    : QMainWindow(parent), currentVTF_(nullptr), currentVMT_(nullptr), 
      checkerboardEnabled_(false), recursiveScan_(false), thumbnailSize_(128),
      lastExportPath_(QString()) {
    
    // Enable drag and drop
    setAcceptDrops(true);
    
    setWindowTitle("VTF-Viewer - Source Engine Texture Viewer");
    resize(1400, 900);
    
    // Center window on screen
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            screen()->availableGeometry()
        )
    );
    
    // Create central widget
    mainSplitter_ = new QSplitter(Qt::Horizontal);
    
    galleryView_ = new GalleryView;
    imageViewer_ = new ImageViewer;
    
    mainSplitter_->addWidget(galleryView_);
    mainSplitter_->addWidget(imageViewer_);
    mainSplitter_->setStretchFactor(0, 1);
    mainSplitter_->setStretchFactor(1, 2);
    
    setCentralWidget(mainSplitter_);
    
    // Create UI elements
    createActions();
    createMenus();
    createToolBar();
    createStatusBar();
    createDockWidgets();
    
    // Connect signals
    connect(galleryView_, &GalleryView::textureSelected,
            this, &MainWindow::onTextureSelected);
    connect(galleryView_, &GalleryView::textureDoubleClicked,
            this, &MainWindow::onTextureDoubleClicked);
    
    // Connect zoom display
    connect(imageViewer_, &ImageViewer::zoomChanged,
            this, &MainWindow::updateZoomDisplay);
    
    // Connect texture count updates from gallery filter
    connect(galleryView_, &GalleryView::visibleCountChanged,
            this, [this](int) { updateTextureCount(); });
}

MainWindow::~MainWindow() {
    delete currentVTF_;
    delete currentVMT_;
}

void MainWindow::createActions() {
    openDirAction_ = new QAction("&Open Directory...", this);
    openDirAction_->setShortcut(QKeySequence::Open);
    openDirAction_->setStatusTip("Open a directory containing VTF/VMT files");
    connect(openDirAction_, &QAction::triggered, this, &MainWindow::openDirectory);
    
    exportCurrentAction_ = new QAction("&Export Current...", this);
    exportCurrentAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
    exportCurrentAction_->setStatusTip("Export the currently selected texture");
    connect(exportCurrentAction_, &QAction::triggered, this, &MainWindow::exportCurrent);
    
    exportAllAction_ = new QAction("Export &All...", this);
    exportAllAction_->setStatusTip("Export all loaded textures");
    connect(exportAllAction_, &QAction::triggered, this, &MainWindow::exportAll);
    
    exitAction_ = new QAction("E&xit", this);
    exitAction_->setShortcut(QKeySequence::Quit);
    exitAction_->setStatusTip("Exit the application");
    connect(exitAction_, &QAction::triggered, this, &QWidget::close);
    
    zoomInAction_ = new QAction("Zoom &In", this);
    zoomInAction_->setShortcut(QKeySequence::ZoomIn);
    connect(zoomInAction_, &QAction::triggered, this, &MainWindow::zoomIn);
    
    zoomOutAction_ = new QAction("Zoom &Out", this);
    zoomOutAction_->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOutAction_, &QAction::triggered, this, &MainWindow::zoomOut);
    
    resetZoomAction_ = new QAction("&Reset Zoom", this);
    resetZoomAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    connect(resetZoomAction_, &QAction::triggered, this, &MainWindow::resetZoom);
    
    fitToWindowAction_ = new QAction("&Fit to Window", this);
    fitToWindowAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F));
    connect(fitToWindowAction_, &QAction::triggered, this, &MainWindow::fitToWindow);
    
    aboutAction_ = new QAction("&About", this);
    aboutAction_->setStatusTip("About VTF-Viewer");
    connect(aboutAction_, &QAction::triggered, this, &MainWindow::about);
    
    // New QoL actions
    copyToClipboardAction_ = new QAction("&Copy to Clipboard", this);
    copyToClipboardAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));
    copyToClipboardAction_->setStatusTip("Copy current image to clipboard");
    connect(copyToClipboardAction_, &QAction::triggered, this, &MainWindow::copyImageToClipboard);
    
    checkerboardAction_ = new QAction("Toggle &Checkerboard", this);
    checkerboardAction_->setShortcut(QKeySequence(Qt::Key_B));
    checkerboardAction_->setStatusTip("Toggle checkerboard transparency background");
    checkerboardAction_->setCheckable(true);
    connect(checkerboardAction_, &QAction::triggered, this, &MainWindow::toggleCheckerboardBackground);
    
    recursiveScanAction_ = new QAction("&Recursive Scan", this);
    recursiveScanAction_->setStatusTip("Scan subdirectories when opening a directory");
    recursiveScanAction_->setCheckable(true);
    connect(recursiveScanAction_, &QAction::triggered, this, &MainWindow::toggleRecursiveScan);
    
    rotateCWAction_ = new QAction("Rotate &Clockwise", this);
    rotateCWAction_->setShortcut(QKeySequence(Qt::Key_R));
    rotateCWAction_->setStatusTip("Rotate image 90° clockwise");
    connect(rotateCWAction_, &QAction::triggered, this, &MainWindow::rotateImageCW);
    
    rotateCCWAction_ = new QAction("Rotate C&ounter-Clockwise", this);
    rotateCCWAction_->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_R));
    rotateCCWAction_->setStatusTip("Rotate image 90° counter-clockwise");
    connect(rotateCCWAction_, &QAction::triggered, this, &MainWindow::rotateImageCCW);
    
    nextTextureAction_ = new QAction("&Next Texture", this);
    nextTextureAction_->setShortcut(QKeySequence(Qt::Key_PageDown));
    nextTextureAction_->setStatusTip("View next texture in gallery");
    connect(nextTextureAction_, &QAction::triggered, this, &MainWindow::nextTexture);
    
    prevTextureAction_ = new QAction("&Previous Texture", this);
    prevTextureAction_->setShortcut(QKeySequence(Qt::Key_PageUp));
    prevTextureAction_->setStatusTip("View previous texture in gallery");
    connect(prevTextureAction_, &QAction::triggered, this, &MainWindow::previousTexture);
    
    fullScreenAction_ = new QAction("&Full Screen", this);
    fullScreenAction_->setShortcuts({QKeySequence(Qt::Key_F11), QKeySequence(Qt::ALT | Qt::Key_Return)});
    fullScreenAction_->setStatusTip("Toggle full screen mode (F11 or Alt+Enter)");
    fullScreenAction_->setCheckable(true);
    connect(fullScreenAction_, &QAction::triggered, this, &MainWindow::toggleFullScreen);
    
    focusSearchAction_ = new QAction("&Focus Search", this);
    focusSearchAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    focusSearchAction_->setStatusTip("Focus the gallery search bar");
    connect(focusSearchAction_, &QAction::triggered, this, &MainWindow::focusSearch);
    
    reloadAction_ = new QAction("&Reload Directory", this);
    reloadAction_->setShortcut(QKeySequence(Qt::Key_F5));
    reloadAction_->setStatusTip("Reload the current directory");
    connect(reloadAction_, &QAction::triggered, this, &MainWindow::reloadDirectory);
    
    closeCurrentAction_ = new QAction("&Close Current", this);
    closeCurrentAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_W));
    closeCurrentAction_->setStatusTip("Close the currently viewed texture");
    connect(closeCurrentAction_, &QAction::triggered, this, &MainWindow::closeCurrent);
    
    firstTextureAction_ = new QAction("&First Texture", this);
    firstTextureAction_->setShortcut(QKeySequence(Qt::Key_Home));
    firstTextureAction_->setStatusTip("Jump to first texture in gallery");
    connect(firstTextureAction_, &QAction::triggered, this, &MainWindow::firstTexture);
    
    lastTextureAction_ = new QAction("&Last Texture", this);
    lastTextureAction_->setShortcut(QKeySequence(Qt::Key_End));
    lastTextureAction_->setStatusTip("Jump to last texture in gallery");
    connect(lastTextureAction_, &QAction::triggered, this, &MainWindow::lastTexture);
    
    openContainingDirAction_ = new QAction("Open Containing &Directory", this);
    openContainingDirAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));
    openContainingDirAction_->setStatusTip("Open the containing directory in file manager");
    connect(openContainingDirAction_, &QAction::triggered, this, &MainWindow::openContainingDir);
    
    copyFilePathAction_ = new QAction("Copy File &Path", this);
    copyFilePathAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C));
    copyFilePathAction_->setStatusTip("Copy the current file path to clipboard");
    connect(copyFilePathAction_, &QAction::triggered, this, &MainWindow::copyFilePath);
    
    focusGalleryAction_ = new QAction("Focus &Gallery", this);
    focusGalleryAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));
    focusGalleryAction_->setStatusTip("Focus the gallery list widget");
    connect(focusGalleryAction_, &QAction::triggered, this, &MainWindow::focusGallery);
    
    reloadCurrentTextureAction_ = new QAction("Reload &Texture", this);
    reloadCurrentTextureAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    reloadCurrentTextureAction_->setStatusTip("Reload the currently viewed texture from disk");
    connect(reloadCurrentTextureAction_, &QAction::triggered, this, &MainWindow::reloadCurrentTexture);
    
    quickExportAllAction_ = new QAction("Quick Export A&ll", this);
    quickExportAllAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_E));
    quickExportAllAction_->setStatusTip("Export all textures using last-used settings");
    connect(quickExportAllAction_, &QAction::triggered, this, &MainWindow::quickExportAll);
    
    reopenLastDirAction_ = new QAction("Reopen &Last Directory", this);
    reopenLastDirAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O));
    reopenLastDirAction_->setStatusTip("Reopen the most recently used directory");
    connect(reopenLastDirAction_, &QAction::triggered, this, &MainWindow::reopenLastDirectory);
    
    togglePropertiesPanelAction_ = new QAction("Toggle &Properties Panel", this);
    togglePropertiesPanelAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
    togglePropertiesPanelAction_->setStatusTip("Show or hide the properties panel");
    togglePropertiesPanelAction_->setCheckable(true);
    togglePropertiesPanelAction_->setChecked(true);
    connect(togglePropertiesPanelAction_, &QAction::triggered, this, &MainWindow::togglePropertiesPanel);
    
    randomTextureAction_ = new QAction("&Random Texture", this);
    randomTextureAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_R));
    randomTextureAction_->setStatusTip("Select a random texture from the gallery");
    connect(randomTextureAction_, &QAction::triggered, this, &MainWindow::randomTexture);
    
    // Load settings
    loadSettings();
}

void MainWindow::createMenus() {
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(openDirAction_);
    fileMenu->addAction(reopenLastDirAction_);
    fileMenu->addAction(reloadAction_);
    fileMenu->addAction(reloadCurrentTextureAction_);
    
    // Recent directories submenu
    recentMenu_ = fileMenu->addMenu("Recent &Directories");
    updateRecentDirectoriesMenu();
    
    fileMenu->addSeparator();
    fileMenu->addAction(exportCurrentAction_);
    fileMenu->addAction(exportAllAction_);
    fileMenu->addAction(quickExportAllAction_);
    fileMenu->addSeparator();
    fileMenu->addAction(closeCurrentAction_);
    fileMenu->addAction(exitAction_);
    
    QMenu* editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction(copyToClipboardAction_);
    editMenu->addAction(copyFilePathAction_);
    editMenu->addSeparator();
    editMenu->addAction(openContainingDirAction_);
    editMenu->addAction(focusSearchAction_);
    editMenu->addAction(focusGalleryAction_);
    editMenu->addSeparator();
    editMenu->addAction(randomTextureAction_);
    
    QMenu* viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction(zoomInAction_);
    viewMenu->addAction(zoomOutAction_);
    viewMenu->addAction(resetZoomAction_);
    viewMenu->addAction(fitToWindowAction_);
    viewMenu->addSeparator();
    viewMenu->addAction(checkerboardAction_);
    viewMenu->addAction(recursiveScanAction_);
    viewMenu->addSeparator();
    viewMenu->addAction(rotateCWAction_);
    viewMenu->addAction(rotateCCWAction_);
    viewMenu->addSeparator();
    viewMenu->addAction(nextTextureAction_);
    viewMenu->addAction(prevTextureAction_);
    viewMenu->addAction(firstTextureAction_);
    viewMenu->addAction(lastTextureAction_);
    viewMenu->addSeparator();
    viewMenu->addAction(fullScreenAction_);
    viewMenu->addSeparator();
    viewMenu->addAction(togglePropertiesPanelAction_);
    
    QMenu* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(aboutAction_);
}

void MainWindow::createToolBar() {
    QToolBar* toolBar = addToolBar("Main Toolbar");
    toolBar->addAction(openDirAction_);
    toolBar->addSeparator();
    toolBar->addAction(exportCurrentAction_);
    toolBar->addSeparator();
    toolBar->addAction(zoomInAction_);
    toolBar->addAction(zoomOutAction_);
    toolBar->addAction(resetZoomAction_);
    toolBar->addAction(fitToWindowAction_);
    
    // Thumbnail size slider
    toolBar->addSeparator();
    QLabel* thumbLabel = new QLabel(" Thumbnails: ");
    toolBar->addWidget(thumbLabel);
    QSlider* thumbSlider = new QSlider(Qt::Horizontal);
    thumbSlider->setRange(64, 256);
    thumbSlider->setValue(thumbnailSize_);
    thumbSlider->setMaximumWidth(120);
    thumbSlider->setToolTip("Adjust thumbnail size");
    connect(thumbSlider, &QSlider::valueChanged, this, [this](int value) {
        thumbnailSize_ = value;
        galleryView_->setThumbnailSize(value);
        saveSettings();
    });
    toolBar->addWidget(thumbSlider);
}

void MainWindow::createStatusBar() {
    formatLabel_ = new QLabel("");
    formatLabel_->setMinimumWidth(80);
    formatLabel_->setAlignment(Qt::AlignCenter);
    formatLabel_->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    statusBar()->addPermanentWidget(formatLabel_);
    
    imageDimensionsLabel_ = new QLabel("");
    imageDimensionsLabel_->setMinimumWidth(100);
    imageDimensionsLabel_->setAlignment(Qt::AlignCenter);
    imageDimensionsLabel_->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    statusBar()->addPermanentWidget(imageDimensionsLabel_);
    
    textureCountLabel_ = new QLabel("0 textures");
    textureCountLabel_->setMinimumWidth(90);
    textureCountLabel_->setAlignment(Qt::AlignCenter);
    textureCountLabel_->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    statusBar()->addPermanentWidget(textureCountLabel_);
    
    zoomLabel_ = new QLabel("100%");
    zoomLabel_->setMinimumWidth(60);
    zoomLabel_->setAlignment(Qt::AlignCenter);
    zoomLabel_->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    statusBar()->addPermanentWidget(zoomLabel_);
    statusBar()->showMessage("Ready");
}

void MainWindow::createDockWidgets() {
    propertiesPanel_ = new PropertiesPanel;
    
    propertiesDock_ = new QDockWidget("Properties", this);
    propertiesDock_->setWidget(propertiesPanel_);
    propertiesDock_->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, propertiesDock_);
}

void MainWindow::openDirectory() {
    QString dir = QFileDialog::getExistingDirectory(this, "Open Directory",
                                                     currentDirectory_);
    if (!dir.isEmpty()) {
        loadDirectory(dir);
    }
}

void MainWindow::loadDirectory(const QString& path) {
    currentDirectory_ = path;
    galleryView_->clear();
    loadedTextures_.clear();
    
    QDir dir(path);
    QStringList filters;
    filters << "*.vtf" << "*.vmt";
    
    QFileInfoList files;
    if (recursiveScan_) {
        QDirIterator it(path, filters, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            files.append(it.fileInfo());
        }
    } else {
        files = dir.entryInfoList(filters, QDir::Files);
    }
    
    if (files.isEmpty()) {
        QMessageBox::information(this, "No Files Found",
                               "No VTF or VMT files found in the selected directory.");
        return;
    }
    
    QProgressDialog progress("Loading textures...", "Cancel", 0, files.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    
    QElapsedTimer loadTimer;
    loadTimer.start();
    
    int count = 0;
    for (const QFileInfo& fileInfo : files) {
        if (progress.wasCanceled()) {
            break;
        }
        
        QString filename = fileInfo.absoluteFilePath();
        
        if (fileInfo.suffix().toLower() == "vtf") {
            VTFReader reader;
            if (reader.loadFile(filename)) {
                QImage thumbnail = reader.getThumbnail(128);
                if (!thumbnail.isNull()) {
                    galleryView_->addTexture(filename, thumbnail);
                    loadedTextures_[fileInfo.fileName()] = filename;
                    count++;
                }
            }
        }
        
        progress.setValue(progress.value() + 1);
        QApplication::processEvents();
    }
    
    double elapsed = loadTimer.elapsed() / 1000.0;
    statusBar()->showMessage(QString("✅ Loaded %1 textures from %2 in %3s")
        .arg(count).arg(QFileInfo(path).fileName()).arg(elapsed, 0, 'f', 1));
    updateTextureCount();
    
    // Update title bar with directory info
    setWindowTitle(QString("%1 (%2 textures) — VTF-Viewer").arg(QFileInfo(path).fileName()).arg(count));
    
    // Auto-select first texture for immediate preview
    if (count > 0) {
        galleryView_->selectFirst();
    }
    
    // Add to recent directories
    addToRecentDirectories(path);
}

void MainWindow::onTextureSelected(const QString& filename) {
    loadTexture(filename);
}

void MainWindow::onTextureDoubleClicked(const QString& filename) {
    loadTexture(filename);
    fitToWindow();
}

void MainWindow::loadTexture(const QString& filename) {
    QFileInfo fileInfo(filename);
    
    // Clean up previous readers
    delete currentVTF_;
    delete currentVMT_;
    currentVTF_ = nullptr;
    currentVMT_ = nullptr;
    
    if (fileInfo.suffix().toLower() == "vtf") {
        currentVTF_ = new VTFReader;
        if (currentVTF_->loadFile(filename)) {
            QImage image = currentVTF_->getImage();
            imageViewer_->setImage(image);
            
            propertiesPanel_->setVTFProperties(
                filename,
                currentVTF_->getWidth(),
                currentVTF_->getHeight(),
                currentVTF_->getFormat(),
                currentVTF_->getFrameCount(),
                currentVTF_->getMipmapCount(),
                currentVTF_->getFlags()
            );
            
            statusBar()->showMessage(QString("Loaded: %1 (%2x%3, %4)")
                .arg(fileInfo.fileName())
                .arg(currentVTF_->getWidth())
                .arg(currentVTF_->getHeight())
                .arg(currentVTF_->getFormat()));
            
            // Update title bar with current file
            setWindowTitle(QString("%1 — VTF-Viewer").arg(fileInfo.fileName()));
            
            // Update dimensions in status bar
            imageDimensionsLabel_->setText(QString("%1×%2").arg(currentVTF_->getWidth()).arg(currentVTF_->getHeight()));
            
            // Update format indicator
            formatLabel_->setText(currentVTF_->getFormat());
        }
    } else if (fileInfo.suffix().toLower() == "vmt") {
        currentVMT_ = new VMTParser;
        if (currentVMT_->loadFile(filename)) {
            propertiesPanel_->setVMTProperties(
                currentVMT_->getShader(),
                currentVMT_->getAllParameters()
            );
            
            // Try to load the base texture
            QString baseTexture = currentVMT_->getBaseTexture();
            if (!baseTexture.isEmpty()) {
                QString vtfPath = fileInfo.absolutePath() + "/" + baseTexture + ".vtf";
                QFileInfo vtfInfo(vtfPath);
                
                if (vtfInfo.exists()) {
                    currentVTF_ = new VTFReader;
                    if (currentVTF_->loadFile(vtfPath)) {
                        QImage image = currentVTF_->getImage();
                        imageViewer_->setImage(image);
                    }
                }
            }
            
            statusBar()->showMessage(QString("Loaded VMT: %1 (Shader: %2)")
                .arg(fileInfo.fileName())
                .arg(currentVMT_->getShader()));
            
            // Update title bar with current file
            setWindowTitle(QString("%1 — VTF-Viewer").arg(fileInfo.fileName()));
            imageDimensionsLabel_->setText("");
            formatLabel_->setText("");
        }
    }
}

void MainWindow::exportCurrent() {
    QString currentFile = galleryView_->getCurrentFilename();
    if (currentFile.isEmpty()) {
        QMessageBox::information(this, "No Selection",
                               "Please select a texture to export.");
        return;
    }
    
    ExportDialog dialog(this);
    if (!lastExportPath_.isEmpty()) {
        dialog.setOutputPath(lastExportPath_);
    }
    if (dialog.exec() == QDialog::Accepted) {
        QString outputPath = dialog.getOutputPath();
        QString format = dialog.getFormat();
        int quality = dialog.getQuality();
        
        if (outputPath.isEmpty()) {
            QMessageBox::warning(this, "Invalid Path",
                               "Please select an output directory.");
            return;
        }
        
        lastExportPath_ = outputPath;
        saveSettings();
        exportTexture(currentFile, outputPath, format, quality);
        QMessageBox::information(this, "Export Complete",
                               "Texture exported successfully.");
    }
}

void MainWindow::exportAll() {
    if (loadedTextures_.isEmpty()) {
        QMessageBox::information(this, "No Textures",
                               "No textures loaded to export.");
        return;
    }
    
    ExportDialog dialog(this);
    if (!lastExportPath_.isEmpty()) {
        dialog.setOutputPath(lastExportPath_);
    }
    
    // Confirmation dialog before batch export
    QMessageBox::StandardButton confirm = QMessageBox::question(this, 
        "Batch Export",
        QString("Export all %1 loaded textures?").arg(loadedTextures_.size()),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (confirm != QMessageBox::Yes) {
        return;
    }
    
    if (dialog.exec() == QDialog::Accepted) {
        QString outputPath = dialog.getOutputPath();
        QString format = dialog.getFormat();
        int quality = dialog.getQuality();
        
        if (outputPath.isEmpty()) {
            QMessageBox::warning(this, "Invalid Path",
                               "Please select an output directory.");
            return;
        }
        
        lastExportPath_ = outputPath;
        saveSettings();
        
        QProgressDialog progress("Exporting textures...", "Cancel", 
                                0, loadedTextures_.size(), this);
        progress.setWindowModality(Qt::WindowModal);
        
        int count = 0;
        for (auto it = loadedTextures_.begin(); it != loadedTextures_.end(); ++it) {
            if (progress.wasCanceled()) {
                break;
            }
            
            exportTexture(it.value(), outputPath, format, quality);
            count++;
            progress.setValue(count);
            QApplication::processEvents();
        }
        
        QMessageBox::information(this, "Export Complete",
                               QString("Exported %1 textures.").arg(count));
    }
}

void MainWindow::exportTexture(const QString& filename, const QString& outputPath,
                               const QString& format, int quality) {
    VTFReader reader;
    if (!reader.loadFile(filename)) {
        return;
    }
    
    QImage image = reader.getImage();
    if (image.isNull()) {
        return;
    }
    
    QFileInfo fileInfo(filename);
    QString outputFile = outputPath + "/" + fileInfo.baseName() + "." + format;
    
    if (format == "jpg") {
        image.save(outputFile, "JPEG", quality);
    } else if (format == "png") {
        image.save(outputFile, "PNG");
    } else if (format == "bmp") {
        image.save(outputFile, "BMP");
    } else if (format == "tga") {
        image.save(outputFile, "TGA");
    } else if (format == "tiff") {
        image.save(outputFile, "TIFF");
    }
}

void MainWindow::zoomIn() {
    imageViewer_->zoomIn();
}

void MainWindow::zoomOut() {
    imageViewer_->zoomOut();
}

void MainWindow::resetZoom() {
    imageViewer_->resetZoom();
}

void MainWindow::fitToWindow() {
    imageViewer_->fitToWindow();
}

void MainWindow::about() {
    QMessageBox::about(this, "About VTF-Viewer",
        "<div style='text-align: center;'>"
        "<h3>VTF-Viewer</h3>"
        "<p>Version 1.3.0</p>"
        "<p>A tool for viewing and exporting Source Engine textures.</p>"
        "<hr>"
        "<p><b>Keyboard Shortcuts:</b></p>"
        "<table align='center' style='margin: 0 auto;'>"
        "<tr><td>Ctrl+O</td><td>&nbsp;Open Directory</td></tr>"
        "<tr><td>Ctrl+E</td><td>&nbsp;Export Current</td></tr>"
        "<tr><td>Ctrl+Shift+E</td><td>&nbsp;Quick Export All</td></tr>"
        "<tr><td>Ctrl+C</td><td>&nbsp;Copy to Clipboard</td></tr>"
        "<tr><td>Ctrl+Shift+C</td><td>&nbsp;Copy File Path</td></tr>"
        "<tr><td>Ctrl+L</td><td>&nbsp;Focus Search Bar</td></tr>"
        "<tr><td>Ctrl+G</td><td>&nbsp;Focus Gallery</td></tr>"
        "<tr><td>Ctrl+W</td><td>&nbsp;Close Current Texture</td></tr>"
        "<tr><td>Ctrl+R</td><td>&nbsp;Reload Current Texture</td></tr>"
        "<tr><td>Ctrl+D</td><td>&nbsp;Open Containing Directory</td></tr>"
        "<tr><td>F5</td><td>&nbsp;Reload Directory</td></tr>"
        "<tr><td>Ctrl++</td><td>&nbsp;Zoom In</td></tr>"
        "<tr><td>Ctrl+-</td><td>&nbsp;Zoom Out</td></tr>"
        "<tr><td>Ctrl+0</td><td>&nbsp;Reset Zoom</td></tr>"
        "<tr><td>Ctrl+F</td><td>&nbsp;Fit to Window</td></tr>"
        "<tr><td>Scroll</td><td>&nbsp;Mouse Wheel Zoom</td></tr>"
        "<tr><td>R</td><td>&nbsp;Rotate Clockwise</td></tr>"
        "<tr><td>Shift+R</td><td>&nbsp;Rotate Counter-Clockwise</td></tr>"
        "<tr><td>B</td><td>&nbsp;Toggle Checkerboard</td></tr>"
        "<tr><td>PgUp</td><td>&nbsp;Previous Texture</td></tr>"
        "<tr><td>PgDown</td><td>&nbsp;Next Texture</td></tr>"
        "<tr><td>Home</td><td>&nbsp;First Texture</td></tr>"
        "<tr><td>End</td><td>&nbsp;Last Texture</td></tr>"
        "<tr><td>F11/Alt+Enter</td><td>&nbsp;Full Screen</td></tr>"
        "<tr><td>Escape</td><td>&nbsp;Exit Full Screen / Clear</td></tr>"
        "<tr><td>Double-click</td><td>&nbsp;Toggle Fit to Window</td></tr>"
        "</table>"
        "<hr>"
        "<p>Built with Qt6. Released under GPL v3.0.</p>"
        "</div>");
}

// ============================================================================
// Drag and Drop Support
// ============================================================================

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        for (const QUrl& url : event->mimeData()->urls()) {
            QString path = url.toLocalFile();
            QFileInfo fileInfo(path);
            if (fileInfo.isDir() || fileInfo.suffix().toLower() == "vtf" || 
                fileInfo.suffix().toLower() == "vmt") {
                event->acceptProposedAction();
                return;
            }
        }
    }
}

void MainWindow::dropEvent(QDropEvent* event) {
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        for (const QUrl& url : mimeData->urls()) {
            QString path = url.toLocalFile();
            QFileInfo fileInfo(path);
            
            if (fileInfo.isDir()) {
                loadDirectory(path);
                break;
            } else if (fileInfo.suffix().toLower() == "vtf" || 
                       fileInfo.suffix().toLower() == "vmt") {
                loadDirectory(fileInfo.absolutePath());
                break;
            }
        }
    }
}

// ============================================================================
// Copy to Clipboard
// ============================================================================

void MainWindow::copyImageToClipboard() {
    if (currentVTF_ == nullptr) {
        statusBar()->showMessage("⚠️ No image to copy", 3000);
        return;
    }
    
    QImage image = currentVTF_->getImage();
    if (!image.isNull()) {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setImage(image);
        statusBar()->showMessage("📋 Image copied to clipboard!", 3000);
    }
}

// ============================================================================
// Checkerboard Background Toggle
// ============================================================================

void MainWindow::toggleCheckerboardBackground() {
    checkerboardEnabled_ = !checkerboardEnabled_;
    checkerboardAction_->setChecked(checkerboardEnabled_);
    imageViewer_->setCheckerboardEnabled(checkerboardEnabled_);
    statusBar()->showMessage(checkerboardEnabled_ ? 
        "🎨 Checkerboard background enabled" : 
        "🎨 Checkerboard background disabled", 2000);
}

// ============================================================================
// Recent Directories Management
// ============================================================================

void MainWindow::updateRecentDirectoriesMenu() {
    recentMenu_->clear();
    
    for (const QString& dir : recentDirectories_) {
        QAction* action = recentMenu_->addAction(dir);
        action->setData(dir);
        connect(action, &QAction::triggered, this, &MainWindow::openRecentDirectory);
    }
    
    if (!recentDirectories_.isEmpty()) {
        recentMenu_->addSeparator();
        QAction* clearAction = recentMenu_->addAction("Clear Recent");
        connect(clearAction, &QAction::triggered, this, &MainWindow::clearRecentDirectories);
    }
    
    recentMenu_->setEnabled(!recentDirectories_.isEmpty());
}

void MainWindow::openRecentDirectory() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        QString dir = action->data().toString();
        if (QDir(dir).exists()) {
            loadDirectory(dir);
        } else {
            QMessageBox::warning(this, "Directory Not Found",
                QString("The directory no longer exists:\n%1").arg(dir));
            recentDirectories_.removeAll(dir);
            updateRecentDirectoriesMenu();
            saveSettings();
        }
    }
}

void MainWindow::clearRecentDirectories() {
    recentDirectories_.clear();
    updateRecentDirectoriesMenu();
    saveSettings();
    statusBar()->showMessage("🗑️ Recent directories cleared", 2000);
}

void MainWindow::addToRecentDirectories(const QString& path) {
    recentDirectories_.removeAll(path);
    recentDirectories_.prepend(path);
    
    // Keep only the last 10 entries
    while (recentDirectories_.size() > 10) {
        recentDirectories_.removeLast();
    }
    
    updateRecentDirectoriesMenu();
    saveSettings();
}

// ============================================================================
// Settings Persistence
// ============================================================================

void MainWindow::loadSettings() {
    QSettings settings;
    recentDirectories_ = settings.value("recentDirectories").toStringList();
    checkerboardEnabled_ = settings.value("checkerboardEnabled", false).toBool();
    checkerboardAction_->setChecked(checkerboardEnabled_);
    recursiveScan_ = settings.value("recursiveScan", false).toBool();
    recursiveScanAction_->setChecked(recursiveScan_);
    thumbnailSize_ = settings.value("thumbnailSize", 128).toInt();
    galleryView_->setThumbnailSize(thumbnailSize_);
    lastExportPath_ = settings.value("lastExportPath").toString();
    
    // Restore window geometry if saved
    if (settings.contains("geometry")) {
        restoreGeometry(settings.value("geometry").toByteArray());
    }
    if (settings.contains("windowState")) {
        restoreState(settings.value("windowState").toByteArray());
    }
    if (settings.contains("splitterState")) {
        mainSplitter_->restoreState(settings.value("splitterState").toByteArray());
    }
}

void MainWindow::saveSettings() {
    QSettings settings;
    settings.setValue("recentDirectories", recentDirectories_);
    settings.setValue("checkerboardEnabled", checkerboardEnabled_);
    settings.setValue("recursiveScan", recursiveScan_);
    settings.setValue("thumbnailSize", thumbnailSize_);
    settings.setValue("lastExportPath", lastExportPath_);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("splitterState", mainSplitter_->saveState());
}

// ============================================================================
// Zoom Display
// ============================================================================

void MainWindow::updateZoomDisplay(double factor, bool fitMode) {
    if (fitMode) {
        zoomLabel_->setText("Fit");
    } else {
        int percent = static_cast<int>(factor * 100.0 + 0.5);
        zoomLabel_->setText(QString("%1%").arg(percent));
        
        // Zoom limit feedback
        if (factor <= 0.1 + 0.001) {
            statusBar()->showMessage("🔍 Minimum zoom reached (10%)", 2000);
        } else if (factor >= 10.0 - 0.001) {
            statusBar()->showMessage("🔍 Maximum zoom reached (1000%)", 2000);
        }
    }
    
    // Update window title with zoom info
    QString currentFile = galleryView_->getCurrentFilename();
    if (!currentFile.isEmpty()) {
        QString baseName = QFileInfo(currentFile).fileName();
        if (fitMode) {
            setWindowTitle(QString("%1 [Fit] — VTF-Viewer").arg(baseName));
        } else {
            int percent = static_cast<int>(factor * 100.0 + 0.5);
            if (percent != 100) {
                setWindowTitle(QString("%1 [%2%] — VTF-Viewer").arg(baseName).arg(percent));
            } else {
                setWindowTitle(QString("%1 — VTF-Viewer").arg(baseName));
            }
        }
    }
}

// ============================================================================
// Recursive Scan Toggle
// ============================================================================

void MainWindow::toggleRecursiveScan() {
    recursiveScan_ = !recursiveScan_;
    recursiveScanAction_->setChecked(recursiveScan_);
    saveSettings();
    statusBar()->showMessage(recursiveScan_ ? 
        "📁 Recursive scanning enabled" : 
        "📁 Recursive scanning disabled", 2000);
}

// ============================================================================
// Close Event — Save State
// ============================================================================

void MainWindow::closeEvent(QCloseEvent* event) {
    saveSettings();
    QMainWindow::closeEvent(event);
}

// ============================================================================
// Image Rotation
// ============================================================================

void MainWindow::rotateImageCW() {
    imageViewer_->rotateClockwise();
    statusBar()->showMessage("🔄 Rotated 90° clockwise", 2000);
}

void MainWindow::rotateImageCCW() {
    imageViewer_->rotateCounterClockwise();
    statusBar()->showMessage("🔄 Rotated 90° counter-clockwise", 2000);
}

// ============================================================================
// Texture Count
// ============================================================================

void MainWindow::updateTextureCount() {
    int visible = galleryView_->getVisibleCount();
    int total = loadedTextures_.size();
    if (visible == total) {
        textureCountLabel_->setText(QString("%1 textures").arg(total));
    } else {
        textureCountLabel_->setText(QString("%1/%2 shown").arg(visible).arg(total));
    }
}

// ============================================================================
// Next / Previous Texture
// ============================================================================

void MainWindow::nextTexture() {
    galleryView_->selectNext();
}

void MainWindow::previousTexture() {
    galleryView_->selectPrevious();
}

// ============================================================================
// Full Screen Toggle
// ============================================================================

void MainWindow::toggleFullScreen() {
    if (isFullScreen()) {
        showNormal();
        fullScreenAction_->setChecked(false);
        statusBar()->showMessage("Exited full screen", 2000);
    } else {
        showFullScreen();
        fullScreenAction_->setChecked(true);
        statusBar()->showMessage("Press F11 to exit full screen", 3000);
    }
}

// ============================================================================
// Focus Search
// ============================================================================

void MainWindow::focusSearch() {
    galleryView_->focusSearch();
    statusBar()->showMessage("🔍 Search focused — start typing to filter", 2000);
}

// ============================================================================
// Reload Directory
// ============================================================================

void MainWindow::reloadDirectory() {
    if (currentDirectory_.isEmpty()) {
        statusBar()->showMessage("⚠️ No directory to reload", 3000);
        return;
    }
    statusBar()->showMessage("🔄 Reloading directory...");
    loadDirectory(currentDirectory_);
}

// ============================================================================
// Escape Key Handling
// ============================================================================

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        if (isFullScreen()) {
            showNormal();
            fullScreenAction_->setChecked(false);
            statusBar()->showMessage("Exited full screen", 2000);
        } else {
            // Clear selection and reset title
            imageViewer_->clear();
            propertiesPanel_->clear();
            imageDimensionsLabel_->setText("");
            setWindowTitle("VTF-Viewer v1.3.0 — Source Engine Texture Viewer");
            statusBar()->showMessage("Selection cleared", 2000);
        }
        event->accept();
        return;
    }
    QMainWindow::keyPressEvent(event);
}

// ============================================================================
// Close Current Texture
// ============================================================================

void MainWindow::closeCurrent() {
    imageViewer_->clear();
    propertiesPanel_->clear();
    imageDimensionsLabel_->setText("");
    formatLabel_->setText("");
    
    delete currentVTF_;
    delete currentVMT_;
    currentVTF_ = nullptr;
    currentVMT_ = nullptr;
    
    if (!currentDirectory_.isEmpty()) {
        setWindowTitle(QString("%1 — VTF-Viewer").arg(QFileInfo(currentDirectory_).fileName()));
    } else {
        setWindowTitle("VTF-Viewer v1.3.0 — Source Engine Texture Viewer");
    }
    statusBar()->showMessage("🗑️ Current texture closed", 2000);
}

// ============================================================================
// First / Last Texture
// ============================================================================

void MainWindow::firstTexture() {
    galleryView_->selectFirst();
}

void MainWindow::lastTexture() {
    galleryView_->selectLast();
}

// ============================================================================
// Open Containing Directory
// ============================================================================

void MainWindow::openContainingDir() {
    QString currentFile = galleryView_->getCurrentFilename();
    if (currentFile.isEmpty()) {
        if (!currentDirectory_.isEmpty()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(currentDirectory_));
        } else {
            statusBar()->showMessage("⚠️ No file or directory to open", 3000);
        }
        return;
    }
    QFileInfo fileInfo(currentFile);
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absolutePath()));
    statusBar()->showMessage(QString("📂 Opened: %1").arg(fileInfo.absolutePath()), 2000);
}

// ============================================================================
// Copy File Path to Clipboard
// ============================================================================

void MainWindow::copyFilePath() {
    QString currentFile = galleryView_->getCurrentFilename();
    if (currentFile.isEmpty()) {
        statusBar()->showMessage("⚠️ No file selected to copy path", 3000);
        return;
    }
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(currentFile);
    statusBar()->showMessage(QString("📋 Path copied: %1").arg(currentFile), 3000);
}

// ============================================================================
// Focus Gallery
// ============================================================================

void MainWindow::focusGallery() {
    galleryView_->focusGalleryList();
    statusBar()->showMessage("🖼️ Gallery focused", 2000);
}

// ============================================================================
// Reload Current Texture
// ============================================================================

void MainWindow::reloadCurrentTexture() {
    QString currentFile = galleryView_->getCurrentFilename();
    if (currentFile.isEmpty()) {
        statusBar()->showMessage("⚠️ No texture to reload", 3000);
        return;
    }
    loadTexture(currentFile);
    statusBar()->showMessage(QString("🔄 Reloaded: %1").arg(QFileInfo(currentFile).fileName()), 2000);
}

// ============================================================================
// Quick Export All (last-used settings, no dialog)
// ============================================================================

void MainWindow::quickExportAll() {
    if (loadedTextures_.isEmpty()) {
        statusBar()->showMessage("⚠️ No textures loaded to export", 3000);
        return;
    }
    if (lastExportPath_.isEmpty()) {
        statusBar()->showMessage("⚠️ No previous export path — use Export All first", 3000);
        return;
    }
    
    int count = 0;
    for (auto it = loadedTextures_.begin(); it != loadedTextures_.end(); ++it) {
        exportTexture(it.value(), lastExportPath_, "png", 90);
        count++;
    }
    statusBar()->showMessage(QString("📦 Quick exported %1 textures to %2").arg(count).arg(lastExportPath_), 3000);
}

// ============================================================================
// Reopen Last Directory
// ============================================================================

void MainWindow::reopenLastDirectory() {
    if (recentDirectories_.isEmpty()) {
        statusBar()->showMessage("⚠️ No recent directories to reopen", 3000);
        return;
    }
    QString lastDir = recentDirectories_.first();
    if (QDir(lastDir).exists()) {
        loadDirectory(lastDir);
    } else {
        statusBar()->showMessage(QString("⚠️ Directory no longer exists: %1").arg(lastDir), 3000);
    }
}

// ============================================================================
// Toggle Properties Panel
// ============================================================================

void MainWindow::togglePropertiesPanel() {
    bool visible = propertiesDock_->isVisible();
    propertiesDock_->setVisible(!visible);
    togglePropertiesPanelAction_->setChecked(!visible);
    statusBar()->showMessage(visible ? 
        "📋 Properties panel hidden" : 
        "📋 Properties panel shown", 2000);
}

// ============================================================================
// Random Texture
// ============================================================================

void MainWindow::randomTexture() {
    if (loadedTextures_.isEmpty()) {
        statusBar()->showMessage("⚠️ No textures loaded", 3000);
        return;
    }
    galleryView_->selectRandom();
    statusBar()->showMessage("🎲 Random texture selected", 2000);
}
