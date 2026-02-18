#ifndef GAME_SHIELD_LIB_SHIELDSECURITYDLL_H
#define GAME_SHIELD_LIB_SHIELDSECURITYDLL_H

#include <windows.h>
#include <string>

class shieldSecurityDll
{
public:
    static void init(const char* dllPath);

    static void finish();

    static void setUserName(const char*);

    static void setCharName(const char*);

    static void init(const wchar_t* dllPath);

    static void setUserName(const wchar_t*);

    static void setCharName(const wchar_t*);

public:
    ~shieldSecurityDll();

private:
    shieldSecurityDll();

    void loadLibrary(const std::string& dllPath);

    static std::string getDefaultPath();

private:
    static shieldSecurityDll* m_inst;

private:
    HMODULE m_lib;
    void (*m_pSetUserName)(const char*);
    void (*m_pSetCharName)(const char*);
    void (*m_pInitialize)(const char*);
    void (*m_pFinalize)();
    void (*m_pSetUserNameW)(const wchar_t*);
    void (*m_pSetCharNameW)(const wchar_t*);

private:
    shieldSecurityDll(const shieldSecurityDll&);
    shieldSecurityDll& operator=(const shieldSecurityDll&);
};

#endif //GAME_SHIELD_LIB_SHIELDSECURITYDLL_H

