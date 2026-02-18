#include "stdafx.h"
#include "SyncProgressCallback.h"
#include "ResourceController.h"

namespace rsc
{
	SyncProgressCallback::SyncProgressCallback(const StartFuncType& fStart, const ProgressFuncType& fProgress, const EndFuncType& fEnd)
	{
		m_cbStart = fStart;
		m_cbProgress = fProgress;
		m_cbEnd = fEnd;
	}

	SyncProgressCallback::~SyncProgressCallback() {}

	void SyncProgressCallback::OnStart()
	{
		m_cbStart();
	}

	void SyncProgressCallback::OnProgress(const float progressRate)
	{
		m_cbProgress(progressRate);
	}

	void SyncProgressCallback::OnEnd(bool success)
	{
		m_cbEnd(success);
	}
}