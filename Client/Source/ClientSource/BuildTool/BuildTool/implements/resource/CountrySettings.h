#pragma once

namespace rsc
{
	class CountryData final
	{
	public:
		void Reset();

		bool				SetCountryName(const std::string& strCountryName);
		const std::string&	GetCountryName() const;

		bool				SetVersion(const std::string& strVersion);
		const std::string&	GetVersion() const;

		bool				SetMediaWorkdir(const Path& path);
		const Path&			GetMediaWorkdir() const;

		bool				SetQAWorkdir(const Path& path);
		const Path&			GetQAWorkdir() const;

		bool				SetLiveWorkdir(const Path& path);
		const Path&			GetLiveWorkdir() const;

		bool				SetPackScriptWorkdir(const Path& path);
		const Path&			GetPackScriptWorkdir() const;

		bool				SetPEFCountryIdx(const int idx);
		int					GetPEFCountryIdx() const;

		bool				SetMediaVCS(const std::string& strVCS);
		const std::string&	GetMediaVCS() const;

		bool				SetQAVCS(const std::string& vcs);
		const std::string&	GetQAVCS() const;

		bool				SetLiveVCS(const std::string& vcs);
		const std::string&	GetLiveVCS() const;

		bool				SetPackScriptVCS(const std::string& vcs);
		const std::string&	GetPackScriptVCS() const;

		void				SetEnableV3UI_1_0(const bool ok);
		bool				IsEnableV3UI_1_0() const;

		void				SetEnableV3UI_1_1(const bool ok);
		bool				IsEnableV3UI_1_1() const;

		void				SetEnableV3UI_1_5(const bool ok);
		bool				IsEnableV3UI_1_5() const;

		bool				SetMediaLocaleFolderRelPath(const Path& path);
		const Path&			GetMediaLocaleFolderRelPath() const;

		bool CheckValid();

	private:
		std::string m_strCountryName;
		std::string m_strVersion;
		Path m_mediaWorkdir;
		Path m_QAWorkdir;
		Path m_liveWorkdir;
		Path m_packScriptWorkdir;
		int	 m_PEFCountryIdx = -1;
		std::string m_strMediaVCS;	// VCS : Version Control System.
		std::string m_strQAVCS;
		std::string m_strLiveVCS;
		std::string m_strPackScriptVCS;

		// V3 UI 사용 유무.
		bool m_enable_V3UI_1_0 = true;
		bool m_enable_V3UI_1_1 = true;
		bool m_enable_V3UI_1_5 = true;

		Path m_mediaLocaleFolderRelPath;	// 미디어 내 국가별 번역 파일 저장소.
	};

	using CountryDataPtr = std::shared_ptr < CountryData >;

	class CountrySettings : public i3::shared_ginst<CountrySettings>
	{
	public:
		CountrySettings();
		~CountrySettings();

		void Load();
		void Save();

		void AddData(const CountryData& countryData);
		void SetData(const CountryData& countryData);
		void DelData(const std::string& strCountryName);
		CountryDataPtr FindData(const std::string& strCountryName);
		bool IsDataExist(const std::string& strCountryName) const;
		void GetDataList(std::vector<CountryDataPtr>& outList) const;

	private:
		using CountryDataTable = std::vector < CountryDataPtr >;
		CountryDataTable m_countryDataList;
	};

	const std::array<const char*, 3>& GetVersionStringList();
	const std::array<const char*, 2>& GetVCSList();
}
