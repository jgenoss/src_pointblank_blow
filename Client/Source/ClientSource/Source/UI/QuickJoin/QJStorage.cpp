#include "pch.h"
#include "QJStorage.h"
#include "../UIUtil.h"
#include <functional>
/*
namespace Qjoin
{
	bool GetEnableModeList(i3::vector<Mode>& outList)
	{
		if (IsAvailableStageMode(STAGE_MODE_DEATHMATCH))
		{
			outList.push_back(Mode(STAGE_MODE_DEATHMATCH, STAGE_OPTION_NONE));
			outList.push_back(Mode(STAGE_MODE_ANNIHILATION, STAGE_OPTION_ANNIHILATION));
		}

		if (IsSpecialMode(STAGE_OPTION_CHAOS))
			outList.push_back(Mode(STAGE_MODE_CHAOS, STAGE_OPTION_NONE));
		
		if (IsSpecialMode(STAGE_OPTION_HEADHUNTER))
			outList.push_back(Mode(STAGE_MODE_HEAD_HUNTER, STAGE_OPTION_NONE));

		if (IsAvailableStageMode(STAGE_MODE_DESTROY))
			outList.push_back(Mode(STAGE_MODE_DESTROY, STAGE_OPTION_NONE));

		if (IsAvailableStageMode(STAGE_MODE_BOMB))
			outList.push_back(Mode(STAGE_MODE_BOMB, STAGE_OPTION_NONE));

		if (IsAvailableStageMode(STAGE_MODE_DEFENCE))
			outList.push_back(Mode(STAGE_MODE_DEFENCE, STAGE_OPTION_NONE));

		if (IsAvailableStageMode(STAGE_MODE_ESCAPE))
			outList.push_back(Mode(STAGE_MODE_ESCAPE, STAGE_OPTION_NONE));

		if (IsAvailableStageMode(STAGE_MODE_CROSSCOUNT))
			outList.push_back(Mode(STAGE_MODE_CROSSCOUNT, STAGE_OPTION_NONE));

		if (IsAvailableStageMode(STAGE_MODE_CONVOY))
			outList.push_back(Mode(STAGE_MODE_CONVOY, STAGE_OPTION_NONE));

		if (IsAvailableStageMode(STAGE_MODE_RUN_AWAY))
			outList.push_back(Mode(STAGE_MODE_RUN_AWAY, STAGE_OPTION_NONE));

		// special mode
		if (IsSpecialMode(STAGE_OPTION_SHOTGUN))
		{
			if (IsSpecialTypeMode(STAGE_MODE_DEATHMATCH, STAGE_OPTION_SHOTGUN))
			{
				outList.push_back(Mode(STAGE_MODE_DEATHMATCH, STAGE_OPTION_SHOTGUN));
				outList.push_back(Mode(STAGE_MODE_ANNIHILATION, STAGE_OPTION_SHOTGUN));
			}

			if (IsSpecialTypeMode(STAGE_MODE_BOMB, STAGE_OPTION_SHOTGUN))
				outList.push_back(Mode(STAGE_MODE_BOMB, STAGE_OPTION_SHOTGUN));

			if (IsSpecialTypeMode(STAGE_MODE_DESTROY, STAGE_OPTION_SHOTGUN))
				outList.push_back(Mode(STAGE_MODE_DESTROY, STAGE_OPTION_SHOTGUN));

			if (IsSpecialTypeMode(STAGE_MODE_DEFENCE, STAGE_OPTION_SHOTGUN))
				outList.push_back(Mode(STAGE_MODE_DEFENCE, STAGE_OPTION_SHOTGUN));
		}

		if (IsSpecialMode(STAGE_OPTION_CHALLENGE))
			outList.push_back(Mode(STAGE_MODE_DEATHMATCH, STAGE_OPTION_CHALLENGE));

		if (IsSpecialMode(STAGE_OPTION_DIEHARD))
			outList.push_back(Mode(STAGE_MODE_DEATHMATCH, STAGE_OPTION_DIEHARD));

		if (IsSpecialMode(STAGE_OPTION_MADNESS))
			outList.push_back(Mode(STAGE_MODE_DEATHMATCH, STAGE_OPTION_MADNESS));

		if (IsSpecialMode(STAGE_OPTION_SNIPER))
			outList.push_back(Mode(STAGE_MODE_DEATHMATCH, STAGE_OPTION_SNIPER));

		if( IsSpecialMode( STAGE_OPTION_KNUCKLE))
			outList.push_back(Mode(STAGE_MODE_DEATHMATCH, STAGE_OPTION_KNUCKLE));

		return outList.empty() ? false : true;
	}

	bool GetEnableStageUIDList(const Mode& mode, i3::vector<STAGE_UID>& outList)
	{
		if (STAGE_MODE_NA == mode.stageType) return false;

		STAGE_MODE stageType = mode.stageType;
		STAGE_OPTION_TYPE optionType = mode.stageOptionType;

		// (STAGE_MODE_ANNIHILATION, STAGE_OPTION_ANNIHILATION), (STAGE_MODE_ANNIHILATION, STAGE_OPTION_SHOTGUN)
		// 위 두 가지 ANNIHILATION모드 스테이지를 GameUI::GetEnableMapList()를 이용하여 검출했더니
		// 이상한 스테이지 ID가 도출되어 아래와 같이 예외처리함
		// * ANNIHILATION 모드는 DEATH-MATCH 맵을 공유한다는 정보를 믿고 처리.
		if (stageType == STAGE_MODE_ANNIHILATION)
		{
			stageType = STAGE_MODE_DEATHMATCH;

			if (optionType == STAGE_OPTION_ANNIHILATION)
				optionType = STAGE_OPTION_NONE;
		}

		i3::vector<STAGE_ID> stageIDList;
		GameUI::GetEnableMapList(stageIDList, stageType, optionType);

		for (size_t i=0; i<stageIDList.size(); i++)
			outList.push_back(GetStageUID_ID(stageIDList[i]));

		i3::sort(outList.begin(), outList.end());
		outList.erase(std::unique(outList.begin(), outList.end()), outList.end());

		i3::vector<STAGE_UID>::const_iterator it = 
			i3::find(outList.begin(), outList.end(), STAGE_UID_NONE);

		if (it != outList.end())
			outList.erase(it);

		return outList.empty() ? false : true;
	}

	//----------------------------------------------
	// 모드별 스테이지 리스트 저장
	//----------------------------------------------
	class QJStorage_Mode2Stage
	{
	public:
		QJStorage_Mode2Stage()
		{
			i3::vector<Mode> modeList;
			GetEnableModeList(modeList);

			for (size_t i=0; i<modeList.size(); i++)
				_AddElement(modeList[i]);
		}

		~QJStorage_Mode2Stage()
		{
			i3::cu::for_each_delete_clear(m_elems);
		}

		bool GetAllMode(i3::vector<Mode>& outList) const
		{
			if (m_elems.empty()) return false;

			i3::vector<Element*>::const_iterator it = m_elems.begin();
			for (; it != m_elems.end(); ++it)
				outList.push_back((*it)->mode);

			return true;
		}

		bool GetStageListByMode(const Mode& mode, i3::vector<STAGE_UID>& outList) const
		{
			struct fn : public i3::binary_function<Element*, Mode, bool>
			{
				bool operator()(const Element* elem, const Mode& md) const
				{
					return elem->mode == md;
				}
			};

			i3::vector<Element*>::const_iterator it = 
				std::find_if(m_elems.begin(), m_elems.end(), std::bind2nd(fn(), mode));

			if (it != m_elems.end())
			{
				const i3::vector<STAGE_UID>& stageList = (*it)->stageList;

				for (size_t i=0; i<stageList.size(); i++)
					outList.push_back(stageList[i]);

				return true;
			}

			return false;
		}

	private:
		void _AddElement(const Mode& mode)
		{
			i3::vector<STAGE_UID> list;
			if (!GetEnableStageUIDList(mode, list)) return;

			Element* elem = new Element; MEMDUMP_NEW(elem, sizeof(Element));
			elem->mode = mode;
			elem->stageList.swap(list);
			m_elems.push_back(elem);
		}

		// 모드 개수는 현재 20개 미만이고(2014.08.02) 특성상 폭발적으로 늘어날 가능성은 없으므로 vector 사용.
		struct Element
		{
			Mode mode;
			i3::vector<STAGE_UID> stageList;

			Element() {}
		};

		i3::vector<Element*> m_elems;
	};

	const QJStorage_Mode2Stage& theStorage_Mode2Stage()
	{
		static QJStorage_Mode2Stage s;
		return s;
	}



	class QJStorage_Stage2Mode : public i3::shared_ginst<QJStorage_Stage2Mode>
	{
	public:
		QJStorage_Stage2Mode()
		{
			const QJStorage_Mode2Stage& sms = theStorage_Mode2Stage();

			i3::vector<Mode> modeList;
			if (!sms.GetAllMode(modeList)) return;

			i3::vector<STAGE_UID> stageList;
			for (size_t i=0; i<modeList.size(); i++)
			{
				const Mode& mode = modeList[i];

				if (!sms.GetStageListByMode(mode, stageList)) 
					continue;

				// 저장된 모든 스테이지를 돌면서 모드를 저장.
				for (size_t j=0; j<stageList.size(); j++)
				{
					i3::vector<Mode>* list = nullptr;

					const STAGE_UID stageUID = stageList[j];

					LookupTbl::iterator it = m_tbl.find(stageUID);
					if (it != m_tbl.end())
					{
						list = it->second;
						list->push_back(mode); // 모드가 중복되어 들어가지만 일단 넣고 아래에서 unique 처리.
					}
					else
					{
						list = new i3::vector<Mode>;
						list->push_back(mode);

						m_tbl.insert(LookupTbl::value_type(stageUID, list));
					}
				}

				stageList.clear();
			}

			// 빈 노드 삭제 및 모드 리스트 정렬/중복 처리.
			LookupTbl::const_iterator it = m_tbl.begin();
			for (; it != m_tbl.end();)
			{
				i3::vector<Mode>* list = it->second;
				
				if (list->empty())
				{
					delete list;
					m_tbl.erase(it++); 
					continue;
				}
				else
					++it;

				i3::sort(list->begin(), list->end());
				list->erase(std::unique(list->begin(), list->end()), list->end());
			}
		}

		~QJStorage_Stage2Mode()
		{
			i3::cu::for_each_delete_clear(m_tbl);
		}

		bool GetModeListByStage(const STAGE_UID stage, i3::vector<Mode>& outList) const
		{
			LookupTbl::const_iterator it = m_tbl.find(stage);
			if (it != m_tbl.end())
			{
				const i3::vector<Mode>* list = it->second;
				if (list)
				{
					for (size_t i=0; i<list->size(); i++)
						outList.push_back((*list)[i]);

					return true;
				}
			}

			return false;
		}

		bool GetAllStage(i3::vector<STAGE_UID>& outList) const
		{
			if (m_tbl.empty()) return false;

			LookupTbl::const_iterator it = m_tbl.begin();
			for (; it != m_tbl.end(); ++it)
			{
				const STAGE_UID& stage = it->first;
				outList.push_back(stage);
			}

			return true;
		}

	private:
		// 스테이지 UID 개수가 100 개 이상일 가능성이 크므로 hashmap 사용.
		typedef i3::unordered_map<STAGE_UID, i3::vector<Mode>*> LookupTbl;

		LookupTbl m_tbl;
	};

	const QJStorage_Stage2Mode& theStorage_Stage2Mode()
	{
		static QJStorage_Stage2Mode s;
		return s;
	}



	bool MapStorage::GetAllMode(i3::vector<Mode>& outList) const
	{
		return theStorage_Mode2Stage().GetAllMode(outList);
	}

	bool MapStorage::GetModeListByStageUID(const STAGE_UID stage, i3::vector<Mode>& outList) const
	{
		return theStorage_Stage2Mode().GetModeListByStage(stage, outList);
	}

	bool MapStorage::GetAllStage(i3::vector<STAGE_UID>& outList) const
	{
		return theStorage_Stage2Mode().GetAllStage(outList);
	}

	bool MapStorage::GetStageListByMode(const Mode& mode, i3::vector<STAGE_UID>& outList) const
	{
		return theStorage_Mode2Stage().GetStageListByMode(mode, outList);
	}
}
*/