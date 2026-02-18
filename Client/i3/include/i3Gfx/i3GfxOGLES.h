#if !defined( __I3G_OGLES_H )
#define __I3G_OGLES_H

#include "i3Base.h"

#if !defined( I3G_OGLES )
#error "This header file is only for WIPI."
#endif

/************************************************************************/
/*			        WIPI-OGL & WIPI-OGLES 1.0v                          */
/*	OGLES 1.0v 각 플랫폼에 대한 함수정의								*/
/*	OGL 이름 규칙 : glfunction()										*/
/*	OGLES 이름 규칙 : glesfuction()										*/
/*	작성자 : 정순구	dfly79@zepetto.com									*/
/*	작성일 : 2005. 09. 27												*/
/*	SKT의 GIGA 및 KTF의 opengl 통합										*/
/************************************************************************/

static EGLDisplay	s_hDisplay;			//	LCD 핸들
static EGLSurface	s_eglWindowSurface;	//	FrameBuffer 핸들
static EGLContext	s_eglContext;		//	Context 핸들

#if defined( I3_WIPI_KTF)
extern "C" {

	#if defined( I3_WIPI_GP)
	// G1000 인터페이스
		extern WIPIC_GP_2D_API* INF_gp2D;
		extern WIPIC_GP_3D_API* INF_gp3D;
		extern WIPIC_GP_MATH_API* INF_gpMATH;
		extern WIPIC_GP_MEDIA_API* INF_gpExtMedia;
		extern WIPIC_GP_BT_API* INF_gpBT;
		extern WIPIC_GP_KEY_API* INF_gpKEY;
	#endif

	extern WIPIC_GP_GLES_API* INF_gpGLES;
	extern WIPIC_GP_EGL_API* INF_gpEGL;
}
#endif

namespace I3OGL
{
	inline EGLDisplay glGetDisplay()
	{
		return s_hDisplay;
	}
	inline EGLSurface glGetWindowSurface()
	{
		return s_eglWindowSurface;
	}
	inline EGLContext glGetContext()
	{
		return s_eglContext;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	// OpenGL ES functions
	inline EGLBoolean eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglChooseConfig( dpy, attrib_list, configs, config_size, num_config);
		#elif defined( I3_WIPI_GP)
			return MC_eglChooseConfig( dpy, attrib_list, configs, config_size, num_config);
		#else
			return ::eglChooseConfig( dpy, attrib_list, configs, config_size, num_config);
		#endif
	}

	inline EGLBoolean eglCopyBuffers(EGLDisplay dpy, EGLSurface surface, void* target)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglCopyBuffers( dpy, surface, (HIMAGE)target);
		#elif defined( I3_WIPI_GP)
			return MC_eglCopyBuffers( dpy, surface, (NativePixmapType)target);
		#else
			return ::eglCopyBuffers( dpy, surface, (NativePixmapType)target);
		#endif
	}

	inline EGLContext eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_list, const EGLint *attrib_list)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglCreateContext( dpy, config, share_list, attrib_list);
		#elif defined( I3_WIPI_GP)
			return MC_eglCreateContext( dpy, config, share_list, attrib_list);
		#else
			return ::eglCreateContext( dpy, config, share_list, attrib_list);
		#endif
	}

	inline EGLSurface eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglCreatePbufferSurface( dpy, config, attrib_list);
		#elif defined( I3_WIPI_GP)
			return MC_eglCreatePbufferSurface( dpy, config, attrib_list);
		#else
			return ::eglCreatePbufferSurface( dpy, config, attrib_list);
		#endif
	}

	inline EGLSurface eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config, void* pixmap, const EGLint *attrib_list)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglCreatePixmapSurface( dpy, config, (HIMAGE)pixmap, attrib_list);
		#elif defined( I3_WIPI_GP)
			return MC_eglCreatePixmapSurface( dpy, config, (NativePixmapType)pixmap, attrib_list);
		#else
			return ::eglCreatePixmapSurface( dpy, config, (NativePixmapType)pixmap, attrib_list);
		#endif
	}

	inline EGLSurface eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, NativeWindowType window, const EGLint *attrib_list)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglCreateWindowSurface( dpy, config, window, attrib_list);
		#elif defined( I3_WIPI_GP)
			return MC_eglCreateWindowSurface( dpy, config, window, attrib_list);
		#else
			return ::eglCreateWindowSurface( dpy, config, window, attrib_list);
		#endif
	}

	inline EGLBoolean eglDestroyContext(EGLDisplay dpy, EGLContext ctx)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglDestroyContext( dpy, ctx);
		#elif defined( I3_WIPI_GP)
			return MC_eglDestroyContext( dpy, ctx);
		#else
			return ::eglDestroyContext( dpy, ctx);
		#endif
	}

	inline EGLBoolean eglDestroySurface(EGLDisplay dpy, EGLSurface surface)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglDestroySurface( dpy, surface);
		#elif defined( I3_WIPI_GP)
			return MC_eglDestroySurface( dpy, surface);
		#else
			return ::eglDestroySurface( dpy, surface);
		#endif
	}

	inline EGLBoolean eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglGetConfigAttrib( dpy, config, attribute, value);
		#elif defined( I3_WIPI_GP)
			return MC_eglGetConfigAttrib( dpy, config, attribute, value);
		#else
			return ::eglGetConfigAttrib( dpy, config, attribute, value);
		#endif
	}

	inline EGLBoolean eglGetConfigs(EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglGetConfigs( dpy, configs, config_size, num_config);
		#elif defined( I3_WIPI_GP)
			return MC_eglGetConfigs( dpy, configs, config_size, num_config);
		#else
			return ::eglGetConfigs( dpy, configs, config_size, num_config);
		#endif
	}

	inline EGLContext eglGetCurrentContext(void)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglGetCurrentContext();
		#elif defined( I3_WIPI_GP)
			return MC_eglGetCurrentContext();
		#else
			return ::eglGetCurrentContext();
		#endif
	}

	inline EGLDisplay eglGetCurrentDisplay(void)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglGetCurrentDisplay();
		#elif defined( I3_WIPI_GP)
			return MC_eglGetCurrentDisplay();
		#else
			return ::eglGetCurrentDisplay();
		#endif
	}

	inline EGLSurface eglGetCurrentSurface(EGLint readdraw)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglGetCurrentSurface( readdraw);
		#elif defined( I3_WIPI_GP)
			return MC_eglGetCurrentSurface( readdraw);
		#else
			return ::eglGetCurrentSurface( readdraw);
		#endif
	} 

	inline EGLDisplay eglGetDisplay(EGLint display)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglGetDisplay((G2D_LCD)display);
		#elif defined( I3_WIPI_GP)
			return MC_eglGetDisplay((NativeDisplayType)display);
		#else
			return eglGetDisplay(display);
		#endif
	}

	inline EGLint eglGetError(void)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglGetError();
		#elif defined( I3_WIPI_GP)
			return MC_eglGetError();
		#else
			return ::eglGetError();
		#endif
	}

	inline void * eglGetProcAddress(const char * procname)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglGetProcAddress(procname);
		#elif defined( I3_WIPI_KTF_KV3600)
			return MC_eglGetProcAddress(procname);
		#else
			return ::eglGetProcAddress(procname);
			//return NULL;
		#endif
	}

	inline EGLBoolean eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor)
	{
		#if defined(I3_WIPI_GIGA)
			return OEMC_g3D_eglInitialize(dpy, major, minor);
		#elif defined( I3_WIPI_GP)
			return MC_eglInitialize(dpy, major, minor);
		#else
			return ::eglInitialize(dpy, major, minor);
		#endif
	}

	inline EGLBoolean eglMakeCurrent (EGLDisplay dpy, EGLSurface edraw, EGLSurface read, EGLContext ctx)
	{
		EGLBoolean ret = 0;

		#if defined( I3_WIPI_GIGA)
			ret = OEMC_g3D_eglMakeCurrent( dpy, edraw, read, ctx);
		#elif defined( I3_WIPI_GP)
			ret = MC_eglMakeCurrent( dpy, edraw, read, ctx);
		#else
			ret = ::eglMakeCurrent( dpy, edraw, read, ctx);
		#endif

		return ret;
	}

	inline EGLBoolean eglQueryContext (EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint *value)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglQueryContext( dpy, ctx, attribute, value);
		#elif defined( I3_WIPI_GP)
			return MC_eglQueryContext( dpy, ctx, attribute, value);
		#else
			return ::eglQueryContext( dpy, ctx, attribute, value);
		#endif
	}

	inline const char * eglQueryString(EGLDisplay dpy, EGLint name)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglQueryString( dpy, name);
		#elif defined( I3_WIPI_GP)
			return MC_eglQueryString(dpy, name);
		#else
			return ::eglQueryString(dpy, name);
		#endif
	}

	inline EGLBoolean eglQuerySurface (EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglQuerySurface( dpy, surface, attribute, value);
		#elif defined( I3_WIPI_GP)
			return MC_eglQuerySurface( dpy, surface, attribute, value);
		#else
			return ::eglQuerySurface( dpy, surface, attribute, value);
		#endif
	}

	inline EGLBoolean eglSwapBuffers (EGLDisplay dpy, EGLSurface edraw)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglSwapBuffers( dpy, edraw);
		#elif defined( I3_WIPI_GP)
			return MC_eglSwapBuffers( dpy, edraw);
		#else
			return ::eglSwapBuffers( dpy, edraw);
		#endif
	}

	inline EGLBoolean eglTerminate(EGLDisplay dpy)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglTerminate( dpy);
		#elif defined( I3_WIPI_GP)
			return MC_eglTerminate( dpy);
		#else
			return ::eglTerminate( dpy);
		#endif
	}
	
	inline EGLBoolean eglWaitGL (void)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglWaitGL();
		#elif defined( I3_WIPI_GP)
			return MC_eglWaitGL();
		#else
			return ::eglWaitGL();
		#endif
	}

	inline EGLBoolean eglWaitNative (EGLint engine)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_eglWaitNative( engine);
		#elif defined( I3_WIPI_GP)
			return MC_eglWaitNative( engine);
		#else
			return ::eglWaitNative( engine);
		#endif
	}

	/************************************************************************/
	/*                 OpenGL functions                                     */
	/************************************************************************/
	
	//////////////////////////////////////////////////////////////////////////
	// Name : glActiveTexture
	// Parameters :
	//	texture
	//		Specifies which texture unit to make active.
	//		The number of texture units is implementation dependent, but must be at least two. texture must be one of GL_TEXTUREi, where 0 ≤ i < GL_MAX_TEXTURE_UNITS, which is an implementation-dependent value.
	//		The intial value is GL_TEXTURE0.
	// Desc :
	//		glActiveTexture selects which texture unit subsequent texture state calls will affect.
	//		The number of texture units an implementation supports is implementation dependent, it must be at least 2.
	inline void glActiveTexture( GLenum texture )
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glActiveTexture( texture);
		#elif defined( I3_WIPI_GP)
			MC_glActiveTexture(texture);
		#else
			::glActiveTexture(texture);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glAlphaFunc
	// Parameters :
	//	func 
	//		Specifies the alpha comparison function.
	//		Symbolic constants GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, and GL_ALWAYS are accepted.
	//		The initial value is GL_ALWAYS.
	//	ref 
	//		Specifies the reference value that incoming alpha values are compared to.
	//		This value is clamped to the range [0, 1], where 0 represents the lowest possible alpha value and 1 the highest possible value.
	//		The initial reference value is 0
	// Desc :
	//		GL_NEVER ->		Never passes.
	//		GL_LESS ->		Passes if the incoming alpha value is less than the reference value.
	//		GL_EQUAL ->		Passes if the incoming alpha value is equal to the reference value.
	//		GL_LEQUAL ->	Passes if the incoming alpha value is less than or equal to the reference value.
	//		GL_GREATER ->	Passes if the incoming alpha value is greater than the reference value.
	//		GL_NOTEQUAL ->	Passes if the incoming alpha value is not equal to the reference value.
	//		GL_GEQUAL ->	Passes if the incoming alpha value is greater than or equal to the reference value.
	//		GL_ALWAYS ->	Always passes (initial value).
	inline void glAlphaFuncf( GLenum func, GLclampf ref )
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glAlphaFuncx( func, __RT(ref));
		#elif defined( I3_WIPI_GP)
			MC_glAlphaFunc(func, ref);
		#else
			::glAlphaFunc(func, ref);
		#endif
	}
	inline void glAlphaFuncx( GLenum func, GLclampx ref )
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glAlphaFuncx(func, ref);
		#elif defined( I3_WIPI_GP)
			MC_glAlphaFuncx(func, ref);
		#else
			::glAlphaFuncx(func, ref);
		#endif
	}

#ifdef I3_FIXED_POINT
	inline void glAlphaFunc( GLenum func, GLclampx ref)
	{
		glAlphaFuncx( func, ref );
	}
#else
	inline void glAlphaFunc( GLenum func, GLclampf ref)
	{
		glAlphaFuncf( func, ref );
	}
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glBindBuffer
	// Desc : OpengGL-ES 1.1v support
	inline void glBindBuffer( UINT32 target, UINT32 buffer )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glBindBuffer( (GLenum) (target), (GLuint) (buffer) );
		#else
			::glBindBuffer( (GLenum) (target), (GLuint) (buffer) );
			//I3TRACE( "[I3OGL::glBindBuffer] Not support.\n" );
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glBindTexture
	// Parameters :
	//	target 
	//		Specifies the target to which the texture is bound.
	//		Must be GL_TEXTURE_2D.
	//	texture 
	//		Specifies the name of a texture.
	inline void glBindTexture( GLenum target, GLuint texture )
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glBindTexture(target, texture);
		#elif defined( I3_WIPI_GP)
			MC_glBindTexture(target, texture);
		#else
			::glBindTexture(target, texture);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glBlendFunc
	// Parameters :
	//	sfactor 
	//		Specifies how the red, green, blue, and alpha source blending factors are computed.
	//		The following symbolic constants are accepted: GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, and GL_SRC_ALPHA_SATURATE.
	//		The initial value is GL_ONE.
	//	dfactor 
	//		Specifies how the red, green, blue, and alpha destination blending factors are computed.
	//		Eight symbolic constants are accepted: GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, and GL_ONE_MINUS_DST_ALPHA.
	//		The initial value is GL_ZERO.
	// Desc :	kм = color bit
	//		GL_ZERO ( 0, 0, 0, 0 ) 
	//		GL_ONE ( 1, 1, 1, 1 ) 
	//		GL_SRC_COLOR ( Rs / kR, Gs / kG, Bs / kB, As / kA ) 
	//		GL_ONE_MINUS_SRC_COLOR ( 1, 1, 1, 1 ) - ( Rs / kR, Gs / kG, Bs / kB, As / kA ) 
	//		GL_DST_COLOR ( Rd / kR, Gd / kG, Bd / kB, Ad / kA ) 
	//		GL_ONE_MINUS_DST_COLOR ( 1, 1, 1, 1 ) - ( Rd / kR, Gd / kG, Bd / kB, Ad / kA ) 
	//		GL_SRC_ALPHA ( As / kA, As / kA, As / kA, As / kA ) 
	//		GL_ONE_MINUS_SRC_ALPHA ( 1, 1, 1, 1 ) - ( As / kA, As / kA, As / kA, As / kA ) 
	//		GL_DST_ALPHA ( Ad / kA, Ad / kA, Ad / kA, Ad / kA ) 
	//		GL_ONE_MINUS_DST_ALPHA ( 1, 1, 1, 1 ) - ( Ad / kA, Ad / kA, Ad / kA, Ad / kA ) 
	//		GL_SRC_ALPHA_SATURATE ( i, i, i, 1 ) 
	inline void glBlendFunc( GLenum sfactor, GLenum dfactor )
	{
		#if defined( I3_WIPI_GIGA)	
			OEMC_g3D_glBlendFunc(sfactor, dfactor);
		#elif defined( I3_WIPI_GP)
			MC_glBlendFunc(sfactor, dfactor);
		#else
			::glBlendFunc(sfactor, dfactor);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glBufferData
	// Desc : OpengGL-ES 1.1v support
	inline void glBufferData( UINT32 target, INT32 SIZE, const void * data, UINT32 usage)
	{
		#ifdef I3_WIPI_KTF_KV3600
			// glBufferData( (GLenum) (target), (GLsizeiptr) (SIZE), (const GLvoid *) (data), (GLenum) (usage) );
			// 아직 지원이 되지 않네요..ㅡ,.-);
		#else
			::glBufferData( (GLenum) (target), (GLsizeiptr) (SIZE), (const GLvoid *) (data), (GLenum) (usage) );
			//I3TRACE( "[I3OGL::glBufferData] Not support.\n" );			
		#endif
	}
	//////////////////////////////////////////////////////////////////////////
	// Name : glBufferSubData
	// Desc : OpengGL-ES 1.1v support
	inline void glBufferSubData( UINT32 target, INT32 OFFSET, INT32 SIZE, const void * data )
	{
		#ifdef I3_WIPI_KTF_KV3600
			//glBufferSubData( (GLenum) (target), (GLintptr) (OFFSET), (GLsizeiptr) (SIZE), (const GLvoid *)(data) );
		#else
			::glBufferSubData( (GLenum) (target), (GLintptr) (OFFSET), (GLsizeiptr) (SIZE), (const GLvoid *)(data) );
			//I3TRACE( "[I3OGL::glBufferSubData] Not support.\n" );
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glClear
	// Parameters :
	//	mask
	//		Bitwise OR of masks that indicate the buffers to be cleared.
	//		Valid masks are GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT, and GL_STENCIL_BUFFER_BIT.
	// Desc :
	//		GL_COLOR_BUFFER_BIT ->		Indicates the color buffer.
	//		GL_DEPTH_BUFFER_BIT ->		Indicates the depth buffer.
	//		GL_STENCIL_BUFFER_BIT ->	Indicates the stencil buffer.
	inline void glClear(GLbitfield mask)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glClear( mask);
		#elif defined( I3_WIPI_GP)
			MC_glClear( mask);
		#else
			::glClear( mask);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glClearColorf
	// Parameters :
	//	red, green, blue, alpha 
	//		Specify the red, green, blue, and alpha values used when the color buffer is cleared.
	//		The initial values are all 0.
	inline void glClearColorf(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glClearColorx( __RT(red), __RT(green), __RT(blue), __RT(alpha));
		#elif defined( I3_WIPI_GP)
			MC_glClearColor(red, green, blue, alpha);
		#else
			::glClearColor(red, green, blue, alpha);
		#endif
	}
	inline void glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glClearColorx (red, green, blue, alpha);
		#elif defined( I3_WIPI_GP)
			MC_glClearColorx(red, green, blue, alpha);
		#else
			::glClearColorx(red, green, blue, alpha);
		#endif
	}

#ifdef I3_FIXED_POINT
	inline void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
	{
		glClearColorx(red, green, blue, alpha);
	}
#else
	inline void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
	{
		glClearColorf(red, green, blue, alpha);
	}
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glClearDepthf
	// Parameters :
	//	depth 
	//		Specifies the depth value used when the depth buffer is cleared.
	//		The initial value is 1.
	inline void glClearDepthf(GLclampf depth)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glClearDepthx( __RT(depth));
		#elif defined( I3_WIPI_GP)
			MC_glClearDepthf(depth);
		#else
			::glClearDepthf(depth);
		#endif
	}
	inline void glClearDepthx(GLclampx depth)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glClearDepthx ( depth);
		#elif defined( I3_WIPI_GP)
			MC_glClearDepthx(depth);
		#else
			::glClearDepthx(depth);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glClearDepth(depth)		glClearDepthx( (GLclampx) (depth) )
#else
	#define glClearDepth(depth)		glClearDepthf( (GLclampf) (depth) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glClearStencil
	// Parameters :
	//	s 
	//		Specifies the index used when the stencil buffer is cleared.
	//		The initial value is 0.
	inline void glClearStencil(GLint s)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glClearStencil( s);
		#elif defined( I3_WIPI_GP)
			MC_glClearStencil(s);
		#else
			::glClearStencil(s);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glClientActiveTexture
	// Parameters :
	//	texture 
	//		Specifies which texture unit to make active.
	//		The number of texture units is implementation dependent, but must be at least two. texture must be one of GL_TEXTUREi, 0 ≤ i < GL_MAX_TEXTURE_UNITS, which is an implementation-dependent value.
	//		The initial value is GL_TEXTURE0.
	inline void glClientActiveTexture(GLenum texture)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glClientActiveTexture(texture);
		#elif defined( I3_WIPI_GP)
			MC_glClientActiveTexture(texture);
		#else
			::glClientActiveTexture(texture);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glClipPlane
	// Desc : OpengGL-ES 1.1v support
	inline void glClipPlanef( GLenum plane, const GLfloat * equation )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glClipPlanef( plane, equation );
		#else
			::glClipPlanef( plane, equation );
			//I3TRACE( "[I3OGL::glClipPlanef] Not support.\n" );
		#endif	
	}
	inline void glClipPlanex( GLenum plane, const GLfixed *equation )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glClipPlanex( plane, equation );
		#else
			::glClipPlanex( plane, equation );
			//I3TRACE( "[I3OGL::glClipPlanex] Not support.\n" );
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glClipPlane( plane, equation)			glClipPlanex( (GLenum) (plane), (const GLfixed *) equation )
#else
	#define glClipPlane( plane, equation)			glClipPlanef( (GLenum) (plane), (const GLfloat *) equation )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glColor4
	// Parameters :
	//	red, green, blue, alpha 
	//		Specify new red, green, blue, and alpha values for the current color.
	// Desc :
	//	The initial value is (1, 1, 1, 1).
	inline void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glColor4x( __RT(red), __RT(green), __RT(blue), __RT(alpha));
		#elif defined( I3_WIPI_GP)
			MC_glColor4f(red, green, blue, alpha);
		#else
			::glColor4f(red, green, blue, alpha);
		#endif
	}
	inline void glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glColor4x( red, green, blue, alpha);
		#elif defined( I3_WIPI_GP)
			MC_glColor4x(red, green, blue, alpha);
		#else
			::glColor4x(red, green, blue, alpha);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glColor4(red, green, blue, alpha)		glColor4x( (GLfixed) (red), (GLfixed) (green), (GLfixed) (blue), (GLfixed) (alpha) )
#else
	#define glColor4(red, green, blue, alpha)		glColor4f( (GLfloat) (red), (GLfloat) (green), (GLfloat) (blue), (GLfloat) (alpha) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glColorMask
	// Parameters :
	//	red, green, blue, alpha 
	//		Specify whether red, green, blue, and alpha can or cannot be written into the color buffer.
	//		The initial values are all GL_TRUE, indicating that all color components can be written.
	inline void glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glColorMask( red, green, blue, alpha);
		#elif defined( I3_WIPI_GP)
			MC_glColorMask(red, green, blue, alpha);
		#else
			::glColorMask(red, green, blue, alpha);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glColorPointer
	// Parameters :
	//	size 
	//		Specifies the number of components per color. Must be 4.
	//		The initial value is 4.
	//	type 
	//		Specifies the data type of each color component in the array. Symbolic constants GL_UNSIGNED_BYTE and GL_FIXED are accepted. However, the initial value is GL_FLOAT.
	//		The common profile accepts the symbolic constant GL_FLOAT as well.
	//	stride 
	//		Specifies the byte offset between consecutive colors. If stride is 0, the colors are understood to be tightly packed in the array.
	//		The initial value is 0.
	//	pointer 
	//		Specifies a pointer to the first component of the first color element in the array.
	inline void glColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glColorPointer( size, type, stride, pointer);
		#elif defined( I3_WIPI_GP)
			MC_glColorPointer(size, type, stride, pointer);
		#else
			::glColorPointer( size, type, stride, pointer);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glCompressedTexImage2D
	// Parameters :
	//	target 
	//		Specifies the target texture. Must be GL_TEXTURE_2D.
	//	level 
	//		Specifies the level-of-detail number. Must be less than or equal to 0. Level 0 indicates a single mip-level. Negative values indicate how many mip-levels are described by data.
	//	internalformat 
	//		Specifies the color components in the texture. The following symbolic constants are accepted: GL_PALETTE4_RGB8_OES, GL_PALETTE4_RGBA8_OES, GL_PALETTE4_R5_G6_B5_OES, GL_PALETTE4_RGBA4_OES, GL_PALETTE4_RGB5_A1_OES, GL_PALETTE8_RGB8_OES, GL_PALETTE8_RGBA8_OES, GL_PALETTE8_R5_G6_B5_OES, GL_PALETTE8_RGBA4_OES, and GL_PALETTE8_RGB5_A1_OES.
	//	width 
	//		Specifies the width of the texture image. Must be 2n + 2border for some integer n. All implementations support texture images that are at least 64 texels wide.
	//	height 
	//		Specifies the height of the texture image. Must be 2m + 2border for some integer m. All implementations support texture images that are at least 64 texels high.
	//	border 
	//		Specifies the width of the border. Must be 0.
	//	imageSize 
	//		Specifies the size of the compressed image data in bytes.
	//	data 
	//		Specifies a pointer to the compressed image data in memory.
	inline void glCompressedTexImage2D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
		#elif defined( I3_WIPI_GP)
			MC_glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
		#else
			::glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glCompressedTexSubImage2D
	// Parameters :
	//	target 
	//		Specifies the target texture. Must be GL_TEXTURE_2D.
	//	level 
	//		Specifies the level-of-detail number.
	//	xoffset 
	//		Specifies a texel offset in the x direction within the texture array.
	//	yoffset 
	//		Specifies a texel offset in the y direction within the texture array.
	//	width 
	//		Specifies the width of the texture subimage.
	//	height 
	//		Specifies the height of the texture subimage.
	//	format 
	//		Specifies the format of the pixel data. Currently, there is no supported format.
	//	imageSize 
	//		Specifies the size of the compressed pixel data in bytes.
	//	data 
	//		Specifies a pointer to the compressed image data in memory.
	inline void glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
		#elif defined( I3_WIPI_GP)
			MC_glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
		#else
			::glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glCopyTexIamge2D
	// Parameters :
	//	target 
	//		Specifies the target texture. Must be GL_TEXTURE_2D.
	//	level 
	//		Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image.
	//	internalformat 
	//		Specifies the color components of the texture. Must be one of the following symbolic constants: GL_ALPHA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, or GL_RGBA.
	//	x, y 
	//		Specify the window coordinates of the lower left corner of the rectangular region of pixels to be copied.
	//	width 
	//		Specifies the width of the texture image. Must be 0 or 2n + 2border for some integer n.
	//	height 
	//		Specifies the height of the texture image. Must be 0 or 2m + 2border for some integer m.
	//	border 
	//		Specifies the width of the border. Must be 0.
	inline void glCopyTexImage2D(GLenum target, GLint level, GLint internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
		#elif defined( I3_WIPI_GP)
			MC_glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
		#else
			::glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glCopyTexSubImage2D
	// Parameters :
	//	target 
	//		Specifies the target texture. Must be GL_TEXTURE_2D.
	//	level 
	//		Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image.
	//	xoffset 
	//		Specifies a texel offset in the x direction within the texture array.
	//	yoffset 
	//		Specifies a texel offset in the y direction within the texture array.
	//	x, y 
	//		Specify the window coordinates of the lower left corner of the rectangular region of pixels to be copied.
	//	width 
	//		Specifies the width of the texture subimage.
	//	height 
	//		Specifies the height of the texture subimage.
	inline void glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
		#elif defined( I3_WIPI_GP)
			MC_glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
		#else
			::glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glCullFace
	// Parameters :
	//	mode 
	//		Specifies whether front- or back-facing polygons are culled.
	//		Symbolic constants GL_FRONT, GL_BACK, and GL_FRONT_AND_BACK are accepted.
	//		The initial value is GL_BACK.
	inline void glCullFace(GLenum mode)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glCullFace( mode);
		#elif defined( I3_WIPI_GP)
			MC_glCullFace( mode);
		#else
			::glCullFace( mode);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glCurrentPaletteMatrixOES (OpenGL ES 1.1)
	// Desc : OpenGL-ES 1.1v support
	inline void glCurrentPaletteMatrixOES( GLuint matrixpaletteindex )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glCurrentPaletteMatrixOES( matrixpaletteindex );
		#else
			::glCurrentPaletteMatrixOES( matrixpaletteindex );
			//I3TRACE( "[I3OGL::glCureentPaletteMatrixOES] Not support.\n" );
		#endif
	}
	//////////////////////////////////////////////////////////////////////////
	// Name : glDeleteBuffers
	// Desc : OpenGL-ES 1.1v support
	inline void glDeleteBuffers( GLsizei n, const GLuint * buffers )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glDeleteBuffers( n, buffers);
		#else
			::glDeleteBuffers( n, buffers);
			//I3TRACE( "[I3OGL::glDeleteBuffers] Not support.\n" );
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glDeleteTextures
	// Parameters :
	//	n 
	//		Specifies the number of textures to be deleted.
	//	textures 
	//		Specifies an array of textures to be deleted.
	inline void glDeleteTextures(GLsizei n, const GLuint *textures)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glDeleteTextures(n, textures);
		#elif defined( I3_WIPI_GP)
			MC_glDeleteTextures(n, textures);
		#else
			::glDeleteTextures(n, textures);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glDepthFunc
	// Parameters :
	//	func 
	//		Specifies the depth comparison function.
	//		Symbolic constants GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, and GL_ALWAYS are accepted.
	//		The initial value is GL_LESS.
	inline void glDepthFunc(GLenum func)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glDepthFunc(func);
		#elif defined( I3_WIPI_GP)
			MC_glDepthFunc(func);
		#else
			::glDepthFunc(func);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glDepthMask
	// Parameters :
	//	flag 
	//	Specifies whether the depth buffer is enabled for writing.
	//	If flag is GL_FALSE, depth buffer writing is disabled, otherwise it is enabled.
	//	The initial value is GL_TRUE.
	inline void glDepthMask(GLboolean flag)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glDepthMask(flag);
		#elif defined( I3_WIPI_GP)
			MC_glDepthMask(flag);
		#else
			::glDepthMask(flag);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glDepthRange
	// Parameters :
	//	near 
	//		Specifies the mapping of the near clipping plane to window coordinates. The initial value is 0.
	//	far 
	//		Specifies the mapping of the far clipping plane to window coordinates. The initial value is 1.
	inline void glDepthRangef(GLclampf gnear, GLclampf gfar)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glDepthRangex( __RT(gnear), __RT(gfar));
		#elif defined( I3_WIPI_GP)
			MC_glDepthRangef(gnear, gfar);
		#else
			::glDepthRangef(gnear, gfar);
		#endif
	}
	inline void glDepthRangex(GLclampx gnear, GLclampx gfar)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glDepthRangex( gnear, gfar);
		#elif defined( I3_WIPI_GP)
			MC_glDepthRangex(gnear, gfar);
		#else
			::glDepthRangex(gnear, gfar);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glDepthRange(gnear, gfar)		glDepthRangex( (GLclampx) (gnear), (GLclampx) (gfar) )
#else
	#define glDepthRange(gnear, gfar)		glDepthRangef( (GLclampf) (gnear), (GLclampf) (gfar) )
#endif
	
	//////////////////////////////////////////////////////////////////////////
	// Name : glDrawTex (OpenGL ES 1.1)
	// Parameters :
	//	x, y, z 
	//		Specify the position of the affected screen rectangle.
	//	width, height 
	//		Specifies the width and height of the affected screen rectangle in pixels.
	inline void glDrawTexsOES( GLshort x, GLshort y, GLshort z, GLshort width, GLshort height )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glDrawTexsOES( x, y, z, width, height );
		#else
			::glDrawTexsOES( x, y, z, width, height );
			//I3TRACE( "[I3OGL::glDrawTexsOES] Not supprot.\n" );
		#endif
	}
	inline void glDrawTexiOES( GLint x, GLint y, GLint z, GLint width, GLint height )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glDrawTexiOES( x, y, z, width, height );
		#else
			::glDrawTexiOES( x, y, z, width, height );
			//I3TRACE( "[I3OGL::glDrawTexiOES] Not support.\n");
		#endif
	}
	inline void glDrawTexfOES( GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glDrawTexfOES( x, y, z, width, height );
		#else
			::glDrawTexfOES( x, y, z, width, height );
			//I3TRACE( "[I3OGL::glDrawTexfOES] Not support.\n" );
		#endif
	}
	inline void glDrawTexxOES( GLfixed x, GLfixed y, GLfixed z, GLfixed width, GLfixed height )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glDrawTexxOES( x, y, z, width, height );
		#else
			::glDrawTexxOES( x, y, z, width, height );
			//I3TRACE( "[I3OGL::glDrawTexxOES] Not support.\n" );
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glDrawTex(x, y, z, width, height)		glDrawTexxOES( (GLfixed) (x), (GLfixed) (y), (GLfixed) (z), (GLfixed) (width), (GLfixed) (height) )
#else
	#define glDrawTex(x, y, z, width, height)		glDrawTexfOES( (GLfloat) (x), (GLfloat) (y), (GLfloat) (z), (GLfloat) (width), (GLfloat) (height) )
#endif
	#define glDrawTexi(x, y, z, width, height)		glDrawTexiOES( (GLint) (x), (GLint) (y), (GLint) (z), (GLint) (width), (GLint) (height) )
	#define glDrawTexs(x, y, z, width, height)		glDrawTexsOES( (GLshort) (x), (GLshort) (y), (GLshort) (z), (GLshort) (width), (GLshort) (height) )

	//////////////////////////////////////////////////////////////////////////
	// Name : glDisable
	// Parameters :
	//	cap 
	//		Specifies a symbolic constant indicating a GL capability.
	// Desc :
	//		GL_ALPHA_TEST ->			If enabled, do alpha testing. See glAlphaFunc.
	//		GL_BLEND ->					If enabled, blend the incoming color values with the values in the color buffers. See glBlendFunc.
	//		GL_CLIP_PLANEi ->			If enabled, clipping plane i is enabled. See glClipPlane.(v1.1)
	//		GL_COLOR_LOGIC_OP  ->		If enabled, apply the currently selected logical operation to the incoming color and color buffer values. See glLogicOp.
	//		GL_COLOR_MATERIAL ->		If enabled, have ambient and diffuse material parameters track the current color.
	//		GL_CULL_FACE ->				If enabled, cull polygons based on their winding in window coordinates. See glCullFace.
	//		GL_DEPTH_TEST ->			If enabled, do depth comparisons and update the depth buffer. Note that even if the depth buffer exists and the depth mask is non-zero, the depth buffer is not updated if the depth test is disabled. See glDepthFunc, glDepthMask, and glDepthRange.
	//		GL_DITHER ->				If enabled, dither color components or indices before they are written to the color buffer.
	//		GL_FOG ->					If enabled, blend a fog color into the posttexturing color. See glFog.
	//		GL_LIGHTi ->				If enabled, include light i in the evaluation of the lighting equation. See glLightModel and glLight.
	//		GL_LIGHTING ->				If enabled, use the current lighting parameters to compute the vertex color. Otherwise, simply associate the current color with each vertex. See glMaterial, glLightModel, and glLight.
	//		GL_LINE_SMOOTH ->			If enabled, draw lines with correct filtering. Otherwise, draw aliased lines. See glLineWidth.
	//		GL_MATRIX_PALETTE_OES ->	If enabled, the matrix palette extension is enabled. See glMatrixMode.
	//		GL_MULTISAMPLE ->			If enabled, perform multisampling of fragments for single-pass antialiasing and other effects. See glSampleCoverage.
	//		GL_NORMALIZE ->				If enabled, normal vectors are scaled to unit length after transformation. See glNormal and glNormalPointer.
	//		GL_POINT_SMOOTH ->			If enabled, draw points with proper filtering. Otherwise, draw aliased points. See glPointSize.
	//		GL_POINT_SPRITE_OES ->		If enabled, point sprites are enabled. See glPointSize and glTexEnv
	//		GL_POLYGON_OFFSET_FILL ->	If enabled, an offset is added to depth values of a polygon's fragments before the depth comparison is performed. See glPolygonOffset.
	//		GL_RESCALE_NORMAL ->		If enabled, normal vectors are scaled by a factor derived from the modelview matrix. See glNormal and glNormalPointer.
	//		GL_SAMPLE_ALPHA_TO_COVERAGE ->If enabled, a temporary coverage value is generated where each bit is determined by the alpha value at the corresponding sample location. The temporary coverage value is then ANDed with the fragment coverage value. Otherwise the fragment coverage value is unchanged at this point. See glSampleCoverage.
	//		GL_SAMPLE_ALPHA_TO_ONE ->	If enabled, each fragment alpha value is replaced by the maximum representable alpha value. Otherwise, the alpha values are not changed. See glSampleCoverage.
	//		GL_SAMPLE_COVERAGE ->		If enabled, the fragment coverage is ANDed with another temporary coverage. This temporary coverage is generated in the same manner as for GL_SAMPLE_ALPHA_TO_COVERAGE described above, but as a function of the value of GL_SAMPLE_COVERAGE_VALUE. If GL_SAMPLE_COVERAGE_INVERT is GL_TRUE, the temporary coverage is inverted (all bit values are inverted) before it is ANDed with the fragment coverage. See glSampleCoverage.
	//		GL_SCISSOR_TEST ->			If enabled, discard fragments that are outside the scissor rectangle. See glScissor.
	//		GL_STENCIL_TEST ->			If enabled, do stencil testing and update the stencil buffer. See glStencilFunc, glStencilMask, and glStencilOp.
	//		GL_TEXTURE_2D ->			If enabled, two-dimensional texturing is performed for the active texture unit. See glActiveTexture, glTexImage2D, glCompressedTexImage2D, and glCopyTexImage2D.
	inline void glDisable(GLenum cap)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glDisable(cap);
		#elif defined( I3_WIPI_GP)
			MC_glDisable(cap);
		#else
			::glDisable(cap);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glDisableClientState
	// Parameters :
	//	array 
	//		Specifies the capability to enable or disable.
	//		Symbolic constants GL_COLOR_ARRAY, GL_MATRIX_INDEX_ARRAY_OES, GL_NORMAL_ARRAY, GL_POINT_SIZE_ARRAY_ARRAY_OES, GL_TEXTURE_COORD_ARRAY, GL_VERTEX_ARRAY, and GL_WEIGHT_ARRAY_OES are accepted.
	// Desc :
	//		GL_COLOR_ARRAY					If enabled, the color array is enabled for writing and used during rendering when glDrawArrays, or glDrawElements is called. See glColorPointer.
	//		GL_MATRIX_INDEX_ARRAY_OES		If enabled, the matrix index array is enabled. See glMatrixIndexPointer
	//		GL_NORMAL_ARRAY					If enabled, the normal array is enabled for writing and used during rendering when glDrawArrays, or glDrawElements is called. See glNormalPointer.
	//		GL_POINT_SIZE_ARRAY_OES 		If enabled, the point size array controls the sizes used to render points and point sprites. In this case the point size defined by glPointSize is ignored. The point sizes supplied in the point size arrays will be the sizes used to render both points and point sprites. See glPointSize
	//		GL_TEXTURE_COORD_ARRAY			If enabled, the texture coordinate array is enabled for writing and used during rendering when glDrawArrays, or glDrawElements is called. See glTexCoordPointer.
	//		GL_VERTEX_ARRAY					If enabled, the vertex array is enabled for writing and used during rendering when glDrawArrays, or glDrawElements is called. See glVertexPointer.
	//		GL_WEIGHT_ARRAY_OES				If enabled, the weight array is enabled. See glWeightPointer
	inline void glDisableClientState(GLenum array)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glDisableClientState(array);
		#elif defined( I3_WIPI_GP)
			MC_glDisableClientState(array);
		#else
			::glDisableClientState(array);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glDrawArrays
	// Parameters :
	//	mode 
	//		Specifies what kind of primitives to render.
	//		Symbolic constants GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, and GL_TRIANGLES are accepted.
	//	first 
	//		Specifies the starting index in the enabled arrays.
	//	count 
	//		Specifies the number of indices to be rendered.
	inline void glDrawArrays(GLenum mode, GLint first, GLsizei count)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glDrawArrays( mode, first, count);
		#elif defined( I3_WIPI_GP)
			MC_glDrawArrays(mode, first, count);
		#else
			::glDrawArrays(mode, first, count);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glDrawElements
	// Parameters :
	//	mode 
	//		Specifies what kind of primitives to render.
	//		Symbolic constants GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, and GL_TRIANGLES are accepted.
	//	count 
	//		Specifies the number of elements to be rendered.
	//	type 
	//		Specifies the type of the values in indices.
	//		Must be either GL_UNSIGNED_BYTE or GL_UNSIGNED_SHORT.
	//	indices 
	//		Specifies a pointer to the location where the indices are stored.
	inline void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glDrawElements( mode, count, type, indices);
		#elif defined( I3_WIPI_GP)
			MC_glDrawElements(mode, count, type, indices);
		#else
			::glDrawElements(mode, count, type, indices);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glEnable
	// Parameters :
	//	cap 
	//		Specifies a symbolic constant indicating a GL capability.
	// Desc :
	//		GL_ALPHA_TEST					If enabled, do alpha testing. See glAlphaFunc.
	//		GL_BLEND						If enabled, blend the incoming color values with the values in the color buffers. See glBlendFunc.
	//		GL_CLIP_PLANEi					If enabled, clipping plane i is enabled. See glClipPlane.(v1.1)
	//		GL_COLOR_LOGIC_OP				If enabled, apply the currently selected logical operation to the incoming color and color buffer values. See glLogicOp.
	//		GL_COLOR_MATERIAL				If enabled, have ambient and diffuse material parameters track the current color.
	//		GL_CULL_FACE					If enabled, cull polygons based on their winding in window coordinates. See glCullFace.
	//		GL_DEPTH_TEST					If enabled, do depth comparisons and update the depth buffer. Note that even if the depth buffer exists and the depth mask is non-zero, the depth buffer is not updated if the depth test is disabled. See glDepthFunc, glDepthMask, and glDepthRange.
	//		GL_DITHER						If enabled, dither color components or indices before they are written to the color buffer.
	//		GL_FOG							If enabled, blend a fog color into the posttexturing color. See glFog.
	//		GL_LIGHTi						If enabled, include light i in the evaluation of the lighting equation. See glLightModel and glLight.
	//		GL_LIGHTING						If enabled, use the current lighting parameters to compute the vertex color. Otherwise, simply associate the current color with each vertex. See glMaterial, glLightModel, and glLight.
	//		GL_LINE_SMOOTH					If enabled, draw lines with correct filtering. Otherwise, draw aliased lines. See glLineWidth.
	//		GL_MATRIX_PALETTE_OES			If enabled, the matrix palette extension is enabled. See glMatrixMode.
	//		GL_MULTISAMPLE					If enabled, perform multisampling of fragments for single-pass antialiasing and other effects. See glSampleCoverage.
	//		GL_NORMALIZE					If enabled, normal vectors are scaled to unit length after transformation. See glNormal and glNormalPointer.
	//		GL_POINT_SMOOTH					If enabled, draw points with proper filtering. Otherwise, draw aliased points. See glPointSize.
	//		GL_POINT_SPRITE_OES				If enabled, point sprites are enabled. See glPointSize and glTexEnv
	//		GL_POLYGON_OFFSET_FILL			If enabled, an offset is added to depth values of a polygon's fragments before the depth comparison is performed. See glPolygonOffset.
	//		GL_RESCALE_NORMAL				If enabled, normal vectors are scaled by a factor derived from the modelview matrix. See glNormal and glNormalPointer.
	//		GL_SAMPLE_ALPHA_TO_COVERAGE 	If enabled, a temporary coverage value is generated where each bit is determined by the alpha value at the corresponding sample location. The temporary coverage value is then ANDed with the fragment coverage value. Otherwise the fragment coverage value is unchanged at this point. See glSampleCoverage.
	//		GL_SAMPLE_ALPHA_TO_ONE			If enabled, each fragment alpha value is replaced by the maximum representable alpha value. Otherwise, the alpha values are not changed. See glSampleCoverage.
	//		GL_SAMPLE_COVERAGE				If enabled, the fragment coverage is ANDed with another temporary coverage. This temporary coverage is generated in the same manner as for GL_SAMPLE_ALPHA_TO_COVERAGE described above, but as a function of the value of GL_SAMPLE_COVERAGE_VALUE. If GL_SAMPLE_COVERAGE_INVERT is GL_TRUE, the temporary coverage is inverted (all bit values are inverted) before it is ANDed with the fragment coverage. See glSampleCoverage.
	//		GL_SCISSOR_TEST					If enabled, discard fragments that are outside the scissor rectangle. See glScissor.
	//		GL_STENCIL_TEST					If enabled, do stencil testing and update the stencil buffer. See glStencilFunc, glStencilMask, and glStencilOp.
	//		GL_TEXTURE_2D					If enabled, two-dimensional texturing is performed for the active texture unit. See glActiveTexture, glTexImage2D, glCompressedTexImage2D, and glCopyTexImage2D.
	inline void glEnable(GLenum cap)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glEnable(cap);
		#elif defined( I3_WIPI_GP)
			MC_glEnable(cap);
		#else
			::glEnable(cap);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glEnableClientState
	// Parameters :
	//	array 
	//		Specifies the capability to enable or disable. Symbolic constants GL_COLOR_ARRAY, GL_MATRIX_INDEX_ARRAY_OES, GL_NORMAL_ARRAY, GL_POINT_SIZE_ARRAY_ARRAY_OES, GL_TEXTURE_COORD_ARRAY, GL_VERTEX_ARRAY, and GL_WEIGHT_ARRAY_OES are accepted.
	//		GL_COLOR_ARRAY					If enabled, the color array is enabled for writing and used during rendering when glDrawArrays, or glDrawElements is called. See glColorPointer.
	//		GL_MATRIX_INDEX_ARRAY_OES 		If enabled, the matrix index array is enabled. See glMatrixIndexPointer
	//		GL_NORMAL_ARRAY					If enabled, the normal array is enabled for writing and used during rendering when glDrawArrays, or glDrawElements is called. See glNormalPointer.
	//		GL_POINT_SIZE_ARRAY_OES 		If enabled, the point size array controls the sizes used to render points and point sprites. In this case the point size defined by glPointSize is ignored. The point sizes supplied in the point size arrays will be the sizes used to render both points and point sprites. See glPointSize
	//		GL_TEXTURE_COORD_ARRAY			If enabled, the texture coordinate array is enabled for writing and used during rendering when glDrawArrays, or glDrawElements is called. See glTexCoordPointer.
	//		GL_VERTEX_ARRAY					If enabled, the vertex array is enabled for writing and used during rendering when glDrawArrays, or glDrawElements is called. See glVertexPointer.
	//		GL_WEIGHT_ARRAY_OES				If enabled, the weight array is enabled. See glWeightPointer
	inline void glEnableClientState(GLenum array)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glEnableClientState( array);
		#elif defined( I3_WIPI_GP)
			MC_glEnableClientState(array);
		#else
			::glEnableClientState(array);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glFinish
	// Desc : glFinish does not return until the effects of all previously called GL commands are complete.
	inline void glFinish()
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glFinish();
		#elif defined( I3_WIPI_GP)
			MC_glFinish();
		#else
			::glFinish();
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glFlush
	// Desc : Different GL implementations buffer commands in several different locations, including network buffers and the graphics accelerator itself. glFlush empties all of these buffers, causing all issued commands to be executed as quickly as they are accepted by the actual rendering engine. Though this execution may not be completed in any particular time period, it does complete in finite time.
	inline void glFlush()
	{
		#if defined( I3_WIPI_GIGA)	
			OEMC_g3D_glFlush();		
			OEMC_g3D_eglxFlush();
		#elif defined( I3_WIPI_GP)
			MC_glFlush();
		#else
			::glFlush();
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glFog
	// Parameters : 
	//	pname 
	//		Specifies a single-valued fog parameter. GL_FOG_MODE, GL_FOG_DENSITY, GL_FOG_START, and GL_FOG_END are accepted.
	//	param 
	//		Specifies the value that pname will be set to.
	inline void glFogf(GLenum pname, GLfloat param)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glFogx( pname, __RT(param));
		#elif defined( I3_WIPI_GP)
			MC_glFogf(pname, param);
		#else
			::glFogf(pname, param);
		#endif
	}
	inline void glFogx(GLenum pname, GLfixed param)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glFogx( pname, param);
		#elif defined( I3_WIPI_GP)
			MC_glFogx(pname, param);
		#else
			::glFogx(pname, param);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glFog(pname, param)		glFogx( (GLenum) (pname), (GLfixed) (param) )
#else
	#define glFog(pname, param)		glFogf( (GLenum) (pname), (GLfloat) (param) )
#endif

	inline void glFogfv(GLenum pname, const GLfloat *params)
	{
		#if defined( I3_WIPI_GIGA)
			"This function can't apply for GIGA."
		#elif defined( I3_WIPI_GP)
			MC_glFogfv(pname, params);
		#else
			::glFogfv(pname, params);
		#endif
	}
	inline void glFogxv(GLenum pname, const GLfixed *params)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glFogxv( pname, params);
		#elif defined( I3_WIPI_GP)
			MC_glFogxv(pname, params);
		#else
			::glFogxv(pname, params);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glFogv(pname, params)		glFogxv( (GLenum) (pname), (const GLfixed *) (params) )
#else
	#define glFogv(pname, params)		glFogfv( (GLenum) (pname), (const GLfloat *) (params) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glFrontFace
	// Parameters :
	//	mode 
	//		Specifies the orientation of front-facing polygons. GL_CW and GL_CCW are accepted.
	//		The initial value is GL_CCW.
	inline void glFrontFace(GLenum mode)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glFrontFace( mode);
		#elif defined( I3_WIPI_GP)
			MC_glFrontFace(mode);
		#else
			::glFrontFace(mode);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glFrustum
	// Parameters :
	//	left, right 
	//		Specify the coordinates for the left and right vertical clipping planes.
	//	bottom, top 
	//		Specify the coordinates for the bottom and top horizontal clipping planes.
	//	near, far 
	//		Specify the distances to the near and far depth clipping planes. Both distances must be positive.
	inline void glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat gnear, GLfloat gfar)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glFrustumx( __RT(left), __RT(right), __RT(bottom), __RT(top), __RT(gnear), __RT(gfar));
		#elif defined( I3_WIPI_GP)
			MC_glFrustumf(left, right, bottom, top, gnear, gfar);
		#else
			::glFrustumf(left, right, bottom, top, gnear, gfar);
		#endif
	}
	inline void glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed gnear, GLfixed gfar)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glFrustumx(left, right, bottom, top, gnear, gfar);
		#elif defined( I3_WIPI_GP)
			MC_glFrustumx(left, right, bottom, top, gnear, gfar);
		#else
			::glFrustumx(left, right, bottom, top, gnear, gfar);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glFrustum(left, right, bottom, top, gnear, gfar)		glFrustumx( (GLfixed) (left), (GLfixed) (right), (GLfixed) (bottom), (GLfixed) (top), (GLfixed) (gnear), (GLfixed) (gfar) )
#else
	#define glFrustum(left, right, bottom, top, gnear, gfar)		glFrustumf( (GLfloat) (left), (GLfloat) (right), (GLfloat) (bottom), (GLfloat) (top), (GLfloat) (gnear), (GLfloat) (gfar) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glGenBuffers
	// Desc : OpenGL-ES 1.1v support
	inline void glGenBuffers( GLsizei n, GLuint * buffers )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glGenBuffers(n, buffers);
		#else
			::glGenBuffers(n, buffers);
			//I3TRACE( "[I3OGL::glGenBuffers] Not support.\n" );
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glGenTextures
	// Parameters :
	//	n
	//		Specifies the number of texture names to be generated.
	//	textures 
	//		Specifies an array in which the generated texture names are stored.
	inline void glGenTextures(GLsizei n, GLuint *textures)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glGenTextures(n, textures);
		#elif defined( I3_WIPI_GP)
			MC_glGenTextures(n, textures);
		#else
			::glGenTextures(n, textures);
		#endif
	}
	//////////////////////////////////////////////////////////////////////////
	// Name : glGet
	// Parameters :
	//	pname 
	//		Specifies the parameter value to be returned. The symbolic constants in the list below are accepted.
	//	params 
	//		Returns the value or values of the specified parameter.
	// Function :
	//		glGetBooleanv	(1.1v)
	//		glGetFixedv		(1.1v)
	//		glGetFloatv		(1.1v)
	//		glGetIntegerv

	//////////////////////////////////////////////////////////////////////////
	// Name : glGetBufferParameteriv
	// Desc : OpenGL-ES 1.1 support
	inline void glGetBufferParameteriv( GLenum target, GLenum pname, GLint *params )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glGetBufferParameteriv( target, pname, params);
		#else
			::glGetBufferParameteriv( target, pname, params);
			//I3TRACE( "[I3OGL::glGetBufferParamteriv] Not support.\n" );
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glGetClipPlane
	// Desc : OpenGL-ES 1.1 support
	inline void glGetClipPlanef( GLenum pname, GLfloat eqn[4] )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glGetClipPlanef( pname, eqn);
		#else
			::glGetClipPlanef( pname, eqn);
			//I3TRACE( "[I3OGL::glGetClipPlanef] Not support.\n" );
		#endif
	}
	inline void glGetClipPlanex( GLenum pname, GLfixed eqn[4] )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glGetClipPlanex( pname, eqn);
		#else
			::glGetClipPlanex( pname, eqn);
			//I3TRACE( "[I3OGL::glGetClipPlanex] Not support.\n" );
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glGetClipPlane(pname, eqn)		glGetClipPlanex( (GLenum) (pname), (GLfixed*) (eqn) )
#else
	#define glGetClipPlane(pname, eqn)		glGetClipPlanef( (GLenum) (pname), (GLfloat*) (eqn) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glGetError
	// Desc :
	//		GL_NO_ERROR					No error has been recorded. The value of this symbolic constant is guaranteed to be 0.
	//		GL_INVALID_ENUM				An unacceptable value is specified for an enumerated argument. The offending command is ignored, and has no other side effect than to set the error flag.
	//		GL_INVALID_VALUE 			A numeric argument is out of range. The offending command is ignored, and has no other side effect than to set the error flag.
	//		GL_INVALID_OPERATION		The specified operation is not allowed in the current state. The offending command is ignored, and has no other side effect than to set the error flag.
	//		GL_STACK_OVERFLOW			This command would cause a stack overflow. The offending command is ignored, and has no other side effect than to set the error flag.
	//		GL_STACK_UNDERFLOW			This command would cause a stack underflow. The offending command is ignored, and has no other side effect than to set the error flag.
	//		GL_OUT_OF_MEMORY			There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.
	inline GLenum glGetError()
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_glGetError();
		#elif defined( I3_WIPI_GP)
			return MC_glGetError();
		#else
			return ::glGetError();
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glGetLightv
	// Parameters :
	//	light 
	//		Specifies a light source.
	//		The number of possible lights depends on the implementation, but at least eight lights are supported.
	//		They are identified by symbolic names of the form GL_LIGHTi where 0 < i < GL_MAX_LIGHTS
	//	pname 
	//		Specifies a light source parameter for light.
	//		Accepted symbolic names are GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR, GL_EMISSION, GL_SPOT_DIRECTION, GL_SPOT_EXPONENT, GL_SPOT_CUTOFF, GL_CONSTANT_ATTENUATION, GL_LINEAR_ATTENUATION, and GL_QUADRATIC_ATTENUATION.
	//	params 
	//		Returns the requested data.
	inline void glGetLightfv( GLenum light, GLenum pname, GLfloat *params )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glGetLightfv( light, pname, params );
		#else
			::glGetLightfv( light, pname, params );
			//I3TRACE( "[I3OGL::glGetLightfv] Not support.\n" );
		#endif
	}
	inline void glGetLightxv( GLenum light, GLenum pname, GLfixed *params )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glGetLightxv( light, pname, params );
		#else
			::glGetLightxv( light, pname, params );
			//I3TRACE( "[I3OGL::glGetLightxv] Not support.\n" );
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glGetLightv(light, pname, params)			glGetLightxv( (GLenum) (light), (GLenum) (pname), (GLfixed *) (params) )
#else
	#define glGetLightv(light, pname, params)			glGetLightfv( (GLenum) (light), (GLenum) (pname), (GLfloat *) (params) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glGetMaterialv
	// Parameters :
	//	face 
	//		Specifies which of the two materials is being queried. GL_FRONT or GL_BACK are accepted, representing the front and back materials, respectively.
	//	pname 
	//		Specifies the material parameter to return. Accepted symbolic names are GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR, GL_EMISSION, and GL_SHININESS.
	//	params 
	//		Returns the requested data.
	inline void glGetMaterialfv( GLenum face, GLenum pname, GLfloat *params )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glGetmaterialfv( face, pname, params );
		#else
			I3TRACE( "[I3OGL::glGetMaterialfv] Not support.\n" );
		#endif
	}
	inline void glGetMaterialxv( GLenum face, GLenum pname, GLfixed *params )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glGetmaterialxv( face, pname, params );
		#else
			I3TRACE( "[I3OGL::glGetMaterialxv] Not support.\n" );
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glGetMaterialv(face, pname, params)			glGetMaterialxv( (GLenum) (face), (GLenum) (pname), (GLfixed *) (params) )
#else
	#define glGetMaterialv(face, pname, params)			glGetMaterialfv( (GLenum) (face), (GLenum) (pname), (GLfloat *) (params) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glGetPointerv
	// Parameters :
	//	pname 
	//		Specifies the array or buffer pointer to be returned. Accepted symbolic names are GL_COLOR_ARRAY_POINTER, GL_NORMAL_ARRAY_POINTER, GL_TEXTURE_COORD_ARRAY_POINTER, GL_VERTEX_ARRAY_POINTER, GL_MATRIX_INDEX_ARRAY_POINTER_OES, GL_POINT_SIZE_ARRAY_POINTER_OES, and GL_WEIGHT_ARRAY_POINTER_OES.
	//	params 
	//		Returns the pointer value specified by pname.
	inline void glGetPointerv( GLenum pname, GLvoid * params)
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glGetPointerv( pname, params );
		#else
			::glGetPointerv( pname, (void**)params );
			//I3TRACE( "[I3OGL::glGetPointerv] Not support.\n" );
		#endif
	}

	inline void glGetIntegerv(GLenum pname, GLint *params)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glGetIntegerv( pname, params);
		#elif defined( I3_WIPI_GP)
			MC_glGetIntegerv(pname, params);
		#else
			::glGetIntegerv(pname, params);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glGetString
	// Parameters :
	//	name 
	//		Specifies a symbolic constant, one of GL_VENDOR, GL_RENDERER, GL_VERSION, or GL_EXTENSIONS.
	inline const GLubyte * glGetString(GLenum name)
	{
		#if defined( I3_WIPI_GIGA)
			return OEMC_g3D_glGetString(name);
		#elif defined( I3_WIPI_GP)
			return MC_glGetString(name);
		#else
			return ::glGetString(name);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glGetTexEnv
	// Parameters :
	//	target 
	//		Specifies a texture environment, which must be GL_TEXTURE_ENV.
	//	pname 
	//		Specifies the symbolic name of a texture environment parameter. Which can be either GL_TEXTURE_ENV_MODE, GL_TEXTURE_ENV_COLOR, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T, or GL_GENERATE_MIPMAP.
	//	params 
	//		Returns the requested data.
	inline void glGetTexEnvfv( GLenum target, GLenum pname, GLfloat * params )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glGetTexEnvfv( target, pname, params );
		#else
			::glGetTexEnvfv( target, pname, params );
			//I3TRACE( "[I3OGL::glGetTexEnvfv] Not support.\n" );
		#endif
	}
	inline void glGetTexEnvxv( GLenum target, GLenum pname, GLfixed * params )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glGetTexEnvxv( target, pname, params );
		#else
			::glGetTexEnvxv( target, pname, params );
			//I3TRACE( "[I3OGL::glGetTexEnvxv] Not support.\n" );
		#endif
	}
	inline void glGetTexEnviv( GLenum target, GLenum pname, GLint * params )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glGetTexEnviv( target, pname, params );
		#else
			::glGetTexEnviv( target, pname, params );
			//I3TRACE( "[I3OGL::glGetTexEnviv] Not support.\n" );
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glGetTexEnv(target, pname, params)			glGetTexEnvxv( (GLenum) (target), (GLenum) (pname), (GLfixed *) (params) )
#else
	#define glGetTexEnv(target, pname, params)			glGetTexEnvfv( (GLenum) (target), (GLenum) (pname), (GLfloat *) (params) )
#endif
	#define glGetTexEnvi(target, pname, params)			glGetTexEnviv( (GLenum) (target), (GLenum) (pname), (GLint *) (params) )

	//////////////////////////////////////////////////////////////////////////
	// Name : glGetTexParameter
	// Parameters :
	//	target 
	//		Specifies the target texture, which must be GL_TEXTURE_2D.
	//	pname 
	//		Specifies the symbolic name of a texture parameter. Which can be one of the following: GL_TEXTURE_MIN_FILTER, GL_TEXTURE_MAG_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T, or GL_GENERATE_MIPMAP.
	//	params 
	//		Returns the texture parameters.
	inline void glGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params )
	{
		I3TRACE( "[I3OGL::glGetTexParameterfv] Not support.\n" );
		return;
	}
	inline void glGetTexParameterxv( GLenum target, GLenum pname, GLfixed *params )
	{
		I3TRACE( "[I3OGL::glGetTexParameterxv] Not support.\n" );
		return;
	}
#ifdef I3_FIXED_POINT
	#define glGetTexParameterv(target, pname, params)	glGetTexParameterxv( (GLenum) (target), (GLenum) (pname), (GLfixed *) (params) )
#else
	#define glGetTexParameterv(target, pname, params)	glGetTexParameterfv( (GLenum) (target), (GLenum) (pname), (GLfloat *) (params))
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glHint
	// Parameters :
	//	target 
	//		Specifies a symbolic constant indicating the behavior to be controlled. GL_FOG_HINT , GL_GENERATE_MIPMAP_HINT , GL_LINE_SMOOTH_HINT , GL_PERSPECTIVE_CORRECTION_HINT, and GL_POINT_SMOOTH_HINT are accepted.
	//	mode 
	//		Specifies a symbolic constant indicating the desired behavior. GL_FASTEST, GL_NICEST, and GL_DONT_CARE are accepted.
	inline void glHint(GLenum target, GLenum mode)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glHint( target, mode);
		#elif defined( I3_WIPI_GP)
			MC_glHint(target, mode);
		#else
			::glHint(target, mode);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glLight
	// Parameters :
	//	light 
	//		Specifies a light. The number of lights depends on the implementation, but at least eight lights are supported.
	//		They are identified by symbolic names of the form GL_LIGHTi where 0 ≤ i < GL_MAX_LIGHTS.
	//	pname 
	//		Specifies a single-valued light source parameter for light. GL_SPOT_EXPONENT, GL_SPOT_CUTOFF, GL_CONSTANT_ATTENUATION, GL_LINEAR_ATTENUATION, and GL_QUADRATIC_ATTENUATION are accepted.
	//	param 
	//		Specifies the value that parameter pname of light source light will be set to.
	inline void glLightf(GLenum light, GLenum pname, GLfloat param)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glLightx( light, pname, __RT(param));
		#elif defined( I3_WIPI_GP)
			MC_glLightf(light, pname, param);
		#else
			::glLightf(light, pname, param);
		#endif
	}
	inline void glLightx(GLenum light, GLenum pname, GLfixed param)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glLightx( light, pname, param);
		#elif defined( I3_WIPI_GP)
			MC_glLightx(light, pname, param);
		#else
			::glLightx(light, pname, param);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glLight(light, pname, param)	glLightx( (GLenum) (light), (GLenum) (pname), (GLfixed) (param) )
#else
	#define glLight(light, pname, param)	glLightf( (GLenum) (light), (GLenum) (pname), (GLfloat) (param) )
#endif

	inline void glLightfv(GLenum light, GLenum pname, const GLfloat *params)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glLightxv(light, pname, __RT(params));
		#elif defined( I3_WIPI_GP)
			MC_glLightfv(light, pname, params);
		#else
			::glLightfv(light, pname, params);
		#endif
	}
	inline void glLightxv(GLenum light, GLenum pname, const GLfixed *params)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glLightxv( light, pname, params);
		#elif defined( I3_WIPI_GP)
			MC_glLightxv(light, pname, params);
		#else
			::glLightxv(light, pname, params);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glLightv(light, pname, params)		glLightxv( (GLenum) (light), (GLenum) (pname), (const GLfixed *) (params) )
#else
	#define glLightv(light, pname, params)		glLightfv( (GLenum) (light), (GLenum) (pname), (const GLfloat *) (params) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glLightModel
	// Parameters :
	//	pname 
	//		Specifies a single-valued lighting model parameter. Must be GL_LIGHT_MODEL_TWO_SIDE.
	//	param 
	//		Specifies the value that param will be set to.
	inline void glLightModelf(GLenum pname, GLfloat param)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glLightModelx( pname, __RT(param));
		#elif defined( I3_WIPI_GP)
			MC_glLightModelf(pname, param);
		#else
			::glLightModelf(pname, param);
		#endif
	}
	inline void glLightModelx(GLenum pname, GLfixed param)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glLightModelx( pname, param);
		#elif defined( I3_WIPI_GP)
			MC_glLightModelx(pname, param);
		#else
			::glLightModelx(pname, param);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glLightModel(pname, param)		glLightModelx( (GLenum) (pname), (GLfixed) (param) )
#else
	#define glLightModel(pname, param)		glLightModelf( (GLenum) (pname), (GLfloat) (param) )
#endif

	inline void glLightModelfv(GLenum pname, const GLfloat *params)
	{
		#if defined( I3_WIPI_GIGA)
		"This function can't apply with GIGA."
		#elif defined( I3_WIPI_GP)
			MC_glLightModelfv(pname, params);
		#else
			::glLightModelfv(pname, params);
		#endif
	}
	inline void glLightModelxv(GLenum pname, const GLfixed *params)
	{
		#if defined( I3_WIPI_GIGA)
		"This function can't apply for GIGA."
		#elif defined( I3_WIPI_GP)
			MC_glLightModelxv(pname, params);
		#else
			::glLightModelxv(pname, params);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glLightModelv(pname, params)		glLightModelxv( (GLenum) (pname), (const GLfixed *) (params) )
#else
	#define glLightModelv(pname, params)		glLightModelfv( (GLenum) (pname), (const GLfloat *) (params) )
#endif
	
	//////////////////////////////////////////////////////////////////////////
	// Name : glLineWidth
	// Parameters :
	//	width 
	//		Specifies the width of rasterized lines.
	//		The initial value is 1.
	inline void glLineWidthf(GLfloat width)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glLineWidthx( __RT(width));
		#elif defined( I3_WIPI_GP)
			MC_glLineWidth(width);
		#else
			::glLineWidth(width);
		#endif
	}
	inline void glLineWidthx(GLfixed width)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glLineWidthx(width);	
		#elif defined( I3_WIPI_GP)
			MC_glLineWidthx(width);
		#else
			::glLineWidthx(width);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glLineWidth(width)		glLineWidthx( (GLfixed) (width) )
#else
	#define glLineWidth(width)		glLineWidthf( (GLfloat) (width) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glLoadIdentity
	// Desc :
	//		glLoadIdentity replaces the current matrix with the identity matrix.
	//		It is semantically equivalent to calling glLoadMatrix with the identity matrix
	//		( 1 0 0 0 
	//		  0 1 0 0 
	//		  0 0 1 0 
	//		  0 0 0 1 ) 
	inline void glLoadIdentity()
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glLoadIdentity();
		#elif defined( I3_WIPI_GP)
			MC_glLoadIdentity();
		#else
			::glLoadIdentity();
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glloadMatrix
	// Parameters :
	//	m 
	//		Specifies a pointer to 16 consecutive values, which are used as the elements of a 4 × 4 column-major matrix.
	// Desc :
	//		M (v)  =	m [0]  m [4]  m [8]	  m [12]		v [0]
	//					m [1]  m [5]  m [9]	  m [13]		v [1]
	//				  (	m [2]  m [6]  m [10]  m [14] ) × (	v [2] )
	//					m [3]  m [7]  m [11]  m [15]		v [3]
	//
	inline void glLoadMatrixf(const GLfloat *m)
	{
		#if defined( I3_WIPI_GIGA)
			"This function can't apply for GIGA."
			MC_glLoadMatrixf(m);
		#else
			::glLoadMatrixf( m );
		#endif
	}
	inline void glLoadMatrixx(const GLfixed *m)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glLoadMatrixx(m);
		#elif defined( I3_WIPI_GP)
			MC_glLoadMatrixx(m);
		#else
			::glLoadMatrixx(m);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glLoadMatrix(m)	glLoadMatrixx( (const GLfixed *) (m) )
#else
	#define glLoadMatrix(m)	glLoadMatrixf( (const GLfloat *) (m) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glLoadPaletteFromModelViewMatrix (OpenGL ES 1.1)
	// Desc : OpenGL-ES 1.1v support
	inline void glLoadPaletteFromModelViewMatrixOES(void)
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glLoadPaletteFromModelViewMatrixOES();
		#else
			I3TRACE( "[I3OGL::glLoadPaletteFromModelViewMatrixOES] Not support.\n" );
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glLogicOp
	// Parameters :
	//	opcode 
	//		Specifies a symbolic constant that selects a logical operation. The following symbols are accepted: GL_CLEAR, GL_SET, GL_COPY, GL_COPY_INVERTED, GL_NOOP, GL_INVERT, GL_AND, GL_NAND, GL_OR, GL_NOR, GL_XOR, GL_EQUIV, GL_AND_REVERSE, GL_AND_INVERTED, GL_OR_REVERSE, and GL_OR_INVERTED. The initial value is GL_COPY.
	// Desc :
	//		GL_CLEAR			0
	//		GL_SET				1
	//		GL_COPY				s
	//		GL_COPY_INVERTED	~ s
	//		GL_NOOP				d
	//		GL_INVERT			~ d
	//		GL_AND				s & d
	//		GL_NAND				~ (s & d)
	//		GL_OR				s | d
	//		GL_NOR				~ (s | d)
	//		GL_XOR				s ^ d
	//		GL_EQUIV			~ (s ^ d)
	//		GL_AND_REVERSE		s & ~ d
	//		GL_AND_INVERTED		~ s & d
	//		GL_OR_REVERSE		s | ~ d
	//		GL_OR_INVERTED		~ s | d
	inline void glLogicOp(GLenum op)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glLogicOp(op);
		#elif defined( I3_WIPI_GP)
			MC_glLogicOp(op);
		#else
			::glLogicOp(op);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glMatrial
	// Parameters :
	//	face 
	//		Specifies which face or faces are being updated. Must be GL_FRONT_AND_BACK.
	//	pname 
	//		Specifies the single-valued material parameter of the face or faces that is being updated. Must be GL_SHININESS.
	//	param 
	//		Specifies the value that parameter GL_SHININESS will be set to.
	inline void glMaterialf(GLenum face, GLenum pname, GLfloat param)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glMaterialx( face, pname, __RT(param));
		#elif defined( I3_WIPI_GP)
			MC_glMaterialf(face, pname, param);
		#else
			::glMaterialf(face, pname, param);
		#endif
	}
	inline void glMaterialx(GLenum face, GLenum pname, GLfixed param)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glMaterialx( face, pname, param);
		#elif defined( I3_WIPI_GP)
			MC_glMaterialx(face, pname, param);
		#else
			::glMaterialx(face, pname, param);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glMaterial(face, pname, param)		glMaterialx( (GLenum) (face), (GLenum) (pname), (GLfixed) (param) )
#else
	#define glMaterial(face, pname, param)		glMaterialf( (GLenum) (face), (GLenum) (pname), (GLfloat) (param) )
#endif

	inline void glMaterialfv(GLenum face, GLenum pname, const GLfloat *params)
	{
		#if defined( I3_WIPI_GIGA)
			"This function can't apply for GIGA."
		#elif defined( I3_WIPI_GP)
			MC_glMaterialfv(face, pname, params);
		#else
			::glMaterialfv(face, pname, params);
		#endif
	}
	inline void glMaterialxv(GLenum face, GLenum pname, const GLfixed *params)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glMaterialxv( face, pname, params);
		#elif defined( I3_WIPI_GP)
			MC_glMaterialxv(face, pname, params);
		#else
			::glMaterialxv(face, pname, params);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glMaterialv(face, pname, params)		glMaterialxv( (GLenum) (face), (GLenum) (pname), (const GLfixed *) (params) )
#else
	#define glMaterialv(face, pname, params)		glMaterialfv( (GLenum) (face), (GLenum) (pname), (const GLfloat *) (params) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glMatrixIndexPointer (OpenGL ES 1.1)
	// Parameters :
	//	size 
	//		Specifies the number of matrix indices per vertex. Must be is less than or equal to GL_MAX_VERTEX_UNITS_OES. The initial value is 0.
	//	type 
	//		Specifies the data type of each matrix index in the array. Symbolic constant GL_UNSIGNED_BYTE is accepted. The initial value is GL_UNSIGNED_BYTE.
	//	stride 
	//		Specifies the byte offset between consecutive matrix indices. If stride is 0, the matrix indices are understood to be tightly packed in the array. The initial value is 0.
	//	pointer 
	//		Specifies a pointer to the first matrix index of the first vertex in the array. The initial value is 0.
	inline void glMatrixIndexPointerOES( GLint SIZE, GLenum TYPE, GLsizei stride, const GLvoid *pointer )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glMatrixIndexPointerOES( SIZE, TYPE, stride, pointer );
		#else
			::glMatrixIndexPointerOES( SIZE, TYPE, stride, pointer );
			//I3TRACE( "[I3OGL::glMatrixIndexPointerOES] Not support.\n" );
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glMatrixMode
	// Parameters :
	//	mode 
	//		Specifies which matrix stack is the target for subsequent matrix operations.
	//		These values are accepted: GL_MODELVIEW, GL_PROJECTION, GL_TEXTURE, and GL_MATRIX_PALETTE_OES.
	//		The initial value is GL_MODELVIEW.
	inline void glMatrixMode( GLenum mode)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glMatrixMode( mode);
		#elif defined( I3_WIPI_GP)
			MC_glMatrixMode(mode);
		#else
			::glMatrixMode(mode);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glMultMatrix
	// Parameters :
	//	m 
	//		Points to 16 consecutive values that are used as the elements of a 4 × 4 column-major matrix.
	// Desc : C - current matrix, M - argument, v - transform
	//		(C × M) × v
	inline void glMultMatrixf(const GLfloat *m)
	{
		#if defined( I3_WIPI_GIGA)
			"This function can't apply for GIGA."
		#elif defined( I3_WIPI_GP)
			MC_glMultMatrixf(m);
		#else
			::glMultMatrixf(m);
		#endif
	}
	inline void glMultMatrixx(const GLfixed *m)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glMultMatrixx(m);
		#elif defined( I3_WIPI_GP)
			MC_glMultMatrixx(m);
		#else
			::glMultMatrixx(m);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glMultMatrix(m)	glMultMatrixx( (const GLfixed *) (m) )
#else
	#define glMultMatrix(m)	glMultMatrixf( (const GLfloat *) (m) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glMultiTexCoord4
	// Parameters :
	//	target 
	//		Specifies texture unit whose coordinates should be modified.
	//		The number of texture units is implementation dependent, but must be at least one.
	//		Must be one of GL_TEXTUREi, where 0 ≤ i < GL_MAX_TEXTURE_UNITS, which is an implementation-dependent value.
	//	s, t, r, q 
	//		Specify s, t, r, and q texture coordinates for target texture unit.
	//		The initial value is (0, 0, 0, 1).
	inline void glMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glMultiTexCoord4x( target, __RT(s), __RT(t), __RT(r), __RT(q));
		#elif defined( I3_WIPI_GP)
			MC_glMultiTexCoord4f(target, s, t, r, q);
		#else
			::glMultiTexCoord4f(target, s, t, r, q);
		#endif
	}
	inline void glMultiTexCoord4x(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glMultiTexCoord4x( target, s, t, r, q);
		#elif defined( I3_WIPI_GP)
			MC_glMultiTexCoord4x(target, s, t, r, q);
		#else
			::glMultiTexCoord4x(target, s, t, r, q);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glMultiTexCoord4(target, s, t, r, q)	glMultiTexCoord4x( (GLenum) (target), (GLfixed) (s), (GLfixed) (t), (GLfixed) (r), (GLfixed) (q) )
#else
	#define glMultiTexCoord4(target, s, t, r, q)	glMultiTexCoord4f( (GLenum) (target), (GLfloat) (s), (GLfloat) (t), (GLfloat) (r), (GLfloat) (q) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glNormal3
	// Parameters :
	//	nx, ny, nz 
	//		Specify the x, y, and z coordinates of the new current normal.
	//		The initial value is (0, 0, 1).
	inline void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glNormal3x( __RT(nx), __RT(ny), __RT(nz));
		#elif defined( I3_WIPI_GP)
			MC_glNormal3f(nx, ny, nz);
		#else
			::glNormal3f(nx, ny, nz);
		#endif
	}
	inline void glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glNormal3x( nx, ny, nz);
		#elif defined( I3_WIPI_GP)
			MC_glNormal3x(nx, ny, nz);
		#else
			::glNormal3x(nx, ny, nz);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glNormal3(nx, ny, nz)	glNormal3x( (GLfixed) (nx), (GLfixed) (ny), (GLfixed) (nz) )
#else
	#define glNormal3(nx, ny, nz)	glNormal3f( (GLfloat) (nx), (GLfloat) (ny), (GLfloat) (nz) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glNormalPointer
	// Parameters :
	//	type 
	//		Specifies the data type of each coordinate in the array. Symbolic constants GL_BYTE, GL_SHORT, and GL_FIXED are accepted.
	//		However, the initial value is GL_FLOAT.
	//		The common profile accepts the symbolic constant GL_FLOAT as well.
	//	stride 
	//		Specifies the byte offset between consecutive normals.
	//		If stride is 0, the normals are understood to be tightly packed in the array.
	//		The initial value is 0.
	//	pointer 
	//		Specifies a pointer to the first coordinate of the first normal in the array.
	//		The initial value is 0.
	inline void glNormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glNormalPointer( type, stride, pointer);
		#elif defined( I3_WIPI_GP)
			MC_glNormalPointer(type, stride, pointer);
		#else
			::glNormalPointer(type, stride, pointer);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glOrtho
	// Parameters :
	//	left, right 
	//		Specify the coordinates for the left and right vertical clipping planes.
	//	bottom, top 
	//		Specify the coordinates for the bottom and top horizontal clipping planes.
	//	near, far 
	//		Specify the distances to the nearer and farther depth clipping planes.
	//		These values are negative if the plane is to be behind the viewer.
	inline void glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat gnear, GLfloat gfar)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glOrthox( __RT(left), __RT(right), __RT(bottom), __RT(top), __RT(gnear), __RT(gfar));
		#elif defined( I3_WIPI_GP)
			MC_glOrthof(left, right, bottom, top, gnear, gfar);
		#else
			::glOrthof(left, right, bottom, top, gnear, gfar);
		#endif
	}
	inline void glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed gnear, GLfixed gfar)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glOrthox( left, right, bottom, top, gnear, gfar);
		#elif defined( I3_WIPI_GP)
			MC_glOrthox(left, right, bottom, top, gnear, gfar);
		#else
			::glOrthox(left, right, bottom, top, gnear, gfar);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glOrtho(left, right, bottom, top, gnear, gfar)		glOrthox( (GLfixed) (left), (GLfixed) (right), (GLfixed) (bottom), (GLfixed) (top), (GLfixed) (gnear), (GLfixed) (gfar) )
#else
	#define glOrtho(left, right, bottom, top, gnear, gfar)		glOrthof( (GLfloat) (left), (GLfloat) (right), (GLfloat) (bottom), (GLfloat) (top), (GLfloat) (gnear), (GLfloat) (gfar) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glPixelStorei
	// Parameters :
	//	pname 
	//		Specifies the symbolic name of the parameter to be set.
	//		GL_PACK_ALIGNMENT affects the packing of pixel data into memory.
	//		GL_UNPACK_ALIGNMENT affects the unpacking of pixel data from memory.
	//	param 
	//		Specifies the value that pname is set to.
	inline void glPixelStorei(GLenum pname, GLint param)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glPixelStorei( pname, param);
		#elif defined( I3_WIPI_GP)
			MC_glPixelStorei(pname, param);
		#else
			::glPixelStorei(pname, param);
		#endif
	}
	
	//////////////////////////////////////////////////////////////////////////
	// Name : glPointParameter
	// Parameters :
	//	pname 
	//		Specifies the single-valued parameter to be updated. Can be either GL_POINT_SIZE_MIN, GL_POINT_SIZE_MAX, or GL_POINT_FADE_THRESHOLD_SIZE.
	//	param 
	//		Specifies the value that the parameter will be set to.
	inline void glPointParameterf( GLenum pname, GLfloat param )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glPointParameterf( pname, param );
		#else
			::glPointParameterf( pname, param );
			//I3TRACE( "[I3OGL::glPointParameterf] Not support.\n" );
		#endif
	}
	inline void glPointParameterx( GLenum pname, GLfixed param )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glPointParameterx( panem, param );
		#else
			::glPointParameterx( pname, param );
			//I3TRACE( "[I3OGL::glPointParameterx] Not support.\n" );
		#endif
	}
	inline void glPointParameterfv( GLenum pname, const GLfloat *params )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glPointParameterfv( pname, params );
		#else
			::glPointParameterfv( pname, params );
			//I3TRACE( "[I3OGL::glPointParameterfv] Not support.\n" );
		#endif
	}
	inline void glPointParameterxv( GLenum pname, const GLfixed *params )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glPointParameterxv( pname, params );
		#else
			::glPointParameterxv( pname, params );
			//I3TRACE( "[I3OGL::glPointParameterxv] Not support.\n" );
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glPointParameter(pname, param)		glPointParameterx( (GLenum) (pname), (GLfixed) param )
	#define glPointParameterv(pname, params)		glPointParameterxv( (GLenum) (pname), (GLfixed *) (params) )
#else
	#define glPointParameter(pname, param)		glPointParameterf( (GLenum) (pname), (GLfloat) param )
	#define glPointParameterv(pname, params)		glPointParameterfv( (GLenum) (pname), (GLfloat *) (params) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glPointSize
	// Parameters
	//	size 
	//		Specifies the diameter of rasterized points.
	//		The initial value is 1.
	inline void glPointSizef(GLfloat size)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glPointSizex( __RT(size));
		#elif defined( I3_WIPI_GP)
			MC_glPointSize(size);
		#else
			::glPointSize(size);
		#endif
	}
	inline void glPointSizex(GLfixed size)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glPointSizex(size);
		#elif defined( I3_WIPI_GP)
			MC_glPointSizex(size);
		#else
			::glPointSizex(size);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glPointSize(size)		glPointSizex( (GLfixed) (size) )
#else
	#define glPointSize(size)		glPointSizef( (GLfloat) (size) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glPolygonOffset
	// Parameters :
	//	factor 
	//		Specifies a scale factor that is used to create a variable depth offset for each polygon.
	//		The initial value is 0.
	//	units 
	//		Is multiplied by an implementation-specific value to create a constant depth offset.
	//		The initial value is 0.
	inline void glPolygonOffsetf(GLfloat factor, GLfloat units)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glPolygonOffsetx( __RT(factor), __RT(units));
		#elif defined( I3_WIPI_GP)
			MC_glPolygonOffset(factor, units);
		#else
			::glPolygonOffset(factor, units);
		#endif
	}
	inline void glPolygonOffsetx(GLfixed factor, GLfixed units)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glPolygonOffsetx(factor, units);
		#elif defined( I3_WIPI_GP)
			MC_glPolygonOffsetx(factor, units);
		#else
			::glPolygonOffsetx(factor, units);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glPolygonOffset(factor, units)		glPolygonOffsetx( (GLfixed) (factor), (GLfixed) (units) )
#else
	#define glPolygonOffset(factor, units)		glPolygonOffsetf( (GLfloat) (factor), (GLfloat) (units) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glPopMatrix
	// Desc :
	//		glPopMatrix pops the current matrix stack, replacing the current matrix with the one below it on the stack.
	inline void glPopMatrix()
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glPopMatrix();
		#elif defined( I3_WIPI_GP)
			MC_glPopMatrix();
		#else
			::glPopMatrix();
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glPushMatrix
	// Desc :
	//		glPushMatrix pushes the current matrix stack down by one, duplicating the current matrix.
	//		That is, after a glPushMatrix call, the matrix on top of the stack is identical to the one below it.
	inline void glPushMatrix()
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glPushMatrix();
		#elif defined( I3_WIPI_GP)
			MC_glPushMatrix();
		#else
			::glPushMatrix();
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glReadPixels
	// Parameters :
	//	x, y 
	//		Specify the window coordinates of the first pixel that is read from the color buffer.
	//		This location is the lower left corner of a rectangular block of pixels.
	//	width, height 
	//		Specify the dimensions of the pixel rectangle. width and height of one correspond to a single pixel.
	//	format 
	//		Specifies the format of the pixel data.
	//		Must be either GL_RGBA or the value of GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES.
	//	type 
	//		Specifies the data type of the pixel data.
	//		Must be either GL_UNSIGNED_BYTE or the value of GL_IMPLEMENTATION_COLOR_READ_TYPE_OES.
	//	pixels 
	//		Returns the pixel data.
	inline void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glReadPixels( x, y, width, height, format, type, pixels);
		#elif defined( I3_WIPI_GP)
			MC_glReadPixels(x, y, width, height, format, type, pixels);
		#else
			::glReadPixels(x, y, width, height, format, type, pixels);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glRotate
	// Parameters :
	//	angle 
	//		Specifies the angle of rotation, in degrees.
	//	x, y, z 
	//		Specify the x, y, and z coordinates of a vector, respectively.
	inline void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glRotatex( __RT(angle), __RT(x), __RT(y), __RT(z));
		#elif defined( I3_WIPI_GP)
			MC_glRotatef(angle, x, y, z);
		#else
			::glRotatef(angle, x, y, z);
		#endif
	}
	inline void glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glRotatex( angle, x, y, z);
		#elif defined( I3_WIPI_GP)
			MC_glRotatex(angle, x, y, z);
		#else
			::glRotatex(angle, x, y, z);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glRotate(angle, x, y, z)		glRotatex( (GLfixed) (angle), (GLfixed) (x), (GLfixed) (y), (GLfixed) (z) )
#else
	#define glRotate(angle, x, y, z)		glRotatef( (GLfloat) (angle), (GLfloat) (x), (GLfloat) (y), (GLfloat) (z) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glSampleCoverage
	// Parameters :
	//	value 
	//		Specifies the coverage of the modification mask.
	//		The value is clamped to the range [0, 1], where 0 represents no coverage and 1 full coverage.
	//		The initial value is 1.
	//	invert 
	//		Specifies whether the modification mask implied by value is inverted or not.
	//		The initial value is GL_FALSE.
	inline void glSampleCoveragef(GLclampf value, GLboolean invert)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glSampleCoveragex( __RT(value), invert);
		#elif defined( I3_WIPI_GP)
			MC_glSampleCoverage(value, invert);
		#else
			::glSampleCoverage(value, invert);
		#endif
	}
	inline void glSampleCoveragex(GLclampx value, GLboolean invert)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glSampleCoveragex( value, invert);
		#elif defined( I3_WIPI_GP)
			MC_glSampleCoveragex(value, invert);
		#else
			::glSampleCoveragex(value, invert);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glSampleCoverage(value, invert)		glSampleCoveragex( (GLclampx) (value), (GLboolean) (invert) )
#else
	#define glSampleCoverage(value, invert)		glSampleCoveragef( (GLclampf) (value), (GLboolean) (invert) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glScale
	// Parameters :
	//	x, y, z 
	//		Specify scale factors along the x, y, and z axes, respectively.
	inline void glScalef(GLfloat x, GLfloat y, GLfloat z)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glScalex( __RT(x), __RT(y), __RT(z));
		#elif defined( I3_WIPI_GP)
			MC_glScalef(x, y, z);
		#else
			::glScalef(x, y, z);
		#endif
	}
	inline void glScalex(GLfixed x, GLfixed y, GLfixed z)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glScalex( x, y, z);
		#elif defined( I3_WIPI_GP)
			MC_glScalex(x, y, z);
		#else
			::glScalex(x, y, z);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glScale(x, y, z)		glScalex( (GLfixed) (x), (GLfixed) (y), (GLfixed) (z) )
#else
	#define glScale(x, y, z)		glScalef( (GLfloat) (x), (GLfloat) (y), (GLfloat) (z) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glScissor
	// Parameters :
	//	x, y 
	//		Specify the lower left corner of the scissor box, in pixels.
	//		The initial value is (0, 0).
	//	width, height 
	//		Specify the width and height of the scissor box.
	//		When a GL context is first attached to a surface (e.g. window), width and height are set to the dimensions of that surface.
	inline void glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glScissor( x, y, width, height);
		#elif defined( I3_WIPI_GP)
			MC_glScissor(x, y, width, height);
		#else
			::glScissor(x, y, width, height);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glShadeModel
	// Parameters :
	//	mode 
	//		Specifies a symbolic value representing a shading technique.
	//		Accepted values are GL_FLAT and GL_SMOOTH.
	//		The initial value is GL_SMOOTH.
	inline void glShadeModel(GLenum mode)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glShadeModel( mode);
		#elif defined( I3_WIPI_GP)
			MC_glShadeModel(mode);
		#else
			::glShadeModel(mode);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glStencilFunc
	// Parameters :
	//	func 
	//		Specifies the test function.
	//		Eight tokens are valid: GL_NEVER, GL_LESS, GL_LEQUAL, GL_GREATER, GL_GEQUAL, GL_EQUAL, GL_NOTEQUAL, and GL_ALWAYS.
	//		The initial value is GL_ALWAYS.
	//	ref 
	//		Specifies the reference value for the stencil test. ref is clamped to the range [ 0 , 2n - 1 ] , where n is the number of bitplanes in the stencil buffer.
	//		The initial value is 0.
	//	mask 
	//		Specifies a mask that is ANDed with both the reference value and the stored stencil value when the test is done.
	//		The initial value is all 1's.
	inline void glStencilFunc(GLenum func, GLint ref, GLuint mask)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glStencilFunc( func, ref, mask);
		#elif defined( I3_WIPI_GP)
			MC_glStencilFunc(func, ref, mask);
		#else
			::glStencilFunc(func, ref, mask);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glStencilMask
	// Parameters :
	//	mask 
	//		Specifies a bit mask to enable and disable writing of individual bits in the stencil planes.
	//		The initial value is all 1's.
	inline void glStencilMask(GLuint mask)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glStencilMask( mask);
		#elif defined( I3_WIPI_GP)
			MC_glStencilMask(mask);
		#else
			::glStencilMask(mask);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glStencilOp
	// Parameters :
	//	fail 
	//		Specifies the action to take when the stencil test fails.
	//		Six symbolic constants are accepted: GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, GL_DECR, and GL_INVERT.
	//		The initial value is GL_KEEP.
	//	zfail 
	//		Specifies the stencil action when the stencil test passes, but the depth test fails.
	//		zfail accepts the same symbolic constants as fail.
	//		The initial value is GL_KEEP.
	//	zpass 
	//		Specifies the stencil action when both the stencil test and the depth test pass, or when the stencil test passes and either there is no depth buffer or depth testing is not enabled.
	//		zpass accepts the same symbolic constants as fail.
	//		The initial value is GL_KEEP.
	inline void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glStencilOp( fail, zfail, zpass);
		#elif defined( I3_WIPI_GP)
			MC_glStencilOp(fail, zfail, zpass);
		#else
			::glStencilOp(fail, zfail, zpass);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glTexCoordPointer
	// Parameters :
	//	size 
	//		Specifies the number of coordinates per array element. Must be 2, 3 or 4.
	//		The initial value is 4.
	//	type 
	//		Specifies the data type of each texture coordinate.
	//		Symbolic constants GL_BYTE, GL_SHORT, and GL_FIXED are accepted.
	//		However, the initial value is GL_FLOAT.
	//		The common profile accepts the symbolic constant GL_FLOAT as well.
	//	stride 
	//		Specifies the byte offset between consecutive array elements.
	//		If stride is 0, the array elements are understood to be tightly packed.
	//		The initial value is 0.
	//	pointer 
	//		Specifies a pointer to the first coordinate of the first element in the array.
	//		The initial value is 0.
	inline void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glTexCoordPointer( size, type, stride, pointer);
		#elif defined( I3_WIPI_GP)
			MC_glTexCoordPointer(size, type, stride, pointer);
		#else
			::glTexCoordPointer(size, type, stride, pointer);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glTexEnv
	// Parameters :
	//	target 
	//		Specifies a texture environment.
	//		Can be either GL_TEXTURE_ENV or GL_POINT_SPRITE_OES.
	//	pname 
	//		Specifies the symbolic name of a single-valued texture environment parameter.
	//		Must be one of GL_TEXTURE_ENV_MODE, GL_TEXTURE_ENV_COLOR, GL_COMBINE_RGB, GL_COMBINE_ALPHA, or GL_COORD_REPLACE_OES.
	//	param 
	//		Specifies a single symbolic constant, one of GL_REPLACE, GL_MODULATE, GL_DECAL, GL_BLEND, or GL_ADD.
	inline void glTexEnvf(GLenum target, GLenum pname, GLfloat param)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glTexEnvx( target, pname, __RT(param));
		#elif defined( I3_WIPI_GP)
			MC_glTexEnvf(target, pname, param);
		#else
			::glTexEnvf(target, pname, param);
		#endif
	}
	inline void glTexEnvx(GLenum target, GLenum pname, GLfixed param)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glTexEnvx( target, pname, param);
		#elif defined( I3_WIPI_GP)
			MC_glTexEnvx(target, pname, param);
		#else
			::glTexEnvx(target, pname, param);
		#endif
	}
	inline void glTexEnvfv(GLenum target, GLenum pname, const GLfloat *params)
	{
		#if defined( I3_WIPI_GIGA)
			"This function can't apply for GIGA."
		#elif defined( I3_WIPI_GP)
			MC_glTexEnvfv(target, pname, params);
		#else
			::glTexEnvfv(target, pname, params);
		#endif
	}
	inline void glTexEnvxv(GLenum target, GLenum pname, const GLfixed *params)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glTexEnvxv( target, pname, params);
		#elif defined( I3_WIPI_GP)
			MC_glTexEnvxv(target, pname, params);
		#else
			::glTexEnvxv(target, pname, params);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glTexEnv(target, pname, param)		glTexEnvx( (GLenum) (target), (GLenum) (pname), (GLfixed) (param) )
	#define glTexEnvv(target, pname, params)	glTexEnvxv( (GLenum) (target), (GLenum) (pname), (const GLfixed *) (params) )
#else
	#define glTexEnv(target, pname, param)		glTexEnvf( (GLenum) (target), (GLenum) (pname), (GLfloat) (param) )
	#define glTexEnvv(target, pname, params)	glTexEnvfv( (GLenum) (target), (GLenum) (pname), (const GLfloat *) (params) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glTexImage2D
	// Parameters :
	//	target 
	//		Specifies the target texture.
	//		Must be GL_TEXTURE_2D.
	//	level 
	//		Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image.
	//		Must be greater or equal 0.
	//	internalformat 
	//		Specifies the color components in the texture. Must be same as format.
	//		The following symbolic values are accepted: GL_ALPHA, GL_RGB, GL_RGBA, GL_LUMINANCE, or GL_LUMINANCE_ALPHA.
	//	width 
	//		Specifies the width of the texture image. Must be 2n + 2border for some integer n.
	//		All implementations support texture images that are at least 64 texels wide.
	//	height 
	//		Specifies the height of the texture image. Must be 2m + 2border for some integer m.
	//		All implementations support texture images that are at least 64 texels high.
	//	border 
	//		Specifies the width of the border. Must be 0.
	//	format 
	//		Specifies the format of the pixel data. Must be same as internalformat.
	//		The following symbolic values are accepted: GL_ALPHA, GL_RGB, GL_RGBA, GL_LUMINANCE, and GL_LUMINANCE_ALPHA.
	//	type 
	//		Specifies the data type of the pixel data.
	//		The following symbolic values are accepted: GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_4_4_4_4, and GL_UNSIGNED_SHORT_5_5_5_1.
	//	pixels 
	//		Specifies a pointer to the image data in memory.
	inline void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
		#elif defined( I3_WIPI_GP)
			MC_glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
		#else
			::glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glTexParameter
	// Parameters :
	//	target 
	//		Specifies the target texture, which must be GL_TEXTURE_2D.
	//	pname 
	//		Specifies the symbolic name of a single-valued texture parameter.
	//		Which can be one of the following: GL_TEXTURE_MIN_FILTER, GL_TEXTURE_MAG_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T, or GL_GENERATE_MIPMAP.
	//	param 
	//		Specifies the value of pname.
	inline void glTexParameterf(GLenum target, GLenum pname, GLfloat param)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glTexParameterx( target, pname, __RT(param));
		#elif defined( I3_WIPI_GP)
			MC_glTexParameterf(target, pname, param);
		#else
			::glTexParameterf(target, pname, param);
		#endif
	}
	inline void glTexParameterx(GLenum target, GLenum pname, GLfixed param)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glTexParameterx( target, pname, param);
		#elif defined( I3_WIPI_GP)
			MC_glTexParameterx(target, pname, param);
		#else
			::glTexParameterx(target, pname, param);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glTexParameter(target, pname, param)		glTexParameterx( (GLenum) (target), (GLenum) (pname), (GLfixed) (param) )
#else
	#define glTexParameter(target, pname, param)		glTexParameterf( (GLenum) (target), (GLenum) (pname), (GLfloat) (param) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glTexSubImage2D
	// Parameters :
	//	target 
	//		Specifies the target texture. Must be GL_TEXTURE_2D.
	//	level 
	//		Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image.
	//	xoffset 
	//		Specifies a texel offset in the x direction within the texture array.
	//	yoffset 
	//		Specifies a texel offset in the y direction within the texture array.
	//	width 
	//		Specifies the width of the texture subimage.
	//	height 
	//		Specifies the height of the texture subimage.
	//	format 
	//		Specifies the of the pixel data.
	//		The following symbolic values are accepted: GL_ALPHA, GL_RGB, GL_RGBA, GL_LUMINANCE, and GL_LUMINANCE_ALPHA.
	//	type 
	//		Specifies the data type of the pixel data.
	//		The following symbolic values are accepted: GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_4_4_4_4, and GL_UNSIGNED_SHORT_5_5_5_1.
	//	pixels 
	//		Specifies a pointer to the image data in memory.
	inline void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
		#elif defined( I3_WIPI_GP)
			MC_glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
		#else
			::glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glTranslate
	// Parameters :
	//	x, y, z 
	//		Specify the x, y, and z coordinates of a translation vector.
	inline void glTranslatef(GLfloat x, GLfloat y, GLfloat z)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glTranslatex( __RT(x), __RT(y), __RT(z));
		#elif defined( I3_WIPI_GP)
			MC_glTranslatef(x, y, z);
		#else
			::glTranslatef(x, y, z);
		#endif
	}
	inline void glTranslatex(GLfixed x, GLfixed y, GLfixed z)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glTranslatex( x, y, z);
		#elif defined( I3_WIPI_GP)
			MC_glTranslatex(x, y, z);
		#else
			::glTranslatex(x, y, z);
		#endif
	}
#ifdef I3_FIXED_POINT
	#define glTranslate(x, y, z)		glTranslatex( (GLfixed) (x), (GLfixed) (y), (GLfixed) (z) )
#else
	#define glTranslate(x, y, z)		glTranslatef( (GLfloat) (x), (GLfloat) (y), (GLfloat) (z) )
#endif

	//////////////////////////////////////////////////////////////////////////
	// Name : glVertexPointer
	// Parameters :
	//	size 
	//		Specifies the number of coordinates per vertex. Must be 2, 3, or 4.
	//		The initial value is 4.
	//	type 
	//		Specifies the data type of each vertex coordinate in the array. Symbolic constants GL_BYTE, GL_SHORT, and GL_FIXED, are accepted.
	//		However, the initial value is GL_FLOAT.
	//		The common profile accepts the symbolic constant GL_FLOAT as well.
	//	stride 
	//		Specifies the byte offset between consecutive vertices.
	//		If stride is 0, the vertices are understood to be tightly packed in the array.
	//		The initial value is 0.
	//	pointer 
	//		Specifies a pointer to the first coordinate of the first vertex in the array.
	//		The initial value is 0.
	inline void glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glVertexPointer( size, type, stride, pointer);
		#elif defined( I3_WIPI_GP)
			MC_glVertexPointer(size, type, stride, pointer);
		#else
			::glVertexPointer(size, type, stride, pointer);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glViewport
	// Parameters :
	//	x, y 
	//		Specify the lower left corner of the viewport rectangle, in pixels.
	//		The initial value is (0, 0).
	//	width, height 
	//		Specify the width and height of the viewport.
	//		When a GL context is first attached to a surface (e.g. window), width and height are set to the dimensions of that surface.
	inline void glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glViewport(x, y, width, height);
		#elif defined( I3_WIPI_GP)
			MC_glViewport(x, y, width, height);
		#else
			::glViewport(x, y, width, height);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Name : glWeightPointerOES (OpenGL ES 1.1)
	// Parameters :
	//	size 
	//		Specifies the number of weights per vertex. Must be is less than or equal to GL_MAX_VERTEX_UNITS_OES. The initial value is 0.
	//	type 
	//		Specifies the data type of each weight in the array. Symbolic constant GL_FIXED is accepted. However, the common profile also accepts the symbolic constant GL_FLOAT as well. The initial value is GL_FIXED for the common lite profile, or GL_FLOAT for the common profile.
	//	stride 
	//		Specifies the byte offset between consecutive weights. If stride is 0, the weights are understood to be tightly packed in the array. The initial value is 0.
	//	pointer 
	//		Specifies a pointer to the first weight of the first vertex in the array. The initial value is 0.
	inline void glWeightPointerOES( GLint SIZE, GLenum TYPE, GLsizei stride, const GLvoid *pointer )
	{
		#ifdef I3_WIPI_KTF_KV3600
			// MC_glWeightPointerOES( SIZE, TYPE, stride, pointer );
		#else
			::glWeightPointerOES( SIZE, TYPE, stride, pointer );
			//I3TRACE( "[I3OGL::glWeightPointerOES] Not support.\n" );
		#endif
	}


	//////////////////////////////////////////////////////////////////////////
	// OpenGL-ES 1.0v Extention functions									//
	//////////////////////////////////////////////////////////////////////////
	inline void glexGenMacros(GLsizei n, GLuint *hMacro)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glexGenMacros( n, hMacro);
		#elif defined( I3_WIPI_GP)
			MC_glexGenMacros(n, hMacro);
		#else
			//::glexGenMacros(n, hMacro);
		#endif
	}

	inline void glexCompileArrays(GLuint hMacro, GLenum mode, GLint first, GLsizei count)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glexCompileArrays( hMacro, mode, first, count);
		#elif defined( I3_WIPI_GP)
			MC_glexCompileArrays(hMacro, mode, first, count);
		#else
			//::glexCompileArrays(hMacro, mode, first, count);
		#endif
	}

	inline void glexCompileElements(GLuint hMacro, GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glexCompileElements( hMacro, mode, count, type, indices);
		#elif defined( I3_WIPI_GP)
			MC_glexCompileElements(hMacro, mode, count, type, indices);
		#else
			//::glexCompileElements(hMacro, mode, count, type, indices);
		#endif
	}

	inline void glexDrawMacros(GLuint hMacro)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glexDrawMacros( hMacro);
		#elif defined( I3_WIPI_GP)
			MC_glexDrawMacros(hMacro);
		#else
			//::glexDrawMacros(hMacro);
		#endif
	}

	inline void glexDeleteMacros(GLsizei n, GLuint *hMacro)
	{
		#if defined( I3_WIPI_GIGA)
			OEMC_g3D_glexDeleteMacros(n, hMacro);
		#elif defined( I3_WIPI_GP)
			MC_glexDeleteMacros(n, hMacro);
		#else
			//::glexDeleteMacros(n, hMacro);
		#endif
	}

	inline BOOL eglCreate( EGLint width, EGLint height, EGLint mode)
	{
	#if defined( I3_WIPI_GIGA)
		// GIGA 디스플레이어 생성
		OEMC_g3D_eglxCreate( width, height, mode);
		s_hDisplay = OEMC_g3D_eglGetDisplay( G2D_LCD_DEFAULT);
	#else
		//	OpenGL-ES 1.0 디바이스 생성
		static const EGLint sc_iConfigAttribs[] =
		{
			#if defined( I3_WIPI_GP)
				EGL_RED_SIZE,       5,
				EGL_GREEN_SIZE,		6,
				EGL_BLUE_SIZE,      5,
				EGL_ALPHA_SIZE,     EGL_DONT_CARE,
				EGL_DEPTH_SIZE,     16,
				EGL_STENCIL_SIZE,   EGL_DONT_CARE,
				EGL_SURFACE_TYPE,   EGL_PIXMAP_BIT,
			#elif defined( I3_WINCE )
				EGL_DEPTH_SIZE,		16,  
				EGL_SURFACE_TYPE,	EGL_WINDOW_BIT,
			#else
				EGL_RED_SIZE,       5,
				EGL_GREEN_SIZE,		6,
				EGL_BLUE_SIZE,      5,
			#endif
				EGL_NONE
		};

		#if defined( I3_WIPI_GP)
			static const EGLint sc_iPBufferAttribs[] =
			{
				EGL_WIDTH,  320,
				EGL_HEIGHT, 240,
				EGL_NONE
			};
		#elif defined( I3_WINCE)
			static const EGLint sc_iPBufferAttribs[] =
			{
				EGL_WIDTH,  800,
				EGL_HEIGHT, 480,
				EGL_NONE
			};		
		#endif
		
		EGLint iNumConfigs;
		EGLint iMajorVersion;								   
		EGLint iMinorVersion;
		
		EGLConfig	eglConfig;

		#if defined( I3_WIPI_GP)
			MC_gp3DSetDisplayMode(GP3DSDM_START);
		#endif 

		// Get the display device
		#if defined( I3_WIPI_GP)
			s_hDisplay = MC_eglGetDisplay(EGL_DEFAULT_DISPLAY);
			if( s_hDisplay == (int)EGL_NO_DISPLAY)	return FALSE;
		#elif defined( I3_WINCE)
			s_hDisplay = ::eglGetDisplay( EGL_DEFAULT_DISPLAY );
			if( s_hDisplay == EGL_NO_DISPLAY)	return FALSE;			
		#else
			s_hDisplay = eglGetDisplay((int)EGL_DEFAULT_DISPLAY);
			if( s_hDisplay == EGL_NO_DISPLAY)	return FALSE;
		#endif

		#if defined( I3_WIPI_GP)
			// Initialize the display
			if(!MC_eglInitialize( s_hDisplay, &iMajorVersion, &iMinorVersion))	return FALSE;
			
			// Create a surface for the main window
			MC_eglGetConfigs( s_hDisplay, NULL, 0, &iNumConfigs);
			// Obtain the first configuration with a depth buffer
			MC_eglChooseConfig( s_hDisplay, sc_iConfigAttribs, &eglConfig, 1, &iNumConfigs);
		#else
			// Initialize the display
			if(!eglInitialize( s_hDisplay, &iMajorVersion, &iMinorVersion))	return FALSE;
			
			// Create a surface for the main window
			eglGetConfigs( s_hDisplay, NULL, 0, &iNumConfigs);
			// Obtain the first configuration with a depth buffer
			eglChooseConfig( s_hDisplay, sc_iConfigAttribs, &eglConfig, 1, &iNumConfigs);
		#endif


		#if defined( I3_WIPI_GP)
			s_eglWindowSurface = MC_eglCreatePbufferSurface( s_hDisplay, eglConfig, sc_iPBufferAttribs);		
		#elif defined( I3_WINCE )
			s_eglWindowSurface = eglCreatePbufferSurface( s_hDisplay, eglConfig, sc_iPBufferAttribs);					
		#else
			s_eglWindowSurface = eglCreateWindowSurface( s_hDisplay, eglConfig, 0, NULL);
		#endif

		if(s_eglWindowSurface == NULL)	return FALSE;
#if 0	
		{
			EGLint err;
			err = MC_eglGetError();
			switch(err) {
			case EGL_NOT_INITIALIZED:
				i3Log("OpenGl ES CreateDevie()", "EGL_NOT_INITIALIZED");
				break;
			case EGL_BAD_ACCESS:
				i3Log("OpenGl ES CreateDevie()", "EGL_BAD_ACCESS");
				break;
			case EGL_BAD_ALLOC:
				i3Log("OpenGl ES CreateDevie()", "EGL_BAD_ALLOC");
				break;
			case EGL_BAD_ATTRIBUTE:
				i3Log("OpenGl ES CreateDevie()", "EGL_BAD_ATTRIBUTE");
				break;
			case EGL_BAD_CONFIG:
				i3Log("OpenGL ES CreateDevice()", "EGL_BAD_CONFIG");
				break;
			case EGL_BAD_CONTEXT:
				i3Log("OpenGL ES CreateDevice()", "EGL_BAD_CONTEXT");
				break;
			case EGL_BAD_CURRENT_SURFACE:
				i3Log("OpenGL ES CreateDevice()", "EGL_BAD_CURRENT_SURFACE");
				break;
			case EGL_BAD_DISPLAY:
				i3Log("OpenGL ES CreateDevice()", "EGL_BAD_DISPLAY");
				break;
			case EGL_BAD_MATCH:
				i3Log("OpenGL ES CreateDevice()", "EGL_BAD_MATCH");
				break;
			case EGL_BAD_NATIVE_PIXMAP:
				i3Log("OpenGL ES CreateDevice()", "EGL_BAD_NATIVE_PIXMAP");
				break;
			case EGL_BAD_NATIVE_WINDOW:
				i3Log("OpenGL ES CreateDevice()", "EGL_BAD_NATIVE_WINDOW");
				break;
			case EGL_BAD_PARAMETER:
				i3Log("OpenGL ES CreateDevice()", "EGL_BAD_PARAMETER");
				break;
			case EGL_BAD_SURFACE:
				i3Log("OpenGL ES CreateDevice()", "EGL_BAD_SURFACE");
				break;
			default:
				i3Log("OpenGL ES CreateDevice()", "Unknown");
				break;
			}
			return FALSE;
		}
#endif
		#if defined( I3_WIPI_GP)
			// Create an OpenGL ES context
			s_eglContext = MC_eglCreateContext( s_hDisplay, eglConfig, 0, NULL);
			if(s_eglContext == (unsigned long)EGL_NO_CONTEXT) {
				i3Log("OpenGL ES CreateDevice()", "Fail to Create an OpenGL ES context");
				return FALSE;
			}

			// Make the context and surface current
			if(!MC_eglMakeCurrent( s_hDisplay, s_eglWindowSurface, s_eglWindowSurface, s_eglContext)) {
				i3Log("OpenGL ES CreateDevice()", "Fail to Make the context and surface current");
				return FALSE;
			}
		#else
			// Create an OpenGL ES context
			s_eglContext = eglCreateContext( s_hDisplay, eglConfig, 0, NULL);
			if(s_eglContext == EGL_NO_CONTEXT) {
				i3Log("OpenGL ES CreateDevice()", "Fail to Create an OpenGL ES context");
				return FALSE;
			}

			// Make the context and surface current
			if(!eglMakeCurrent( s_hDisplay, s_eglWindowSurface, s_eglWindowSurface, s_eglContext)) {
				i3Log("OpenGL ES CreateDevice()", "Fail to Make the context and surface current");
				return FALSE;
			}
		#endif
	#endif
		return TRUE;
	}

}

#endif	// __I3G_OGLES_H
