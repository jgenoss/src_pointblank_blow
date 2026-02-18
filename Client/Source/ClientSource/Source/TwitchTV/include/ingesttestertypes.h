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

#define TTV_INGEST_TESTER_DEFAULT_DURATION_MS 8000
#define TTV_INGEST_TESTER_DEFAULT_DELAY_MS 2000

/**
 * TTV_IngestTesterState - The possible states the Ingest Tester can be in.
 */
typedef enum
{
	TTV_INGEST_TESTER_STATE_UNINITIALIZED,          // Init not called
	TTV_INGEST_TESTER_STATE_STARTING,
	TTV_INGEST_TESTER_STATE_CONNECTING_TO_SERVER,
	TTV_INGEST_TESTER_STATE_TESTING_SERVER,
	TTV_INGEST_TESTER_STATE_DONE_TESTING_SERVER,
	TTV_INGEST_TESTER_STATE_DONE_TESTING_FINISHED,
	TTV_INGEST_TESTER_STATE_DONE_TESTING_CANCELLING,
	TTV_INGEST_TESTER_STATE_DONE_TESTING_CANCELLED,
	TTV_INGEST_TESTER_STATE_DONE_TESTING_FAILED

} TTV_IngestTesterState;

typedef enum
{
	TTV_RTMP_STATE_INVALID = -1,
	TTV_RTMP_STATE_IDLE,
	TTV_RTMP_STATE_INITIALIZE,
	TTV_RTMP_STATE_HANDSHAKE,
	TTV_RTMP_STATE_CONNECT,
	TTV_RTMP_STATE_CREATE_STREAM,
	TTV_RTMP_STATE_PUBLISH,
	TTV_RTMP_STATE_SEND_VIDEO,
	TTV_RTMP_STATE_SHUTDOWN,
	TTV_RTMP_STATE_ERROR
	
} TTV_RTMPState;