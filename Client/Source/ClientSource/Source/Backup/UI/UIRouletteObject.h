#if !defined( __ROULETTE_OBJECT_H)
#define __ROULETTE_OBJECT_H


#define ROULETTE_MAX_SLOT 12

#define ROULETTE_GRADE_5STAR 5
#define ROULETTE_GRADE_7STAR 7
#define ROULETTE_GRADE_9STAR 9

enum ROULETTE_ANIM_STATE
{
	
	//룰렛 정지 동작 
	ROULETTE_END_CASE_0 ,
	ROULETTE_END_CASE_1,
	ROULETTE_END_CASE_2,
	ROULETTE_END_CASE_3,
	ROULETTE_END_CASE_4,
	ROULETTE_END_CASE_5,
	ROULETTE_END_CASE_6,
	ROULETTE_END_CASE_7,
	ROULETTE_END_CASE_8,
	ROULETTE_END_CASE_9,
	ROULETTE_END_CASE_10,
	ROULETTE_END_CASE_11,
	ROULETTE_END_CASE_12,
	//룰렛 아이들 동작	
	ROULETTE_IDLE_CASE_0 ,
	ROULETTE_IDLE_CASE_1 ,
	ROULETTE_IDLE_CASE_2 ,
	ROULETTE_IDLE_CASE_3 ,
	ROULETTE_IDLE_CASE_4 ,
	ROULETTE_IDLE_CASE_5 ,
	ROULETTE_IDLE_CASE_6 ,
	ROULETTE_IDLE_CASE_7 ,
	ROULETTE_IDLE_CASE_8 ,
	ROULETTE_IDLE_CASE_9 ,
	ROULETTE_IDLE_CASE_10 ,
	ROULETTE_IDLE_CASE_11 ,
	ROULETTE_IDLE_CASE_12 ,
	//룰렛 아이들 동작	
	ROULETTE_START_CASE_0 ,
	ROULETTE_START_CASE_1 ,
	ROULETTE_START_CASE_2 ,
	ROULETTE_START_CASE_3 ,
	ROULETTE_START_CASE_4 ,
	ROULETTE_START_CASE_5 ,
	ROULETTE_START_CASE_6 ,
	ROULETTE_START_CASE_7 ,
	ROULETTE_START_CASE_8 ,
	ROULETTE_START_CASE_9 ,
	ROULETTE_START_CASE_10 ,
	ROULETTE_START_CASE_11 ,
	ROULETTE_START_CASE_12 ,
	
	ROULETTE_COUNT
};

enum ROULETTE_STATE
{
	ROULETTE_STATE_IDLE,
	ROULETTE_STATE_START,
	ROULETTE_STATE_FINISH,
};

enum RS_ITEM_TYPE			
{
	RS_ITEM_NULL			= -1,
	RS_CHARA_MALE			= 0,		// 꽝 상품
	RS_CHARA_FEMALE			= 1,		// 별 1개
	RS_WEAPON				= 2,		// 별 2개 	
	RS_ITEM					= 3,		// 별 3개 
};


struct RS_SLOT_INFO	// 뽑기 상점 아이템 정보
{


	RS_ITEM_GRADE		eItemGrade;		// 등급. 별 갯수. 0 : 꽝, 1~3 : 각 등급
	RS_ITEM_TYPE		eItemType;
	UINT8				nItemIndex;

	void Reset()
	{
		eItemGrade = RS_ITEM_GRADE_NOT_SETTING;
		eItemType = RS_ITEM_NULL;
		nItemIndex =0;

	}
};




class RouletteObject : public i3Object
{
	I3_CLASS_DEFINE( RouletteObject, i3Object);
public:

	
//	void ProcessObject( RT_REAL32 rDeltaSeconds);
protected:
	// Persistant members
	/////////////////////////////////////////////////////////
	///룰렛관련 소스 추가합니다.
	/////////////////////////////////////////////////////////

private:
	i3Animation* m_pAnim[ROULETTE_COUNT];
	ROULETTE_ANIM_STATE m_eRouletteAnimState;
	ROULETTE_STATE		m_eRouletteState;
	REAL32 m_rStateTime;
	REAL32 m_rRotationTime;


	i3AttrSet*			m_arRouletteIconSet[ROULETTE_MAX_SLOT];
	i3AttrSet*			m_arRouletteSlotSet[ROULETTE_MAX_SLOT];
	i3TextureBindAttr*  m_arRouletteIconTexBindAttr[ROULETTE_MAX_SLOT];
	i3TextureBindAttr*  m_arRouletteSlotTexBindAttr[ROULETTE_MAX_SLOT];
	

	i3Texture*			m_pRouletteNotSettingTexture;
	i3Texture*			m_pRouletteBoxTexture;
	i3Texture*			m_pRouletteWeaponTexture[3];
	i3Texture*			m_pRouletteItemTexture;
	i3Texture*			m_pRouletteCharaMaleTexture[3];
	i3Texture*			m_pRouletteCharaFemaleTexture[3];
	i3Texture*			m_pRouletteSlotGradeTexture[4];

	RS_SLOT_INFO * m_slot[12];


	INT32				m_nStar;
	INT32				m_nSlot;
	INT32				m_nItemCount;
	void				_SetSlot(INT32 nArg){ m_nSlot = nArg;}
	INT32				_GetSlot()const{return m_nSlot;}
	void				_SetStar(INT32 nArg){ m_nStar = nArg;}
	INT32				_GetStar()const{return m_nStar;}
	

public:
	void				SetItemCount(INT32 nArg){ m_nItemCount = nArg;}
	INT32				GetItemCount()const{return m_nItemCount;}
	
	i3::vector<INT32>	m_SlotIndex;
	i3::vector<INT32>	m_WriteData;


public:
	RouletteObject();
	~RouletteObject();


	virtual void	Create( i3Framework * pFramework, i3GameResObject * pRes);
	virtual void	OnUpdate( REAL32 tm);


	//룰렛 애니메이션 STATE값을 설정합니다.
	void			SetRouletteAnimState(ROULETTE_ANIM_STATE eState,REAL32 rStartTime);
	//룰렛 IDLE상태가시작하는 단계인지 끝나는 단계인지 판별합니다..
	void			SetRouletteState(ROULETTE_STATE eState);

	
	ROULETTE_ANIM_STATE GetRouletteAnimState() { return m_eRouletteAnimState; }
	ROULETTE_STATE GetRouletteState()			 { return m_eRouletteState; }


	void			InitAnimation( void);


	void			InitRoulette( void);

	void			ResetObject();
	void			ResetObjectOnInit();
	void			EnableObject()	{}
	void			ProcessObject( RT_REAL32 rDeltaSeconds) {}
	void			SetControledObjectState( INT32 state) {}


	RS_SLOT_INFO			GetSlot(INT32 nArg){return *m_slot[nArg];}
	
	bool IsRouletteIdleState();
	RS_ITEM_TYPE ConvertItemIdToClassType(UINT32 pitemid);

	//룰렛 기능별 구현
	void	Shuffle(void);
	bool	InitButton(INT32 Grade);
	void	SetItem(RS_ITEM_INFO ItemInfo,INT32 m_pSelectTapIdx);
	void	SetBoomItem(INT32 pindex, INT32 ItemType);
	bool	CheckStar(INT32 nTemp,INT32 nTemp2);
	bool	isCheckRoulette();

	void	WriteData(INT32 TapIndex);	
private:
	INT32 _FindSlot(RS_ITEM_GRADE eGrade,INT32 nIndex, INT32 nTest);
	




};

#endif
