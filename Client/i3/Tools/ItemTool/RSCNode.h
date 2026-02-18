#ifndef _RSCNODE_H__
#define _RSCNODE_H__

#include "RSCGenDefine.h"


class RSCNode :	public i3NamedElement
{
	I3_CLASS_DEFINE( RSCNode, i3NamedElement);

protected:
	RSC_DATA			m_Entry;

	RSCNode		*		m_pParent;
//	BinList			m_ChildList;			// BinList일 이유가 없어보인다...일반 리스트로 바꾼다..(비교함수도 없었음..)
	i3::vector<RSCNode*>	m_ChildList;
	UINT128				m_MD5;

public:
	RSCNode(void);
	virtual ~RSCNode(void);
	UINT32		Export( i3FileStream * pStream);

	const UINT128&		getMD5(void) const		{ return m_MD5; }

	RSCNode	*	GetParent(void)					{	return m_pParent;}
	void		SetParent( RSCNode * pParent)	{	m_pParent = pParent;}

	UINT32		GetStyle()						{	return m_Entry.m_Attr;}
	void		SetStyle(UINT32 nAttr)			{	m_Entry.m_Attr = nAttr;}
	void		AddStyle(UINT32 nAttr)			{	m_Entry.m_Attr |= nAttr;}
	void		RemoveStyle( UINT32 nAttr)		{	m_Entry.m_Attr &= ~nAttr;}

	const RSC_DATA *	GetData()	const					{	return &m_Entry;}
	RSC_DATA*			GetData()							{ return &m_Entry; }
	void		GetFullPath( char * pszPath);

	char*		GetRelativePath()				{	return m_Entry.m_szFilePath;}
	void		SetRelativePath( char * pszPath);

	void		AddChild( RSCNode * pChild);
	void		RemoveChild( RSCNode * pChild);
	void		RemoveAllChild( void);

	INT32		GetChildCount()					{	return (INT32)m_ChildList.size();					}
	RSCNode	*	GetChild( INT32 nIndex)			{	return m_ChildList[nIndex];	}
	void		GetAllChildCount(INT32 & nCount);

	RSCNode *	_Rec_FindChildByName( const char * pszName, const char * pszRelPath);
	RSCNode *	FindChildByName( const char * pszName);

	void		ScanAllNode( i3::vector_set<RSCNode*, struct RSCNode_NodeComp>& List);

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

struct RSCNode_NodeComp
{
	bool operator()(const RSCNode* p1, const RSCNode* p2) const { return p1->getMD5() < p2->getMD5();   }
	bool operator()(const RSCNode* p1, const UINT128& c2) const { return p1->getMD5() < c2;   }
	bool operator()(const UINT128& c1, const RSCNode* p2) const { return c1 < p2->getMD5();   }
};

INT32	GetPriority( UINT32 nType);

struct RSCNode_ExportComp
{
	bool operator()(const RSCNode * p1, const RSCNode * p2) const 
	{
		UINT32 prior1 = GetPriority( p1->GetData()->m_Attr & I3RSC_TYPE_MASK);
		UINT32 prior2 = GetPriority( p2->GetData()->m_Attr & I3RSC_TYPE_MASK);
		return prior1 <= prior2;
	}
};


#endif