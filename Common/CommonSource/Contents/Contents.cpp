#include "pch.h"
#include "Contents.h"


//////////////////////////////////////////////////////////////////////////
// CContents
CContents::CContents()
: m_i32Version(0)
, m_bEnable(false)
{
}

CContents::~CContents()
{
}

void CContents::SetContents(INT32 i32Version, bool bEnable)
{
	m_i32Version	= i32Version;
	m_bEnable		= bEnable;
}

void CContents::CopyToPacket(ContentsInfo* pContents)
{
	pContents->m_i32Version	= m_i32Version;
	pContents->m_bEnable	= m_bEnable;
}



//////////////////////////////////////////////////////////////////////////
// CContentsMgr
CContentsMgr::CContentsMgr()
{
}

CContentsMgr::~CContentsMgr()
{
}

void CContentsMgr::InsertContents(CONTENTS eContents, INT32 i32Version, bool bEnable)
{
	if( CONTENTS_COUNT <= eContents )
		return;

	m_Contents[eContents].SetContents(i32Version, bEnable);
}

void CContentsMgr::GetContentsList(ContentsInfo* pPacket)
{
	for( INT32 i = 0; i < CONTENTS_COUNT; ++i )
		m_Contents[i].CopyToPacket( &pPacket[i] );
}

// 항상 On 상태여야 하는 Contents 확인
bool CContentsMgr::CheckDefaultContents()
{
	if( !IsContensEnabled(CONTENTS_CALCULATE_POINT) )
		return false;
	
	if( !IsContensEnabled(CONTENTS_REPARE_DURABILITY) )
		return false;
	
	if( !IsContensEnabled(CONTENTS_DEFAULT_WEAPON) )
		return false;

	if( !IsContensEnabled(CONTENTS_MULTI_SLOT) )
		return false; 

	return true;
}
