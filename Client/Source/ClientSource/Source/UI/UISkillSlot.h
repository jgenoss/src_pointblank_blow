#if !defined(__UI_SKILL_SLOT_H__)
#define __UI_SKILL_SLOT_H__

class UIBase;

class SkillSlot_Callback
{
public:
	virtual void	OnLoadSlot( INT32 idx, i3UIScene* scn) = 0;
	virtual void	OnEnableSlot(INT32 idx, i3UIScene* scn) = 0;
};

template<class T,	void (T::*ON_LOAD_SLOT)(INT32, i3UIScene*),
					void (T::*ON_ENABLE_SLOT)(INT32, i3UIScene*)
>

class UISkillSlot_Callback : public SkillSlot_Callback
{
public:
	UISkillSlot_Callback() : m_obj(NULL) {}
	void	SetObj(T* obj) { m_obj = obj; }
public:
	virtual void	OnLoadSlot( INT32 idx, i3UIScene* scn)	{ (m_obj->*ON_LOAD_SLOT)(idx,scn);	}
	virtual void	OnEnableSlot(INT32 idx, i3UIScene* scn)	{ (m_obj->*ON_ENABLE_SLOT)(idx,scn);}
	T*				m_obj;
};

class UISkillSlot
{
public:
	UISkillSlot();
	~UISkillSlot();

	void			OnCreate(UIBase* parent, INT32 row);
	void			RegisterSlotScene( const char *szSlotScene, const LuaFuncReg* Lua_Func, bool bLoaded = false);
	void			SetCallback(SkillSlot_Callback* callback)	{ m_Callback = callback;}

private:
	UIBase*					m_Parent;
	SkillSlot_Callback*		m_Callback;
	INT32					m_Row;
};

#endif