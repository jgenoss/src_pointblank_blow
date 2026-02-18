#ifndef GAME_SHIELD_LIB_SHIELDSECURITY_H
#define GAME_SHIELD_LIB_SHIELDSECURITY_H

// path to frost's dll to load, if not specified or not called
// then by default loads "gameShieldDll.dll" from ".\\frost" directory.
// use frostInitialize() for ascii or frostInitializeW() for unicode
void frostInitialize(const char* dllPath = 0);

void frostInitializeW(const wchar_t* dllPath = 0);

// call at the end of application, to free frost's resources
void frostFinalize();

// call to set userName
// use frostSetUserName() for ascii or frostSetUserNameW() for unicode
void frostSetUserName(const char* uin);
void frostSetUserNameW(const wchar_t* uin);

// call to set selected char's name
// use frostSetCharName() for ascii or frostSetCharNameW() for unicode
void frostSetCharName(const char* cName);
void frostSetCharNameW(const wchar_t* cName);

#endif //GAME_SHIELD_LIB_SHIELDSECURITY_H

