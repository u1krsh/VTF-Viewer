#ifndef VMTPARSER_H
#define VMTPARSER_H

#include <QString>
#include <QMap>
#include <memory>

namespace VTFLib {
    class VMTFile;
}

class VMTParser {
public:
    VMTParser();
    ~VMTParser();
    
    bool loadFile(const QString& filename);
    
    QString getShader() const;
    QString getBaseTexture() const;
    QString getParameter(const QString& name) const;
    QMap<QString, QString> getAllParameters() const;
    
    bool isLoaded() const;
    
private:
    std::unique_ptr<VTFLib::VMTFile> vmtFile_;
    void extractParameters(QMap<QString, QString>& params) const;
};

#endif // VMTPARSER_H
