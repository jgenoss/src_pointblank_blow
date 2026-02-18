#include "../gameShieldLib/shieldSecurity.h"

#include <iostream>
#include <string>
#include <windows.h>

#ifdef _DEBUG
#pragma comment(lib, "../gameShieldLib/debug/gameShieldLib.lib")
#else
#pragma comment(lib, "../gameShieldLib/release/gameShieldLib.lib")
#endif

int main()
{
    frostInitialize(".\\frost\\gameShieldDll.dll");
    std::cout << "frostInitialize() - ok" << std::endl; 
    while(true) {
        std::cout << "enter x to exit, or i to init again" << std::endl;
        char str[1024] = {0};
        std::cin >> str;
        if(std::string(str) == "x" ) {
            std::cout << "exiting, call frostFinalize()" << std::endl;
            frostFinalize();
            break;
        } else if(std::string(str) == "i") {
            frostFinalize();
            frostInitialize(".\\frost\\gameShieldDll.dll");
        }
    }
    return 0;
}

int main1()
{
    unsigned m_r[64] = {0};
    for (int  i = 0; i < 64; i += 4) {
        if (i < 16) {
            m_r[i + 0] =  7;
            m_r[i + 1] = 12;
            m_r[i + 2] = 17;
            m_r[i + 3] = 22;
        } else if (i < 32) {
            m_r[i + 0] =  5;
            m_r[i + 1] = 9;
            m_r[i + 2] = 14;
            m_r[i + 3] = 20;
        } else if (i < 48) {
            m_r[i + 0] =  4;
            m_r[i + 1] = 11;
            m_r[i + 2] = 16;
            m_r[i + 3] = 23;
        } else {
            m_r[i + 0] =  6;
            m_r[i + 1] = 10;
            m_r[i + 2] = 15;
            m_r[i + 3] = 21;
        }
    }
    for (int  ii = 0; ii < 64; ++ii) {
        std::cout << m_r[ii] << ", ";
        if((ii+1) % 8 == 0)std::cout << std::endl;
    }
	return 0;
}

