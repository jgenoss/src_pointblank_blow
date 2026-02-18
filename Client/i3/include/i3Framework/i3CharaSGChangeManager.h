#pragma once


// 입력 데이터  : 첫인수) .I3CHR파일이름 (확장자가 없어도 되고 있어도 됨)
//                둘째인수) .I3S파일 이름 (확장자가 없어도 되고 있어도 됨)
//				: 일단 weapon.pef에 등록된 무기CHR이름을 우선 고려해서 작성하므로, 
//                pef내용대로 I3CHR파일을 입력시 전체경로를 쓰지 않고 pef값 그대로 넣는다..
//               이제 키검색으로는 파일의 타이틀만 쓰이게 될것임...
//

struct i3CharaSGChangeInfo;

class I3_EXPORT_FRAMEWORK i3CharaSGChangeManager : public i3::shared_ginst<i3CharaSGChangeManager>
{
public:
	~i3CharaSGChangeManager();
	
	bool	AddData(const i3::wliteral_range& wstrCharaFile, 
					const i3::wliteral_range& wstrSGFileToChange, i3::rc_string& outFakeCharaFile);

	bool	AddData(const i3::rc_string& strCharaFile,
					const i3::rc_string& strSGFileToChange, i3::rc_string& outFakeCharaFile);
	
	const i3CharaSGChangeInfo*	FindSGChangeInfo(const i3::literal_range& strFakeCharaFile) const;


	static i3::rc_string		GetRefi3CharaFileName(const i3CharaSGChangeInfo* info);
	static i3::rc_string		GetTgti3CharaFileName(const i3CharaSGChangeInfo* info);
	static i3::rc_string		GetTgtI3SFileName(const i3CharaSGChangeInfo* info);

private:
	
	i3::unordered_map<i3::rc_string, i3CharaSGChangeInfo*>	m_clonemap;

};