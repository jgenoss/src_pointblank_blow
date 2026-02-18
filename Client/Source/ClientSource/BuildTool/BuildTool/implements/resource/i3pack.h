#pragma once

namespace rsc
{
	class i3pack final
	{
	public:
		i3pack();
		explicit i3pack(const char* pszSrcWorkdir);
		i3pack(const char* pszSrcWorkdir, const char* pszTgtWorkdir);
		~i3pack();

		bool LoadPack(const char* pszi3PackAbsPath);

		bool SavePack(const char* targetDir, const char* fileName);
		bool SavePack(const char* fileName);
		
		bool AddFileToPack(const char* fileRelPath, const bool includeSubdirOK);
		bool AddFileToPack(const char* fileRelPath);
		
		bool RemoveFileFromPack(const char* fileRelPath, const bool includeSubdirOK);
		bool RemoveFileFromPack(const char* fileRelPath);

		void EncryptFileInPack(const char* fileRelPath, const bool enc = true);
		
		bool SetChecksumToPack(const char* i3packName);
		bool SetChecksumToPack(const char* workdir, const char* i3packName);
		
		void ExportFiles(const char* targetDir);

		void SetPackSrcWorkdir(const std::string& strPath)	{ m_strPackSrcWorkdir = strPath; }
		void SetPackTgtPath(const std::string& strPath)		{ m_strPackTgtPath = strPath; }

		i3PackNode* GetRootNode() const { return m_node; }

		void CopyFrom(const i3pack& src);

	private:
		void		_CreatePack();
		i3PackNode* _AddPathsToPackNode(const char* fileAbsPath, const bool isDir);
		bool		_CheckPathValid(const char* path) const;
		void		_AddDirectory(const char* fileAbsPath, i3PackNode* node, const bool includeSubdirOK);
		bool		_BuildupFileList(const char* fileAbsPath, i3PackNode* parentNode);
		void		_Rec_Export(i3FileStream* fs, i3PackNode* node, const char* path);

	private:
		i3PackNode* m_node = nullptr;
		std::string m_strPackSrcWorkdir;	// 팩 대상 파일 원본 작업경로(국가 QA 폴더).
		std::string m_strPackTgtPath;		// 팩 저장 경로(국가 LIVE Pack 폴더).
	};

	namespace i3packUtil
	{
		bool DisableChecksum(const char* pszi3packAbsPath);
		void GetChecksumCode(const char* pszi3packAbsPath, std::vector<UINT32>& out);
		void CheckOffsetValidity(const char* pszi3PackAbsPath);
	}
}