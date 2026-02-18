#ifndef __ARACOM4CLI_H__
#define __ARACOM4CLI_H__

#ifdef ARARIOAPI2012_LIB
	#define ARAPI_DLL
#else
	#ifdef _USRDLL
		#define ARAPI_DLL	__declspec(dllexport) 
	#else
		#define ARAPI_DLL	__declspec(dllimport) 
	#endif
#endif

#include "AuthFunctor.h"

extern "C" 
{
	/*
	* Initialize
	* Start Function
	*/
	ARAPI_DLL bool __cdecl StartAraService( bool useAsync = false );

	/*
	* Uninitialize
	* Stop Function
	*/
	ARAPI_DLL void __cdecl StopAraService();


	/*
	* SetResultCallbackFunction
	* Regist callback function...
	*/
	ARAPI_DLL void __cdecl SetResultCallbackFunction( IFunctor* pCallBackFunc_ );
	ARAPI_DLL void __cdecl SetResultCallbackFunctionBilling( IFunctor* pCallBackFunc_ );

	/*
	* SetAuthInfoPath
	* Registe path of "arauthinfo.ini"
	*/
	ARAPI_DLL void __cdecl SetAuthInfoPath( LPCWSTR _path );
}

#endif //__ARACOM4CLI_H__