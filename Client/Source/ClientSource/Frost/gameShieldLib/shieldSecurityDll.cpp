#include "pch.h"
#include "shieldSecurityDll.h"
#include <fstream>
#include <direct.h>

std::ofstream frostLogFile;

shieldSecurityDll* shieldSecurityDll::m_inst = 0;

inline unsigned char utf16_to_ascii(wchar_t w)
{
    if (0x47f == w) w = 0x448;
    if ((w < 0x450) && (0x40f < w)) {
        return (unsigned char)(w - 0x350);
    } else if(0x401 == w) {
        return (unsigned char)0xa8;
    } else if (0x451 == w) {
        return (unsigned char)0xb8;
    }
    return (unsigned char)w;
}

static void utf16_to_ascii(const wchar_t* wstr, std::string& str)
{
    str = "";
    size_t len = 0;
    if(wstr) {
        const wchar_t* u = wstr;
        while(*u) {
            ++len;
            ++u;
        }
    }
    str.reserve(len);
    for(size_t i=0; i<len; ++i) {
        str += (char)utf16_to_ascii(wstr[i]);
    }
}

static std::string utf16_to_ascii(const wchar_t* wstr)
{
    std::string str;
    utf16_to_ascii(wstr, str);
    return str;
}

std::string shieldSecurityDll::getDefaultPath()
{
    unsigned FilenameSize = 2048;
    char* lpFilename = new char[FilenameSize];
    GetModuleFileNameA(0, lpFilename, FilenameSize);
    std::string myModulePath(lpFilename);
    delete[] lpFilename;
    std::string::size_type i = myModulePath.find_last_of("\\/");
    if(std::string::npos != i) {
        myModulePath = myModulePath.substr(0, i);
    } else {
        myModulePath = ".";
    }
    myModulePath += "\\frost\\gameShieldDll.dll";
    return myModulePath;
}

static void open_log_file(const std::string& logDir)
{
    if(frostLogFile.is_open()) return;
    std::string fpath = logDir;
    std::string::size_type i = 0;
    while(i < logDir.size()) {
        i = logDir.find_first_of("\\/", i);
        if(std::string::npos == i) {
            _mkdir(logDir.c_str());
            break;
        }
        _mkdir(logDir.substr(0, i).c_str());
        ++i;
    }
    fpath += ".\\frostStatic.log";
    SetFileAttributesA(fpath.c_str(), FILE_ATTRIBUTE_NORMAL);
    DeleteFileA(fpath.c_str());
    frostLogFile.open(fpath.c_str(), std::ios::app);
}

shieldSecurityDll::shieldSecurityDll():
    m_pSetUserName(0),
    m_pSetCharName(0),
    m_pInitialize(0),
    m_pFinalize(0),
    m_pSetUserNameW(0),
    m_pSetCharNameW(0),
    m_lib(0)
{
}

shieldSecurityDll::~shieldSecurityDll()
{
    if(m_pFinalize) {
        m_pFinalize();
    }
}

void shieldSecurityDll::loadLibrary(const std::string& dllPath)
{
    m_lib = LoadLibraryA(dllPath.c_str());
    if(m_lib) {
        frostLogFile << "LoadLibraryA(" << dllPath << ") ok " << std::endl;

        m_pSetUserName  = (void (*)(const char*)) GetProcAddress(m_lib, "frostSetUserName");
        m_pSetCharName  = (void (*)(const char*)) GetProcAddress(m_lib, "frostSetCharName");
        m_pInitialize   = (void (*)(const char*)) GetProcAddress(m_lib, "frostInitialize");
        m_pFinalize     = (void (*)()) GetProcAddress(m_lib, "frostFinalize");
        m_pSetUserNameW = (void (*)(const wchar_t*)) GetProcAddress(m_lib, "frostSetUserNameW");
        m_pSetCharNameW = (void (*)(const wchar_t*)) GetProcAddress(m_lib, "frostSetCharNameW");

        frostLogFile << "frostSetUserName: "  << (DWORD)m_pSetUserName  << std::endl;
        frostLogFile << "frostSetCharName: "  << (DWORD)m_pSetCharName  << std::endl;
        frostLogFile << "frostInitialize: "   << (DWORD)m_pInitialize   << std::endl;
        frostLogFile << "frostFinalize: "     << (DWORD)m_pFinalize     << std::endl;
        frostLogFile << "frostSetUserNameW: " << (DWORD)m_pSetUserNameW << std::endl;
        frostLogFile << "frostSetCharNameW: " << (DWORD)m_pSetCharNameW << std::endl;
    } else {
        frostLogFile << "LoadLibraryA(" << dllPath << ") failed, err " << GetLastError() << std::endl;
    }
}

void shieldSecurityDll::init(const char* path0)
{
    std::string path = path0 ? std::string(path0) : getDefaultPath();
    std::string fpath(path);
    std::string::size_type i = fpath.find_last_of("\\/");
    if(i != std::string::npos) {
        fpath = fpath.substr(0, i);
    } else {
        fpath = "";
    }
    open_log_file(fpath);
    frostLogFile << "init(" << path << ")" << std::endl;
    if(!m_inst) {
        m_inst = new shieldSecurityDll();
        frostLogFile << "inst of shieldSecurityDll created" << std::endl;
        m_inst->loadLibrary(path);
        if(m_inst->m_pInitialize) {
            m_inst->m_pInitialize(path.c_str());
        }
    }
}

void shieldSecurityDll::init(const wchar_t* path)
{
    init(path ? utf16_to_ascii(path).c_str() : 0);
    frostLogFile << "initW() called" << std::endl;
}

void shieldSecurityDll::finish()
{
    frostLogFile << "finish() " << std::endl;
    delete m_inst;
    m_inst = 0;
}

void shieldSecurityDll::setUserName(const char* uin)
{
    frostLogFile << "setUserName(" << uin << ")  " << std::endl;
    if(m_inst && m_inst->m_pSetUserName) {
        m_inst->m_pSetUserName(uin);
    }
}

void shieldSecurityDll::setCharName(const char* cName)
{
    frostLogFile << "setCharName(" << cName << ")  " << std::endl;
    if(m_inst && m_inst->m_pSetCharName) {
        m_inst->m_pSetCharName(cName);
    }
}

void shieldSecurityDll::setUserName(const wchar_t* uin)
{
    frostLogFile << "setUserNameW(" << utf16_to_ascii(uin ? uin : L"").c_str() << ")  " << std::endl;
    if(m_inst) {
        if(m_inst->m_pSetUserNameW) {
            m_inst->m_pSetUserNameW(uin);
        } else if (m_inst->m_pSetUserName) {
            m_inst->m_pSetUserName((const char*)uin);
        }
    }
}

void shieldSecurityDll::setCharName(const wchar_t* cName)
{
    frostLogFile << "setCharNameW(" << utf16_to_ascii(cName ? cName : L"").c_str() << ")  " << std::endl;
    if(m_inst) {
        if(m_inst->m_pSetCharNameW) {
            m_inst->m_pSetCharNameW(cName);
        } else if(m_inst->m_pSetCharName) {
            m_inst->m_pSetCharName((const char*)cName);
        }
    }
}

