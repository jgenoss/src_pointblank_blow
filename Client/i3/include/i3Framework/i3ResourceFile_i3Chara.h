#pragma once


//
//  I3CHR파일을 공유하려는 목적에서...로딩 처리에 대해서만 구현변경을 가한다..

class i3ResourceFile_i3Chara : public i3ResourceFile
{
	I3_CLASS_DEFINE( i3ResourceFile_i3Chara, i3ResourceFile);	
public:
	UINT32			LoadOrClone( const char * pszPath, bool bCommonRes, bool bInThread);
	bool			IsClonedResult() const { return m_IsClonedResult; }

private:
	bool			m_IsClonedResult = false;

};
