//*************************************************************************************
//	class name : i3OptRemoveVertexComponent
//	Vertex component 일부중(Normal, Color ... ) 원하는component를 제거 할수 있다.
//	by KOMET 2006.02.22
//*************************************************************************************
#if !defined( __I3_OPT_REMOVE_VERTEX_COMPONENT_H)
#define __I3_OPT_REMOVE_VERTEX_COMPONENT_H

#include "i3SceneOptimizer.h"
#include "i3OptGeometryBase.h"

struct OPT_REMOVE_INFO
{
	bool	_bRemoveNormal = false;			//	Normal 제거할지 여부 (default: false)
	bool	_bRemoveColor = false;			//	Color 제거할지 여부 (default: false)
	bool	_bRemoveTexcoord = false;		//	Texcoord 제거할지 여부 (default: false)	
	bool	_bRemoveTangent = false;
	bool	_bRemoveAllTexcoord = false;	// 기존 _bRemoveTexcoord는 두번째 텍스쳐UV만 제거한다..
} ;

class I3_EXPORT_OPT i3OptRemoveVertexComponent : public i3OptGeometryBase
{
	I3_EXPORT_CLASS_DEFINE( i3OptRemoveVertexComponent, i3OptGeometryBase);

protected:
	OPT_REMOVE_INFO	m_OptInfo;

public:
	i3OptRemoveVertexComponent(void);

public:

	bool	getRemoveNormal(void)				{	return m_OptInfo._bRemoveNormal;	}
	bool	getRemoveColor(void)				{	return m_OptInfo._bRemoveColor;	}
	bool	getRemoveTexcoord(void)				{	return m_OptInfo._bRemoveTexcoord;	}
	bool	getRemoveTangent(void)				{	return m_OptInfo._bRemoveTangent; }
	bool	getRemoveAllTexcoord(void)			{ return m_OptInfo._bRemoveAllTexcoord;  }

	void	SetRemoveNormal( bool bFlag)		{	m_OptInfo._bRemoveNormal = bFlag;	}
	void	SetRemoveColor( bool bFlag)			{	m_OptInfo._bRemoveColor = bFlag;	}
	void	SetRemoveTexcoord( bool bFlag)		{	m_OptInfo._bRemoveTexcoord = bFlag;	}
	void	SetRemoveTangent( bool bFlag)		{	m_OptInfo._bRemoveTangent = bFlag; }
	void	SetRemoveAllTexcoord(bool bFlag) { m_OptInfo._bRemoveAllTexcoord = bFlag; }

public:
	void				setDefaultInfo(void);
	i3VertexArray *		rebuildVertexArray( i3VertexArray * pSrcVA, i3VertexFormat * pFormat);
	void				copyVertexComponent( i3VertexArray * pDstVA, i3VertexArray * pSrcVA);

	virtual void	Trace( i3Node * pRoot) override;

	virtual const char * getDescName(void) override;
	virtual	bool		SaveToXML( i3XMLElement * pXML) override;
	virtual	bool		LoadFromXML( i3XMLElement * pXML) override;

};

#endif // __I3_OPT_REMOVE_VERTEX_COMPONENT_H


