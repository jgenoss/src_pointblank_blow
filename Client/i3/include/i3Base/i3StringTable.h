#ifndef __I3_STRING_TABLE_H__
#define __I3_STRING_TABLE_H__

#include "i3ElementBase.h"

class i3Stream;

class I3_EXPORT_BASE i3StringTable : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3StringTable, i3ElementBase );
protected:
	#ifdef I3_COMPILER_VC
	#pragma pack( push, 1)
	#endif

	struct STRING_INFO
	{
		UINT32 LabelPos = 0;
		UINT32 StringPos = 0;
	} ;

	#ifdef I3_COMPILER_VC
	#pragma pack( pop)
	#endif

	STRING_INFO *m_pTable = nullptr;
	char	*m_pLabelBuf = nullptr;
	WCHAR16	*m_pStringBuf = nullptr;

	UINT32	m_nNumString = 0;
	UINT32	m_nTotalLabelSize = 0;
	UINT32	m_nTotalStringSize = 0;

protected:
	bool NewBuffers();

public:
	~i3StringTable();

	void DestroyBuffers();
	bool CreateFromStream( i3Stream *pStream );

	UINT32 GetCount(){ return m_nNumString;	}

	const char*	GetLabel( INT32 nIndex );

	const WCHAR16 *GetString( const char *szLabel );
	const WCHAR16 *GetString( INT32 nIndex );
	INT32	GetStringIndex( const char *szLabel );
};

#endif //__I3_STRING_TABLE_H__
