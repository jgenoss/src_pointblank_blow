#if !defined( __I3_FRAMEWORK_META_H)
#define __I3_FRAMEWORK_META_H

I3_EXPORT_FRAMEWORK void i3FrameworkRegisterMetas(void);

#if defined( I3_WINDOWS)
extern HINSTANCE g_hInstFramework;
#endif

#endif
