#pragma once
#include <windows.h>
#include <string>
#include <sstream>

class IniFile {
private:
    std::string filePath;

public:
    // Constructor que recibe la ruta del archivo
    explicit IniFile(const std::string& path) : filePath(path) {}

    // Escritura Genérica
    template <typename T>
    void WriteValue(const std::string& section, const std::string& key, const T& value) {
        std::stringstream ss;
        ss << value;
        WritePrivateProfileStringA(section.c_str(), key.c_str(), ss.str().c_str(), filePath.c_str());
    }

    // Lectura Genérica
    template <typename T>
    T ReadValue(const std::string& section, const std::string& key, const T& defaultValue) {
        char buffer[255];
        std::stringstream ss;
        
        // Obtenemos el valor como string desde el archivo
        GetPrivateProfileStringA(section.c_str(), key.c_str(), "", buffer, 255, filePath.c_str());
        
        std::string result(buffer);
        if (result.empty()) {
            return defaultValue;
        }

        // Convertimos el string al tipo T solicitado
        T val;
        std::stringstream converter(result);
        if (!(converter >> val)) {
            return defaultValue;
        }
        return val;
    }

    // Especialización para std::string (evita problemas con espacios)
    std::string ReadString(const std::string& section, const std::string& key, const std::string& defaultValue = "") {
        char buffer[1024];
        GetPrivateProfileStringA(section.c_str(), key.c_str(), defaultValue.c_str(), buffer, 1024, filePath.c_str());
        return std::string(buffer);
    }
};