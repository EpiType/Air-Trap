/*
** SimpleJsonParser.hpp
** Minimal JSON parser for weapon configs (no external dependencies)
*/
#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <cctype>

namespace rtp::config {

class SimpleJson {
public:
    std::unordered_map<std::string, std::string> data;

    bool parse(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        return parseObject(content);
    }

    bool parseContent(const std::string& jsonContent) {
        return parseObject(jsonContent);
    }

    std::string getString(const std::string& key, const std::string& defaultVal = "") const {
        auto it = data.find(key);
        return (it != data.end()) ? it->second : defaultVal;
    }

    int getInt(const std::string& key, int defaultVal = 0) const {
        auto it = data.find(key);
        if (it == data.end()) return defaultVal;
        try {
            return std::stoi(it->second);
        } catch (...) {
            return defaultVal;
        }
    }

    float getFloat(const std::string& key, float defaultVal = 0.0f) const {
        auto it = data.find(key);
        if (it == data.end()) return defaultVal;
        try {
            return std::stof(it->second);
        } catch (...) {
            return defaultVal;
        }
    }

    bool getBool(const std::string& key, bool defaultVal = false) const {
        auto it = data.find(key);
        if (it == data.end()) return defaultVal;
        return it->second == "true" || it->second == "1";
    }

private:
    bool parseObject(const std::string& content) {
        size_t pos = 0;
        skipWhitespace(content, pos);
        
        if (pos >= content.size() || content[pos] != '{') return false;
        pos++; // skip '{'

        while (pos < content.size()) {
            skipWhitespace(content, pos);
            if (pos >= content.size()) break;
            if (content[pos] == '}') break;

            // Parse key
            std::string key = parseString(content, pos);
            if (key.empty()) break;

            skipWhitespace(content, pos);
            if (pos >= content.size() || content[pos] != ':') break;
            pos++; // skip ':'

            skipWhitespace(content, pos);
            
            // Parse value (can be string, number, bool, or nested object)
            if (content[pos] == '{') {
                // Nested object - store raw content for later parsing
                std::string nestedContent = extractNestedObject(content, pos);
                data[key] = nestedContent;
            } else {
                std::string value = parseValue(content, pos);
                data[key] = value;
            }

            skipWhitespace(content, pos);
            if (pos < content.size() && content[pos] == ',') pos++;
        }

        return true;
    }

    void skipWhitespace(const std::string& str, size_t& pos) {
        while (pos < str.size() && std::isspace(str[pos])) pos++;
    }

    std::string parseString(const std::string& str, size_t& pos) {
        if (pos >= str.size() || str[pos] != '"') return "";
        pos++; // skip opening quote

        std::string result;
        while (pos < str.size() && str[pos] != '"') {
            if (str[pos] == '\\' && pos + 1 < str.size()) {
                pos++;
            }
            result += str[pos++];
        }
        
        if (pos < str.size()) pos++; // skip closing quote
        return result;
    }

    std::string parseValue(const std::string& str, size_t& pos) {
        if (pos >= str.size()) return "";

        if (str[pos] == '"') {
            return parseString(str, pos);
        }

        // Parse number or boolean
        std::string result;
        while (pos < str.size() && str[pos] != ',' && str[pos] != '}' && str[pos] != ']') {
            if (!std::isspace(str[pos])) {
                result += str[pos];
            }
            pos++;
        }
        return result;
    }

    std::string extractNestedObject(const std::string& str, size_t& pos) {
        if (pos >= str.size() || str[pos] != '{') return "";
        
        size_t start = pos;
        int braceCount = 0;
        
        while (pos < str.size()) {
            if (str[pos] == '{') braceCount++;
            else if (str[pos] == '}') {
                braceCount--;
                if (braceCount == 0) {
                    pos++;
                    return str.substr(start, pos - start);
                }
            }
            pos++;
        }
        
        return "";
    }

public:
    const std::unordered_map<std::string, std::string>& getData() const { return data; }
};

// Helper to parse nested weapon objects from weapons.json
inline std::unordered_map<std::string, SimpleJson> parseWeaponsFile(const std::string& filepath = "config/common/weapons.json") {
    std::unordered_map<std::string, SimpleJson> weapons;
    
    std::ifstream file(filepath);
    if (!file.is_open()) return weapons;

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    size_t pos = 0;
    while (pos < content.size() && std::isspace(content[pos])) pos++;
    if (pos >= content.size() || content[pos] != '{') return weapons;
    pos++;

    while (pos < content.size()) {
        while (pos < content.size() && std::isspace(content[pos])) pos++;
        if (pos >= content.size() || content[pos] == '}') break;

        // Parse weapon name (key)
        if (content[pos] != '"') break;
        pos++;
        std::string weaponName;
        while (pos < content.size() && content[pos] != '"') {
            weaponName += content[pos++];
        }
        if (pos < content.size()) pos++; // skip closing quote

        while (pos < content.size() && std::isspace(content[pos])) pos++;
        if (pos >= content.size() || content[pos] != ':') break;
        pos++;

        while (pos < content.size() && std::isspace(content[pos])) pos++;
        
        // Extract weapon object
        if (content[pos] == '{') {
            size_t objStart = pos;
            int braceCount = 0;
            while (pos < content.size()) {
                if (content[pos] == '{') braceCount++;
                else if (content[pos] == '}') {
                    braceCount--;
                    if (braceCount == 0) {
                        pos++;
                        std::string weaponContent = content.substr(objStart, pos - objStart);
                        SimpleJson weaponData;
                        weaponData.parseContent(weaponContent);
                        weapons[weaponName] = weaponData;
                        break;
                    }
                }
                pos++;
            }
        }

        while (pos < content.size() && std::isspace(content[pos])) pos++;
        if (pos < content.size() && content[pos] == ',') pos++;
    }

    return weapons;
}

} // namespace rtp::config
