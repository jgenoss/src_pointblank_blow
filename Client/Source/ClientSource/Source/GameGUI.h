#pragma once

enum GUI_COLOR_TYPE
{
	GCT_DEFAULT = 0,
	GCT_FOCUS,
	GCT_STATIC,
	GCT_INVERSE,
	GCT_NOTIFY,
	GCT_BLUE,
	GCT_MY_SLOT,
	GCT_TEAM_CHAT,
	GCT_CLAN_CHAT,
	GCT_WISPER_CHAT,
	GCT_MEGAPHONE_CHAT,
	GCT_DEATHMATCH_MODE,
	GCT_BOMB_MODE,
	GCT_DESTROY_MODE,
	GCT_TRAINING_MODE,
	GCT_VALUE,
	GCT_RED,
	GCT_LIGHT_GREEN,
	GCT_YELLOW,
	GCT_ANNIHILATION_MODE,
	GCT_SHOOTINGFIELD_MODE,
	GCT_MATCHMAKING,
	GCT_INVALID_C,
	GCT_VALID_C,
	GCT_BOSS_MODE,
	GCT_SNIPER_MODE,
	GCT_SHOTGUN_MODE,
	GCT_DEFENCE_MODE,
	GCT_SUDDEN_DEATH_MODE,
	GCT_HEAD_HUNTER_MODE,
	GCT_HEAD_KILLER_MODE,
	GCT_KNUCKLE_MODE,

	GCT_MAX
};

namespace	GameGUI
{
	void InitColor(void);

	I3COLOR* GetColor(GUI_COLOR_TYPE colorType);
	const I3COLOR* GameModeColor(UINT32 GameMode);

	void	RemoveColorKeyword(const char * pStr, char * OutStr, INT32 OutStrSize);
	void	RemoveColorKeyword(string & str);

	enum
	{
		RANK_MARK_SIZE = 22,
		RANK_MARK_INTERVAL_SIZE = 24,
		RANK_MARK_HORZ_COUNT = 10,
	};
	void	SetRankStatic(i3GuiStatic * pStatic);

	struct ResizingForm
	{
	public:
		ResizingForm();

		void	Reset();
		int		Get(int x, int y) const;
		void	Set(int x, int y, int idx);

	private:
		int idx[3][3] = { 0 };
	};
	bool	ResizingControl(i3GuiControl * pControl, const ResizingForm * pForm, REAL32 Width, REAL32 Height);
}

//----------------------------------////----------------------------------//
namespace	GameGUI
{
	template< class T >
	void FLUSH_CONTROL(T & control)
	{
		control->m_bOnceUpdate = true; 
		control->OnUpdate(0.0f);
	};
}


//----------------------------------////----------------------------------//
#define FINDCTRL_CRC

#if defined( FINDCTRL_CRC)
struct STRCINFO
{
	UINT32			crc = 0;
	i3GameNode *	pCtrl = nullptr;
};

template<> struct i3::less<STRCINFO*> 
{
	bool operator()( const STRCINFO * p1, const STRCINFO * p2) const { return p1->crc < p2->crc; }
	bool operator()( UINT32 crc1, const STRCINFO * p2) const { return crc1 < p2->crc; }
	bool operator()( const STRCINFO * p1, UINT crc2) const { return p1->crc < crc2; }
};

static i3GameNode * FindControl( const i3::vector_set<STRCINFO*>& List, const char * pszCtrlName)
{
	UINT32 CRC = CRC32( 0xFFFFFFFF, (UINT8 *) pszCtrlName, i3::generic_string_size( pszCtrlName));

	i3::vector_set<STRCINFO*>::const_iterator it = List.find_other(CRC);

	if( it == List.end() )
		return nullptr;
	else
	{
		STRCINFO * pCtl = *it; 
		return pCtl->pCtrl;
	}
}

//#define LINK_CONTROL_EX(str,control,class,id)	control = (class*)FindControl( plist, str); I3ASSERT( control != nullptr); control->SetControlID(id);
template<typename T> inline  void LinkControlEx(const i3::vector_set<STRCINFO*>& list, const char * str, T*& control,INT32 id)
{
	// control = dynamic_cast<i3GuiObjBase*>(FindControl( plist, str)); 
	control = (T*)(FindControl( list, str)); 
	if( control == nullptr)
	{
		char szTemp[256] = "";
		i3::snprintf( szTemp, 256, "Cannot find a Gui Control (%s)", str);
		I3PRINTLOG(I3LOG_FATAL,  szTemp);	// Control이 없을 경우 어차피 이걸 사용하는 부분에서 크래시나므로 여기서 미리 크래시시킴
	}
	I3ASSERT( control != nullptr); 
	if(control != nullptr) {
		control->SetControlID(id);
	}
}
#endif //end of #if defined( FINDCTRL_CRC)
