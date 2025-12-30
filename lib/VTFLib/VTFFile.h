#ifndef VTFFILE_H
#define VTFFILE_H

#include "VTFFormat.h"
#include <string>
#include <vector>
#include <cstdint>

namespace VTFLib {

class VTFFile {
public:
    VTFFile();
    ~VTFFile();
    
    // Load VTF file from disk
    bool Load(const std::string& filename);
    
    // Get header information
    uint16_t GetWidth() const { return header_.width; }
    uint16_t GetHeight() const { return header_.height; }
    uint16_t GetDepth() const { return header_.depth; }
    uint16_t GetFrameCount() const { return header_.frames; }
    uint8_t GetMipmapCount() const { return header_.mipmapCount; }
    VTFImageFormat GetFormat() const { return static_cast<VTFImageFormat>(header_.highResImageFormat); }
    uint32_t GetFlags() const { return header_.flags; }
    
    // Get image data (returns RGBA8888 format)
    bool GetImageData(uint8_t* buffer, uint32_t frame = 0, uint32_t mipmap = 0);
    
    // Get raw image data size for a specific mipmap level
    uint32_t GetImageDataSize(uint32_t mipmap = 0) const;
    
    // Check if file is loaded
    bool IsLoaded() const { return loaded_; }
    
private:
    VTFHeader header_;
    std::vector<uint8_t> imageData_;
    bool loaded_;
    
    // Helper functions
    uint32_t ComputeImageSize(uint16_t width, uint16_t height, VTFImageFormat format) const;
    uint32_t ComputeMipmapOffset(uint32_t frame, uint32_t mipmap) const;
    void DecompressDXT1(const uint8_t* src, uint8_t* dst, uint16_t width, uint16_t height);
    void DecompressDXT5(const uint8_t* src, uint8_t* dst, uint16_t width, uint16_t height);
    void ConvertToRGBA8888(const uint8_t* src, uint8_t* dst, uint16_t width, uint16_t height, VTFImageFormat format);
};

} // namespace VTFLib

#endif // VTFFILE_H
