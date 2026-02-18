#pragma once

#include "Weapon_Pane.h"
#include "ItemList_Pane.h"
//#include "ConsoleLogger.h"
#include <sstream>
#include <set>
#include <deque>
#include "i3Base/string/ext/change_file_ext.h"
#include "Equip_Pane.h"
class CMainFrame;

class CGlobalInstance : public CSingleton<CGlobalInstance>
{
	std::map<int, CWeapon_Pane*>	m_mapPane;
	//CWeapon_Pane		*m_pWeaponPane;
	CItemList_Pane		*m_pItemListPane;
	//CEquip_Pane			*m_pEquipPane;
	//CConsoleLoggerEx	*m_pConsole_Log;
	std::deque<std::string> m_deqStatusBarMsg;
	INT32				m_nPrograssbar;
	bool				m_bSave;
public :
	CWnd*				m_CurFocusWindow;

public:
	CGlobalInstance();
	~CGlobalInstance();
	void Create(CMainFrame *pMainFrame);
	void Update();

	CWeapon_Pane* GetWeaponPane(int nPropIndex);
	CItemList_Pane* GetItemListPane(){ return m_pItemListPane; }
	void SetSave(bool bState){ m_bSave = bState; }
	bool IsSave(){ return m_bSave; }
	//CConsoleLoggerEx* GetConsoleLogger() { return m_pConsole_Log; }

	//언더바 들어간거 다삭제
	void _CompressString(std::string &str);
	void _ReplaceString(std::string &str, std::string findString, std::string replaceString);
	char* StringToChar(CString& _strSource, char* _szResult, int nSize);
	std::vector< std::string > _Split(const std::string& src, const std::string & delims, unsigned int maxSplits = 0);
	

	UINT8 _toParseUINT8(const char* Src);
	INT32 _toParseUINT32(const char* Src);
	REAL32 _toParseREAL32(const char* Src);
	VEC2D _toParseVEC2D(const char* Src);
	VEC3D _toParseVEC3D(const char* Src);
	VEC4D _toParseVEC4D(const char* Src);
	I3COLOR _toParseCOLOR(const char* Src);
	MATRIX _toParseMATRIX(const char* Src);
	

	void _toString(UINT8 Src, char* Dest, size_t sizeOfDest);
	void _toString(INT32 Src, char* Dest, size_t sizeOfDest);
	void _toString(REAL32 Src, char* Dest, size_t sizeOfDest);
	void _toString(const VEC2D & Src, char* Dest, size_t sizeOfDest);
	void _toString(const VEC3D & Src, char* Dest, size_t sizeOfDest);
	void _toString(const VEC4D & Src, char* Dest, size_t sizeOfDest);
	void _toString(const I3COLOR & Src, char* Dest, size_t sizeOfDest);
	void _toString(const MATRIX & Src, char* Dest, size_t sizeOfDest);
	


	//value값을 문자열로 변환
	void _Convert2String(char * conv, size_t sizeOfString, i3RegData * pData);
	void _String2Convert(char * conv, size_t sizeOfString, i3RegData * pData);

	void cprintf(const char *format,...);

	void SetPrograssBar(INT32 nPos);

	void AddStatusBarMsg(std::string sText){ m_deqStatusBarMsg.push_front(sText); }

	void AddRegistry();
	void DeleteRegistry();
	void StartSave();
	void Save();
	int GetCurFocusWindow();
	
};