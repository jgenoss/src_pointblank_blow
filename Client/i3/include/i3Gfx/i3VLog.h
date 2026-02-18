#if !defined( __I3_VLOG_H)
#define __I3_VLOG_H

#define I3VLOG_PORT		5858

#include "i3VLogDefine.h"

enum VLOG_POINT_TYPE
{
	VLOG_POINT_TYPE_RECT = 0,
};

namespace i3VLog
{
	I3_EXPORT_GFX	void	Init( const char * pszIP = "127.0.0.1");
	I3_EXPORT_GFX	void	Finish(void);

	I3_EXPORT_GFX	void	Begin(void);
	I3_EXPORT_GFX	void	End(void);

	I3_EXPORT_GFX	void	SetColor( COLOR * pColor);
	I3_EXPORT_GFX	void	SetColor( COLORREAL * pColor);
	I3_EXPORT_GFX	void	SetColor( REAL32 r, REAL32 g, REAL32 b, REAL32 a);
	I3_EXPORT_GFX	void	SetColor( UINT8 r, UINT8 g, UINT8 b, UINT8 a);
	I3_EXPORT_GFX	void	SetRandomColor( UINT8 a = 255);

	I3_EXPORT_GFX	void	Point( VEC3D * p, VLOG_POINT_TYPE type = VLOG_POINT_TYPE_RECT, REAL32 sz = 1.0f);
	I3_EXPORT_GFX	void	Line( VEC3D * p1, VEC3D * p2);
	I3_EXPORT_GFX	void	Tri( VEC3D * p1, VEC3D * p2, VEC3D * p3);
	I3_EXPORT_GFX	void	Plane( VEC4D * pPlane, REAL32 sz);
	I3_EXPORT_GFX	void	Draw( i3BoundBox * pBox);
	I3_EXPORT_GFX	void	Draw( i3Vector3List * pList);
	I3_EXPORT_GFX	void	Draw( i3PolygonList * pList);
};

#endif
