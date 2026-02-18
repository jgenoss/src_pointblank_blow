#pragma once

#include <functional>
#include "ContentsDefines.h"

namespace rsc
{
	class ResourceController;

	class ISyncProgressCallback
	{
	public:
		virtual ~ISyncProgressCallback() {}

		virtual void OnStart() = 0;
		virtual void OnProgress(const float progressRate) = 0;
		virtual void OnEnd(const bool success) = 0;
	};

	class SyncProgressCallback_Null : public ISyncProgressCallback
	{
	public:
		virtual ~SyncProgressCallback_Null() {}

		virtual void OnStart() override {}
		virtual void OnProgress(const float progressRate) override {}
		virtual void OnEnd(const bool success) override {}
	};

	class SyncProgressCallback : public ISyncProgressCallback
	{
	public:
		using StartFuncType		= std::function < void() >;
		using ProgressFuncType	= std::function < void(const float) >;
		using EndFuncType		= std::function < void(const bool) >;

		SyncProgressCallback(const StartFuncType& fStart, const ProgressFuncType& fProgress, const EndFuncType& fEnd);
		virtual ~SyncProgressCallback();

		virtual void OnStart() override;
		virtual void OnProgress(const float progressRate) override;
		virtual void OnEnd(const bool success) override;

	private:
		StartFuncType m_cbStart;
		ProgressFuncType m_cbProgress;
		EndFuncType m_cbEnd;
	};

	using SyncProgressCallbackPtr = std::shared_ptr < ISyncProgressCallback >;
}