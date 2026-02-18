#ifndef _QUICK_JOIN_SLOT_SCENE_H_
#define _QUICK_JOIN_SLOT_SCENE_H_

#include "../iSlot.h"
#include "../UIDefine.h"
#include "QJStorage.h"
#include "i3Base/itl/function/function.h"

#include "CSI_TypeDef.h"

namespace Qjoin
{
	class CollMgr;

	class ICollegue
	{
	public:
		ICollegue(CollMgr* m, i3UIScene* scene, const char* name);
		virtual ~ICollegue() {}

		void OnClicked();
		const i3::string& GetName() const { return m_name; }

	protected:
		CollMgr*	m_mgr;
		i3UIScene*	m_scene;
		i3::string	m_name;
	};

	class Coll_StartBtn : public ICollegue, public i3::noncopyable
	{
		friend class CollMgr;

	public:
		Coll_StartBtn(CollMgr* m, i3UIScene* scene, const char* name);
		virtual ~Coll_StartBtn();

	private:
		void Enable();
		void Disable();
		bool IsEnable() const;
	};

	class Coll_ModeCombobox : public ICollegue, public i3::noncopyable
	{
		friend class CollMgr;

	public:
		Coll_ModeCombobox(CollMgr* m, i3UIScene* scene, const char* name);
		virtual ~Coll_ModeCombobox();

	private:
		void		AddAllMode();
		void		AddModesByStage(const STAGE_UID stageUID);
		void		SelectMode(const CSI_RULE& rule, bool select_dummy = false);
		CSI_RULE*	GetCurrMode() const;

	private:
		i3::vector<CSI_RULE*>	m_rules;
		i3UIComboBox*			m_combobox;
		CSI_RULE*				m_dummyrule;				//Ç¥½Ã¿ë.
	};

	class Coll_StageCombobox : public ICollegue, public i3::noncopyable
	{
		friend class CollMgr;

	public:
		Coll_StageCombobox(CollMgr* m, i3UIScene* scene, const char* name);
		virtual ~Coll_StageCombobox();

	private:
		void AddAllStages();
		void AddStagesBydMode(const CSI_RULE* rule);
		STAGE_UID SelectStage(const STAGE_UID uid);
		STAGE_UID GetCurrStageUID() const;

	private:
		i3::vector<STAGE_UID>	m_stage_uids;
		i3UIComboBox*			m_combobox;
	};

	class Coll_Imgbox : public ICollegue, public i3::noncopyable
	{
		friend class CollMgr;

	public:
		Coll_Imgbox(CollMgr* m, i3UIScene* scene, const char* name);
		virtual ~Coll_Imgbox();

	private:
		void SetImage(const STAGE_UID uid);
	};

	class CollMgr : public i3::noncopyable
	{
		friend class Coll_StartBtn;
		friend class Coll_ModeCombobox;
		friend class Coll_StageCombobox;
		friend class Coll_Imgbox;

	public:
		explicit CollMgr(const INT32 sceneType);
		~CollMgr();

		void RegisterColl(ICollegue* coll, i3::function<> fn);
		void operator()(const ICollegue* coll);

		void ConstructOnLoadScenes(const CSI_RULE& rule, const STAGE_UID stageUID);
		void RefreshStartBtn();

	private:
		void _None();
		void _OnStartBtnClicked();
		void _OnModeSelClicked();
		void _OnStageSelClicked();

		ICollegue* _GetColl(const char* collname) const;

		struct Coll
		{
			ICollegue* coll; i3::function<> fn;
		};

		struct find_coll : i3::binary_function<const Coll*, const ICollegue*, bool>
		{
			bool operator()(const Coll* elem, const ICollegue* coll) const
			{
				return elem->coll == coll;
			}
		};

		i3::vector<Coll*> m_colls;

		INT32 m_sceneType;
	};
}

#endif