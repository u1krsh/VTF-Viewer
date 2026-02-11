#ifndef PROPERTIESPANEL_H
#define PROPERTIESPANEL_H

#include <QWidget>
#include <QTextEdit>

class PropertiesPanel : public QWidget {
    Q_OBJECT
    
public:
    explicit PropertiesPanel(QWidget* parent = nullptr);
    
    void setVTFProperties(const QString& filename, int width, int height, 
                         const QString& format, int frames, int mipmaps, quint32 flags);
    void setVMTProperties(const QString& shader, const QMap<QString, QString>& parameters);
    void clear();
    
private:
    QTextEdit* textEdit_;
    
    QString formatFlags(quint32 flags);
    QString formatFileSize(qint64 bytes);
};

#endif // PROPERTIESPANEL_H
