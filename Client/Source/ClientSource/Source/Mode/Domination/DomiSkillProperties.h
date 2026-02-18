#ifndef _DOMI_SKILL_PROPERTIES_H_
#define _DOMI_SKILL_PROPERTIES_H_

namespace domi
{
	// 추가 변경없이 파일로부터 읽어들인 초깃값을 계속 들고간다.
	template<class T>
	class Value
	{
	public:
		explicit Value(const T& val) : m_val(val) {}
		T GetVal() const { return m_val; }

	private:
		T m_val;
	};

	// SetFactor를 사용하여 가중치 적용된 값을 저장.
	template<class T>
	class WeightedValue
	{
	public:
		explicit WeightedValue(const T& val) : m_currVal(val), m_origVal(val) {} 

		void SetFactor(const REAL32 ratio) 
		{ 
			const REAL32 decrFactor = (1.0f - ratio) + I3_EPS;
			const T decrease = static_cast<T>(m_origVal * decrFactor);
			m_currVal = m_origVal - decrease;
			if (m_currVal < 0) m_currVal = 0;
		}

		T GetVal() const { return m_currVal; }

	private:
		T m_currVal;
		T m_origVal;
	};

	// 아래 주요 구조체들의 변수들은 모두 포인터로 선언한다.
	// Value/WeightedValue 클래스의 초깃값은(파일에서 읽어들여 저장할) 생성시에만 가능하기 때문.
	struct SENTRYGUN_ELEM : i3::noncopyable
	{
		WeightedValue<UINT16>* install_sp;
		WeightedValue<UINT16>* repair_sp;
		Value<INT16>* max_hp;

		SENTRYGUN_ELEM();
		~SENTRYGUN_ELEM();
	};

	struct DUMMY_ELEM : i3::noncopyable
	{
		WeightedValue<UINT16>* install_sp;
		WeightedValue<UINT16>* repair_sp;
		Value<INT16>* max_hp;
		Value<INT16>* explosion_damage;

		DUMMY_ELEM();
		~DUMMY_ELEM();
	};

	struct SUPPLYCOST_ELEM : i3::noncopyable
	{
		WeightedValue<UINT16>* hpsupply_sp;
		WeightedValue<UINT16>* ammosupply_sp;

		SUPPLYCOST_ELEM();
		~SUPPLYCOST_ELEM();
	};

	struct USERSKILL_ELEM : i3::noncopyable
	{
		Value<INT8>* level_max;
		i3::vector<WeightedValue<INT16>*> cost_sp;
		i3::vector<Value<INT16>*> mininum_round;
		i3::vector<Value<REAL32>*> incr_ratio;

		USERSKILL_ELEM();
		~USERSKILL_ELEM();

		WeightedValue<INT16>& GetCostSp(const INT32 lv);
		INT16 GetMinRound(const INT32 lv) const;
		REAL32 GetIncrRatio(const INT32 lv) const;
		UINT32 GetIncrPercentage(const INT32 lv) const;
	};

	struct SP_ELEM : i3::noncopyable
	{
		Value<UINT16>* default;
		Value<UINT16>* max;

		SP_ELEM();
		~SP_ELEM();
	};

	class SkillProp : public i3::shared_ginst<SkillProp>
	{
	public:
		SkillProp();
		~SkillProp();

		void LoadFromFile();
		void Clear();

		SUPPLYCOST_ELEM& GetSupplycost();
		SENTRYGUN_ELEM& GetSentrygun(const INT32 lv);
		DUMMY_ELEM& GetDummy(const INT32 lv);
		const SP_ELEM& GetSP(const INT32 round);
		USERSKILL_ELEM& GetUserSkill(const INT32 type);

	private:
		SUPPLYCOST_ELEM m_supplycost;
		i3::vector<SENTRYGUN_ELEM*> m_sentryguns;
		i3::vector<DUMMY_ELEM*> m_dummys;
		i3::vector<SP_ELEM*> m_sps;
		i3::vector<USERSKILL_ELEM*> m_userskills;
		bool m_isLoaded;
	};
}

#endif