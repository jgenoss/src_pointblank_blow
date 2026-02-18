#if !defined ( I3_INPUT_METHOD_CONTEXT_H)
#define I3_INPUT_METHOD_CONTEXT_H

#if defined( I3_WINDOWS)


/* IME Header files */ 
#include <imm.h>
#include <objbase.h>
#include <dimm.h>

class i3Ime;
class I3_EXPORT_INPUT i3InputMethodContext : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3InputMethodContext, i3ElementBase);

public:
	i3InputMethodContext( void);
	virtual ~i3InputMethodContext( void);

	void	Create( void);
	bool	IsNT( void)			{ return m_bIsNT;				}

	INPUTCONTEXT *	LockIMC( HIMC hImc);
	bool			UnlockIMC( HIMC hImc);
	LPVOID			LockIMCC( HIMCC);
	bool			UnlockIMCC( HIMCC);

	void			SetCompositioning(i3Ime * pIme)		{ m_pIme = pIme;		}
	i3Ime		 *	GetCompositioning(void)				{ return m_pIme;		}
	bool			isVistaorLater(void)				{ return (m_dwMajorVersion >= 6);}

protected:

	HINSTANCE		m_hDllImm32 = nullptr;
	bool			m_bIsNT = false;	//	System version
	DWORD			m_dwMajorVersion = 0, m_dwMinorVersion = 0;

	INPUTCONTEXT*	(WINAPI * _ImmLockIMC)( HIMC ) = nullptr;
	bool			(WINAPI * _ImmUnlockIMC)( HIMC ) = nullptr;
	LPVOID			(WINAPI * _ImmLockIMCC)( HIMCC ) = nullptr;
	bool			(WINAPI * _ImmUnlockIMCC)( HIMCC ) = nullptr;

	i3Ime		*	m_pIme = nullptr;
};

#endif

#endif //I3_INPUT_IME_CONTEXT_H


