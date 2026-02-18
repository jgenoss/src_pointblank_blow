#pragma once

//
// cpp 구현은 i3PackNode.cpp에 있음...  이 헤더는 내부용이다..( i3PackNode.cpp / i3PackVolume.cpp ) 에만 쓴다..
//
struct i3PackFileProgressCallback;

class i3ResourceFile_Pack : public i3ResourceFile
{
public:

	i3ResourceFile_Pack(i3PackNode* root, i3PackFileProgressCallback* cb); 

	void		OnProgress_Start(); 
	void		OnProgress_Write_ContentsFile_Start( const char* szResFilePath, bool is_enc );
	void		OnProgress_Write_ContentsFile_End( const char* szResFilePath, bool is_enc, bool is_success );
	void		OnProgress_End();
	
private:
	static void GetFileCount(i3PackNode* node, int& out_num );

	i3PackFileProgressCallback* m_cb;
	int			m_curr_file_count;
	int			m_total_files;
	int			m_success_count;
};

