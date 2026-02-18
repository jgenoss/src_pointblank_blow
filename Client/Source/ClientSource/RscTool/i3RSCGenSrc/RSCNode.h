#ifndef _RSCNODE_H__
#define _RSCNODE_H__

#include "RSCGenDefine.h"


class RSCNode :	public i3NamedElement
{
	I3_CLASS_DEFINE( RSCNode, i3NamedElement);
public:
	RSCNode(void);
	virtual ~RSCNode(void);
	UINT32		Export( i3FileStream * pStream);

	RSCNode	*	GetParent(void)					{	return m_pParent;}
	void		SetParent( RSCNode * pParent)	{	m_pParent = pParent;}

	UINT32		GetStyle()						{	return m_Entry.m_Attr;}
	void		SetStyle(UINT32 nAttr)			{	m_Entry.m_Attr = nAttr;}
	void		AddStyle(UINT32 nAttr)			{	m_Entry.m_Attr |= nAttr;}
	void		RemoveStyle( UINT32 nAttr)		{	m_Entry.m_Attr &= ~nAttr;}

	const RSC_DATA *	GetData()	const					{	return &m_Entry;}
	RSC_DATA*			GetData()							{ return &m_Entry; }
	void		GetFullPath( const i3::const_char_range& work_path, i3::string& outFullPath);


	const i3::rc_string&	GetRelativePath() const { return m_Entry.m_strFilePath; }
	void					SetRelativePath(const i3::rc_string& relPath);



	void		AddChild( RSCNode * pChild);
	void		SetChild(INT32 idx, RSCNode* newChild);
	void		RemoveChild( RSCNode * pChild);
	void		RemoveAllChild( void);

	INT32		GetChildCount()					{	return (INT32)m_ChildList.size();					}
	INT32		FindChildIndex(RSCNode* child);

	RSCNode	*	GetChild( INT32 nIndex)			{	return m_ChildList[nIndex];	}
	void		GetAllChildCount(INT32 & nCount);

	RSCNode *	_Rec_FindChildByName( const char * pszName, const char * pszRelPath);
	RSCNode *	FindChildByName( const char * pszName);

	void		GatherAllNode( i3::vector<RSCNode*>& List);
	void		GatherAllNode_NonFolder( i3::vector<RSCNode*>& List);

	void		GatherAllNodeToPathMap( i3::unordered_map<i3::fixed_string, RSCNode*>& inout);
	void		AddThisToPathMap(i3::unordered_map<i3::fixed_string, RSCNode*>& inout);

	static i3::fixed_string		CalcKeyFixedString( const i3::rc_string& relPath);

protected:
	RSC_DATA				m_Entry;
	RSCNode		*			m_pParent;
	i3::vector<RSCNode*>	m_ChildList;

};

INT32	GetPriority( UINT32 nType);

struct RSCNode_ExportComp
{
	bool operator()(const RSCNode * p1, const RSCNode * p2) const 
	{
		UINT32 prior1 = GetPriority( p1->GetData()->m_Attr & I3RSC_TYPE_MASK);
		UINT32 prior2 = GetPriority( p2->GetData()->m_Attr & I3RSC_TYPE_MASK);
		return prior1 < prior2;					// <=는 쓰면 안된다.. < 변경..
	}
};


#endif