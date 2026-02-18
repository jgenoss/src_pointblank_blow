#if !defined( __I3_AI_H)
#define __I3_AI_H

class i3AI;

#include "i3GameNode.h"
#include "i3AIState.h"
#include "i3AIDriver.h"

class i3Chara;

bool operator<(const i3AIState& a1, const i3AIState& a2);
template<> struct i3::less<i3AIState*> : i3::less_ptr<i3AIState*> {};


class I3_EXPORT_FRAMEWORK i3AI : public i3ResourceObject
{
	I3_EXPORT_CLASS_DEFINE( i3AI, i3ResourceObject);
protected:
	// Persistant members
	UINT32				m_Style = 0;
	i3::vector_multiset<i3AIState*>	m_AIStateList;		//	(i3AIState *)

	i3::rc_string		m_strLuaPath;
	i3::rc_string		m_strDriverMetaName;

	// Volatile members
	i3AIDriver *		m_pDriver = nullptr;

public:
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
	INT32			FindAIState( i3AIState * pState)	
	{ 
		size_t idx = i3::vu::index_of(m_AIStateList.get_vector(), pState);
		return (idx != m_AIStateList.size() ) ? (INT32)idx : -1;
	}

	void			RemoveAIStateAll( void);
	void			RemoveAIStateFromId( INT32 id);
	void			RemoveAIStateFromIndex( INT32 index);
	void			RemoveAIState( i3AIState * pAIState);

	void			Sort(void);

	INT32			getAIStateCount()					{	return (INT32)m_AIStateList.size();					}	
	i3AIState *		getAIState( INT32 index)
	{
		if( index > -1 && (UINT32)index < m_AIStateList.size())
			return m_AIStateList.get_vector()[index];
		return nullptr;
	}

	const char *	getLuaSourceName(void) const		{	return m_strLuaPath.c_str(); }
	void			setLuaSourceName( const char * pszPath);

	const char *	getDriverMetaName(void) const		{ return m_strDriverMetaName.c_str(); }
	void			setDriverMetaName( const char * pszPath);

	i3AIDriver *	getAIDriver(void)					{ return m_pDriver; }
	void			setAIDriver( i3AIDriver * pDriver)	{ I3_REF_CHANGE( m_pDriver, pDriver); }

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif
