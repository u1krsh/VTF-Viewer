#ifndef VTFREADER_H
#define VTFREADER_H

#include <QImage>
#include <QString>
#include <memory>

namespace VTFLib {
    class VTFFile;
}

// Reader class for loading and parsing VTF textures
class VTFReader {
public:
    VTFReader();
    ~VTFReader();
    
    // Load texture from file path
    bool loadFile(const QString& filename);
    // Get QImage representing texture frame and mipmap level
    QImage getImage(int frame = 0, int mipmap = 0);
    // Get scaled thumbnail representation
    QImage getThumbnail(int maxSize = 128);
    
    int getWidth() const;
    int getHeight() const;
    int getFrameCount() const;
    int getMipmapCount() const;
    QString getFormat() const;
    quint32 getFlags() const;
    
    bool isLoaded() const;
    
private:
    std::unique_ptr<VTFLib::VTFFile> vtfFile_;
};

#endif // VTFREADER_H
