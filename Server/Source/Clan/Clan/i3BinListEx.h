#ifndef __I3BINLISTEX_H__
#define __I3BINLISTEX_H__

class i3BinListEx : public i3BinList
{
	I3_CLASS_DEFINE( i3BinListEx );

public:
	i3BinListEx();
	~i3BinListEx();

	void*				FindItemEx( UINT32 ui32Item );
	void*				DeleteEx( UINT32 ui32Item );
};

class i3BinListEx64 : public i3BinList
{
	I3_CLASS_DEFINE( i3BinListEx64 );

public:
	i3BinListEx64();
	~i3BinListEx64();

	void*				FindItemEx( INT64 i64Item );
	void*				DeleteEx( INT64 i64Item );
};

#endif