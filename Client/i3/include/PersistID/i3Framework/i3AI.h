#if !defined( __I3_AI_H)
#define __I3_AI_H

class i3AI;

#include "i3GameNode.h"
#include "i3AIState.h"
#include "i3AIDriver.h"

class i3Chara;

class I3_EXPORT_FRAMEWORK i3AI : public i3ResourceObject
{
	I3_CLASS_DEFINE( i3AI);
protected:
	// Persistant members
	UINT32				m_Style;
	i3BinList			m_AIStateList;		//	(i3AIState *)

	char *				m_pszLuaPath;
	char *				m_pszDriverMetaName;

	// Volatile members
	i3AIDriver *		m_pDriver;

public:
	i3AI(void);
	virtual ~i3AI(void);

	UINT32			getStyle(void)						{ return m_Style; }
	void			setStyle( UINT32 style)				{ m_Style = style; }
	void			addStyle( UINT32 style)				{ m_Style |= style; }
	void			removeStyle( UINT32 style)			{ m_Style &= ~style; }
	bool			isStyle( UINT32 style)				{ return (m_Style & style) != 0; }

	//	AI State	by KOMET
	INT32			AddAIState( i3AIState * pAIState);
	INT32			FindAIStateByName( const char * szName);
	INT32			FindAIStateByID( INT32 id);
	INT32			FindAIState( i3AIState * pState)	{ return m_AIStateList.IndexOf( pState); }
	void			RemoveAIStateAll( void);
	void			RemoveAIStateFromId( INT32 id);
	void			RemoveAIStateFromIndex( INT32 index);
	void			RemoveAIState( i3AIState * pAIState);

	void			Sort(void);

	INT32			getAIStateCount()					{	return m_AIStateList.GetCount();					}	
	i3AIState *		getAIState( INT32 index)			{	return (i3AIState *) m_AIStateList.GetItem( index);	}

	char *			getLuaSourceName(void)				{	return m_pszLuaPath; }
	void			setLuaSourceName( const char * pszPath);

	char *			getDriverMetaName(void)				{ return m_pszDriverMetaName; }
	void			setDriverMetaName( const char * pszPath);

	i3AIDriver *	getAIDriver(void)					{ return m_pDriver; }
	void			setAIDriver( i3AIDriver * pDriver)	{ I3_REF_CHANGE( m_pDriver, pDriver); }

	virtual void	OnBuildObjectList( i3List * pList);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
