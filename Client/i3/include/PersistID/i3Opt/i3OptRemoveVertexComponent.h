//*************************************************************************************
//	class name : i3OptRemoveVertexComponent
//	Vertex component 일부중(Normal, Color ... ) 원하는component를 제거 할수 있다.
//	by KOMET 2006.02.22
//*************************************************************************************
#if !defined( __I3_OPT_REMOVE_VERTEX_COMPONENT_H)
#define __I3_OPT_REMOVE_VERTEX_COMPONENT_H

#include "i3OptPCH.h"
#include "i3SceneOptimizer.h"

typedef struct _tagOptRemoveInfo
{
	BOOL	_bRemoveNormal;			//	Normal 제거할지 여부 (default: FALSE)
	BOOL	_bRemoveColor;			//	Color 제거할지 여부 (default: FALSE)
	BOOL	_bRemoveTexcoord;		//	Texcoord 제거할지 여부 (default: FALSE)	

} OPT_REMOVE_INFO;

class I3_EXPORT_OPT i3OptRemoveVertexComponent : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptRemoveVertexComponent);

protected:
	OPT_REMOVE_INFO	m_OptInfo;

public:
	i3OptRemoveVertexComponent(void);
	virtual ~i3OptRemoveVertexComponent(void);

	virtual BOOL OnNode( i3Node * pNode); 
	
	virtual void SetProperty( char * pszFieldName, char * pszValue);

public:
	BOOL	RemoveComponent( i3Node * pNode);
	
	void	SetRemoveNormal( BOOL bFlag)		{	m_OptInfo._bRemoveNormal = bFlag;	}
	void	SetRemoveColor( BOOL bFlag)			{	m_OptInfo._bRemoveColor = bFlag;	}
	void	SetRemoveTexcoord( BOOL bFlag)		{	m_OptInfo._bRemoveTexcoord = bFlag;	}

protected:
	void	setDefaultInfo(void);
	BOOL	removeNormal( i3GeometryAttr * pGeoAttr);
	BOOL	removeColor( i3GeometryAttr * pGeoAttr);
	BOOL	removeTextureCoord( i3GeometryAttr * pGeoAttr);
	BOOL	rebuildVertexArray( i3GeometryAttr * pGeoAttr);
	void	copyVertexComponent( i3VertexArray * pDstVA, i3VertexArray * pSrcVA);

};

#endif // __I3_OPT_REMOVE_VERTEX_COMPONENT_H


