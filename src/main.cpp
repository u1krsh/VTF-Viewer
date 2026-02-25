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
    QApplication::setApplicationName("VTF-Viewer");
    QApplication::setApplicationVersion("1.3.0");
    QApplication::setOrganizationName("VTF-Viewer");
    QApplication::setOrganizationDomain("vtf-viewer.local");
    QApplication::setWindowIcon(QIcon(":/icons/icon.png"));

    // Set modern font
    QFont font("Segoe UI", 10);
    QApplication::setFont(font);

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
    window.setWindowTitle("VTF-Viewer v1.4.0 — Source Engine Texture Viewer");
    window.show();
    
    int result = QApplication::exec();

    // Cleanup
    VTFLib::Shutdown();
    
    return result;
}
