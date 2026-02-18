#pragma once


#if !defined( __FUNCSIG__)
#define __FUNCSIG__		"Unknown function"
#endif

#define I3PRINTLOG(logtype, fmt, ...) {	i3Error::SetEnv( __FILE__, __LINE__, __FUNCSIG__, logtype); i3Error::Log(fmt, __VA_ARGS__); }

//#define I3NOTICE(fmt, ...) \
//	i3Error::SetEnv( __FILE__, __LINE__, __FUNCSIG__, I3LOG_NOTICE);\
//	i3Error::Log(fmt, __VA_ARGS__)

//#define I3WARN(fmt, ...) \
//	i3Error::SetEnv( __FILE__, __LINE__, __FUNCSIG__, I3LOG_WARN);\
//	i3Error::Log(fmt, __VA_ARGS__)

//#define I3FATAL(fmt, ...) \
//	i3Error::SetEnv( __FILE__, __LINE__, __FUNCSIG__, I3LOG_FATAL);\
//	i3Error::Log(fmt, __VA_ARGS__)


#define	I3ERRLOG_MSG( code, msg)	{ I3PRINTLOG(I3LOG_WARN,  "%s (%s)", i3Error::getMsg((code)), (msg));	}
#define	I3ERRLOG( code)				{ I3PRINTLOG(I3LOG_WARN,  "%s", i3Error::getMsg((code)));	}

#define	I3BCLOG(file, line, funsig, fmt, ...) \
	i3Error::SetEnv( file, line, funsig, I3LOG_WARN); \
	i3Error::Log(fmt, __VA_ARGS__)

#define LOCATION __FILE__, __LINE__, __FUNCSIG__

