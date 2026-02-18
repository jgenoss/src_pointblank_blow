/********************************************************************************************
 * Twitch Broadcasting SDK
 *
 * This software is supplied under the terms of a license agreement with Twitch Interactive, Inc. and
 * may not be copied or used except in accordance with the terms of that agreement
 * Copyright (c) 2012-2014 Twitch Interactive, Inc.
 *********************************************************************************************/

#ifndef TTVSDK_BROADCAST_INTERFACE_H
#define TTVSDK_BROADCAST_INTERFACE_H

#include "twitchsdktypes.h"
#include "twitchcore/types/errortypes.h"
#include "broadcasttypes.h"
#include "broadcastlistener.h"
#include "ingesttester.h"

namespace ttv
{
	namespace broadcast
	{
		class IBroadcaster;
		class IBroadcastListener;
	}
}

/**
 * The state machine which manages the broadcasting state.  It provides a high-level interface to the SDK libraries.  The BroadcastController (BC) exposed through
 * this interface performs many operations asynchronously and hides the details.
 *
 * Events will be fired during the call to BC.Update().
 *
 * The typical order of operations a client of BC will take is:
 *
 * - Call BC.Init()
 * - Call BC.Login()
 * - Determine the server to use for broadcasting
 * - Call BC.StartBroadcast()
 * - Submit frames (this is done differently depending on platform), see platform-specific documentation for details
 * - Call BC.StopBroadcast()
 * - Call BC.Shutdown()
 *
 * When setting up the VideoParams for the broadcast you should use the bitrate version of GetRecommendedVideoParams.  This will setup the resolution and other parameters
 * based on your connection to the server.  The other version is more explicit and may be useful for advanced users and testing but is generally more confusing and produces
 * poorer results for the typical user who doesn't understand the settings.  We've found that many users just crank up the settings and don't understand why their
 * broadcast doesn't look that great or the stream drops (network backup).  Simply giving the users a slider for maximum bitrate generally produces better visual quality.
 * In a more advanced integration the bitrate can actually be determined by running the IngestTester (coming soon).
 */
class ttv::broadcast::IBroadcaster
{
public:
	
	virtual TTV_ErrorCode Init(ttv::broadcast::IBroadcastListener* broadcastListener) = 0;
	virtual TTV_ErrorCode Shutdown(bool synchronous) = 0;
	virtual TTV_ErrorCode Login(const char* username, const char* password, const char* clientSecret) = 0;
	virtual TTV_ErrorCode Login(const TTV_AuthToken& authToken) = 0;
	virtual TTV_ErrorCode Logout() = 0;
	virtual TTV_ErrorCode UpdateStreamInfo(const char* game, const char* title) = 0;
	virtual TTV_ErrorCode RunCommercial(int timebreak) = 0;
	virtual TTV_ErrorCode RequestRecordingStatus() = 0;
	virtual TTV_ErrorCode SetVideoParams(uint maxKbps, uint frameRate, float bitsPerPixel, float aspectRatio) = 0;
	virtual TTV_ErrorCode SetCustomVideoParams(uint width, uint height, uint frameRate) = 0;
	virtual TTV_ErrorCode StartIngestTest() = 0;
	virtual TTV_ErrorCode CancelIngestTest() = 0;
	virtual TTV_ErrorCode StartBroadcast() = 0;
	virtual TTV_ErrorCode StopBroadcast() = 0;
	virtual TTV_ErrorCode PauseBroadcast() = 0;
	virtual TTV_ErrorCode ResumeBroadcast() = 0;
	virtual TTV_ErrorCode SubmitVideoFrame(const uint8_t* frameBuffer,TTV_BufferUnlockCallback callback, void* userData) = 0;
	virtual TTV_ErrorCode SubmitAudioSamples(const int16_t* samplesBuffer, uint numSamples) = 0;
	virtual TTV_ErrorCode SendActionMetaData(const char* name, uint64_t streamTime, const char* humanDescription, const char* data) = 0;
	virtual TTV_ErrorCode StartSpanMetaData(const char* name, uint64_t streamTime, unsigned long &sequenceId, const char* humanDescription, const char* data) = 0;
	virtual TTV_ErrorCode EndSpanMetaData(const char* name, uint64_t streamTime, unsigned long sequenceId, const char* humanDescription, const char* data) = 0;
	virtual TTV_ErrorCode RequestGameNameList(const char* str) = 0;
	
	virtual TTV_ErrorCode Update() = 0;
	
	// Accessors
	virtual bool IsInitialized() = 0;
	virtual bool IsBroadcasting() = 0;
	virtual bool IsReadyToBroadcast() = 0;
	virtual bool IsIngestTesting() = 0;
	virtual bool IsPaused() = 0;
	virtual bool IsLoggedIn() = 0;
	virtual bool IsAutomaticGameAudioCaptureSupported() = 0;
	virtual bool IsAutomaticGameVideoCaptureSupported() = 0;
	virtual bool GetCaptureMicrophone() = 0;
	virtual TTV_BroadcastControllerGameAudioSubmissionMethod GetGameAudioCaptureMethod() = 0;
	virtual TTV_BroadcastControllerGameVideoSubmissionMethod GetGameVideoCaptureMethod() = 0;
	virtual TTV_BroadcastControllerState GetCurrentState() = 0;
	virtual TTV_ArchivingState GetArchivingState() = 0;
	virtual TTV_ErrorCode GetAuthToken(TTV_AuthToken& authToken) = 0;
	virtual TTV_ErrorCode GetViewerCount(TTV_StreamInfo& viewerCount) = 0;
	virtual TTV_ErrorCode GetChannelInfo(TTV_ChannelInfo& channelInfo) = 0;
	virtual ttv::broadcast::IngestTester* GetIngestTester() = 0;
	virtual TTV_ErrorCode GetIngestList(TTV_IngestList& ingestList) = 0; // you must call ttv::Free on the struct when done
	virtual TTV_ErrorCode GetIngestServer(TTV_IngestServer& ingestServer) = 0;
	virtual float GetMicrophoneVolume() = 0;
	virtual float GetAutomaticSubmissionVolume() = 0;
	virtual float GetManualSubmissionVolume() = 0;
	virtual uint GetOutputWidth() = 0;
	virtual uint GetOutputHeight() = 0;
	virtual TTV_PixelFormat GetPixelFormat() = 0;
	virtual uint GetMaxBitrate() = 0;
	virtual uint GetFramerate() = 0;
	virtual bool GetFlipFramesVertically() = 0;
	virtual ITTVPluginVideoEncoder* GetEncoderPlugin() = 0;
	virtual TTV_ErrorCode GetCurrentBroadcastTime(uint64_t& broadcastTime) = 0;
	
	// Mutators
	virtual TTV_ErrorCode SetBroadcastListener(ttv::broadcast::IBroadcastListener* broadcastListener) = 0;
	virtual TTV_ErrorCode SetCaptureMicrophone(bool captureMicrophone) = 0;
	virtual TTV_ErrorCode SetGameAudioSubmissionMethod(TTV_BroadcastControllerGameAudioSubmissionMethod gameAudioCaptureMethod) = 0;
	virtual TTV_ErrorCode SetGameVideoSubmissionMethod(TTV_BroadcastControllerGameVideoSubmissionMethod gameVideoCaptureMethod) = 0;
	virtual TTV_ErrorCode SetIngestServer(TTV_IngestServer ingestServer) = 0;
	virtual TTV_ErrorCode SetMicrophoneVolume(float microphoneVolume) = 0;
	virtual TTV_ErrorCode SetAutomaticSubmissionVolume(float automaticVolume) = 0;
	virtual TTV_ErrorCode SetManualSubmissionVolume(float manualVolume) = 0;
	virtual TTV_ErrorCode SetOutputWidth(uint width) = 0;
	virtual TTV_ErrorCode SetOutputHeight(uint height) = 0;
	virtual TTV_ErrorCode SetPixelFormat(TTV_PixelFormat pixelFormat) = 0;
	virtual TTV_ErrorCode SetMaxBitrate(uint maxKbps) = 0;
	virtual TTV_ErrorCode SetFramerate(uint targetFps) = 0;
	virtual TTV_ErrorCode SetFlipFramesVertically(bool verticalFlip) = 0;
	virtual TTV_ErrorCode SetEncoderPlugin(ITTVPluginVideoEncoder* encoderPlugin) = 0;
};

#endif /* TTVSDK_BROADCAST_INTERFACE_H */
