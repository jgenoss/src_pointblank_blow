#if !defined( __MVF_NODE_H)
#define __MVF_NODE_H

class MVFNode;

typedef void (*NODETRAVERSEPROC)( char * pszFull, MVFNode * pNode, void * pUserData);

typedef struct ALIGN4 _tagPersistMVFData
{
	UINT32	m_Attr;
	UINT32	m_Size;						// 압축되지 않은 상태의 크기

	union
	{
		struct
		{
			UINT32	m_CompressedSize;			// 압축 파일인 경우, 압축 Data의 크기, Folder의 경우
			UINT32	m_Offset;
		} file;

		struct
		{
			UINT32	m_StartChildIndex;
			UINT32	m_ChildCount;
		} dir;
	};
} MVF_DATA;

class MVFNode : public i3NamedElement
{
	I3_CLASS_DEFINE( MVFNode);
protected:
	MVF_DATA		m_Entry;

	MVFNode *		m_pParent;
	i3BinList		m_ChildList;

public:
	MVFNode(void);
	virtual ~MVFNode(void);


	MVFNode *	GetParent(void)					{ return m_pParent; }
	void		SetParent( MVFNode * pParent)	{ m_pParent = pParent; }

	void		AddChild( MVFNode * pChild);
	void		RemoveChild( MVFNode * pChild);
	void		RemoveAllChild(void);
	INT32		GetChildCount(void)				{ return m_ChildList.GetCount(); }
	MVFNode *	GetChild( INT32 idx)			{ return (MVFNode *) m_ChildList.Items[idx]; }
	MVFNode *	FindChildByName( const char * pszName);

	UINT32		GetStyle(void)			{ return m_Entry.m_Attr; }
	void		SetStyle( UINT32 attr)	{ m_Entry.m_Attr = attr; }
	void		AddStyle( UINT32 attr)	{ m_Entry.m_Attr |= attr; }
	void		RemoveStyle( UINT32 attr)	{ m_Entry.m_Attr &= ~attr; }

	UINT32		GetSize(void)			{ return m_Entry.m_Size; }
	void		SetSize( UINT32 sz)		{ m_Entry.m_Size = sz; }

	UINT32		GetOffset(void)			{ return m_Entry.file.m_Offset; }
	void		SetOffset( UINT32 off)	{ m_Entry.file.m_Offset = off; }

	void		GetEntry( I3_MVF_ENTRY * pDest)
	{
		pDest->m_Attr = m_Entry.m_Attr;
		pDest->m_Size = m_Entry.m_Size;
		pDest->dir.m_StartChildIndex = m_Entry.dir.m_StartChildIndex;
		pDest->dir.m_ChildCount = m_Entry.dir.m_ChildCount;
	}

	void		SetEntry( I3_MVF_ENTRY * pSrc)
	{
		m_Entry.m_Attr = pSrc->m_Attr;
		m_Entry.m_Size = pSrc->m_Size;
		m_Entry.dir.m_StartChildIndex = pSrc->dir.m_StartChildIndex;
		m_Entry.dir.m_ChildCount = pSrc->dir.m_ChildCount;
	}

	void		Traverse( char * pszFull, NODETRAVERSEPROC pProc, void * pUserData);

	void		GetFullPath( char * pszPath);
};

#endif
