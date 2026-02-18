#pragma once

namespace rsc
{
	// 패킹 시 추가되는 파일 또는 폴더 정보.
	struct PackAddTarget
	{
		std::string		strRelPath;
		bool			includeSubFolderOK;
	};

	using PackAddTargetPtr = std::shared_ptr<PackAddTarget>;

	// 팩 분석 정보.
	struct PackAnalysis
	{
		std::vector<PathPtr>			targetList;
		std::string						stri3PackName;			// 팩 파일명.
		std::vector<PackAddTargetPtr>	addList;				// 추가 대상 상대경로.
		std::vector<std::string>		strRemoveRelPathList;	// 제거 대상 상대경로.
		std::vector<std::string>		strEncryptRelPathList;	// 암호화 대상 상대경로.
		bool							checksumOK;				// 팩 체크섬.
	};

	void ClearPackAnalysis(PackAnalysis& src);

	struct PackRuleCallback
	{
		virtual void OnPackComplete(const std::vector<PathPtr>& srcPackTgtList,
									const Path& srcWorkdir,
									const Path& dstWorkdir,
									const PackAnalysis& analysis,
									const bool success,
									const wchar_t* errMessage)
		{}

		virtual void OnCopyComplete(const std::vector<PathPtr>& srcCopyOrFailList,
									const Path& srcWorkdir,
									const Path& dstWorkdir, 
									const bool success,
									const wchar_t* errMessage)
		{}

		virtual ~PackRuleCallback() {}
	};

	using PackRuleCallbackPtr = std::shared_ptr<PackRuleCallback>;

	class IPackRule
	{
	public:
		IPackRule(const std::vector<PathPtr>& srcList,	// 패킹 대상 원본 파일들.
				  const Path& srcWorkdir, 
				  const Path& dstWorkdir);
		
		virtual ~IPackRule() = 0;

		void operator()(bool bCheckDuplicatedFile);

		virtual bool Analyze(PackAnalysis& outAnalysis, std::vector<PathPtr>& outCopyList) = 0;

		void				SetCallback(PackRuleCallbackPtr cb)	{ m_callback = std::move(cb);	}
		PackRuleCallback*	GetCallback() const					{ return m_callback.get();		}
	
	protected:
		void _Copy(const std::vector<PathPtr>& copyList);
		bool _Pack(const PackAnalysis& analysis, bool bCheckDuplicatedFile);
		
		std::vector<PathPtr> m_srcPackTgtList;		// 원본 파일 중 팩 대상 파일.
		std::vector<PathPtr> m_srcCopyTgtList;		// 원본 파일 중 카피 대상 파일.

		const Path m_srcWorkdir;
		const Path m_dstWorkdir;
		
		PackRuleCallbackPtr m_callback;
	};

	using PackRulePtr = std::shared_ptr < IPackRule > ;
}