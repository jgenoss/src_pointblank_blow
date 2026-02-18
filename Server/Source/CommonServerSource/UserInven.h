#ifndef __USER_INVEN_H__
#define __USER_INVEN_H__

class CUserInven
{
	INVEN_BUFFER			m_Inven[MAX_INVEN_COUNT];

public:
	CUserInven();
	~CUserInven();

	BOOL					InsertItem( INVEN_BUFFER* pInvenItem );
	BOOL					DeleteItem( T_ItemDBIdx& TItemDBIdx );
};

#endif