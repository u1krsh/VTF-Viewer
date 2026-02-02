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
#include <QApplication>
#include <QScreen>
#include <QStyle>

MainWindow::MainWindow(QWidget* parent) 
    : QMainWindow(parent), currentVTF_(nullptr), currentVMT_(nullptr) {
    
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
}

void MainWindow::createMenus() {
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(openDirAction_);
    fileMenu->addSeparator();
    fileMenu->addAction(exportCurrentAction_);
    fileMenu->addAction(exportAllAction_);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction_);
    
    QMenu* viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction(zoomInAction_);
    viewMenu->addAction(zoomOutAction_);
    viewMenu->addAction(resetZoomAction_);
    viewMenu->addAction(fitToWindowAction_);
    
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
}

void MainWindow::createStatusBar() {
    statusBar()->showMessage("Ready");
}

void MainWindow::createDockWidgets() {
    propertiesPanel_ = new PropertiesPanel;
    
    QDockWidget* propertiesDock = new QDockWidget("Properties", this);
    propertiesDock->setWidget(propertiesPanel_);
    propertiesDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, propertiesDock);
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
    
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
    
    if (files.isEmpty()) {
        QMessageBox::information(this, "No Files Found",
                               "No VTF or VMT files found in the selected directory.");
        return;
    }
    
    QProgressDialog progress("Loading textures...", "Cancel", 0, files.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    
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
    
    statusBar()->showMessage(QString("✅ Loaded %1 textures").arg(count));
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
    if (dialog.exec() == QDialog::Accepted) {
        QString outputPath = dialog.getOutputPath();
        QString format = dialog.getFormat();
        int quality = dialog.getQuality();
        
        if (outputPath.isEmpty()) {
            QMessageBox::warning(this, "Invalid Path",
                               "Please select an output directory.");
            return;
        }
        
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
    if (dialog.exec() == QDialog::Accepted) {
        QString outputPath = dialog.getOutputPath();
        QString format = dialog.getFormat();
        int quality = dialog.getQuality();
        
        if (outputPath.isEmpty()) {
            QMessageBox::warning(this, "Invalid Path",
                               "Please select an output directory.");
            return;
        }
        
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
        "<p>Version 1.0.0</p>"
        "<p>A tool for viewing and exporting Source Engine textures.</p>"
        "<hr>"
        "<p><b>Keyboard Shortcuts:</b></p>"
        "<table align='center' style='margin: 0 auto;'>"
        "<tr><td>Ctrl+O</td><td>&nbsp;Open Directory</td></tr>"
        "<tr><td>Ctrl+E</td><td>&nbsp;Export Current</td></tr>"
        "<tr><td>Ctrl++</td><td>&nbsp;Zoom In</td></tr>"
        "<tr><td>Ctrl+-</td><td>&nbsp;Zoom Out</td></tr>"
        "<tr><td>Ctrl+0</td><td>&nbsp;Reset Zoom</td></tr>"
        "<tr><td>Ctrl+F</td><td>&nbsp;Fit to Window</td></tr>"
        "</table>"
        "<hr>"
        "<p>Built with Qt6. Released under GPL v3.0.</p>"
        "</div>");
}
