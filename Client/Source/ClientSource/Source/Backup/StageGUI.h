#if !defined( __STAGE_GUI_H)
#define __STAGE_GUI_H

#if JuuL_0

#include "StageDef.h"
#include "GuiNotifyReceiver.h"

class CMessageBox;


typedef		void	(*ErrorMsgHandler)(void* pThis,INT32 nParam);


class CStageGUI : public i3Stage
{
	I3_ABSTRACT_CLASS_DEFINE( CStageGUI, i3Stage);

public:
	CStageGUI(void);
	virtual ~CStageGUI(void);

	void	SetRoot( i3GuiRoot * pRoot);
	i3GuiRoot * getRoot()	{	return m_pGUIRoot;}

	//
	void	OnCreate();
	void	Create();
	void	Destroy( void);

	//
	BOOL	LoadGUIFile( const char * szGUIFileName);

	// Control Notify를 받아 해결하기위해 모든 하위 클래스는 이 함수를 오버라이드해야한다.
	// 자신이 담당하는 Control Notify에 대한 Handler 함수를 호출한다.
	// 만약 자신이 담당하지 않는 Control의 경우 상위 클래스의 OnControlNotify()에 전임한다.
	virtual void OnControlNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual void OnPreControlNotify(void);
	virtual void OnPostControlNotify(void);
	virtual void OnExitKey(void);	

	virtual void OnCheckAwayInput(REAL32 rDeltaSeconds);

	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);

	void	SetAwayInputTime(REAL32 rAwayInputTime,REAL32 rCautionTime);
	BOOL	isAwayInput(void)								{ return m_bOffAwayInput ? FALSE : m_rAwayInputTime <= 0.0f;	}
	BOOL	isCaution(void)									{ return m_bOffCaution ? FALSE : m_rCautionTime <= 0.0f;		}
	void	setOffAwayInput(void)							{ m_bOffAwayInput = TRUE;	}
	void	setOffCaution(void)								{ m_bOffCaution = TRUE;		}
	BOOL	getOffAwayInput(void)							{ return m_bOffAwayInput;	}
	
protected:
	virtual void OnLoadEnd();
	virtual void OnUpdate( REAL32 rDeltaSeconds);

	virtual void _ExitKey(void);
	
private:
	// GuiRoot에서 Notify를 받아와 OnControlNotify()를 호출한다
	INT32	_ControlNotifyPump(void);	

public:	
	void DriveAway(INT32 nReason);
	static void CbExitNormal(void* pThis,INT32 nParam);
	static void CbEmptyCautionBox(void* pThis,INT32 nParam);	// 확인버튼 있는 WaitBox의 포인터 초기화용

protected:
	i3GuiRoot * m_pGUIRoot;
	char		m_szUIFileName[MAX_PATH];
	
	REAL32		m_rAwayInputTime;
	REAL32		m_rCautionTime;
	BOOL		m_bOffAwayInput;
	BOOL		m_bOffCaution;
	CMessageBox*	m_pCautionMessageBox;
};

#endif

#endif
