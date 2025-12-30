#ifndef VMTFILE_H
#define VMTFILE_H

#include <string>
#include <map>
#include <vector>

namespace VTFLib {

class VMTNode {
public:
    VMTNode(const std::string& name = "") : name_(name) {}
    
    void SetName(const std::string& name) { name_ = name; }
    std::string GetName() const { return name_; }
    
    void SetValue(const std::string& value) { value_ = value; }
    std::string GetValue() const { return value_; }
    
    void AddChild(const VMTNode& child) { children_.push_back(child); }
    const std::vector<VMTNode>& GetChildren() const { return children_; }
    
    VMTNode* FindChild(const std::string& name);
    
private:
    std::string name_;
    std::string value_;
    std::vector<VMTNode> children_;
};

class VMTFile {
public:
    VMTFile();
    ~VMTFile();
    
    bool Load(const std::string& filename);
    bool Parse(const std::string& content);
    
    VMTNode* GetRoot() { return &root_; }
    const VMTNode* GetRoot() const { return &root_; }
    
    std::string GetShader() const { return shader_; }
    std::string GetBaseTexture() const;
    
private:
    VMTNode root_;
    std::string shader_;
    
    void ParseNode(const std::string& content, size_t& pos, VMTNode& node);
    void SkipWhitespace(const std::string& content, size_t& pos);
    std::string ReadToken(const std::string& content, size_t& pos);
    std::string ReadQuotedString(const std::string& content, size_t& pos);
};

} // namespace VTFLib

#endif // VMTFILE_H
