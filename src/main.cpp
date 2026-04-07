#include "MainWindow.h"
#include "VTFLib.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QFont>
#include <QIcon>

// Application entry point
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // Set application metadata
    QApplication::setApplicationName("VTF-Viewer");
    QApplication::setApplicationVersion("1.4.0");
    // Set organization name
    QApplication::setOrganizationName("VTF-Viewer");
    QApplication::setOrganizationDomain("vtf-viewer.local");
    QApplication::setWindowIcon(QIcon(":/icons/icon.png"));

    // Set modern font
    // Use Segoe UI as default font for cleaner UI
    QFont font("Segoe UI", 10);
    QApplication::setFont(font);

    // Load revolutionary stylesheet
    // Load custom application stylesheet
    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile);
        QString stylesheet = stream.readAll();
        app.setStyleSheet(stylesheet);
        styleFile.close();
    }
    
    // Initialize VTFLib
    // Initialize the VTFLib parser
    VTFLib::Initialize();
    
    // Create and show main window
    // Create the main window instance
    MainWindow window;
    window.setWindowTitle("VTF-Viewer v1.4.0 — Source Engine Texture Viewer");
    window.show();
    
    int result = QApplication::exec();

    // Cleanup
    // Clean up and shutdown VTFLib
    VTFLib::Shutdown();
    
    return result;
}

// qol checkpoint 105
