#ifndef PROPERTIESPANEL_H
#define PROPERTIESPANEL_H

#include <QWidget>
#include <QTextEdit>

// Panel to display metadata details of files
class PropertiesPanel : public QWidget {
    Q_OBJECT
    
public:
    explicit PropertiesPanel(QWidget* parent = nullptr);
    
    void setVTFProperties(const QString& filename, int width, int height, 
                         const QString& format, int frames, int mipmaps, quint32 flags);
    void setVMTProperties(const QString& shader, const QMap<QString, QString>& parameters);
    // Clear properties text box
    void clear();
    
private:
    QTextEdit* textEdit_;
    
    // Format texture flags to a human-readable list
    QString formatFlags(quint32 flags);
    // Format file size in B, KB, MB or GB
    QString formatFileSize(qint64 bytes);
    QString calculateAspectRatio(int width, int height);
    QString estimateMemorySize(int width, int height, int mipmaps);
};

#endif // PROPERTIESPANEL_H
