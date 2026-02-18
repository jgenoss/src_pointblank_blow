#if !defined( __I3_BASE_META_H)
#define __I3_BASE_META_H

I3_EXPORT_BASE void i3BaseRegisterMetas(void);

#if (defined( I3_WINDOWS) || defined( I3_WINCE) ) && defined( I3_DLL )
I3_EXTERN_BASE HINSTANCE g_hInstBase;
#endif

#endif
