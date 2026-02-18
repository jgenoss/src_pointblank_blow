#if !defined( __I3_TDK_OBJECT_BASE_H)
#define __I3_TDK_OBJECT_BASE_H

#include "i3Base.h"

// i3TDKObjectBase는 다른 Library Group에서 사용하는 다른 Object들을 Capsulate 하여 
// TDK Library 용으로 표현하기 위한 Base Class이다.
// TDK Library를 통해 편집되는 모든 Object들은 i3TDKObjectBase Class에서 파생된 Encapsulation Class로
// 표현되어야 한다.
class I3_EXPORT_TDK i3TDKObjectBase : public i3NamedElement
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3TDKObjectBase, i3NamedElement);
protected:
	i3TDKObjectBase *		m_pParent = nullptr;
	i3TDKObjectBase *		m_pNext = nullptr;
	i3TDKObjectBase *		m_pChild = nullptr;

public:
	i3TDKObjectBase(void) {}
	virtual ~i3TDKObjectBase(void);

	i3TDKObjectBase *	GetParent(void)							{ return m_pParent; }
	void				SetParent( i3TDKObjectBase * pParent)	{ m_pParent = pParent; }

#ifdef I3_DEBUG
	void			AddChild( i3TDKObjectBase *pChild, const char *szFile, INT32 nLine );
#endif
	void			AddChild( i3TDKObjectBase *pChild )		{ AddChildToTail( pChild ); }
	void			AddChildToHead( i3TDKObjectBase * pChild);
	void			AddChildToTail( i3TDKObjectBase * pChild);
	void			RemoveChild( i3TDKObjectBase * pChild);
	void			RemoveAllChild(void);
	i3TDKObjectBase *	FindChildByName( const char * pszName, BOOL bRecurs = TRUE);
	i3TDKObjectBase *	GetFirstChild(void)					{ return m_pChild; }
	i3TDKObjectBase *	GetChildByIndex( INT32 nIndex );

	i3TDKObjectBase *	GetNext(void)						{ return m_pNext; }
	void			SetNext( i3TDKObjectBase * pNext);

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);
};

#ifdef I3_DEBUG
#define I3_TDKOBJ_ADDCHILD( P, C )	(P)->AddChild( C, __FILE__, __LINE__ );
#else
#define I3_TDKOBJ_ADDCHILD( P, C )	(P)->AddChild( C );
#endif

#endif
