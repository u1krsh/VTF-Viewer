#ifndef VTFREADER_H
#define VTFREADER_H

#include <QImage>
#include <QString>
#include <memory>

namespace VTFLib {
    class VTFFile;
}

class VTFReader {
public:
    VTFReader();
    ~VTFReader();
    
    bool loadFile(const QString& filename);
    QImage getImage(int frame = 0, int mipmap = 0);
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
