#if !defined( __I3G_DEFINE_OGLES_H )
#define __I3G_DEFINE_OGLES_H

#if !defined( I3G_OGLES )
#error "This header file is only for WIPI."
#endif

#include "i3Base.h"

#include "gl.h"
#include "egl.h"

////////////////////////////////////////////////////////////////////////////////////////
// Primitive Types
enum I3G_PRIMITIVE
{
	I3G_PRIM_NONE			=	-1,
	I3G_PRIM_POINTLIST		=	GL_POINTS,
	I3G_PRIM_LINELIST		=	GL_LINES,
	I3G_PRIM_LINESTRIP		=	GL_LINE_STRIP,
	I3G_PRIM_TRILIST		=	GL_TRIANGLES,
	I3G_PRIM_TRISTRIP		=	GL_TRIANGLE_STRIP,
	I3G_PRIM_TRIFAN			=	GL_TRIANGLE_FAN,
	I3G_PRIM_POINTSPRITE	=	GL_POINTS,	// SCEGU_PRIM_RECTANGLES,
	I3G_PRIM_KEEP			=	-1,	// AP2 명령어에서 쓰기 위한 것.
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
	I3G_MATRIX_VIEW		=	0,

	// Identifies the transformation matrix being set as the projection transformation matrix.
	// The default value is NULL ( the identity matrix ). 
	I3G_MATRIX_PROJECTION	=	1,

	// Identifies the transformation matrix being set for the specified texture stage. 
	I3G_MATRIX_TEXTURE0		=	2,
	I3G_MATRIX_TEXTURE1		=	3,
	I3G_MATRIX_TEXTURE2		=	4,
	I3G_MATRIX_TEXTURE3		=	5,
	I3G_MATRIX_TEXTURE4		=	6,
	I3G_MATRIX_TEXTURE5		=	7,
	I3G_MATRIX_TEXTURE6		=	8,
	I3G_MATRIX_TEXTURE7		=	9,

	I3G_MATRIX_WORLD			=	10
};

///////////////////////////////////////////////////////////////////////////////////////
// Clear Flags
typedef UINT32					I3G_CLEAR;

#define	I3G_CLEAR_COLOR			GL_COLOR_BUFFER_BIT
#define I3G_CLEAR_DEPTH			GL_DEPTH_BUFFER_BIT
#define I3G_CLEAR_STENCIL		GL_STENCIL_BUFFER_BIT

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
	I3G_TEXTURE_WRAP_WRAP			=	GL_REPEAT,
	I3G_TEXTURE_WRAP_CLAMP			=	GL_CLAMP_TO_EDGE,
};

enum	I3G_TEXTURE_FILTER
{
	I3G_TEXTURE_FILTER_NONE					= -1,
	I3G_TEXTURE_FILTER_POINT				= GL_NEAREST,
	I3G_TEXTURE_FILTER_LINEAR				= GL_LINEAR,
	I3G_TEXTURE_FILTER_POINT_MIPMAP_POINT	= GL_NEAREST_MIPMAP_NEAREST,
	I3G_TEXTURE_FILTER_POINT_MIPMAP_LINEAR	= GL_NEAREST_MIPMAP_LINEAR,
	I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT	= GL_LINEAR_MIPMAP_NEAREST,
	I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
};

enum	I3G_MIPMAP
{
	I3G_MIPMAP_DISABLE						=	0,
	I3G_MIPMAP_AUTO_GEN,
	I3G_MIPMAP_LOAD,
};

enum I3G_TEXTURE_COORD_SOURCE
{
	I3G_TEXTURE_COORD_SOURCE_VERTEX						=	0,
	I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_POSITION		=	1,
	I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_NORMAL		=	2,
	I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_REFLECTION	=	3,
};

enum I3G_TEXTURE_FUNCTION
{
	I3G_TEXTURE_FUNC_REPLACE							=	GL_REPLACE,
	I3G_TEXTURE_FUNC_MODULATE							=	GL_MODULATE,
	I3G_TEXTURE_FUNC_ADD								=	GL_ADD,
	I3G_TEXTURE_FUNC_SPECULAR_HIGHLIGHT					=	GL_BLEND,
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
	I3G_LIGHT_DIRECTIONAL					=	0,
	I3G_LIGHT_SPOT							=	1,
	I3G_LIGHT_POINT							=	2,
};

#endif
