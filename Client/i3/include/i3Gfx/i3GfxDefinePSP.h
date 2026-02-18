#if !defined( __I3G_DEFINE_PSP_H )
#define __I3G_DEFINE_PSP_H

#if !defined( I3G_PSP )
#error "This header file is only for SONY PSP."
#endif

#include "i3Base.h"
#include <psptypes.h>
#include <libgu.h>

////////////////////////////////////////////////////////////////////////////////////////
// Primitive Types
enum I3G_PRIMITIVE
{
	I3G_PRIM_NONE			=	-1,
	I3G_PRIM_POINTLIST		=	0,	// SCEGU_PRIM_POINTS,
	I3G_PRIM_LINELIST		=	1,	// SCEGU_PRIM_LINES,
	I3G_PRIM_LINESTRIP		=	2,	// SCEGU_PRIM_LINE_STRIP,
	I3G_PRIM_TRILIST		=	3,	// SCEGU_PRIM_TRIANGLES,
	I3G_PRIM_TRISTRIP		=	4,	// SCEGU_PRIM_TRIANGLE_STRIP,
	I3G_PRIM_TRIFAN			=	5,	// SCEGU_PRIM_TRIANGLE_FAN,
	I3G_PRIM_POINTSPRITE	=	6,	// SCEGU_PRIM_RECTANGLES,
	I3G_PRIM_KEEP			=	7,	// AP2 명령어에서 쓰기 위한 것.
};

////////////////////////////////////////////////////////////////////////////////////////
// Lock Method
typedef UINT32				 I3G_LOCKMETHOD;

#define	I3G_LOCK_NONE				0

	//
	// The application overwrites, with a write-only operation, 
	// every location within the region being locked. 
	// This is a valid flag only when using dynamic textures.
#define I3G_LOCK_OVERWRITE			0x00000001

	// The application will not write to the buffer.
	// This enables resources stored in non-native formats
	// to save the recompression step when unlocking. 
#define I3G_LOCK_READONLY			0x00000002

	// By default, a lock on a resource adds a dirty region to that resource.
	// This flag prevents any changes to the dirty state of the resource.
	// Applications should use this flag when they have additional information 
	// about the actual set of regions changed during the lock operation. 
#define I3G_LOCK_NO_DIRTY_UPDATE	0x00000004

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
#define I3G_LOCK_NOSYSLOCK			0x00000008

#define I3G_LOCK_NOOVERWRITE		0x00000010


////////////////////////////////////////////////////////////////////////////////////////
// Matrix Index
enum I3G_MATRIX_STACK
{
	// Identifies the transformation matrix being set as the view transformation matrix.
	// The default value is NULL ( the identity matrix ). 
	I3G_MATRIX_VIEW		=	SCEGU_MATRIX_VIEW,

	// Identifies the transformation matrix being set as the projection transformation matrix.
	// The default value is NULL ( the identity matrix ). 
	I3G_MATRIX_PROJECTION	=	SCEGU_MATRIX_PROJECTION,

	// Identifies the transformation matrix being set for the specified texture stage. 
	I3G_MATRIX_TEXTURE0		=	SCEGU_MATRIX_TEXTURE,
	I3G_MATRIX_TEXTURE1		=	SCEGU_MATRIX_TEXTURE,
	I3G_MATRIX_TEXTURE2		=	SCEGU_MATRIX_TEXTURE,
	I3G_MATRIX_TEXTURE3		=	SCEGU_MATRIX_TEXTURE,
	I3G_MATRIX_TEXTURE4		=	SCEGU_MATRIX_TEXTURE,
	I3G_MATRIX_TEXTURE5		=	SCEGU_MATRIX_TEXTURE,
	I3G_MATRIX_TEXTURE6		=	SCEGU_MATRIX_TEXTURE,
	I3G_MATRIX_TEXTURE7		=	SCEGU_MATRIX_TEXTURE,

	I3G_MATRIX_WORLD			=	SCEGU_MATRIX_WORLD
};

///////////////////////////////////////////////////////////////////////////////////////
// Clear Flags
typedef UINT32					I3G_CLEAR;

#define	I3G_CLEAR_COLOR			SCEGU_CLEAR_COLOR
#define I3G_CLEAR_DEPTH			SCEGU_CLEAR_DEPTH
#define I3G_CLEAR_STENCIL		SCEGU_CLEAR_STENCIL

////////////////////////////////////////////////////////////////////////////////////////
// Usage Flags
typedef UINT32					I3G_USAGE;

#define	I3G_USAGE_NONE					0
#define I3G_USAGE_FREQUENT_UPDATE		0x00000001
#define I3G_USAGE_SOFTWAREPROCESSING	0x00000002
#define I3G_USAGE_WRITEONLY				0x00000004
#define I3G_USAGE_POINT_SPRITES			0x00000008
#define I3G_USAGE_RENDERTARGET			0x00000010

////////////////////////////////////////////////////////////////////////////////////////
// Resource Type
enum I3G_RESOURCE_TYPE
{
	I3G_RESOURCE_TYPE_NONE					= 0,
	I3G_RESOURCE_TYPE_RENDERTARGET,
	I3G_RESOURCE_TYPE_TEXTURE,
	I3G_RESOURCE_TYPE_VOLUME,
	I3G_RESOURCE_TYPE_VOLUMETEXTURE,
	I3G_RESOURCE_TYPE_CUBETEXTURE,
	I3G_RESOURCE_TYPE_VERTEXARRAY,
	I3G_RESOURCE_TYPE_INDEXARRAY,
};

///////////////////////////////////////////////////////////////////////////////////////
// Texture Wrap Mode
enum I3G_TEXTURE_WRAP
{
	I3G_TEXTURE_WRAP_WRAP			=	SCEGU_REPEAT,
	I3G_TEXTURE_WRAP_CLAMP			=	SCEGU_CLAMP,
};

enum	I3G_TEXTURE_FILTER
{
	I3G_TEXTURE_FILTER_NONE					= -1,
	I3G_TEXTURE_FILTER_POINT				= SCEGU_NEAREST,
	I3G_TEXTURE_FILTER_LINEAR				= SCEGU_LINEAR,
	I3G_TEXTURE_FILTER_POINT_MIPMAP_POINT	= SCEGU_NEAREST_MIPMAP_NEAREST,
	I3G_TEXTURE_FILTER_POINT_MIPMAP_LINEAR	= SCEGU_NEAREST_MIPMAP_LINEAR,
	I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT	= SCEGU_LINEAR_MIPMAP_NEAREST,
	I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR = SCEGU_LINEAR_MIPMAP_LINEAR,
};

enum	I3G_MIPMAP
{
	I3G_MIPMAP_DISABLE						=	0,
	I3G_MIPMAP_AUTO_GEN,
	I3G_MIPMAP_LOAD,
};

enum I3G_TEXTURE_COORD_SOURCE
{
	I3G_TEXTURE_COORD_SOURCE_VERTEX						=	SCEGU_PROJ_UV,
	I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_POSITION		=	SCEGU_PROJ_XYZ,
	I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_NORMAL		=	SCEGU_PROJ_NORMALIZE_NORMAL,
	I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_REFLECTION	=	SCEGU_PROJ_NORMAL,
};

enum I3G_TEXTURE_FUNCTION
{
	I3G_TEXTURE_FUNC_REPLACE							=	SCEGU_TEX_REPLACE,
	I3G_TEXTURE_FUNC_MODULATE							=	SCEGU_TEX_MODULATE,
	I3G_TEXTURE_FUNC_ADD								=	SCEGU_TEX_ADD,
	I3G_TEXTURE_FUNC_SPECULAR_HIGHLIGHT					=	SCEGU_TEX_BLEND,
};

enum I3G_CUBE_TEXTURE_FACE
{
	I3G_CUBE_TEXTURE_FACE_PX							=	0,
	I3G_CUBE_TEXTURE_FACE_NX							=	1,
	I3G_CUBE_TEXTURE_FACE_PY							=	2,
	I3G_CUBE_TEXTURE_FACE_NY							=	3,
	I3G_CUBE_TEXTURE_FACE_PZ							=	4,
	I3G_CUBE_TEXTURE_FACE_NZ							=	5,
};

///////////////////////////////////////////////////////////////////////////////////////
// Light type
enum	I3G_LIGHT_TYPE
{
	I3G_LIGHT_NONE							=	-1,
	I3G_LIGHT_DIRECTIONAL					=	SCEGU_LIGHT_DIRECTION,
	I3G_LIGHT_SPOT							=	SCEGU_LIGHT_SPOT,
	I3G_LIGHT_POINT							=	SCEGU_LIGHT_POINT,
};

#endif
