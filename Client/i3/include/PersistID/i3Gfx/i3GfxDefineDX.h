#if !defined( __I3G_DEFINE_DX_H)
#define __I3G_DEFINE_DX_H

#if !defined(I3_WINDOWS) || !defined(I3G_DX)
#error "This header file is only for DirectX and Windows."
#endif

#include <d3d9.h>
#include <ddraw.h>
#include <dxerr9.h>
#include <d3dx9shader.h>
// <dshow.h>의 <windowsx.h> 부분의 Define 부분이 다른 타 함수들과 너무 많은 충동을 일으켜 우선 Defne 로 막아놨습니다. 20070530 김현우..
#define  _INC_WINDOWSX
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <dshow.h>


////////////////////////////////////////////////////////////////////////////////////////
// Primitive Types
enum I3G_PRIMITIVE
{
	I3G_PRIM_NONE			=	0,
	I3G_PRIM_POINTLIST		=	D3DPT_POINTLIST,
	I3G_PRIM_LINELIST		=	D3DPT_LINELIST,
	I3G_PRIM_LINESTRIP		=	D3DPT_LINESTRIP,
	I3G_PRIM_TRILIST		=	D3DPT_TRIANGLELIST,
	I3G_PRIM_TRISTRIP		=	D3DPT_TRIANGLESTRIP,
	I3G_PRIM_TRIFAN			=	D3DPT_TRIANGLEFAN,
	I3G_PRIM_POINTSPRITE	=	D3DPT_POINTLIST,
};

////////////////////////////////////////////////////////////////////////////////////////
// Lock Method
typedef DWORD				 I3G_LOCKMETHOD;

#define	I3G_LOCK_NONE				0

#define I3G_LOCK_DONOTWAIT			D3DLOCK_DONOTWAIT

	//
	// The application overwrites, with a write-only operation, 
	// every location within the region being locked. 
	// This is a valid flag only when using dynamic textures.
#define I3G_LOCK_OVERWRITE			D3DLOCK_DISCARD

	// The application will not write to the buffer.
	// This enables resources stored in non-native formats
	// to save the recompression step when unlocking. 
#define I3G_LOCK_READONLY			D3DLOCK_READONLY

	// By default, a lock on a resource adds a dirty region to that resource.
	// This flag prevents any changes to the dirty state of the resource.
	// Applications should use this flag when they have additional information 
	// about the actual set of regions changed during the lock operation. 
#define I3G_LOCK_NO_DIRTY_UPDATE	D3DLOCK_NO_DIRTY_UPDATE

	// The default behavior of a video memory lock is to reserve a system-wide
	// critical section, guaranteeing that no display mode changes will occur
	// for the duration of the lock.
	// This flag causes the system-wide critical section not to be held
	// for the duration of the lock. 
	// The lock operation is slightly more expensive,
	// but can enable the system to perform other duties,
	// such as moving the mouse cursor.
	// This flag is useful for long-duration locks, such as the lock
	// of the back buffer for software rendering that would otherwise
	// adversely affect system responsiveness. 
#define I3G_LOCK_NOSYSLOCK			D3DLOCK_NOSYSLOCK

#define I3G_LOCK_NOOVERWRITE		D3DLOCK_NOOVERWRITE


////////////////////////////////////////////////////////////////////////////////////////
// Matrix Index
enum I3G_MATRIX_STACK
{
	// Identifies the transformation matrix being set as the view transformation matrix.
	// The default value is NULL (the identity matrix). 
	I3G_MATRIX_VIEW		=	D3DTS_VIEW,

	// Identifies the transformation matrix being set as the projection transformation matrix.
	// The default value is NULL (the identity matrix). 
	I3G_MATRIX_PROJECTION	=	D3DTS_PROJECTION,

	// Identifies the transformation matrix being set for the specified texture stage. 
	I3G_MATRIX_TEXTURE0		=	D3DTS_TEXTURE0,
	I3G_MATRIX_TEXTURE1		=	D3DTS_TEXTURE1,
	I3G_MATRIX_TEXTURE2		=	D3DTS_TEXTURE2,
	I3G_MATRIX_TEXTURE3		=	D3DTS_TEXTURE3,
	I3G_MATRIX_TEXTURE4		=	D3DTS_TEXTURE4,
	I3G_MATRIX_TEXTURE5		=	D3DTS_TEXTURE5,
	I3G_MATRIX_TEXTURE6		=	D3DTS_TEXTURE6,
	I3G_MATRIX_TEXTURE7		=	D3DTS_TEXTURE7,

	I3G_MATRIX_WORLD			=	D3DTS_WORLD
};

///////////////////////////////////////////////////////////////////////////////////////
// Clear Flags
typedef DWORD					I3G_CLEAR;

#define	I3G_CLEAR_COLOR		D3DCLEAR_TARGET
#define I3G_CLEAR_DEPTH		D3DCLEAR_ZBUFFER
#define I3G_CLEAR_STENCIL	D3DCLEAR_STENCIL

////////////////////////////////////////////////////////////////////////////////////////
// Usage Flags
typedef DWORD					I3G_USAGE;

#define	I3G_USAGE_NONE					0
#define	I3G_USAGE_FREQUENT_UPDATE		D3DUSAGE_DYNAMIC
#define I3G_USAGE_SOFTWAREPROCESSING	D3DUSAGE_SOFTWAREPROCESSING
#define I3G_USAGE_WRITEONLY				D3DUSAGE_WRITEONLY
#define I3G_USAGE_POINT_SPRITES			D3DUSAGE_POINTS
#define	I3G_USAGE_RENDERTARGET			D3DUSAGE_RENDERTARGET

////////////////////////////////////////////////////////////////////////////////////////
// Resource Type
enum I3G_RESOURCE_TYPE
{
	I3G_RESOURCE_TYPE_NONE					= 0,
	I3G_RESOURCE_TYPE_RENDERTARGET			= D3DRTYPE_SURFACE,
	I3G_RESOURCE_TYPE_TEXTURE				= D3DRTYPE_TEXTURE,
	I3G_RESOURCE_TYPE_VOLUME					= D3DRTYPE_VOLUME,
	I3G_RESOURCE_TYPE_VOLUMETEXTURE			= D3DRTYPE_VOLUMETEXTURE,
	I3G_RESOURCE_TYPE_CUBETEXTURE			= D3DRTYPE_CUBETEXTURE,
	I3G_RESOURCE_TYPE_VERTEXARRAY			= D3DRTYPE_VERTEXBUFFER,
	I3G_RESOURCE_TYPE_INDEXARRAY				= D3DRTYPE_INDEXBUFFER
};

///////////////////////////////////////////////////////////////////////////////////////
// Texture Wrap Mode
enum I3G_TEXTURE_WRAP
{
	I3G_TEXTURE_WRAP_WRAP			=	D3DTADDRESS_WRAP,
	I3G_TEXTURE_WRAP_CLAMP			=	D3DTADDRESS_CLAMP,
	I3G_TEXTURE_WRAP_BORDER			=	D3DTADDRESS_BORDER
};

enum	I3G_TEXTURE_FILTER
{
	I3G_TEXTURE_FILTER_NONE					= 0,
	I3G_TEXTURE_FILTER_POINT				= 1,
	I3G_TEXTURE_FILTER_LINEAR				= 2,
	I3G_TEXTURE_FILTER_POINT_MIPMAP_POINT	= 3,
	I3G_TEXTURE_FILTER_POINT_MIPMAP_LINEAR	= 4,
	I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT	= 5,
	I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR = 6,
	I3G_TEXTURE_FILTER_ANISOTROPIC			= 7,
};

enum	I3G_MIPMAP
{
	I3G_MIPMAP_DISABLE						=	0,
	I3G_MIPMAP_AUTO_GEN,
	I3G_MIPMAP_LOAD,
};

enum I3G_TEXTURE_COORD_SOURCE
{
	I3G_TEXTURE_COORD_SOURCE_VERTEX						=	D3DTSS_TCI_PASSTHRU,
	I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_POSITION		=	D3DTSS_TCI_CAMERASPACEPOSITION,
	I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_NORMAL		=	D3DTSS_TCI_CAMERASPACENORMAL,
	I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_REFLECTION	=	D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR,
};

enum I3G_TEXTURE_FUNCTION
{
	I3G_TEXTURE_FUNC_REPLACE							=	0,
	I3G_TEXTURE_FUNC_MODULATE							=	1,
	I3G_TEXTURE_FUNC_ADD								=	2,
	I3G_TEXTURE_FUNC_SPECULAR_HIGHLIGHT					=	3,
};

enum I3G_CUBE_TEXTURE_FACE
{
	I3G_CUBE_TEXTURE_FACE_PX							=	D3DCUBEMAP_FACE_POSITIVE_X,
	I3G_CUBE_TEXTURE_FACE_NX							=	D3DCUBEMAP_FACE_NEGATIVE_X,
	I3G_CUBE_TEXTURE_FACE_PY							=	D3DCUBEMAP_FACE_POSITIVE_Y,
	I3G_CUBE_TEXTURE_FACE_NY							=	D3DCUBEMAP_FACE_NEGATIVE_Y,
	I3G_CUBE_TEXTURE_FACE_PZ							=	D3DCUBEMAP_FACE_POSITIVE_Z,
	I3G_CUBE_TEXTURE_FACE_NZ							=	D3DCUBEMAP_FACE_NEGATIVE_Z,
};

///////////////////////////////////////////////////////////////////////////////////////
// Light type
enum	I3G_LIGHT_TYPE
{
	I3G_LIGHT_NONE							=	0,
	I3G_LIGHT_DIRECTIONAL					=	D3DLIGHT_DIRECTIONAL,
	I3G_LIGHT_SPOT							=	D3DLIGHT_SPOT,
	I3G_LIGHT_POINT							=	D3DLIGHT_POINT,
};


#ifdef I3_DEBUG
#define CHECKRESULT( a)\
{\
	HRESULT __Rv;\
	\
	__Rv = (a);\
	if( FAILED( __Rv))\
	{\
		char conv[256];\
		\
		sprintf( conv, "%s(%d)", __FILE__, __LINE__);\
		I3FATAL( "NAME:%s\nDESC:%s\n", DXGetErrorString( __Rv), DXGetErrorDescription( __Rv));\
	}\
}
#else
#define CHECKRESULT(a)			(a)
#endif

#endif