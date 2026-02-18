#pragma once


class CFileList;

class CCoreFileChecker
{
public:
	CCoreFileChecker(void);

public:
	bool	Open();
	int		Check(const tstring & FilePath);

	// cpp에 선언된 파일을 검사한다.
	int		CoreFilesCheck();

	const UINT32* GetMd5(const tstring & FilePath);

private:
	std::tr1::shared_ptr<CFileList>	m_FileList;
	bool					m_IsNTFS;
};
