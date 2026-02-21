#include "PropertiesPanel.h"
#include "VTFFormat.h"
#include <QVBoxLayout>
#include <QFileInfo>
#include <numeric>

PropertiesPanel::PropertiesPanel(QWidget* parent) : QWidget(parent) {
    textEdit_ = new QTextEdit;
    textEdit_->setReadOnly(true);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(textEdit_);
}

QString PropertiesPanel::formatFileSize(qint64 bytes) {
    if (bytes < 1024) {
        return QString("%1 B").arg(bytes);
    } else if (bytes < 1024 * 1024) {
        return QString("%1 KB").arg(bytes / 1024.0, 0, 'f', 1);
    } else if (bytes < 1024LL * 1024 * 1024) {
        return QString("%1 MB").arg(bytes / (1024.0 * 1024.0), 0, 'f', 2);
    } else {
        return QString("%1 GB").arg(bytes / (1024.0 * 1024.0 * 1024.0), 0, 'f', 2);
    }
}

void PropertiesPanel::setVTFProperties(const QString& filename, int width, int height,
                                      const QString& format, int frames, int mipmaps, quint32 flags) {
    QFileInfo fileInfo(filename);
    
    // Calculate pixel count for display
    QString pixelCount;
    double megapixels = (width * height) / 1000000.0;
    if (megapixels >= 1.0) {
        pixelCount = QString(" (%1 MP)").arg(megapixels, 0, 'f', 1);
    } else {
        pixelCount = QString(" (%1 KP)").arg((width * height) / 1000.0, 0, 'f', 1);
    }
    
    QString html = "<h3>VTF Texture Properties</h3>";
    html += "<table cellpadding='4'>";
    html += QString("<tr><td><b>File:</b></td><td>%1</td></tr>").arg(fileInfo.fileName());
    html += QString("<tr><td><b>Path:</b></td><td>%1</td></tr>").arg(fileInfo.absolutePath());
    html += QString("<tr><td><b>File Size:</b></td><td>%1</td></tr>").arg(formatFileSize(fileInfo.size()));
    html += QString("<tr><td><b>Dimensions:</b></td><td>%1 x %2%3</td></tr>").arg(width).arg(height).arg(pixelCount);
    html += QString("<tr><td><b>Aspect Ratio:</b></td><td>%1</td></tr>").arg(calculateAspectRatio(width, height));
    html += QString("<tr><td><b>Format:</b></td><td>%1</td></tr>").arg(format);
    html += QString("<tr><td><b>Frames:</b></td><td>%1</td></tr>").arg(frames);
    html += QString("<tr><td><b>Mipmaps:</b></td><td>%1</td></tr>").arg(mipmaps);
    html += QString("<tr><td><b>Flags:</b></td><td>%1</td></tr>").arg(formatFlags(flags));
    html += "</table>";
    
    textEdit_->setHtml(html);
}

void PropertiesPanel::setVMTProperties(const QString& shader, const QMap<QString, QString>& parameters) {
    QString html = "<h3>VMT Material Properties</h3>";
    html += "<table cellpadding='4'>";
    html += QString("<tr><td><b>Shader:</b></td><td>%1</td></tr>").arg(shader);
    html += QString("<tr><td><b>Parameters:</b></td><td>%1</td></tr>").arg(parameters.size());
    
    for (auto it = parameters.begin(); it != parameters.end(); ++it) {
        html += QString("<tr><td><b>%1:</b></td><td>%2</td></tr>")
                .arg(it.key()).arg(it.value());
    }
    
    html += "</table>";
    textEdit_->setHtml(html);
}

void PropertiesPanel::clear() {
    textEdit_->clear();
}

QString PropertiesPanel::formatFlags(quint32 flags) {
    QStringList flagList;
    
    if (flags & VTFLib::TEXTUREFLAGS_POINTSAMPLE) flagList << "PointSample";
    if (flags & VTFLib::TEXTUREFLAGS_TRILINEAR) flagList << "Trilinear";
    if (flags & VTFLib::TEXTUREFLAGS_CLAMPS) flagList << "ClampS";
    if (flags & VTFLib::TEXTUREFLAGS_CLAMPT) flagList << "ClampT";
    if (flags & VTFLib::TEXTUREFLAGS_ANISOTROPIC) flagList << "Anisotropic";
    if (flags & VTFLib::TEXTUREFLAGS_SRGB) flagList << "sRGB";
    if (flags & VTFLib::TEXTUREFLAGS_NORMAL) flagList << "Normal Map";
    if (flags & VTFLib::TEXTUREFLAGS_NOMIP) flagList << "NoMip";
    if (flags & VTFLib::TEXTUREFLAGS_NOLOD) flagList << "NoLOD";
    if (flags & VTFLib::TEXTUREFLAGS_ENVMAP) flagList << "EnvMap";
    
    if (flagList.isEmpty()) {
        return "None (0x0)";
    }
    
    return QString("%1 (%2 flags, 0x%3)")
        .arg(flagList.join(", "))
        .arg(flagList.size())
        .arg(flags, 0, 16);
}

QString PropertiesPanel::calculateAspectRatio(int width, int height) {
    if (width <= 0 || height <= 0) return "N/A";
    int g = std::gcd(width, height);
    return QString("%1:%2").arg(width / g).arg(height / g);
}
