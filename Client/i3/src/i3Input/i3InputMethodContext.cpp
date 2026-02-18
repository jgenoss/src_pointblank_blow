#include "i3InputType.h"
#include "i3InputMethodContext.h"

#if defined( I3_WINDOWS)

I3_CLASS_INSTANCE( i3InputMethodContext);

i3InputMethodContext::i3InputMethodContext( void)
{
	// Check System version 
	{
		OSVERSIONINFOEX osvi;

		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

		if( GetVersionEx ((OSVERSIONINFO *) &osvi))
		{
			m_bIsNT = (bool)(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT );

			m_dwMajorVersion = osvi.dwMajorVersion;
			m_dwMinorVersion = osvi.dwMinorVersion;
			// (vista) major = 6 minor = 0 (win7) major = 6 minor = 1
		}
	}
}

i3InputMethodContext::~i3InputMethodContext( void)
{
	if( m_hDllImm32 != nullptr)
	{
		FreeLibrary( m_hDllImm32);
		m_hDllImm32 = nullptr;
	}
}

void i3InputMethodContext::Create( void)
{
	char szPath[ MAX_PATH+1] = "";

	if( m_hDllImm32 == nullptr)
	{
		if( GetSystemDirectoryA( szPath, MAX_PATH+1 ) ) 
		{
			strncat( szPath, "\\imm32.dll", sizeof(szPath)-1 );
			m_hDllImm32 = LoadLibraryA( szPath );
			
			if( m_hDllImm32 != nullptr)
			{
				_ImmLockIMC		= (INPUTCONTEXT*(WINAPI *)( HIMC ))
					GetProcAddress( m_hDllImm32, "ImmLockIMC" );
				_ImmUnlockIMC	= (bool(WINAPI *)( HIMC ))
					GetProcAddress( m_hDllImm32, "ImmUnlockIMC" );
				_ImmLockIMCC	= (LPVOID(WINAPI *)( HIMCC ))
					GetProcAddress( m_hDllImm32, "ImmLockIMCC" );
				_ImmUnlockIMCC	= (bool(WINAPI *)( HIMCC ))
					GetProcAddress( m_hDllImm32, "ImmUnlockIMCC" );
			}
			else
			{
				I3ASSERT(m_hDllImm32 != nullptr);
			}
		}
	}
}

INPUTCONTEXT * i3InputMethodContext::LockIMC(HIMC hImc)
{
	if(_ImmLockIMC == nullptr)
	{
		return nullptr;
	} 
	else 
	{
		return _ImmLockIMC(hImc);
	}
}

bool i3InputMethodContext::UnlockIMC(HIMC hImc)
{
	if(_ImmUnlockIMC == nullptr)
	{
		return false;
	}
	else 
	{
		return _ImmUnlockIMC(hImc);
	}
}

LPVOID i3InputMethodContext::LockIMCC( HIMCC himcc )
{
	if(_ImmLockIMCC == nullptr)
	{
		return nullptr;
	}
	else 
	{
		return _ImmLockIMCC(himcc);
	}
}

bool i3InputMethodContext::UnlockIMCC( HIMCC himcc )
{
	if(_ImmUnlockIMCC == nullptr)
	{
		return false;
	}
	else 
	{
		return _ImmUnlockIMCC(himcc);
	}
}

#endif