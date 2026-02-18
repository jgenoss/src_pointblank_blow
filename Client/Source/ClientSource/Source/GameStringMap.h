#ifndef __GAME_STRING_MAP_H__
#define __GAME_STRING_MAP_H__


//////////////////////////////////////////////////////////////////////////
// CGameStringMap
//////////////////////////////////////////////////////////////////////////

// 키는 i3::string / 값은 i3::rc_string이 되도록 수정..

class CGameStringMap
{
public:

public:
	enum GameStringIndex
	{
		GSI_STRING = 0,
		GSI_AICHARACTER,	GSI_BASIC,	GSI_CHARACTER,	GSI_EQUIPMENT,
		GSI_GOODS,		GSI_QUEST,		GSI_STAGE,		GSI_WEAPON,		GSI_WORLD,
		GSI_MAX,
	};
	typedef i3::vector< i3::rc_wstring > StringVector;
	typedef i3::unordered_map< i3::string, size_t > StringHashMap;		
	
private:
	struct StringTableContext
	{
		StringVector string_list;
		StringHashMap label_map;
	};
	StringTableContext m_StringTable[GSI_MAX];
	
private:
	enum { MAX_CACHE_SIZE = 256, };

private:
	bool _Create( const char* local_path, const char* local_name);			// 이런 함수 추가... 
	bool _AddFromTextFilesImpl(StringTableContext& table, const i3::string& strLocalStringPath, bool case_sensitive );
	const i3::rc_wstring& _GetStringImpl(StringTableContext& table, const i3::string& strLabel, bool case_sensitive);
	const i3::rc_wstring& _GetStringImpl(StringTableContext& table, const char* strLabel, bool case_sensitive);
	int _GetStringHandleImpl( StringTableContext& table,  const i3::string& strLabel, bool case_sensitive);
	const i3::rc_wstring& _GetStringByHandleImpl(StringTableContext& table, int nHandle);
	static bool _IsCaseSensitive(GameStringIndex idx );

public:
	static		CGameStringMap* Get();

	CGameStringMap();
	virtual ~CGameStringMap();

	bool CreateForPointBlank();
	void	 Destroy();


	// 라벨로 지역화된 문자열를 얻는다.(내부적으로 복사 없이 반환. map를 검사하므로)
	// 만약, 존재하지 않는 라벨이라면 라벨이름 자체를 문자열로 만들어 반환한다.
	const i3::rc_wstring&	GetString(  const i3::string& strLabel, GameStringIndex idx );
	const i3::rc_wstring&	GetString( const char* strLabel, GameStringIndex idx);


	// 라벨로 지역화된 문자열의 핸들을 얻는다.(내부적으론 빠른 검색을 위한 위치값)
	// 만약, 존재하지 않는 라벨이라면 -1을 반환한다.
	int				GetStringHandle( const i3::string& strLabel, GameStringIndex idx );

	// 핸들로 지역화된 문자열를 얻는다.(빠른 검색)
	// 만약, 존재하지 않는 핸들이라면 핸들의 숫자값을 문자열로 만들어 반환한다.
	const i3::rc_wstring&	GetStringByHandle( int nHandle, GameStringIndex idx );

	// 데이터 개수 반환
	int GetCount(GameStringIndex idx);

	// 자료구조 반환
	const StringVector&	GetStringList(GameStringIndex idx) const;
	const StringHashMap&		GetLabelMap(GameStringIndex idx) const;

	bool LoadingKoreanLanguage();
	bool LoadingEnglishLanguage();
};

//////////////////////////////////////////////////////////////////////////
// 속도와 크게 상관없으면 이걸 쓴다. (STL map)
// 이걸 주로 많이 애용.
inline const wchar_t* GAME_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_STRING).c_str();}
inline const wchar_t* GAME_PEF_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_AICHARACTER).c_str();}
inline const wchar_t* GAME_BASIC_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_BASIC).c_str();}
inline const wchar_t* GAME_CHARACTER_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_CHARACTER).c_str();}
inline const wchar_t* GAME_EQUIPMENT_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_EQUIPMENT).c_str();}
inline const wchar_t* GAME_GOODS_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_GOODS).c_str();}
inline const wchar_t* GAME_QUEST_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_QUEST).c_str();}
inline const wchar_t* GAME_STAGE_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_STAGE).c_str();}
inline const wchar_t* GAME_WEAPON_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_WEAPON).c_str();}
inline const wchar_t* GAME_WORLD_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_WORLD).c_str();}

inline const i3::rc_wstring& GAME_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_STRING);}
inline const i3::rc_wstring& GAME_PEF_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_AICHARACTER);}
inline const i3::rc_wstring& GAME_BASIC_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_BASIC);}
inline const i3::rc_wstring& GAME_CHARACTER_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_CHARACTER);}
inline const i3::rc_wstring& GAME_EQUIPMENT_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_EQUIPMENT);}
inline const i3::rc_wstring& GAME_GOODS_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_GOODS);}
inline const i3::rc_wstring& GAME_QUEST_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_QUEST);}
inline const i3::rc_wstring& GAME_STAGE_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_STAGE);}
inline const i3::rc_wstring& GAME_WEAPON_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_WEAPON);}
inline const i3::rc_wstring& GAME_WORLD_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_WORLD);}


inline const wchar_t* GAME_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_STRING).c_str();}
inline const wchar_t* GAME_PEF_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_AICHARACTER).c_str();}
inline const wchar_t* GAME_BASIC_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_BASIC).c_str();}
inline const wchar_t* GAME_CHARACTER_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_CHARACTER).c_str();}
inline const wchar_t* GAME_EQUIPMENT_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_EQUIPMENT).c_str();}
inline const wchar_t* GAME_GOODS_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_GOODS).c_str();}
inline const wchar_t* GAME_QUEST_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_QUEST).c_str();}
inline const wchar_t* GAME_STAGE_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_STAGE).c_str();}
inline const wchar_t* GAME_WEAPON_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_WEAPON).c_str();}
inline const wchar_t* GAME_WORLD_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_WORLD).c_str();}

inline const i3::rc_wstring& GAME_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_STRING);}
inline const i3::rc_wstring& GAME_PEF_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_AICHARACTER);}
inline const i3::rc_wstring& GAME_BASIC_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_BASIC);}
inline const i3::rc_wstring& GAME_CHARACTER_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_CHARACTER);}
inline const i3::rc_wstring& GAME_EQUIPMENT_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_EQUIPMENT);}
inline const i3::rc_wstring& GAME_GOODS_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_GOODS);}
inline const i3::rc_wstring& GAME_QUEST_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_QUEST);}
inline const i3::rc_wstring& GAME_STAGE_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_STAGE);}
inline const i3::rc_wstring& GAME_WEAPON_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_WEAPON);}
inline const i3::rc_wstring& GAME_WORLD_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_WORLD);}




//////////////////////////////////////////////////////////////////////////
// 속도가 아주 중요한 곳이면 GET_GAME_STRING_HANDLE 를 이용해서 핸들을 한번 저장했다가,
// GAME_STRING_BY_HANDLE를 이용하면 된다. (STL vector)
inline int GET_GAME_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_STRING);}
inline int GET_GAME_PEF_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_AICHARACTER);}
inline int GET_GAME_BASIC_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_BASIC);}
inline int GET_GAME_CHARACTER_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_CHARACTER);}
inline int GET_GAME_EQUIPMENT_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_EQUIPMENT);}
inline int GET_GAME_GOODS_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_GOODS);}
inline int GET_GAME_QUEST_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_QUEST);}
inline int GET_GAME_STAGE_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_STAGE);}
inline int GET_GAME_WEAPON_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_WEAPON);}
inline int GET_GAME_WORLD_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_WORLD);}

inline const wchar_t* GAME_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_STRING).c_str();}
inline const wchar_t* GAME_PEF_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_AICHARACTER).c_str();}
inline const wchar_t* GAME_BASIC_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_BASIC).c_str();}
inline const wchar_t* GAME_CHARACTER_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_CHARACTER).c_str();}
inline const wchar_t* GAME_EQUIPMENT_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_EQUIPMENT).c_str();}
inline const wchar_t* GAME_GOODS_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_GOODS).c_str();}
inline const wchar_t* GAME_QUEST_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_QUEST).c_str();}
inline const wchar_t* GAME_STAGE_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_STAGE).c_str();}
inline const wchar_t* GAME_WEAPON_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_WEAPON).c_str();}
inline const wchar_t* GAME_WORLD_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_WORLD).c_str();}

inline const i3::rc_wstring& GAME_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_STRING);}
inline const i3::rc_wstring& GAME_PEF_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_AICHARACTER);}
inline const i3::rc_wstring& GAME_BASIC_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_BASIC);}
inline const i3::rc_wstring& GAME_CHARACTER_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_CHARACTER);}
inline const i3::rc_wstring& GAME_EQUIPMENT_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_EQUIPMENT);}
inline const i3::rc_wstring& GAME_GOODS_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_GOODS);}
inline const i3::rc_wstring& GAME_QUEST_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_QUEST);}
inline const i3::rc_wstring& GAME_STAGE_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_STAGE);}
inline const i3::rc_wstring& GAME_WEAPON_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_WEAPON);}
inline const i3::rc_wstring& GAME_WORLD_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_WORLD);}



#endif //__GAME_STRING_MAP_H__

