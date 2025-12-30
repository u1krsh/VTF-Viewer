#include "VTFFile.h"
#include <fstream>
#include <cstring>
#include <algorithm>

namespace VTFLib {

VTFFile::VTFFile() : loaded_(false) {
    memset(&header_, 0, sizeof(VTFHeader));
}

VTFFile::~VTFFile() {
}

bool VTFFile::Load(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Read header
    file.read(reinterpret_cast<char*>(&header_), sizeof(VTFHeader));
    
    // Verify signature
    if (strncmp(header_.signature, VTF_SIGNATURE, 4) != 0) {
        return false;
    }
    
    // Check version (support 7.0 - 7.5)
    if (header_.version[0] != 7 || header_.version[1] > 5) {
        return false;
    }
    
    // Calculate total image data size
    uint32_t totalSize = 0;
    for (uint32_t frame = 0; frame < header_.frames; ++frame) {
        for (uint32_t mip = 0; mip < header_.mipmapCount; ++mip) {
            uint16_t mipWidth = std::max(1, header_.width >> mip);
            uint16_t mipHeight = std::max(1, header_.height >> mip);
            totalSize += ComputeImageSize(mipWidth, mipHeight, 
                static_cast<VTFImageFormat>(header_.highResImageFormat));
        }
    }
    
    // Seek to image data (after header)
    file.seekg(header_.headerSize);
    
    // Read low-res image if present
    if (static_cast<VTFImageFormat>(header_.lowResImageFormat) != IMAGE_FORMAT_NONE) {
        uint32_t lowResSize = ComputeImageSize(header_.lowResImageWidth, 
            header_.lowResImageHeight, 
            static_cast<VTFImageFormat>(header_.lowResImageFormat));
        file.seekg(lowResSize, std::ios::cur);
    }
    
    // Read image data
    imageData_.resize(totalSize);
    file.read(reinterpret_cast<char*>(imageData_.data()), totalSize);
    
    loaded_ = true;
    return true;
}

uint32_t VTFFile::ComputeImageSize(uint16_t width, uint16_t height, VTFImageFormat format) const {
    uint32_t bpp = GetImageFormatBPP(format);
    
    // DXT formats are block-compressed (4x4 blocks)
    if (format == IMAGE_FORMAT_DXT1 || format == IMAGE_FORMAT_DXT1_ONEBITALPHA) {
        uint32_t blockWidth = (width + 3) / 4;
        uint32_t blockHeight = (height + 3) / 4;
        return blockWidth * blockHeight * 8; // 8 bytes per block for DXT1
    } else if (format == IMAGE_FORMAT_DXT3 || format == IMAGE_FORMAT_DXT5) {
        uint32_t blockWidth = (width + 3) / 4;
        uint32_t blockHeight = (height + 3) / 4;
        return blockWidth * blockHeight * 16; // 16 bytes per block for DXT3/5
    }
    
    return (width * height * bpp) / 8;
}

uint32_t VTFFile::ComputeMipmapOffset(uint32_t frame, uint32_t mipmap) const {
    uint32_t offset = 0;
    VTFImageFormat format = static_cast<VTFImageFormat>(header_.highResImageFormat);
    
    // Add offsets for previous frames
    for (uint32_t f = 0; f < frame; ++f) {
        for (uint32_t m = 0; m < header_.mipmapCount; ++m) {
            uint16_t mipWidth = std::max(1, header_.width >> m);
            uint16_t mipHeight = std::max(1, header_.height >> m);
            offset += ComputeImageSize(mipWidth, mipHeight, format);
        }
    }
    
    // Add offsets for previous mipmaps in current frame
    // VTF stores mipmaps from smallest to largest
    for (uint32_t m = header_.mipmapCount - 1; m > mipmap; --m) {
        uint16_t mipWidth = std::max(1, header_.width >> m);
        uint16_t mipHeight = std::max(1, header_.height >> m);
        offset += ComputeImageSize(mipWidth, mipHeight, format);
    }
    
    return offset;
}

void VTFFile::DecompressDXT1(const uint8_t* src, uint8_t* dst, uint16_t width, uint16_t height) {
    uint32_t blockCountX = (width + 3) / 4;
    uint32_t blockCountY = (height + 3) / 4;
    
    for (uint32_t by = 0; by < blockCountY; ++by) {
        for (uint32_t bx = 0; bx < blockCountX; ++bx) {
            const uint8_t* block = src + (by * blockCountX + bx) * 8;
            
            // Read color endpoints
            uint16_t c0 = block[0] | (block[1] << 8);
            uint16_t c1 = block[2] | (block[3] << 8);
            
            // Decode RGB565 colors
            uint8_t r0 = ((c0 >> 11) & 0x1F) << 3;
            uint8_t g0 = ((c0 >> 5) & 0x3F) << 2;
            uint8_t b0 = (c0 & 0x1F) << 3;
            
            uint8_t r1 = ((c1 >> 11) & 0x1F) << 3;
            uint8_t g1 = ((c1 >> 5) & 0x3F) << 2;
            uint8_t b1 = (c1 & 0x1F) << 3;
            
            // Compute color palette
            uint8_t colors[4][4];
            colors[0][0] = r0; colors[0][1] = g0; colors[0][2] = b0; colors[0][3] = 255;
            colors[1][0] = r1; colors[1][1] = g1; colors[1][2] = b1; colors[1][3] = 255;
            
            if (c0 > c1) {
                colors[2][0] = (2 * r0 + r1) / 3;
                colors[2][1] = (2 * g0 + g1) / 3;
                colors[2][2] = (2 * b0 + b1) / 3;
                colors[2][3] = 255;
                
                colors[3][0] = (r0 + 2 * r1) / 3;
                colors[3][1] = (g0 + 2 * g1) / 3;
                colors[3][2] = (b0 + 2 * b1) / 3;
                colors[3][3] = 255;
            } else {
                colors[2][0] = (r0 + r1) / 2;
                colors[2][1] = (g0 + g1) / 2;
                colors[2][2] = (b0 + b1) / 2;
                colors[2][3] = 255;
                
                colors[3][0] = 0;
                colors[3][1] = 0;
                colors[3][2] = 0;
                colors[3][3] = 0; // Transparent
            }
            
            // Decode pixels
            uint32_t indices = block[4] | (block[5] << 8) | (block[6] << 16) | (block[7] << 24);
            
            for (uint32_t py = 0; py < 4; ++py) {
                for (uint32_t px = 0; px < 4; ++px) {
                    uint32_t x = bx * 4 + px;
                    uint32_t y = by * 4 + py;
                    
                    if (x < width && y < height) {
                        uint32_t index = (indices >> ((py * 4 + px) * 2)) & 0x3;
                        uint32_t dstOffset = (y * width + x) * 4;
                        
                        dst[dstOffset + 0] = colors[index][0];
                        dst[dstOffset + 1] = colors[index][1];
                        dst[dstOffset + 2] = colors[index][2];
                        dst[dstOffset + 3] = colors[index][3];
                    }
                }
            }
        }
    }
}

void VTFFile::DecompressDXT5(const uint8_t* src, uint8_t* dst, uint16_t width, uint16_t height) {
    uint32_t blockCountX = (width + 3) / 4;
    uint32_t blockCountY = (height + 3) / 4;
    
    for (uint32_t by = 0; by < blockCountY; ++by) {
        for (uint32_t bx = 0; bx < blockCountX; ++bx) {
            const uint8_t* block = src + (by * blockCountX + bx) * 16;
            
            // Alpha block
            uint8_t a0 = block[0];
            uint8_t a1 = block[1];
            uint64_t alphaBits = 0;
            for (int i = 0; i < 6; ++i) {
                alphaBits |= static_cast<uint64_t>(block[2 + i]) << (i * 8);
            }
            
            // Compute alpha palette
            uint8_t alphas[8];
            alphas[0] = a0;
            alphas[1] = a1;
            
            if (a0 > a1) {
                for (int i = 2; i < 8; ++i) {
                    alphas[i] = ((8 - i) * a0 + (i - 1) * a1) / 7;
                }
            } else {
                for (int i = 2; i < 6; ++i) {
                    alphas[i] = ((6 - i) * a0 + (i - 1) * a1) / 5;
                }
                alphas[6] = 0;
                alphas[7] = 255;
            }
            
            // Color block (same as DXT1)
            const uint8_t* colorBlock = block + 8;
            uint16_t c0 = colorBlock[0] | (colorBlock[1] << 8);
            uint16_t c1 = colorBlock[2] | (colorBlock[3] << 8);
            
            uint8_t r0 = ((c0 >> 11) & 0x1F) << 3;
            uint8_t g0 = ((c0 >> 5) & 0x3F) << 2;
            uint8_t b0 = (c0 & 0x1F) << 3;
            
            uint8_t r1 = ((c1 >> 11) & 0x1F) << 3;
            uint8_t g1 = ((c1 >> 5) & 0x3F) << 2;
            uint8_t b1 = (c1 & 0x1F) << 3;
            
            uint8_t colors[4][3];
            colors[0][0] = r0; colors[0][1] = g0; colors[0][2] = b0;
            colors[1][0] = r1; colors[1][1] = g1; colors[1][2] = b1;
            colors[2][0] = (2 * r0 + r1) / 3;
            colors[2][1] = (2 * g0 + g1) / 3;
            colors[2][2] = (2 * b0 + b1) / 3;
            colors[3][0] = (r0 + 2 * r1) / 3;
            colors[3][1] = (g0 + 2 * g1) / 3;
            colors[3][2] = (b0 + 2 * b1) / 3;
            
            uint32_t colorIndices = colorBlock[4] | (colorBlock[5] << 8) | 
                                   (colorBlock[6] << 16) | (colorBlock[7] << 24);
            
            // Decode pixels
            for (uint32_t py = 0; py < 4; ++py) {
                for (uint32_t px = 0; px < 4; ++px) {
                    uint32_t x = bx * 4 + px;
                    uint32_t y = by * 4 + py;
                    
                    if (x < width && y < height) {
                        uint32_t pixelIndex = py * 4 + px;
                        uint32_t colorIndex = (colorIndices >> (pixelIndex * 2)) & 0x3;
                        uint32_t alphaIndex = (alphaBits >> (pixelIndex * 3)) & 0x7;
                        uint32_t dstOffset = (y * width + x) * 4;
                        
                        dst[dstOffset + 0] = colors[colorIndex][0];
                        dst[dstOffset + 1] = colors[colorIndex][1];
                        dst[dstOffset + 2] = colors[colorIndex][2];
                        dst[dstOffset + 3] = alphas[alphaIndex];
                    }
                }
            }
        }
    }
}

void VTFFile::ConvertToRGBA8888(const uint8_t* src, uint8_t* dst, 
                                uint16_t width, uint16_t height, VTFImageFormat format) {
    uint32_t pixelCount = width * height;
    
    switch (format) {
        case IMAGE_FORMAT_RGBA8888:
            memcpy(dst, src, pixelCount * 4);
            break;
            
        case IMAGE_FORMAT_BGRA8888:
            for (uint32_t i = 0; i < pixelCount; ++i) {
                dst[i * 4 + 0] = src[i * 4 + 2]; // R
                dst[i * 4 + 1] = src[i * 4 + 1]; // G
                dst[i * 4 + 2] = src[i * 4 + 0]; // B
                dst[i * 4 + 3] = src[i * 4 + 3]; // A
            }
            break;
            
        case IMAGE_FORMAT_RGB888:
            for (uint32_t i = 0; i < pixelCount; ++i) {
                dst[i * 4 + 0] = src[i * 3 + 0];
                dst[i * 4 + 1] = src[i * 3 + 1];
                dst[i * 4 + 2] = src[i * 3 + 2];
                dst[i * 4 + 3] = 255;
            }
            break;
            
        case IMAGE_FORMAT_BGR888:
            for (uint32_t i = 0; i < pixelCount; ++i) {
                dst[i * 4 + 0] = src[i * 3 + 2];
                dst[i * 4 + 1] = src[i * 3 + 1];
                dst[i * 4 + 2] = src[i * 3 + 0];
                dst[i * 4 + 3] = 255;
            }
            break;
            
        default:
            // For unsupported formats, fill with magenta
            for (uint32_t i = 0; i < pixelCount; ++i) {
                dst[i * 4 + 0] = 255;
                dst[i * 4 + 1] = 0;
                dst[i * 4 + 2] = 255;
                dst[i * 4 + 3] = 255;
            }
            break;
    }
}

bool VTFFile::GetImageData(uint8_t* buffer, uint32_t frame, uint32_t mipmap) {
    if (!loaded_ || frame >= header_.frames || mipmap >= header_.mipmapCount) {
        return false;
    }
    
    uint16_t mipWidth = std::max(1, header_.width >> mipmap);
    uint16_t mipHeight = std::max(1, header_.height >> mipmap);
    uint32_t offset = ComputeMipmapOffset(frame, mipmap);
    
    VTFImageFormat format = static_cast<VTFImageFormat>(header_.highResImageFormat);
    const uint8_t* srcData = imageData_.data() + offset;
    
    // Decompress or convert based on format
    if (format == IMAGE_FORMAT_DXT1 || format == IMAGE_FORMAT_DXT1_ONEBITALPHA) {
        DecompressDXT1(srcData, buffer, mipWidth, mipHeight);
    } else if (format == IMAGE_FORMAT_DXT5) {
        DecompressDXT5(srcData, buffer, mipWidth, mipHeight);
    } else if (format == IMAGE_FORMAT_DXT3) {
        // DXT3 is similar to DXT5 but with different alpha encoding
        // For simplicity, treat as DXT5
        DecompressDXT5(srcData, buffer, mipWidth, mipHeight);
    } else {
        ConvertToRGBA8888(srcData, buffer, mipWidth, mipHeight, format);
    }
    
    return true;
}

uint32_t VTFFile::GetImageDataSize(uint32_t mipmap) const {
    if (mipmap >= header_.mipmapCount) {
        return 0;
    }
    
    uint16_t mipWidth = std::max(1, header_.width >> mipmap);
    uint16_t mipHeight = std::max(1, header_.height >> mipmap);
    
    // Always return RGBA8888 size
    return mipWidth * mipHeight * 4;
}

} // namespace VTFLib
