#ifndef _QUICK_START_STORAGE_H_
#define _QUICK_START_STORAGE_H_
/*
namespace Qjoin
{
	struct Mode
	{
		STAGE_MODE stageType;
		STAGE_OPTION_TYPE stageOptionType;

		Mode() : stageType(STAGE_MODE_NA), stageOptionType(STAGE_OPTION_NONE) {}
		Mode(const STAGE_MODE& type, const STAGE_OPTION_TYPE& option)
			: stageType(type), stageOptionType(option) {}
	};

	inline bool operator == (const Mode& lhs, const Mode& rhs)
	{
		if ((lhs.stageType == rhs.stageType) && (lhs.stageOptionType == rhs.stageOptionType)) return true;
		return false;
	}

	inline bool operator < (const Mode& lhs, const Mode& rhs)
	{
		if (lhs.stageType < rhs.stageType) return true;
		if (lhs.stageType > rhs.stageType) return false;
		if (lhs.stageOptionType < rhs.stageOptionType) return true;

		return false;
	}

	class MapStorage : public i3::shared_ginst<MapStorage>
	{
	public:
		bool GetAllMode(i3::vector<Mode>& outList) const;
		bool GetModeListByStageUID(const STAGE_UID stage, i3::vector<Mode>& outList) const;
		bool GetAllStage(i3::vector<STAGE_UID>& outList) const;
		bool GetStageListByMode(const Mode& mode, i3::vector<STAGE_UID>& outList) const;
	};
}
*/
#endif