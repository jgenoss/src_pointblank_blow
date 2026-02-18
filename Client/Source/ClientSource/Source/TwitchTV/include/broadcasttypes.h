/********************************************************************************************
 * Twitch Broadcasting SDK
 *
 * This software is supplied under the terms of a license agreement with Twitch Interactive, Inc. and
 * may not be copied or used except in accordance with the terms of that agreement
 * Copyright (c) 2012-2014 Twitch Interactive, Inc.
 *********************************************************************************************/

#pragma once

#include "twitchsdktypes.h"
#include "twitchcore/types/errortypes.h"
#include "twitchcore/types/coretypes.h"

#define TTV_BC_DEFAULT_STREAM_INFO_UPDATE_INTERVAL 30

/**
 * TTV_BroadcastControllerState - The possible states the BroadcastController can be in.
 */
typedef enum
{
	TTV_BC_STATE_UNINITIALIZED,          //!< InitializeTwitch not called.
	TTV_BC_STATE_INITIALIZED,            //!< InitializeTwitch has been called.
	TTV_BC_STATE_AUTHENTICATING,         //!< Requesting an AuthToken.
	TTV_BC_STATE_AUTHENTICATED,          //!< Have an AuthToken (not necessarily a valid one).
	TTV_BC_STATE_LOGGING_IN,              //!< Waiting to see if the AuthToken is valid).
	TTV_BC_STATE_LOGGED_IN,               //!< AuthToken is valid.
	TTV_BC_STATE_FINDING_INGEST_SERVER,    //!< Determining which server we can braodcast to.
	TTV_BC_STATE_RECEIVED_INGEST_SERVERS,  //!< Received the list of ingest servers.
	TTV_BC_STATE_READY_TO_BROADCAST,       //!< Idle and ready to broadcast.
	TTV_BC_STATE_STARTING,               //!< Processing a request to start broadcasting.
	TTV_BC_STATE_BROADCASTING,           //!< Currently broadcasting.
	TTV_BC_STATE_PAUSED,                 //!< Streaming but paused.
	TTV_BC_STATE_STOPPING,               //!< Processing a request to stop broadcasting.
	TTV_BC_STATE_SHUTTING_DOWN,           //!< Shutting down the broadcast controller.
	TTV_BC_STATE_SHUT_DOWN,           //!< Broadcast controller has shutdown.
	TTV_BC_STATE_INGEST_TESTING           //!< Running the ingest tester.
} TTV_BroadcastControllerState;

typedef enum
{
	TTV_BC_GAME_AUDIO_SUBMISSION_METHOD_NONE,	//!< Will not broadcast game audio.
	TTV_BC_GAME_AUDIO_SUBMISSION_METHOD_MANUAL,	//!< Audio will be submitted manually to the SDK.
	TTV_BC_GAME_AUDIO_SUBMISSION_METHOD_AUTOMATIC	//!< Submit game audio automatically where possible.
} TTV_BroadcastControllerGameAudioSubmissionMethod;

typedef enum
{
	TTV_BC_GAME_VIDEO_SUBMISSION_METHOD_NONE,	//!< Will not broadcast game video.
	TTV_BC_GAME_VIDEO_SUBMISSION_METHOD_MANUAL,	//!< Video frames will be submitted manually to the SDK.
	TTV_BC_GAME_VIDEO_SUBMISSION_METHOD_AUTOMATIC	//!< Capture video frames automatically when possible.
} TTV_BroadcastControllerGameVideoSubmissionMethod;
