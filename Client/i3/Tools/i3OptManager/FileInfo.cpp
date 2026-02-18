#include "stdafx.h"
#include "FileInfo.h"

WarnInfo::WarnInfo(void)
{
	m_pWarn = NULL;
}

WarnInfo::~WarnInfo(void)
{
}

void WarnInfo::CopyMsgList( i3StringList * pMsgList)
{
	INT32 i;

	for( i = 0; i < pMsgList->GetCount(); i++)
	{
		m_MsgList.Add( pMsgList->Items[i]);
	}
}

/////////////////////////////////////////////////////////////////////
I3_CLASS_INSTANCE( CFileInfo, i3ElementBase);

CFileInfo::CFileInfo(void)
{
}

CFileInfo::~CFileInfo(void)
{
	_RemoveAllWarn();
}

void CFileInfo::_RemoveAllWarn(void)
{
	INT32 i;
	WarnInfo * pInfo;

	for( i = 0; i < m_WarnList.GetCount(); i++)
	{
		pInfo = (WarnInfo *) m_WarnList.Items[i];

		delete pInfo;
	}

	m_WarnList.Clear();
}

void CFileInfo::addWarn( Warn * pWarn, i3StringList * pMsgList)
{
	WarnInfo * pInfo;

	pInfo = new WarnInfo;

	pInfo->setWarn( pWarn);
	pInfo->CopyMsgList( pMsgList);

	m_WarnList.Add( pInfo);
}


