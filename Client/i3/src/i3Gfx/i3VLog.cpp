#include "i3GfxType.h"
#include "i3VLog.h"
#include "i3VLogDefine.h"

static i3Udp *		s_pLogSocket = nullptr;
static UINT32		s_IP;


I3_EXPORT_GFX	
void	i3VLog::Init( const char * pszIP)
{
	s_pLogSocket = i3Udp::new_object();

	s_pLogSocket->CreateSocket();

	s_IP = inet_addr( pszIP);
}

I3_EXPORT_GFX
void	i3VLog::Finish(void)
{
	if( s_pLogSocket != nullptr)
	{
		s_pLogSocket->DeleteSocket();

		s_pLogSocket->Release();
		s_pLogSocket = nullptr;
	}
}

I3_EXPORT_GFX	
void	i3VLog::Begin( void)
{
	I3VLOG_PACK_BEGIN data;

	data.init();

	if( s_pLogSocket == nullptr)
	{
		Init( "127.0.0.1");
	}

	if( s_pLogSocket != nullptr)
	{
		s_pLogSocket->Send( (const char *) &data, sizeof(data), s_IP, I3VLOG_PORT);
	}
}

I3_EXPORT_GFX
void	i3VLog::End( void)
{
	I3VLOG_PACK_END data;

	data.init();

	if( s_pLogSocket != nullptr)
	{
		s_pLogSocket->Send( (const char *) &data, sizeof(data), s_IP, I3VLOG_PORT);
	}
}

I3_EXPORT_GFX	
void	i3VLog::SetColor( COLOR * pColor)
{
	I3VLOG_PACK_COLOR data;

	data.init();
	data.color = i3Color::ConvertARGB32( pColor);

	if( s_pLogSocket != nullptr)
	{
		s_pLogSocket->Send( (const char *) &data, sizeof(data), s_IP, I3VLOG_PORT);
	}
}

I3_EXPORT_GFX
void	i3VLog::SetColor( COLORREAL * pColor)
{
	I3VLOG_PACK_COLOR data;
	COLOR col;

	data.init();

	i3Color::Set( &col, pColor);
	data.color = i3Color::ConvertARGB32( &col);

	if( s_pLogSocket != nullptr)
	{
		s_pLogSocket->Send( (const char *) &data, sizeof(data), s_IP, I3VLOG_PORT);
	}
}

I3_EXPORT_GFX
void	i3VLog::SetColor( REAL32 r, REAL32 g, REAL32 b, REAL32 a)
{
	COLORREAL col;

	i3Color::Set( &col, r, g, b, a);

	SetColor( &col);
}

I3_EXPORT_GFX
void	i3VLog::SetColor( UINT8 r, UINT8 g, UINT8 b, UINT8 a)
{
	COLOR col;

	i3Color::Set( &col, r, g, b, a);

	SetColor( &col);
}

I3_EXPORT_GFX
void	i3VLog::SetRandomColor( UINT8 a)
{
	UINT8 r, g, b;

	r = static_cast<UINT8>( (i3Math::Rand() % 128) + 128 );
	g = static_cast<UINT8>( (i3Math::Rand() % 128) + 128 );
	b = static_cast<UINT8>( (i3Math::Rand() % 128) + 128 );
	
	SetColor( (UINT8) r, g, b, a);
}

I3_EXPORT_GFX
void	i3VLog::Point( VEC3D * p, VLOG_POINT_TYPE type, REAL32 sz)
{
	I3VLOG_PACK_POINT data;

	data.init();
	data.pos.x = p->x;
	data.pos.y = p->y;
	data.pos.z = p->z;
	data.point_type = (UINT16) type;
	data.scale = sz;

	if( s_pLogSocket != nullptr)
	{
		s_pLogSocket->Send( (const char *) &data, sizeof(data), s_IP, I3VLOG_PORT);
	}
}


I3_EXPORT_GFX
void	i3VLog::Line( VEC3D * p1, VEC3D * p2)
{
	I3VLOG_PACK_LINE data;

	data.init();
	data.start.x = p1->x;
	data.start.y = p1->y;
	data.start.z = p1->z;
	data.end.x = p2->x;
	data.end.y = p2->y;
	data.end.z = p2->z;

	if( s_pLogSocket != nullptr)
	{
		s_pLogSocket->Send( (const char *) &data, sizeof(data), s_IP, I3VLOG_PORT);
	}
}

I3_EXPORT_GFX
void	i3VLog::Tri( VEC3D * p1, VEC3D * p2, VEC3D * p3)
{
	I3VLOG_PACK_TRI data;

	data.init();
	data.p1.x = p1->x;	data.p1.y = p1->y;	data.p1.z = p1->z;
	data.p2.x = p2->x;	data.p2.y = p2->y;	data.p2.z = p2->z;
	data.p3.x = p3->x;	data.p3.y = p3->y;	data.p3.z = p3->z;

	if( s_pLogSocket != nullptr)
	{
		s_pLogSocket->Send( (const char *) &data, sizeof(data), s_IP, I3VLOG_PORT);
	}
}

I3_EXPORT_GFX
void	i3VLog::Draw( i3BoundBox * pBox)
{
	VEC3D pos[8];
	//INT32 i;

	pBox->GetCornerVertices( pos);

	Line( &pos[0], &pos[1]);
	Line( &pos[1], &pos[2]);
	Line( &pos[2], &pos[3]);
	Line( &pos[3], &pos[0]);
	
	Line( &pos[4], &pos[5]);
	Line( &pos[5], &pos[6]);
	Line( &pos[6], &pos[7]);
	Line( &pos[7], &pos[4]);

	Line( &pos[0], &pos[4]);
	Line( &pos[1], &pos[5]);
	Line( &pos[2], &pos[6]);
	Line( &pos[3], &pos[7]);
}

I3_EXPORT_GFX
void	i3VLog::Draw( i3Vector3List * pList)
{
	INT32 i;
	
	if( pList->getCount() <= 0)
		return;

	for( i = 1; i < pList->getCount(); i++)
	{
		Line( pList->GetVector( i -1), pList->GetVector( i));
	}
}

I3_EXPORT_GFX
void	i3VLog::Draw( i3PolygonList * pList)
{
	INT32 i;
	i3Vector3List * pVecList;

	for( i = 0; i < pList->getCount(); i++)
	{
		pVecList = pList->getPolygon( i);

		Draw( pVecList);
	}
}

I3_EXPORT_GFX
void	i3VLog::Plane( VEC4D * pPlane, REAL32 sz)
{
	VEC3D right, up, nright, nup, off;
	MATRIX mtx;

	i3Matrix::MakeWithAt( &mtx, (VEC3D *) pPlane, &I3_YAXIS);

	i3Vector::Copy( &right, i3Matrix::GetRight( &mtx));
	i3Vector::Normalize( &right, &right);

	i3Vector::Cross( &up, &right, (VEC3D *) pPlane);
	i3Vector::Normalize( &up, &up);

	i3Vector::Scale( &nright, &right, -sz);
	i3Vector::Scale( &right, &right, sz);

	i3Vector::Scale( &nup, &up, -sz);
	i3Vector::Scale( &up, &up, sz);

	VEC3D p1, p2, p3, p4;

	i3Vector::Add( &p1, &nright, &nup);
	i3Vector::Add( &p2, &right, &nup);
	i3Vector::Add( &p3, &right, &up);
	i3Vector::Add( &p4, &nright, &up);

	i3Vector::Scale( &off, (VEC3D *) pPlane, pPlane->w);

	i3Vector::Add( &p1, &p1, &off);
	i3Vector::Add( &p2, &p2, &off);
	i3Vector::Add( &p3, &p3, &off);
	i3Vector::Add( &p4, &p4, &off);

	Line( &p1, &p2);
	Line( &p2, &p3);
	Line( &p3, &p4);
	Line( &p4, &p1);

	// Normal
	i3Vector::Add( &p1, &off, (VEC3D*) pPlane);
	Line( &off, &p1);
}

