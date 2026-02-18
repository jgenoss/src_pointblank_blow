#include "pch.h"
#include "shieldSecurity.h"
#include "shieldSecurityDll.h"

void frostInitialize(const char* dllPath)
{
    shieldSecurityDll::init(dllPath);
}

void frostFinalize()
{
    shieldSecurityDll::finish();
}

void frostSetUserName(const char* uin)
{
    shieldSecurityDll::setUserName(uin);
}

void frostSetCharName(const char* uin)
{
    shieldSecurityDll::setCharName(uin);
}

void frostInitializeW(const wchar_t* dllPath)
{
    shieldSecurityDll::init(dllPath);
}

void frostSetUserName(const wchar_t* uin)
{
    shieldSecurityDll::setUserName(uin);
}

void frostSetCharName(const wchar_t* uin)
{
    shieldSecurityDll::setCharName(uin);
}

