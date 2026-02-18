#pragma once

//
// 순수하게 i3PackVolume과 i3PackNode 및 I3_PACK_FILE_REF 를 빨리 찾아내기 위해서 사용..
// 초기 로딩 후 게임 종료때까지 지워지지 않을 것이므로 레퍼런스카운팅없이 날포인터만으로 모두 관리하도록 처리한다.
// i3PackFileFindEntity의 첫 두 성분은 널값이 아니어야 합니다...
//
class i3PackVolume;
class i3PackNode;
struct I3_PACK_FILE_REF;

struct i3PackFileFindEntity
{
	i3PackVolume*		pPackVolume;
	i3PackNode*			pPackNode;
	I3_PACK_FILE_REF*	pPackFileRef;							
};


// 외부에 노출될 필요가 없는 클래스이므로 일단은 익스포트 키워드를 쓰지 않는다..
// 디렉토리를 오픈할수도 있기 때문에, 디렉토리 저장 기능도 함께 넣어둡니다..


class i3PackFileFindMgr : public i3::shared_ginst<i3PackFileFindMgr>
{
public:	
	~i3PackFileFindMgr();
	
	// 이 두함수는 인라인이기 때문에 I3_EXPORT_BASE가 필요한 것은 아니다..
	void				SetErrorMessageType(I3LOG_TYPE logType) { m_LogType = logType;  }
	I3LOG_TYPE			GetErrorMessageType() const { return m_LogType;  }

	const i3PackFileFindEntity*	FindEntity(const i3::literal_range& strResourceFilePath) const;
	void					AddPackVolume(i3PackVolume* pPackVolume);
	void					RemoveAll();

private:
	
	i3::unordered_map<i3::rc_string, i3PackFileFindEntity*>		m_FindMap;			// make_unix_path와 소문자화만으로 문자열키를 확정시킴..
	I3LOG_TYPE				m_LogType = I3LOG_NOTICE;
};
