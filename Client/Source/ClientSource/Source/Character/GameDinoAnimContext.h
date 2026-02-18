#if !defined( __GAME_DINO_ANIM_CONTEXT_H__)
#define __GAME_DINO_ANIM_CONTEXT_H__

#include "../GameCharaAnimContext.h"

class CGameDinoAnimContext : public CGameCharaAnimContext
{
	I3_CLASS_DEFINE( CGameDinoAnimContext, CGameCharaAnimContext);

	friend class CGameCharaDino;
	friend class CGameCharaDinoTRex;
	friend class CGameCharaDinoRaptor;
	friend class CGameCharaDinoElite;
	friend class CGameCharaDinoSting;
	friend class CGameCharaDinoTank;
	friend class CGameCharaDinoAcid;
	friend class CGameCharaDinoStingMars;
	friend class CGameCharaDinoCCRaptor;
	friend class CGameCharaDinoCCSting;
	

	friend class CGameCharaActionContext;
	friend class CGameDinoActionContext;
	friend class CGameDinoBoneContext;

	friend class CGameCharaActionControl;
	friend class CGameDinoAnimControl3PV;

private:
	ID_LOWER_DINO_AI	m_nOldDinoLowerId = ID_LOWER_DINO_UNKNOWN;
	ID_UPPER_DINO_AI	m_nOldDinoUpperId = ID_UPPER_DINO_UNKNOWN;

	INT8				m_nDinoAILowerId[ CHARA_BODYLOWER_MAXCOUNT][ ID_LOWER_AI_MAX];
	INT8				m_nDinoAIUpperId[ CHARA_VIEW_MAX][ ID_UPPER_DINO_AI_MAX];

protected:

	/** \brief 공룡 상체 animation을 설정한다.
		\param[in] aiID ID_UPPER_DINO_AI index
		\param[in] startTime 시작 시간
		\param[in] scale animation scale 값 */
	bool			_Play_Anim_DinoUpper( ID_UPPER_DINO_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

	/** \brief 하체 animation을 설정한다.
		\param[in] aiID ID_LOWER_AI index
		\param[in] startTime 시작 시간
		\param[in] scale animation scale 값 */
	bool			_Play_AnimDino_Lower( ID_LOWER_DINO_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

public:
	CGameDinoAnimContext() {}

	ID_LOWER_DINO_AI	getOldDinoLowerID( void)					{ return m_nOldDinoLowerId; }

	/** \brief 이전 lower ai id를 설정합니다. */
	void				setOldDinoLowerID( ID_LOWER_DINO_AI id)		{ I3ASSERT( id < ID_LOWER_DINO_AI_MAX); m_nOldDinoLowerId = id; }

	/** \brief 공룡의 이전 upper ai id를 반환합니다. */
	ID_UPPER_DINO_AI	getOldDinoUpperID( void)				{ return m_nOldDinoUpperId; }

	/** \brief 공룡의 이전 upper ai id를 설정합니다. */
	void				setOldDinoUpperID( ID_UPPER_DINO_AI id)			{ I3ASSERT( id < ID_UPPER_DINO_AI_MAX); m_nOldDinoUpperId = id; }

	/** \brief 공룡의 Upper AI id를 반환합니다.
		\param[in] viewType 1/3인칭 flag
		\param[in] ai ID_UPPER_DINO_AI index */
	INT32				getDinoAIUpperID( ID_UPPER_DINO_AI ai);

		/** \brief 공룡의 Upper AI id를 설정합니다.
		\param[in] viewType 1/3인칭 flag
		\param[in] ai ID_UPPER_DINO_AI index
		\param[in] id id */
	void				setDinoAIUpperID( CHARA_VIEW_TYPE viewType, ID_UPPER_DINO_AI ai, INT32 id)
	{
		I3ASSERT( viewType < CHARA_VIEW_MAX);
		I3ASSERT( ai < ID_UPPER_DINO_AI_MAX);

		I3ASSERT( id < 128);
		m_nDinoAIUpperId[ viewType][ ai] = (INT8) id;
	}

	INT32				getDinoAILowerID( ID_LOWER_DINO_AI ai);

	void				setDinoAILowerID( CHARA_ACTION_BODYLOWER body,ID_LOWER_DINO_AI ai, INT32 id)
	{
		I3ASSERT( body < CHARA_BODYLOWER_MAXCOUNT);
		I3ASSERT( ai < ID_LOWER_DINO_AI_MAX);
		I3ASSERT( id < 128);

		m_nDinoAILowerId[ body][ ai] = (INT8) id;
	}

	/** \brief 뛰는 방향에 따른 DinoLower AI id를 반환합니다. */
	ID_LOWER_DINO_AI	getMoveIdByDirForDino( VEC3D * vDir);

	/** \brief 걷는 방향에 따른 Lower AI id를 반환합니다. */
	ID_LOWER_DINO_AI	getWalkIdByDirForDino( VEC3D * vDir);


	/** \brief Bone List를 작성합니다.
		\note Bone에 대한 처리를 할 시 필요함. */
	virtual void		BindBoneList() override;

};


#endif
