#ifndef __I3_PARSER_HELPER_H__
#define __I3_PARSER_HELPER_H__

#include "i3Base/string/compare/generic_is_iequal.h"

inline INT32 GetIndexFromString( const char *szBuf, const char *szString[], UINT32 nNum )
{
	UINT32 i;
	for( i = 0; i < nNum; i++ )
	{
		//if( stricmp( szBuf, szString[i] ) == 0 )
		if( i3::generic_is_iequal( szBuf, szString[i] ) )			
		{
			return i;
		}
	}
	return -1;
}

#define IF_STR(_STR_)									if(stricmp(szBuf,_STR_)==0)

// Get Value from the Parser
#define _GET_VALUE(_LABEL_,_DEFAULT_,_VAR_,_IMPORTANT_)	if(!pParser->GetValue(_LABEL_,_DEFAULT_,_VAR_))\
														{\
															BOOL bTemp = _IMPORTANT_;\
															if( bTemp)\
															{\
																I3PRINTLOG(I3LOG_FATAL,  "Failed to find label '%s'", _LABEL_);\
																goto ExitLabel;\
															}\
														}

#define GET_VALUE(_LABEL_,_DEFAULT_,_VAR_)					_GET_VALUE(_LABEL_,_DEFAULT_,_VAR_,FALSE)
#define GET_IMPORTANT_VALUE(_LABEL_,_DEFAULT_,_VAR_)		_GET_VALUE(_LABEL_,_DEFAULT_,_VAR_,TRUE)

// Get String from the Parser
#define _GET_STRING(_LABEL_,_DEFAULT_,_VAR_,_BUFSZ_,_IMPORTANT_)	if(!pParser->GetString(_LABEL_,_DEFAULT_,_VAR_,_BUFSZ_))\
														{\
															BOOL bTemp = _IMPORTANT_;\
															if( bTemp)\
															{\
																I3PRINTLOG(I3LOG_FATAL,  "Failed to find label '%s'", _LABEL_);\
																goto ExitLabel;\
															}\
														}

#define GET_STRING(_LABEL_,_DEFAULT_,_VAR_,_BUFSZ_)				_GET_STRING(_LABEL_,_DEFAULT_,_VAR_,_BUFSZ_,FALSE)
#define GET_IMPORTANT_STRING(_LABEL_,_DEFAULT_,_VAR_,_BUFSZ_)	_GET_STRING(_LABEL_,_DEFAULT_,_VAR_,_BUFSZ_,TRUE)


// Get index from string value
#define _GET_INDEX(_LABEL_,_STRLIST_,_DEFAULT_,_VAR_,_IMPORTANT_)	{\
																		if(!pParser->GetIndex(_LABEL_,_STRLIST_,_DEFAULT_,_VAR_,sizeof(_STRLIST_)/sizeof(_STRLIST_[0])))\
																		{\
																			if(_VAR_<0)\
																			{\
																				I3PRINTLOG(I3LOG_FATAL,  "Unknown '%s', '%s'", _LABEL_, pParser->GetTempBuffer() );\
																				goto ExitLabel;\
																			}\
																			else\
																			{\
																				BOOL bTemp = _IMPORTANT_;\
																				if( bTemp)\
																				{\
																				I3PRINTLOG(I3LOG_FATAL,  "Failed to find label '%s'", _LABEL_);\
																				goto ExitLabel;\
																				}\
																			}\
																		}\
																	}\

#define GET_INDEX(_LABEL_,_STRLIST_,_DEFAULT_,_VAR_)			_GET_INDEX(_LABEL_,_STRLIST_,_DEFAULT_,_VAR_,FALSE)
#define GET_IMPORTANT_INDEX(_LABEL_,_STRLIST_,_VAR_)			_GET_INDEX(_LABEL_,_STRLIST_,0,_VAR_,TRUE)

#define _GET_VEC3D(_LABEL_,_VAR_,_IMPORTANT_)			{\
															REAL32 rTemp[3];\
															if( ! pParser->GetValues(_LABEL_, 0.0f, rTemp, 3, ',' ) )\
															{\
																if(_IMPORTANT_)\
																{\
																	I3PRINTLOG(I3LOG_FATAL,  "Failed to find label '%s'", _LABEL_);\
																	goto ExitLabel;\
																}\
															}\
															else\
															{\
																i3Vector::Set( &_VAR_, rTemp[0], rTemp[1], rTemp[2] );\
															}\
														}

#define GET_VEC3D(_LABEL_,_VAR_)						_GET_VEC3D(_LABEL_,_VAR_,FALSE)	
#define GET_IMPORTANT_VEC3D(_LABEL_,_VAR_)				_GET_VEC3D(_LABEL_,_VAR_,TRUE)	

#define _GET_MAX_VEC3D(_LABEL_,_VAR_,_IMPORTANT_)		{\
															REAL32 rTemp[3];\
															if( ! pParser->GetValues(_LABEL_, 0.0f, rTemp, 3, ',' ) )\
															{\
																if(_IMPORTANT_)\
																{\
																	I3PRINTLOG(I3LOG_FATAL,  "Failed to find label '%s'", _LABEL_);\
																	goto ExitLabel;\
																}\
															}\
															else\
															{\
																i3Vector::Set( &_VAR_, -rTemp[0], rTemp[2], rTemp[1] );\
															}\
														}

#define GET_MAX_VEC3D(_LABEL_,_VAR_)						_GET_MAX_VEC3D(_LABEL_,_VAR_,FALSE)	
#define GET_IMPORTANT_MAX_VEC3D(_LABEL_,_VAR_)				_GET_MAX_VEC3D(_LABEL_,_VAR_,TRUE)	

// Get Degree Angle from the Parser and Covert it to Radian.
#define _GET_RAD_FROM_DEG(_LABEL_,_DEFAULT_,_VAR_,_IMPORTANT_)	{\
																	REAL32 rTemp;\
																	if(!pParser->GetValue(_LABEL_,_DEFAULT_,&rTemp))\
																	{\
																		if(_IMPORTANT_)\
																		{\
																			I3PRINTLOG(I3LOG_FATAL,  "Failed to find label '%s'", _LABEL_);\
																			goto ExitLabel;\
																		}\
																	}\
																	_VAR_ = ((rTemp * I3_PI) * 0.0055555555555555555555555555555556f);\
																}

#define GET_RAD_FROM_DEG(_LABEL_,_DEFAULT_,_VAR_)			_GET_RAD_FROM_DEG(_LABEL_,_DEFAULT_,_VAR_,FALSE)
#define GET_IMPORTANT_RAD_FROM_DEG(_LABEL_,_DEFAULT_,_VAR_)	_GET_RAD_FROM_DEG(_LABEL_,_DEFAULT_,_VAR_,TRUE)

#endif //__I3_PARSER_HELPER_H__
