#include "MainWindow.h"
#include "VTFLib.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QFont>
#include <QIcon>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // Set application metadata
    app.setApplicationName("VTF-Viewer");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("VTF-Viewer");
    app.setOrganizationDomain("vtf-viewer.local");
    app.setWindowIcon(QIcon(":/icons/icon.png"));
    
    // Set modern font
    QFont font("Segoe UI", 10);
    app.setFont(font);
    
    // Load revolutionary stylesheet
    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile);
        QString stylesheet = stream.readAll();
        app.setStyleSheet(stylesheet);
        styleFile.close();
    }
    
    // Initialize VTFLib
    VTFLib::Initialize();
    
    // Create and show main window
    MainWindow window;
    window.setWindowTitle("VTF-Viewer - Source Engine Texture Viewer");
    window.show();
    
    int result = app.exec();
    
    // Cleanup
    VTFLib::Shutdown();
    
    return result;
}
