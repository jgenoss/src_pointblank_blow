#pragma once

//
// 그닥 유용하지 않고 구현도 이상하지만...일단은 급하게 필요로 하는 기존 코드 함수들을 이쪽으로 정리...
//

namespace vt_util
{
	I3_EXPORT_FRAMEWORK
	void GetVariousUIFileName(char* res, const i3::literal_range& strDefault, int idx);


	// 파일 포맷 2개가 운용되어야 하므로, 버추얼텍스쳐 파일 존재 확인시 IsFile대신에 이걸 선택해야한다.

	// UIL파일 존재여부 함수는 만들어두긴 했지만, 활용할 일은 없을것 같다..
	I3_EXPORT_FRAMEWORK
	bool IsAnyUilFileExist_InVTexDirectory(const i3::literal_range& path);
	
	// 실제로는 VirtualTexture/VTexList 폴더 중 어느 uil파일을 선택할지 결정해서 리턴해야 됨...
	// UIL파일 포맷 변경으로 인해, 이 함수 말고 다른 함수를 따로 만들어서 처리해야함..
	I3_EXPORT_FRAMEWORK
	bool SelectUilFileByDate_InVTexDirectory(const i3::literal_range& path, i3::stack_string& outFullPath,
			bool& outIsVTexListFormat);
	
	I3_EXPORT_FRAMEWORK
	bool SelectUILibraryFileByDate_InVTexDirectory(const i3::literal_range& path, i3::stack_string& outFullPath,
			bool& outIsUILibHdrFormat, bool& outIsVTexListFormat);


	I3_EXPORT_FRAMEWORK
	bool IsVirtualTextureFileInHardDisk(const i3::literal_range& strFullPath, bool isVTexListFormat);
	

	// PACK파일과 실제폴더 파일간 호환성이 있는 폴더내 서브폴더 및 파일이름 수집함수가 필요하다.
	I3_EXPORT_FRAMEWORK
	bool CollectSubDirectoryAndFilename(const i3::literal_range& strDir,
		i3::vector<i3::rc_string>& vecSubDir, 
		i3::vector<i3::rc_string>& vecFileName, bool& outIsInPackFile);


	/*
	
	I3_EXPORT_FRAMEWORK
	bool SelectVirtualTextureFileByDate(const i3::literal_range& inputPath, 
		i3::rc_string& outResultPath, bool& outIsNewFormat);
		*/
}
