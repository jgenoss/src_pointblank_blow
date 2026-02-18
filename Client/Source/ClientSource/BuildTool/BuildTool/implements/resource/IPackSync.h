#pragma once

#include "IPackRule.h"

namespace rsc
{
	struct PackScriptCallback
	{
		virtual void OnPackComplete(const std::vector<PathPtr>& srcPackTgtList,
									const Path& srcWorkdir, 
									const Path& dstWorkdir, 
									const Path& dsti3PackRelPath,
									const Path& packScriptAbsPath) 
		{}

		virtual ~PackScriptCallback() {}
	};

	using PackScriptCallbackPtr = std::shared_ptr<PackScriptCallback>;



	class IPackSync
	{
	public:
		IPackSync();
		virtual ~IPackSync() = 0;

		bool operator()(const std::vector<PathPtr>& fileRelPathList, const Path& srcWorkdir, const Path& dstWorkdir);

		void				SetCallback(PackScriptCallbackPtr cb)	{ m_callback = cb; }
		PackScriptCallback*	GetCallback() const						{ return m_callback.get(); }

	protected:
		// 기존 팩스크립트 이용 패킹.
		void _PackingByPackScript(const std::vector<PathPtr>& srcList,
								  const Path& srcWorkdir, 
								  const Path& dstWorkdir, 
								std::vector<PathPtr>& outUnperformedList,
								bool bCheckDuplicatedFile);

		// 컨텐츠 카테고리별 패킹룰 생성함수 등록.
		virtual void _RegisterRuleCreators() = 0;

		// 룰에 따른 패킹 실시.
		void _PackingByRule(std::vector<PathPtr>& inoutRelPathList, const Path& srcWorkdir, const Path& dstWorkdir
		, bool bCheckDuplicatedFile);

		using PackRuleCreatorFuncType =
			std::function<void (std::vector<PathPtr>&, const Path&, const Path&, std::vector<PackRulePtr>&)>;

		std::vector<PackRuleCreatorFuncType> m_ruleCreators;

		PackScriptCallbackPtr m_callback;
	};

	using PackSyncPtr = std::shared_ptr<IPackSync>;
}