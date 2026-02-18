#pragma once

class CFieldShop: public i3ElementBase
{
	I3_CLASS_DEFINE(CFieldShop);

private:
	FieldShopInfo*		m_pFieldShopInfo;

public:
	// constructor / destructor
	CFieldShop();
	~CFieldShop();

	// external interface
	UINT32				GetVersion();	
	void				SetVersion( UINT32 ui32Version );

	FieldShopInfo*	GetFieldShopInfo() { return		m_pFieldShopInfo; } 	
};
