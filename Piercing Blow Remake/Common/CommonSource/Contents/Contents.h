#pragma once

#include "ContentsDefine.h"


//////////////////////////////////////////////////////////////////////////
// CContents
class CContents
{
public:
	bool				IsEnabled()															{	return m_bEnable;		}
	INT32				GetVersion()														{	return m_i32Version;	}

	void				SetContents(INT32 i32Version, bool bEnable);
	void				CopyToPacket(ContentsInfo* pContents);


public:
	CContents();
	virtual ~CContents();


private:
	INT32				m_i32Version;
	bool				m_bEnable;
};



//////////////////////////////////////////////////////////////////////////
// CContentsMgr
class CContentsMgr
{
public:
	bool				IsContensEnabled(CONTENTS eContents)								{ return m_Contents[eContents].IsEnabled(); }
	INT32				GetContensVersion(CONTENTS eContents)								{ return m_Contents[eContents].GetVersion(); }
	bool				CheckDefaultContents();

	void				InsertContents(CONTENTS eContents, INT32 i32Version, bool bEnable);
	void				GetContentsList(ContentsInfo* pPacket);


public:
	CContentsMgr();
	virtual ~CContentsMgr();


private:
	CContents			m_Contents[CONTENTS_COUNT];
};
