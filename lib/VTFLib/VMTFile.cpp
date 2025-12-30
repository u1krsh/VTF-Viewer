#include "VMTFile.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>

namespace VTFLib {

VMTNode* VMTNode::FindChild(const std::string& name) {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    for (auto& child : children_) {
        std::string childName = child.GetName();
        std::transform(childName.begin(), childName.end(), childName.begin(), ::tolower);
        
        if (childName == lowerName) {
            return &child;
        }
    }
    return nullptr;
}

VMTFile::VMTFile() {
}

VMTFile::~VMTFile() {
}

bool VMTFile::Load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return Parse(buffer.str());
}

bool VMTFile::Parse(const std::string& content) {
    size_t pos = 0;
    SkipWhitespace(content, pos);
    
    // Read shader name
    shader_ = ReadToken(content, pos);
    if (shader_.empty()) {
        return false;
    }
    
    SkipWhitespace(content, pos);
    
    // Expect opening brace
    if (pos >= content.length() || content[pos] != '{') {
        return false;
    }
    pos++;
    
    // Parse root node
    root_.SetName(shader_);
    ParseNode(content, pos, root_);
    
    return true;
}

void VMTFile::SkipWhitespace(const std::string& content, size_t& pos) {
    while (pos < content.length()) {
        if (std::isspace(content[pos])) {
            pos++;
        } else if (content[pos] == '/' && pos + 1 < content.length() && content[pos + 1] == '/') {
            // Skip single-line comment
            while (pos < content.length() && content[pos] != '\n') {
                pos++;
            }
        } else {
            break;
        }
    }
}

std::string VMTFile::ReadToken(const std::string& content, size_t& pos) {
    SkipWhitespace(content, pos);
    
    if (pos >= content.length()) {
        return "";
    }
    
    if (content[pos] == '"') {
        return ReadQuotedString(content, pos);
    }
    
    std::string token;
    while (pos < content.length() && !std::isspace(content[pos]) && 
           content[pos] != '{' && content[pos] != '}') {
        token += content[pos++];
    }
    
    return token;
}

std::string VMTFile::ReadQuotedString(const std::string& content, size_t& pos) {
    if (pos >= content.length() || content[pos] != '"') {
        return "";
    }
    
    pos++; // Skip opening quote
    std::string str;
    
    while (pos < content.length() && content[pos] != '"') {
        if (content[pos] == '\\' && pos + 1 < content.length()) {
            pos++; // Skip escape character
        }
        str += content[pos++];
    }
    
    if (pos < content.length()) {
        pos++; // Skip closing quote
    }
    
    return str;
}

void VMTFile::ParseNode(const std::string& content, size_t& pos, VMTNode& node) {
    while (pos < content.length()) {
        SkipWhitespace(content, pos);
        
        if (pos >= content.length()) {
            break;
        }
        
        if (content[pos] == '}') {
            pos++;
            break;
        }
        
        // Read parameter name
        std::string name = ReadToken(content, pos);
        if (name.empty()) {
            break;
        }
        
        SkipWhitespace(content, pos);
        
        // Check if it's a nested node
        if (pos < content.length() && content[pos] == '{') {
            pos++;
            VMTNode child(name);
            ParseNode(content, pos, child);
            node.AddChild(child);
        } else {
            // Read parameter value
            std::string value = ReadToken(content, pos);
            VMTNode child(name);
            child.SetValue(value);
            node.AddChild(child);
        }
    }
}

std::string VMTFile::GetBaseTexture() const {
    VMTNode* baseTexNode = const_cast<VMTNode*>(&root_)->FindChild("$basetexture");
    if (baseTexNode) {
        return baseTexNode->GetValue();
    }
    return "";
}

} // namespace VTFLib
