/********************************************************************************************
 * Twitch Broadcasting SDK
 *
 * This software is supplied under the terms of a license agreement with Twitch Interactive, Inc. and
 * may not be copied or used except in accordance with the terms of that agreement
 * Copyright (c) 2012-2013 Twitch Interactive, Inc.
 *********************************************************************************************/

#ifndef TTVSDK_BROADCASTLISTENER_H
#define TTVSDK_BROADCASTLISTENER_H

#include "broadcasttypes.h"

namespace ttv
{
	namespace broadcast
	{		
		class IBroadcastListener;
	}
}


/**
 * Listener interface for callbacks from the broadcast controller.
 */
class ttv::broadcast::IBroadcastListener
{
public:
		
	virtual void LoginRequestCompletedCallback(TTV_ErrorCode result, const TTV_AuthToken& authToken);
	virtual void GameNameListRequestCompletedCallback(TTV_ErrorCode result, const TTV_GameInfoList& list);
	virtual void RunCommercialRequestCompletedCallback(TTV_ErrorCode result);
	virtual void RecordingStatusRequestCompletedCallback(TTV_ErrorCode result, const TTV_ArchivingState& archivingState);
	virtual void StreamInfoUpdateRequestCompletedCallback(TTV_ErrorCode result, const TTV_StreamInfoForSetting& info);
	virtual void IngestListReceivedCallback(const TTV_IngestList& list);
	virtual void SendActionMetaDataRequestCompletedCallback(TTV_ErrorCode result);
	virtual void StartSpanMetaDataRequestCompletedCallback(TTV_ErrorCode result);
	virtual void EndSpanMetaDataRequestCompletedCallback(TTV_ErrorCode result);
	virtual void FrameSubmissionIssueCallback(TTV_ErrorCode result);
	virtual void BroadcastStartedCallback();
	virtual void BroadcastStoppedCallback();
	virtual void BroadcastStateChangedCallback(TTV_BroadcastControllerState state);
	virtual void LoggedOutCallback();
};

#endif	/* TTVSDK_BROADCASTLISTENER_H */
