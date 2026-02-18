#ifndef __I3_STRING_TABLE_H__
#define __I3_STRING_TABLE_H__

#include "i3ElementBase.h"

class i3Stream;

class I3_EXPORT_BASE i3StringTable : public i3ElementBase
{
	I3_CLASS_DEFINE( i3StringTable );
protected:
	#ifdef I3_COMPILER_VC
	#pragma pack( push, 1)
	#endif

	typedef struct
	{
		UINT32 LabelPos;
		UINT32 StringPos;
	} STRING_INFO;

	#ifdef I3_COMPILER_VC
	#pragma pack( pop)
	#endif

	STRING_INFO *m_pTable;		
	char	*m_pLabelBuf;
	WCHAR16	*m_pStringBuf;	

	UINT32	m_nNumString;
	UINT32	m_nTotalLabelSize;
	UINT32	m_nTotalStringSize;

protected:
	BOOL NewBuffers();

public:
	i3StringTable();
	~i3StringTable();

	void DestroyBuffers();
	BOOL CreateFromStream( i3Stream *pStream );

	UINT32 GetCount(){ return m_nNumString;	}

	const char*	GetLabel( INT32 nIndex );

	const WCHAR16 *GetString( const char *szLabel );
	const WCHAR16 *GetString( INT32 nIndex );
	INT32	GetStringIndex( const char *szLabel );
};

#endif //__I3_STRING_TABLE_H__
