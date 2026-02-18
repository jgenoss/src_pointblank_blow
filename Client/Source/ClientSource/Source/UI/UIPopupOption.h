#if !defined( __UI_POPUP_OPTION_H__)
#define __UI_POPUP_OPTION_H__

#include "UIPopupBase.h"
#include "UIOptionBase.h"

class UIPopupOption : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupOption, UIPopupBase);

public:
	static UIPopupOption* instance();

private:
	i3::vector< class UIOptionBase* > m_list;
	i3::vector< KeyItem > m_popupKeyItem[UI_KEYCTRL_TYPE_MAX];

public:
	void change_option(int idx) const;

	void apply_save_option() const;	//apply 후에 save 한다.
	void load_apply_option(bool restore) const;	//restore와 같은 기능이다.
	bool is_modified_option() const;

	UIOptionBase* GetOption(UINT32 opt) { return m_list[opt]; }
	UIOptionBase* GetCurrentOption() const;

public:
	bool m_InGame;	//로비와 인게임 두곳 모두 사용하기 때문에 인게임용 flag. 활성화 방식이 다름
	bool m_StartGame;	//게임 시작시에 설정된 값으로 게임을 셋팅한다.
	
public:
	void clicked(int idx) const;

public:
	UIPopupOption();
	virtual ~UIPopupOption();

	virtual void OnCreate( i3GameNode * pParent) override;
	virtual void _InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	virtual bool OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual	void OnLoadAllScenes() override;
	
	virtual bool OnExitStart(void) override;
	virtual void OnUnloadAllScenes() override;

	virtual void OnUpdate( REAL32 rDeltaSeconds) override;
	virtual bool OnKeyInput(i3InputDeviceManager * pInputMng) override;
	virtual bool OnKey_Escape() override;

	virtual void ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;
	virtual void SetInputDisable( bool bDisable) override;
private:
	// About Key Item Add
	void initialize_keyitem();	
	void add_key_item(UI_KEYCTRL_LIST_TYPE list_idx, GAME_KEY_MAP menu);
	void add_key_item(const i3::rc_wstring& wstr_text);

public:	
	// m_popupKeyItem 사용 가능 여부 return
	bool	isInitKeyItem(); 
	// Envset KeyMapArray 값을 불러와 KeyItem 에 저장
	bool	isLoadKeyItem();

	KeyItem			getKeyItem(UI_KEYCTRL_LIST_TYPE _type, int _idx);
	void			setKeyItem_value(UI_KEYCTRL_LIST_TYPE _type, int _idx, GAME_KEY_STRUCT _value);
	GAME_KEY_STRUCT getKeyItem_value(UI_KEYCTRL_LIST_TYPE _type, int _idx);

	// m_popupKeItem 의 사이즈 return
	size_t			getKeyListSize(UI_KEYCTRL_LIST_TYPE _type);
	// KeyItem의 Slot Type return. TITLE인지 ITEM인지
	SLOT_TYPE		getKeyItem_slotType(UI_KEYCTRL_LIST_TYPE _type, int _idx);
	
	// 중복된 KeyItem 검사. 존재시 return true
	bool			isDuplicateKey(UINT32 _key, INT8 _device_type, UI_KEYCTRL_LIST_TYPE _type, int _idx);
	// Envset KeymapArray와 비교. 다를 경우 return false
	bool			isMatchValue( UI_KEYCTRL_LIST_TYPE _type, int _idx);
	// 전체 KeyItem 초기화
	void			setKeyItem_default();
	// Envset KeymappArray에 저장
	void			saveKeyItem( UI_KEYCTRL_LIST_TYPE _type, int _idx);
	// 사용되지 않은 Key 체크
	bool			isEmptyKeyItem();
};

#endif
