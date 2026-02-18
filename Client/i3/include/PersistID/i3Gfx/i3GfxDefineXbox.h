#if !defined( __I3G_DEFINE_XB_H)
#define __I3G_DEFINE_XB_H

#if !defined(I3_XBOX) || !defined(I3G_XBOX)
#error "This header file is only for Xbox."
#endif

#include <xtl.h>


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
	I3G_PRIM_TRIFAN			=	D3DPT_TRIANGLEFAN
};

////////////////////////////////////////////////////////////////////////////////////////
// Lock Method
typedef DWORD				 I3G_LOCKMETHOD;

#define	I3G_LOCK_NONE				0

#define I3G_LOCK_OVERWRITE			0

	// This flag prevents the automatic flushing of the pre-transformation and
	// lighting vertex cache. The reason to prevent the flushing is that the
	// title may need to append more vertices onto the end of the vertex buffer
	// using D3DLOCK_NOOVERWRITE. The GPU reads from the vertex buffer 32 bytes
	// at a time. Its possible that the previous vertex information did not end
	// on a 32 byte boundary, in which case, care must be taken to ensure that
	// the appended vertex information is aligned on the next 32 byte boundary
	// or unexpected behavior may result.
#define I3G_LOCK_NOFLUSH			D3DLOCK_NOFLUSH

	// The title promises to not write to the vertex buffer
	// This enables resources stored in non-native formats
	// to save the recompression step when unlocking. 
#define I3G_LOCK_READONLY			D3DLOCK_READONLY

	// The title promises to not overwrite any existing vertex.
#define I3G_LOCK_NOOVERWRITE		D3DLOCK_NOOVERWRITE

////////////////////////////////////////////////////////////////////////////////////////
// Matrix Index
enum I3G_MATRIX_STACK
{
	// Identifies the transformation matrix being set as the view transformation matrix.
	// The default value is NULL (the identity matrix). 
	I3G_MATRIX_VIEW			=	D3DTS_VIEW,

	// Identifies the transformation matrix being set as the projection transformation matrix.
	// The default value is NULL (the identity matrix). 
	I3G_MATRIX_PROJECTION	=	D3DTS_PROJECTION,

	// Identifies the transformation matrix being set for the specified texture stage. 
	I3G_MATRIX_TEXTURE0		=	D3DTS_TEXTURE0,
	I3G_MATRIX_TEXTURE1		=	D3DTS_TEXTURE1,
	I3G_MATRIX_TEXTURE2		=	D3DTS_TEXTURE2,
	I3G_MATRIX_TEXTURE3		=	D3DTS_TEXTURE3,
	// I3G_MATRIX_TEXTURE4		=	D3DTS_TEXTURE4,
	// I3G_MATRIX_TEXTURE5		=	D3DTS_TEXTURE5,
	// I3G_MATRIX_TEXTURE6		=	D3DTS_TEXTURE6,
	// I3G_MATRIX_TEXTURE7		=	D3DTS_TEXTURE7,

	I3G_MATRIX_WORLD			=	D3DTS_WORLD,
	I3G_MATRIX_WORLD1			=	D3DTS_WORLD1,
	I3G_MATRIX_WORLD2			=	D3DTS_WORLD2,
	I3G_MATRIX_WORLD3			=	D3DTS_WORLD3,
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
#define I3G_USAGE_SOFTWAREPROCESSING	0
#define I3G_USAGE_WRITEONLY				D3DUSAGE_WRITEONLY
#define I3G_USAGE_POINT_SPRITES			D3DUSAGE_POINTS
#define I3G_USAGE_RTPATCHES				D3DUSAGE_RTPATCHES
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
	I3G_TEXTURE_WRAP_MIRROR			=	D3DTADDRESS_MIRROR,
	I3G_TEXTURE_WRAP_CLAMP			=	D3DTADDRESS_CLAMP,
	I3G_TEXTURE_WRAP_BORDER			=	D3DTADDRESS_BORDER,
	I3G_TEXTURE_WRAP_CLAMTOEDGE		=	D3DTADDRESS_CLAMPTOEDGE, // Xbox extension
	// I3G_TEXTURE_WRAP_MIRRORONCE		=	D3DTADDRESS_MIRRORONCE,
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
	I3G_TEXTURE_CORRD_SOURCE_CAMERA_SPACE_POSITION		=	D3DTSS_TCI_CAMERASPACEPOSITION,
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


#endif