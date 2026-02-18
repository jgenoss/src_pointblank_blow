#if !defined( __UI_SLOT_INVENCHARA_H__)
#define __UI_SLOT_INVENCHARA_H__

#include "UI_V11/UISlotInvenBase_V11.h"

class UISlotInvenChara_V11 : public UISlotInvenBase_V11
{
	I3_CLASS_DEFINE( UISlotInvenChara_V11, UISlotInvenBase_V11);

private:
	INT32			m_nTeam;
	INT32			m_nUseTerm;
	INT32			m_CharaID;

	bool			m_bNotEquip;

public:
	INT32			getTeam( void)					{ return m_nTeam; }
	void			setTeam( INT32 val)				{ m_nTeam = val; }

	INT32			getUseTerm( void)				{ return m_nUseTerm; }
	void			setUseTerm( INT32 val)			{ m_nUseTerm = val; }

	INT32			getCharaID( void)				{ return m_CharaID; }
	void			setCharaID( INT32 val)			{ m_CharaID = val; }

public:
	UISlotInvenChara_V11();
	virtual ~UISlotInvenChara_V11();

	virtual bool	OnSelect( bool bSound = true);
	void			SetNotEquip( bool bVal);
};

#endif
