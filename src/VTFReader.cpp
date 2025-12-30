#include "VTFReader.h"
#include "VTFFile.h"
#include "VTFFormat.h"
#include <QDebug>

VTFReader::VTFReader() : vtfFile_(std::make_unique<VTFLib::VTFFile>()) {
}

VTFReader::~VTFReader() {
}

bool VTFReader::loadFile(const QString& filename) {
    return vtfFile_->Load(filename.toStdString());
}

QImage VTFReader::getImage(int frame, int mipmap) {
    if (!vtfFile_->IsLoaded()) {
        return QImage();
    }
    
    int width = std::max(1, vtfFile_->GetWidth() >> mipmap);
    int height = std::max(1, vtfFile_->GetHeight() >> mipmap);
    
    QImage image(width, height, QImage::Format_RGBA8888);
    
    if (vtfFile_->GetImageData(image.bits(), frame, mipmap)) {
        return image;
    }
    
    return QImage();
}

QImage VTFReader::getThumbnail(int maxSize) {
    if (!vtfFile_->IsLoaded()) {
        return QImage();
    }
    
    // Find appropriate mipmap level for thumbnail
    int mipmap = 0;
    int width = vtfFile_->GetWidth();
    int height = vtfFile_->GetHeight();
    
    while (mipmap < vtfFile_->GetMipmapCount() - 1) {
        int mipWidth = width >> (mipmap + 1);
        int mipHeight = height >> (mipmap + 1);
        
        if (mipWidth < maxSize || mipHeight < maxSize) {
            break;
        }
        mipmap++;
    }
    
    QImage img = getImage(0, mipmap);
    
    // Scale to fit maxSize if needed
    if (img.width() > maxSize || img.height() > maxSize) {
        img = img.scaled(maxSize, maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    
    return img;
}

int VTFReader::getWidth() const {
    return vtfFile_->GetWidth();
}

int VTFReader::getHeight() const {
    return vtfFile_->GetHeight();
}

int VTFReader::getFrameCount() const {
    return vtfFile_->GetFrameCount();
}

int VTFReader::getMipmapCount() const {
    return vtfFile_->GetMipmapCount();
}

QString VTFReader::getFormat() const {
    return QString::fromUtf8(VTFLib::GetImageFormatName(vtfFile_->GetFormat()));
}

quint32 VTFReader::getFlags() const {
    return vtfFile_->GetFlags();
}

bool VTFReader::isLoaded() const {
    return vtfFile_->IsLoaded();
}
