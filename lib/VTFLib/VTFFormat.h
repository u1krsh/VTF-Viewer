#ifndef VTFFORMAT_H
#define VTFFORMAT_H

#include <cstdint>

// VTF File Format Structures
namespace VTFLib {

#pragma pack(push, 1)

// VTF Header signature
const char VTF_SIGNATURE[] = "VTF\0";

// Image formats
enum VTFImageFormat {
    IMAGE_FORMAT_NONE = -1,
    IMAGE_FORMAT_RGBA8888 = 0,
    IMAGE_FORMAT_ABGR8888,
    IMAGE_FORMAT_RGB888,
    IMAGE_FORMAT_BGR888,
    IMAGE_FORMAT_RGB565,
    IMAGE_FORMAT_I8,
    IMAGE_FORMAT_IA88,
    IMAGE_FORMAT_P8,
    IMAGE_FORMAT_A8,
    IMAGE_FORMAT_RGB888_BLUESCREEN,
    IMAGE_FORMAT_BGR888_BLUESCREEN,
    IMAGE_FORMAT_ARGB8888,
    IMAGE_FORMAT_BGRA8888,
    IMAGE_FORMAT_DXT1,
    IMAGE_FORMAT_DXT3,
    IMAGE_FORMAT_DXT5,
    IMAGE_FORMAT_BGRX8888,
    IMAGE_FORMAT_BGR565,
    IMAGE_FORMAT_BGRX5551,
    IMAGE_FORMAT_BGRA4444,
    IMAGE_FORMAT_DXT1_ONEBITALPHA,
    IMAGE_FORMAT_BGRA5551,
    IMAGE_FORMAT_UV88,
    IMAGE_FORMAT_UVWQ8888,
    IMAGE_FORMAT_RGBA16161616F,
    IMAGE_FORMAT_RGBA16161616,
    IMAGE_FORMAT_UVLX8888
};

// VTF Flags
enum VTFFlags {
    TEXTUREFLAGS_POINTSAMPLE = 0x00000001,
    TEXTUREFLAGS_TRILINEAR = 0x00000002,
    TEXTUREFLAGS_CLAMPS = 0x00000004,
    TEXTUREFLAGS_CLAMPT = 0x00000008,
    TEXTUREFLAGS_ANISOTROPIC = 0x00000010,
    TEXTUREFLAGS_HINT_DXT5 = 0x00000020,
    TEXTUREFLAGS_SRGB = 0x00000040,
    TEXTUREFLAGS_NORMAL = 0x00000080,
    TEXTUREFLAGS_NOMIP = 0x00000100,
    TEXTUREFLAGS_NOLOD = 0x00000200,
    TEXTUREFLAGS_MINMIP = 0x00000400,
    TEXTUREFLAGS_PROCEDURAL = 0x00000800,
    TEXTUREFLAGS_ONEBITALPHA = 0x00001000,
    TEXTUREFLAGS_EIGHTBITALPHA = 0x00002000,
    TEXTUREFLAGS_ENVMAP = 0x00004000,
    TEXTUREFLAGS_RENDERTARGET = 0x00008000,
    TEXTUREFLAGS_DEPTHRENDERTARGET = 0x00010000,
    TEXTUREFLAGS_NODEBUGOVERRIDE = 0x00020000,
    TEXTUREFLAGS_SINGLECOPY = 0x00040000,
    TEXTUREFLAGS_UNUSED0 = 0x00080000,
    TEXTUREFLAGS_NODEPTHBUFFER = 0x00800000,
    TEXTUREFLAGS_CLAMPU = 0x02000000,
    TEXTUREFLAGS_VERTEXTEXTURE = 0x04000000,
    TEXTUREFLAGS_SSBUMP = 0x08000000,
    TEXTUREFLAGS_BORDER = 0x20000000
};

// VTF Header (Version 7.2+)
struct VTFHeader {
    char signature[4];          // "VTF\0"
    uint32_t version[2];        // version[0].version[1]
    uint32_t headerSize;
    uint16_t width;
    uint16_t height;
    uint32_t flags;
    uint16_t frames;
    uint16_t firstFrame;
    uint8_t padding0[4];
    float reflectivity[3];
    uint8_t padding1[4];
    float bumpmapScale;
    uint32_t highResImageFormat;
    uint8_t mipmapCount;
    uint32_t lowResImageFormat;
    uint8_t lowResImageWidth;
    uint8_t lowResImageHeight;
    
    // Version 7.2+
    uint16_t depth;
};

#pragma pack(pop)

// Helper functions
inline int GetImageFormatBPP(VTFImageFormat format) {
    switch (format) {
        case IMAGE_FORMAT_RGBA8888:
        case IMAGE_FORMAT_ABGR8888:
        case IMAGE_FORMAT_ARGB8888:
        case IMAGE_FORMAT_BGRA8888:
        case IMAGE_FORMAT_BGRX8888:
        case IMAGE_FORMAT_UVWQ8888:
        case IMAGE_FORMAT_UVLX8888:
            return 32;
        case IMAGE_FORMAT_RGB888:
        case IMAGE_FORMAT_BGR888:
        case IMAGE_FORMAT_RGB888_BLUESCREEN:
        case IMAGE_FORMAT_BGR888_BLUESCREEN:
            return 24;
        case IMAGE_FORMAT_RGB565:
        case IMAGE_FORMAT_IA88:
        case IMAGE_FORMAT_BGR565:
        case IMAGE_FORMAT_BGRX5551:
        case IMAGE_FORMAT_BGRA4444:
        case IMAGE_FORMAT_BGRA5551:
        case IMAGE_FORMAT_UV88:
            return 16;
        case IMAGE_FORMAT_I8:
        case IMAGE_FORMAT_P8:
        case IMAGE_FORMAT_A8:
            return 8;
        case IMAGE_FORMAT_DXT1:
        case IMAGE_FORMAT_DXT1_ONEBITALPHA:
            return 4;
        case IMAGE_FORMAT_DXT3:
        case IMAGE_FORMAT_DXT5:
            return 8;
        case IMAGE_FORMAT_RGBA16161616F:
        case IMAGE_FORMAT_RGBA16161616:
            return 64;
        default:
            return 0;
    }
}

inline const char* GetImageFormatName(VTFImageFormat format) {
    switch (format) {
        case IMAGE_FORMAT_RGBA8888: return "RGBA8888";
        case IMAGE_FORMAT_ABGR8888: return "ABGR8888";
        case IMAGE_FORMAT_RGB888: return "RGB888";
        case IMAGE_FORMAT_BGR888: return "BGR888";
        case IMAGE_FORMAT_RGB565: return "RGB565";
        case IMAGE_FORMAT_I8: return "I8";
        case IMAGE_FORMAT_IA88: return "IA88";
        case IMAGE_FORMAT_P8: return "P8";
        case IMAGE_FORMAT_A8: return "A8";
        case IMAGE_FORMAT_ARGB8888: return "ARGB8888";
        case IMAGE_FORMAT_BGRA8888: return "BGRA8888";
        case IMAGE_FORMAT_DXT1: return "DXT1";
        case IMAGE_FORMAT_DXT3: return "DXT3";
        case IMAGE_FORMAT_DXT5: return "DXT5";
        case IMAGE_FORMAT_BGRX8888: return "BGRX8888";
        case IMAGE_FORMAT_BGR565: return "BGR565";
        case IMAGE_FORMAT_BGRX5551: return "BGRX5551";
        case IMAGE_FORMAT_BGRA4444: return "BGRA4444";
        case IMAGE_FORMAT_DXT1_ONEBITALPHA: return "DXT1_ONEBITALPHA";
        case IMAGE_FORMAT_BGRA5551: return "BGRA5551";
        case IMAGE_FORMAT_UV88: return "UV88";
        case IMAGE_FORMAT_UVWQ8888: return "UVWQ8888";
        case IMAGE_FORMAT_RGBA16161616F: return "RGBA16161616F";
        case IMAGE_FORMAT_RGBA16161616: return "RGBA16161616";
        case IMAGE_FORMAT_UVLX8888: return "UVLX8888";
        default: return "UNKNOWN";
    }
}

} // namespace VTFLib

#endif // VTFFORMAT_H
