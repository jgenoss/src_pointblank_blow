#if !defined( _I3_GAME_PATH_H_)
#define _I3_GAME_PATH_H_

#include "i3GameObj.h"

class i3PathPoint;

typedef struct _tagi3PathTransform
{
	VEC3D	m_vPoint;
	REAL32	m_rRadius;
	REAL32	m_rTheta;
	UINT32	pad[1];
}I3_PATH_TRANSFORM;

typedef	struct	_tagi3PathLinkInfo
{
	i3PathPoint	*	m_pNext;
	UINT32			m_nExpense;
	UINT32			pad[2];
}I3_PATH_LINKINFO;

class I3_EXPORT_FRAMEWORK i3PathPoint : public i3NamedElement
{
	I3_CLASS_DEFINE( i3PathPoint);

protected:
	I3_PATH_TRANSFORM	m_Transform;
	i3List				m_linkToNextList;

public:
	i3PathPoint(void);
	virtual ~i3PathPoint(void);

public:
	void	setPosition( VEC3D	* pVec)	{	i3Vector::Copy( &m_Transform.m_vPoint, pVec);}
	VEC3D*	getPosition(void)			{	return &m_Transform.m_vPoint;}

	void	setRadius( REAL32	rRadius){	m_Transform.m_rRadius = rRadius;}
	REAL32	getRadius(void)				{	return m_Transform.m_rRadius;}

	void	setTheta( REAL32	rTheta) {	m_Transform.m_rTheta = rTheta;}
	REAL32	getTheta( void)				{	return m_Transform.m_rTheta;}

public:
	INT32	getNextLinkCount(void)					{	return m_linkToNextList.GetCount();}
	INT32	addNextLink( I3_PATH_LINKINFO * pLink)	{	return m_linkToNextList.Add( pLink);}

	i3List *	getNextPointList(void)				{	return &m_linkToNextList;}
	I3_PATH_LINKINFO * getNextLinkInfo(INT32 nIdx)
	{
		if( nIdx < getNextLinkCount())
		{
			return (I3_PATH_LINKINFO *)m_linkToNextList.Items[nIdx];
		}

		return NULL;
	}

public:
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

class I3_EXPORT_FRAMEWORK i3GamePath : public i3GameObj
{
	I3_CLASS_DEFINE( i3GamePath);

protected:
	i3List	m_PathPointList;

public:
	i3GamePath(void);
	virtual ~i3GamePath( void);

	INT32	getPathPointCount(void)				{	return m_PathPointList.GetCount();}
	INT32	addPathPoint( i3PathPoint * pPoint)	{	return m_PathPointList.Add( pPoint);}
	i3PathPoint * getPathPoint(INT32 nIdx)
	{
		if( nIdx < getPathPointCount())
		{
			return (i3PathPoint *)m_PathPointList.Items[nIdx];
		}

		return NULL;
	}

	i3List	*	getPathPointList(void)			{	return &m_PathPointList;}

public:
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif