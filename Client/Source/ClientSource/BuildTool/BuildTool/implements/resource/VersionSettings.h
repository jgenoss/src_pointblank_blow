#pragma once

namespace rsc
{
	// 버전 정보 : 소스 코드 경로 등. 
	// 애초에 버전 관리는 스트링으로 했으나 소스코드 경로가 필요하게 됨에 따라 별도로 추가함.
	class VersionData final
	{
	public:
		void SetVersion(const std::string& strVer);
		const std::string& GetVersion() const;

		void SetPointBlankAbsPath(const Path& path);
		const Path& GetPointBlankAbsPath() const;

		void Seti3EngineAbsPath(const Path& path);
		const Path& Geti3EngineAbsPath() const;

	private:
		std::string m_strVersion;	// 버전을 스트링으로 관리. enum 으로 교체해야겠다.
		Path m_PointBlankAbsPath;
		Path m_i3EngineAbsPath;
	};

	using VerDataPtr = std::shared_ptr < VersionData >;

	class VersionSettings : public i3::shared_ginst<VersionSettings>
	{
	public:
		VersionSettings();
		~VersionSettings();

		void Load();
		void Save();

		void AddData(const VersionData& verData);
		void SetData(const VersionData& verData);
		VerDataPtr FindData(const std::string& strVer) const;
		void GetAllDataList(std::vector<VerDataPtr>& out) const;

	private:
		using VerDataTable = std::vector < VerDataPtr >;
		VerDataTable m_versionDataList;
	};
}