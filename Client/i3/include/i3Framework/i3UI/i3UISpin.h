#ifndef __I3UI_SPIN_H__
#define __I3UI_SPIN_H__

#include "i3UIControl.h"
#include "i3UITemplate_Spin.h"

class i3UIButton;
class i3UIEditBox;

class I3_EXPORT_FRAMEWORK i3UISpin : public i3UIControl
{
	I3_EXPORT_CLASS_DEFINE( i3UISpin, i3UIControl);

protected:
	REAL32			m_fValueMin = 0.0f;
	REAL32			m_fValueMax = 100.0f;
	REAL32			m_fValueTick = 1.0f;	// Prev, Next 버튼을 눌렀을 때 이동할 단위
	REAL32			m_fCurValue = 0.0f;
	
	i3::rc_string	m_strBuddyName;
	i3UIControl*	m_pBuddyControl = nullptr;
	bool			m_bAutoWrap = false;
	bool			m_bAutoUpdateBuddy = false;

	i3UIButton *	m_pBtnUp = nullptr;
	i3UIButton *	m_pBtnDown = nullptr;

protected:
	virtual void	_UpdateShapeState(void) override;

	void			_OnBtnUpPushed( void);
	void			_OnBtnDownPushed( void);
	
	void			_UpdateToBuddy( void);
	
	void			_UpdateComponents(void);
public:
	virtual ~i3UISpin(); 

	void			InitBuddy( void);

	void			getSpinRange( REAL32 * pMin, REAL32 * pMax)
	{
		* pMin = m_fValueMin;
		* pMax = m_fValueMax;
	}

	void			setSpinRange( REAL32 minValue, REAL32 maxValue)
	{
		m_fValueMin = minValue;
		m_fValueMax = maxValue;
	}

	REAL32			getSpinPos(void)							{ return m_fCurValue; }
	void			setSpinPos( REAL32 pos);

	REAL32			getTickSize( void)							{	return m_fValueTick;	}
	void			setTickSize( REAL32 val)					{	m_fValueTick = val;		}
	
	virtual void	OnEnabled(bool bEnable) override;
	virtual void	OnMove( REAL32 x, REAL32 y) override;
	virtual void	OnSize( REAL32 cx, REAL32 cy) override;

	virtual void	OnBindTemplate(void) override;
	virtual void	OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2) override;
	
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method ) override;

#if defined( I3_DEBUG)
	virtual void	GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List) override;
	virtual void	OnTDKPropertyChanged( const char* szName) override;
#endif

	static	void	RegisterLuaFunction(LuaState * pState);
};

#endif //__I3UI_SPIN_H__
