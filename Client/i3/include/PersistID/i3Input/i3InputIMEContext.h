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
	I3_CLASS_DEFINE( i3InputIMEContext);

public:
	i3InputIMEContext( void);
	virtual ~i3InputIMEContext( void);

	void	Create( void);
	BOOL	IsNT( void)			{ return m_bIsNT;						}

	INPUTCONTEXT *	LockIMC( HIMC hImc);
	BOOL			UnlockIMC( HIMC hImc);
	LPVOID			LockIMCC( HIMCC);
	BOOL			UnlockIMCC( HIMCC);

	void			SetCompositioning(i3InputIME * pIME)		{ m_pCompositionIME = pIME;		}
	i3InputIME *	GetCompositioning(void)						{ return m_pCompositionIME;		}

protected:

	HINSTANCE		m_hDllImm32;
	BOOL			m_bIsNT;	//	System version

	INPUTCONTEXT*	(WINAPI * _ImmLockIMC)( HIMC );
	BOOL			(WINAPI * _ImmUnlockIMC)( HIMC );
	LPVOID			(WINAPI * _ImmLockIMCC)( HIMCC );
	BOOL			(WINAPI * _ImmUnlockIMCC)( HIMCC );

	i3InputIME *	m_pCompositionIME;
};

#endif

#endif //I3_INPUT_IME_CONTEXT_H


