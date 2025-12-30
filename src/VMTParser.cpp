#include "VMTParser.h"
#include "VMTFile.h"

VMTParser::VMTParser() : vmtFile_(std::make_unique<VTFLib::VMTFile>()) {
}

VMTParser::~VMTParser() {
}

bool VMTParser::loadFile(const QString& filename) {
    return vmtFile_->Load(filename.toStdString());
}

QString VMTParser::getShader() const {
    if (!isLoaded()) {
        return QString();
    }
    return QString::fromStdString(vmtFile_->GetShader());
}

QString VMTParser::getBaseTexture() const {
    if (!isLoaded()) {
        return QString();
    }
    return QString::fromStdString(vmtFile_->GetBaseTexture());
}

QString VMTParser::getParameter(const QString& name) const {
    if (!isLoaded()) {
        return QString();
    }
    
    VTFLib::VMTNode* node = const_cast<VTFLib::VMTFile*>(vmtFile_.get())->GetRoot()->FindChild(name.toStdString());
    if (node) {
        return QString::fromStdString(node->GetValue());
    }
    
    return QString();
}

QMap<QString, QString> VMTParser::getAllParameters() const {
    QMap<QString, QString> params;
    
    if (!isLoaded()) {
        return params;
    }
    
    extractParameters(params);
    return params;
}

void VMTParser::extractParameters(QMap<QString, QString>& params) const {
    const VTFLib::VMTNode* root = vmtFile_->GetRoot();
    if (!root) {
        return;
    }
    
    for (const auto& child : root->GetChildren()) {
        QString name = QString::fromStdString(child.GetName());
        QString value = QString::fromStdString(child.GetValue());
        params[name] = value;
    }
}

bool VMTParser::isLoaded() const {
    return vmtFile_ != nullptr && vmtFile_->GetRoot() != nullptr;
}
