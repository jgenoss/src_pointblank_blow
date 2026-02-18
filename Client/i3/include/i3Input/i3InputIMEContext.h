#if !defined ( I3_INPUT_IME_CONTEXT_H)
#define I3_INPUT_IME_CONTEXT_H

#if defined( I3_WINDOWS)


/* IME Header files */ 
#include <imm.h>
#include <objbase.h>
#include <dimm.h>

class i3InputIME;
class I3_EXPORT_INPUT i3InputIMEContext : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3InputIMEContext, i3ElementBase);

public:
	i3InputIMEContext( void);
	virtual ~i3InputIMEContext( void);

	void	Create( void);
	bool	IsNT( void)			{ return m_bIsNT;						}

	INPUTCONTEXT *	LockIMC( HIMC hImc);
	bool			UnlockIMC( HIMC hImc);
	LPVOID			LockIMCC( HIMCC);
	bool			UnlockIMCC( HIMCC);

	void			SetCompositioning(i3InputIME * pIME)		{ m_pCompositionIME = pIME;		}
	i3InputIME *	GetCompositioning(void)						{ return m_pCompositionIME;		}

	bool			isVistaorLater(void)						{ return (m_dwMajorVersion >= 6);}

protected:

	HINSTANCE		m_hDllImm32 = nullptr;
	bool			m_bIsNT = false;	//	System version
	DWORD			m_dwMajorVersion = 0, m_dwMinorVersion = 0;

	INPUTCONTEXT*	(WINAPI * _ImmLockIMC)( HIMC ) = nullptr;
	bool			(WINAPI * _ImmUnlockIMC)( HIMC ) = nullptr;
	LPVOID			(WINAPI * _ImmLockIMCC)( HIMCC ) = nullptr;
	bool			(WINAPI * _ImmUnlockIMCC)( HIMCC ) = nullptr;

	i3InputIME *	m_pCompositionIME = nullptr;
};

#endif

#endif //I3_INPUT_IME_CONTEXT_H


