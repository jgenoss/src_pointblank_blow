/********************************************************************************************
 * Twitch Broadcasting SDK
 *
 * This software is supplied under the terms of a license agreement with Twitch Interactive, Inc. and
 * may not be copied or used except in accordance with the terms of that agreement
 * Copyright (c) 2012-2014 Twitch Interactive, Inc.
 *********************************************************************************************/

#ifndef TTVSDK_SDKMANAGER_INTERFACE_H
#define TTVSDK_SDKMANAGER_INTERFACE_H

#include "twitchsdktypes.h"
#include "twitchcore/types/errortypes.h"
#include "twitchcore/types/memorytypes.h"
#include "twitchcore/types/tracingtypes.h"
#include "broadcastinterface.h"

namespace ttv
{
	class ISDKManager;
}

/**
 * C++ entry point and configuration management for the Twitch SDK
 */
class ttv::ISDKManager
{
public:
	
	virtual TTV_ErrorCode Init(const TTV_MemCallbacks* memCallbacks, const char* clientID, const wchar_t* dllPath) = 0;
	virtual TTV_ErrorCode SetTraceLevel(TTV_MessageLevel traceLevel) = 0;
	virtual TTV_ErrorCode SetTraceChannelLevel(const char* channel, TTV_MessageLevel traceLevel) = 0;
	virtual TTV_ErrorCode SetTraceOutput(const wchar_t* outputFileName) = 0;
	virtual ttv::broadcast::IBroadcaster* GetBroadcaster(TTV_ErrorCode *ec) = 0;
		
	static TTV_ErrorCode Free(TTV_IngestList& ingestList);
	static TTV_ErrorCode Free(TTV_LiveGameStreamList& liveGameStreamList);
	static TTV_ErrorCode Free(TTV_GameInfoList& gameInfoList);
};

#endif /* TTVSDK_SDKMANAGER_INTERFACE_H */