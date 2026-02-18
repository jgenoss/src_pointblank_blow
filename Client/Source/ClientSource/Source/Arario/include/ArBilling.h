#pragma once

//#include "ararioApiCommon.h"
#include "ARTPBilling.h"

extern "C" 
{
	// for billing
	//!====================================================================================================================
	//! @brief	try getting riocash
	//!			result can know by return value.
	//!
	ARAPI_DLL int  __cdecl GetRioCash( ARTP_RCS_GETRIOCASH& _in, ARTP_RET_RCS_GETRIOCASH& _out );

	//!====================================================================================================================
	//! @brief	try using riocash	
	//!
	ARAPI_DLL int __cdecl UseRioCash( ARTP_RCS_USERIOCASH& _in, ARTP_RET_RCS_USERIOCASH& _out );

	//!====================================================================================================================
	//! @brief	try rollback
	//!
	ARAPI_DLL int __cdecl Rollback( ARTP_RCS_ROLLBACK& _in, ARTP_RET_RCS_ROLLBACK& _out );

	//!====================================================================================================================
	//! @brief	try getting riocash by async
	//!			result can know by return value.
	//!
	ARAPI_DLL void  __cdecl GetRioCashAsync( ARTP_RCS_GETRIOCASH& _in );

	//!====================================================================================================================
	//! @brief	try using riocash by async
	//!
	ARAPI_DLL void __cdecl UseRioCashAsync( ARTP_RCS_USERIOCASH& _in );

////========================================================================================================================////
////==============Start CF Block       =====================================================================================////
////========================================================================================================================////
	//!====================================================================================================================
	//! @brief	try using riocash by async (CF)
	//!
	ARAPI_DLL void __cdecl UseRioCashAsyncCF( ARTP_RCS_USERIOCASH_CF& _in );

	//!====================================================================================================================
	//! @brief	edit by shim kyu woong at 2015.2.2.
	//!			request by smilegate
	//!			try using riocash by async multiple(CF) for the buying multiple gacha set 
ARAPI_DLL void __cdecl UseRioCashAsyncCFMultiple( ARTP_RCS_USERIOCASH_CF_MULTIPLE& _in );

	//!====================================================================================================================
	//! @brief	try using riocash	
	//!
ARAPI_DLL int __cdecl UseRioCash4CFMultiple( ARTP_RCS_USERIOCASH_CF_MULTIPLE& _in, ARTP_RET_RCS_USERIOCASH_CF_MULTIPLE& _out );


////========================================================================================================================////
////==============End CF Block         =====================================================================================////
////========================================================================================================================////


	//!====================================================================================================================
	//! @brief	try rollback by async
	//!
	ARAPI_DLL void __cdecl RollbackAsync( ARTP_RCS_ROLLBACK& _in );
}