#include "pch.h"
#include "LadderTrigger.h"
#include "../BattleSlotContext.h"
#include "../GameCharaInputContext.h"
#include "../GameCharaMoveContext.h"

#if defined( USE_EDITDLG)
#include "i3TDKUpdateSystem.h"
#include "i3Level/i3LevelElement3D.h"
#endif

#include "GameCharaActionContext.h"
#include "GameCharaBoneContext.h"

struct LadderStateInfo
{
	CGameCharaBase* m_localClimber;
	MATRIX*			m_matTrigger;

	VEC3D			m_vAxisHalfLength;				// width,height,depth	

	VEC4D			m_planeLadderLeft;
	VEC4D			m_planeLadderRight;
	VEC4D			m_planeLadderFront_Ortho;
	VEC4D			m_planeLadderBack_Ortho;
	VEC4D			m_planeLadderTop_Ortho;
	VEC4D			m_planeLadderBottm_Ortho;		// 맨 밑바닥이라기보다는, 사다리 전면부의 밑바닥으로 보고 평면식을 구한다..
	VEC4D			m_planeLadderFront;
	
	VEC3D			m_vNegativeLadderAxisY;			// 여기서부터 작업재개할것 ..(2015.01.06.수빈)
	const VEC3D*	m_CurrAccDir;	
	bool			m_bJumping;
};

const VEC3D*	GetCurrLadderAccAxis(const LadderStateInfo* info)
{
	I3ASSERT(info != nullptr);
	return info->m_CurrAccDir;
}

const VEC3D*	GetCurrLadderRightAxis(const LadderStateInfo* info)
{
	I3ASSERT(info != nullptr);
	return reinterpret_cast<const VEC3D*>(&info->m_planeLadderRight);		
}



bool			IsJumpingOnLadder(const LadderStateInfo* info)
{
	return info != nullptr && info->m_bJumping;
}

void			ResetLadderJump(const LadderStateInfo* info)
{
	if (IsJumpingOnLadder(info) == false)
		return;

	const_cast<LadderStateInfo*>(info)->m_bJumping = false;
	
	if (info->m_localClimber)
		info->m_localClimber->removeEnableStateMask(CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
}

namespace
{
	const VEC3D		NEGATIVE_YAXIS = VEC3D(0.f, -1.f, 0.f);
	
	bool			FaceEachOther(LadderStateInfo* info)
	{
		const VEC3D* vAtTrigger = reinterpret_cast<const VEC3D*>(&info->m_planeLadderBack_Ortho);
		VEC3D vAtClimber;
		
		i3Vector::Cross(&vAtClimber, i3Matrix::GetRight(info->m_localClimber->getSceneObject()->GetCacheMatrix()), &I3_YAXIS);
		i3Vector::Normalize(&vAtClimber, &vAtClimber);
		
		REAL32 r = i3Vector::Dot(vAtTrigger, &vAtClimber);
// cos 60 == 1/2  // 0.5보다 작으면 사다리 두께쪽 진행 방향과 플레이어 방향간 각도가 60도보다 커지는 것임....   
		return r > 0.5f;
	}

	// 여기서는 아래로 30도 밑인 경우에 한해서 아래방향을 향한다고 처리한다...

	// 카메라나 씬오브젝트의 행렬은 3인칭 시점에서 맞지 않거나 Y성분을 반영하지 못하거나 이유로 쓰면 안된다...
	static const REAL32 lower_threshold_rad = I3_DEG2RAD(-30.f);

	bool			IsCharacterHeadedToLower(CGameCharaBase* chara)
	{
		REAL32 rho = chara->getBoneContext()->getRhoValue();				// getViewRho()
		
		return rho < lower_threshold_rad;

//		MATRIX* matCam = g_pCamera->getCamMatrix();
//		VEC3D* camAt = i3Matrix::GetAt(matCam);				// 놀랍게도 카메라의 at벡터는 진행 방향의 반대방향이다.. (왜 이렇게 되있지..)
//		return ( camAt->y > 0.5f);		// 일단 방향벡터가 윗쪽이면 무조건 false.. ( 원래는 camAt->y < -0.5f 이어야 햇으나, 카메라 At벡터가 반대방향이므로 부호를 바꾼다..)
	}									// 정규수직벡터와의 내적이 120도 둔각만 이루면 되기 때문에 cos 120 보다 작은값이면 아래로 향한다고 가정한다.

	const VEC3D*	GetTriggerAxis(LadderStateInfo* info, INT32 axis)
	{
		return reinterpret_cast<const VEC3D*>(&info->m_matTrigger->vec[ axis ] );			// vec[ info->m_idxAxis[axis]  ]
	}

	void			CalcTriggerDirOrtho(LadderStateInfo* info, VEC3D* outDirOrtho)
	{
		const VEC3D* pvDir = GetTriggerAxis(info, 2);
		i3Vector::Cross(outDirOrtho, pvDir, &I3_YAXIS);			// 수평 Right Axis를 구하고..
		i3Vector::Normalize(outDirOrtho, outDirOrtho);
		i3Vector::Cross(outDirOrtho, &I3_YAXIS, outDirOrtho);	// 그걸로 다시 DirOrtho를 구하면 됨..
	}

	const VEC3D*	GetTriggerCenter(LadderStateInfo* info)
	{
		return reinterpret_cast<const VEC3D*>(&info->m_matTrigger->vec[3]);
	}
	
	REAL32			GetTriggerHalfLength(LadderStateInfo* info, INT32 axis)
	{
		return info->m_vAxisHalfLength.vec[axis];
	}

	NxVec3 GetTriggerLength(CLadderTrigger* trigger)
	{
		NxShape* const * ashapeTrigger = trigger->getNxActor()->getShapes();
		I3ASSERT( ashapeTrigger != nullptr );
		I3ASSERT( ashapeTrigger[0]->isBox() );
		const NxBoxShape* shapeTrigger = static_cast<const NxBoxShape*>(ashapeTrigger[0]);
		return shapeTrigger->getDimensions();
	}

	void CalcLadderLength(CLadderTrigger* trigger, VEC3D* outHalfLength)
	{
		NxVec3 vDim = GetTriggerLength(trigger);
		
		outHalfLength->x = vDim.get()[0];
		outHalfLength->y = vDim.get()[1];
		outHalfLength->z = vDim.get()[2];
	}

	/*
	void CalcAxisIndexAndLength( CLadderTrigger* trigger, INT32 (&outAxis)[3], VEC3D* outHalfLength )
	{
		INT32 HeightAxis = 0;

		{
			const MATRIX* matTrigger = trigger->GetMatrix();
			REAL32 fMaxUpValue = -2.f;			// 상식적으로 나올수 없는 Min값으로 대충 설정..(-1이 계산 최소값임)

			for (INT32 i = 0 ; i < 3 ; ++i)
			{
				const VEC3D* vAxis = reinterpret_cast<const VEC3D*>(&matTrigger->vec[i]);
				REAL32 dotVal = i3Vector::Dot(vAxis, &I3_YAXIS);

				if (  dotVal > fMaxUpValue )
				{
					fMaxUpValue = dotVal;	HeightAxis = i;
				}
			}
		}

		NxVec3 vDim = GetTriggerLength(trigger);

		INT32 DepthAxis = 0;
		{
			REAL32 minVal = FLT_MAX;			// 상식적으로 나올수 없는 Max값으로 대충설정..
		
			for (INT32 i = 0 ; i < 3; ++i)
			{
				if (i == HeightAxis ) continue;
				REAL32 rVal = vDim.get()[i];	

				if ( minVal > rVal ) { minVal = rVal;	DepthAxis = i; }
			}
		}

		INT32 WidthAxis = 0;
		{
			for (INT32 i = 0 ; i < 3; ++i )
			{
				if ( i == HeightAxis ) continue;
				if ( i == DepthAxis ) continue;
				WidthAxis = i;
			}
		}

		outAxis[0] = WidthAxis;
		outAxis[1] = HeightAxis;
		outAxis[2] = DepthAxis;
		outHalfLength->x = vDim.get()[WidthAxis];
		outHalfLength->y = vDim.get()[HeightAxis];
		outHalfLength->z = vDim.get()[DepthAxis];
	}
	*/

	void    CalcClimberPos(LadderStateInfo* info, VEC3D* out)
	{
		info->m_localClimber->getMoveContext()->GetPosition(out);
	}
	
	void	CalcClimberRadius(LadderStateInfo* info, REAL32& outRadius)
	{
		NxCapsuleController* nxCtrler = (NxCapsuleController*)info->m_localClimber->getMoveContext()->getPhysixController()->getNxController();
		outRadius = nxCtrler->getRadius();
	}

	REAL32 CalcDist_LeftPlane(LadderStateInfo* info, VEC3D* playerPos)
	{
		return i3Plane::Dot(&info->m_planeLadderLeft, playerPos);
	}

	REAL32 CalcDist_RightPlane(LadderStateInfo* info, VEC3D* playerPos)
	{
		return i3Plane::Dot(&info->m_planeLadderRight, playerPos);
	}


	REAL32 CalcDist_FrontPlane(LadderStateInfo* info, VEC3D* playerPos)
	{
		return i3Plane::Dot(&info->m_planeLadderFront, playerPos);
	}

	REAL32 CalcDist_FrontOrthoPlane(LadderStateInfo* info, VEC3D* playerPos)
	{
		return i3Plane::Dot(&info->m_planeLadderFront_Ortho, playerPos);
	}

	REAL32	CalcDist_BackOrthoPlane( LadderStateInfo* info, VEC3D* playerPos)
	{
		return i3Plane::Dot(&info->m_planeLadderBack_Ortho, playerPos);
	}
	
	REAL32 CalcDist_TopOrthoPlane(LadderStateInfo* info, VEC3D* playerPos)
	{
		return i3Plane::Dot(&info->m_planeLadderTop_Ortho, playerPos);
	}

	REAL32 CalcDist_BottomOrthoPlane(LadderStateInfo* info, VEC3D* playerPos)
	{
		return i3Plane::Dot(&info->m_planeLadderBottm_Ortho, playerPos);
	}
	
	LadderStateInfo*	CreateLadderInfo(CLadderTrigger* trigger)
	{
		LadderStateInfo* new_info = new LadderStateInfo;
		new_info->m_localClimber = nullptr;
		new_info->m_matTrigger = trigger->GetMatrix();

	//	CalcAxisIndexAndLength(trigger, new_info->m_idxAxis, &new_info->m_vAxisHalfLength);
		CalcLadderLength(trigger, &new_info->m_vAxisHalfLength);

		const VEC3D* vTriggerRight = GetTriggerAxis(new_info, 0);
		const VEC3D* vTriggerUp = GetTriggerAxis(new_info, 1);
		const VEC3D* vTriggerDir   = GetTriggerAxis(new_info, 2);

		new_info->m_vNegativeLadderAxisY = *vTriggerUp;
		new_info->m_vNegativeLadderAxisY *= -1.f;

//		if (new_info->m_idxAxis[2] == 0)		// 두께에 해당되는 것이 X축이라면 왼쪽 오른쪽은 뒤바뀌게 된다..
//			vTriggerRight *= -1.f;
		
		const VEC3D* vTriggerPos = GetTriggerCenter(new_info);

		{
			VEC3D vTriggerWidth = *vTriggerRight;
			vTriggerWidth *= GetTriggerHalfLength(new_info, 0);
			

			{	// 좌평면..
				new_info->m_planeLadderLeft.x = -vTriggerRight->x;
				new_info->m_planeLadderLeft.y = -vTriggerRight->y;
				new_info->m_planeLadderLeft.z = -vTriggerRight->z;

				VEC3D vTriggerLeftPos = *vTriggerPos;	vTriggerLeftPos -= vTriggerWidth;
				new_info->m_planeLadderLeft.w = 
					-i3Vector::Dot( reinterpret_cast<VEC3D*>( &new_info->m_planeLadderLeft), &vTriggerLeftPos);
			}

			{	// 우평면
				new_info->m_planeLadderRight.x = vTriggerRight->x;
				new_info->m_planeLadderRight.y = vTriggerRight->y;
				new_info->m_planeLadderRight.z = vTriggerRight->z;

				VEC3D vTriggerRightPos = *vTriggerPos;	vTriggerRightPos += vTriggerWidth;
				new_info->m_planeLadderRight.w = 
					-i3Vector::Dot( reinterpret_cast<VEC3D*>( &new_info->m_planeLadderRight), &vTriggerRightPos);
			}
		}


		{	
			VEC3D	vDirOrtho;
			CalcTriggerDirOrtho(new_info, &vDirOrtho);

			VEC3D vTriggerHeight = *vTriggerUp;
			vTriggerHeight *= GetTriggerHalfLength(new_info, 1);
			VEC3D vTriggerDepth  = *vTriggerDir;
			vTriggerDepth *= GetTriggerHalfLength(new_info, 2);

			{	// 사다리 정면 직교 평면..
				new_info->m_planeLadderFront_Ortho.x = -vDirOrtho.x;
				new_info->m_planeLadderFront_Ortho.y = -vDirOrtho.y;
				new_info->m_planeLadderFront_Ortho.z = -vDirOrtho.z;

				VEC3D vTriggerFrontPos = *vTriggerPos;
				vTriggerFrontPos -= vTriggerHeight;		vTriggerFrontPos -= vTriggerDepth;

				new_info->m_planeLadderFront_Ortho.w =
					-i3Vector::Dot(reinterpret_cast<VEC3D*>( &new_info->m_planeLadderFront_Ortho), &vTriggerFrontPos);
			}


			{	// 사다리 후면 직교 평면..
				new_info->m_planeLadderBack_Ortho.x = vDirOrtho.x;
				new_info->m_planeLadderBack_Ortho.y = vDirOrtho.y;
				new_info->m_planeLadderBack_Ortho.z = vDirOrtho.z;

				VEC3D vTriggerBackPos = *vTriggerPos;
				vTriggerBackPos += vTriggerHeight;		vTriggerBackPos += vTriggerDepth;

				new_info->m_planeLadderBack_Ortho.w =
					-i3Vector::Dot(reinterpret_cast<VEC3D*>( &new_info->m_planeLadderBack_Ortho), &vTriggerBackPos);
			}

			{	// 사다리 상단 직교 평면..
				new_info->m_planeLadderTop_Ortho.x = 0.f;
				new_info->m_planeLadderTop_Ortho.y = 1.f;
				new_info->m_planeLadderTop_Ortho.z = 0.f;

				VEC3D vTriggerTopPos = *vTriggerPos;
				vTriggerTopPos += vTriggerHeight;		vTriggerTopPos -= vTriggerDepth;

				new_info->m_planeLadderTop_Ortho.w = 
					-i3Vector::Dot(reinterpret_cast<VEC3D*>( &new_info->m_planeLadderTop_Ortho), &vTriggerTopPos);
			}
			{
				// 사다리 하단 직교 평면..
				new_info->m_planeLadderBottm_Ortho.x = 0.f;
				new_info->m_planeLadderBottm_Ortho.y = -1.f;
				new_info->m_planeLadderBottm_Ortho.z = 0.f;
				
				VEC3D vTriggerBottomPos = *vTriggerPos;
				vTriggerBottomPos -= vTriggerHeight;	vTriggerBottomPos -= vTriggerDepth;
				new_info->m_planeLadderBottm_Ortho.w = 
					-i3Vector::Dot(reinterpret_cast<VEC3D*>( &new_info->m_planeLadderBottm_Ortho), &vTriggerBottomPos);
			}
		//	const VEC3D* pvDir = GetTriggerAxis(new_info, 2);

			{	// 사다리 앞 평면..
				new_info->m_planeLadderFront.x = -vTriggerDir->x;
				new_info->m_planeLadderFront.y = -vTriggerDir->y;
				new_info->m_planeLadderFront.z = -vTriggerDir->z;

				VEC3D vTriggerFrontPos = *vTriggerPos;
				vTriggerFrontPos -= vTriggerDepth;	

				new_info->m_planeLadderFront.w =
					-i3Vector::Dot(reinterpret_cast<VEC3D*>( &new_info->m_planeLadderFront), &vTriggerFrontPos);
			}

		}

		new_info->m_CurrAccDir = GetTriggerAxis(new_info, 1);		// 높이축이 디폴트 진행방향..
		new_info->m_bJumping = false;

		return new_info;
	}


	void	ResetClimberFromLadderInfo(LadderStateInfo* info)
	{
		CGameCharaBase* climber = info->m_localClimber;
		climber->removeCharaStateMask(CHARA_STATE_CLIMB);
		climber->removeEnableStateMask(CHARA_STATE_DISABLE_GRAVITY);
		climber->getMoveContext()->setLadderStateInfo(nullptr);
		info->m_localClimber = nullptr;
		info->m_bJumping = false;
	}

	void	SetClimberIntoLadderInfo(LadderStateInfo* info, CGameCharaBase* climber)
	{
		info->m_localClimber = climber;

		const LadderStateInfo* old_info =	climber->getMoveContext()->getLadderStateInfo();
		
		if (old_info != nullptr )
		{
			I3ASSERT(old_info != info);
			ResetClimberFromLadderInfo( const_cast<LadderStateInfo*>(old_info) );
		}

		climber->getMoveContext()->setLadderStateInfo(info);
	}
			
	bool	IsClimberExistedFromLadderInfo(LadderStateInfo* info )
	{
		return info->m_localClimber != nullptr;
	}

}


I3_CLASS_INSTANCE( CLadderTrigger);

CLadderTrigger::CLadderTrigger() : m_LadderInfo(nullptr) 
{

}

CLadderTrigger::~CLadderTrigger()
{
	delete m_LadderInfo;
}


void	CLadderTrigger::OnInitTrigger()
{
	m_LadderInfo = CreateLadderInfo(this);
}

void CLadderTrigger::OnEnter(i3PhysixShapeSet* pTriggerShapeSet, i3ElementBase* pDest)
{
	if ( IsClimberExistedFromLadderInfo(m_LadderInfo) )		// info가 존재하면 트리거진입을 무시한다.
	{
		if (!m_LadderInfo->m_bJumping)
			return;

		CGameCharaBase * climber = m_LadderInfo->m_localClimber;

		// 점프 혹은 낙하 중이라면 별도처리를 한다..
		//	if (climber->isCharaStateMask(CHARA_STATE_JUMP))			// 점프하며 사다리에 올라탄 경우 버그로 자동이동되는걸 막기 위해서는, 점프를 강제로 멈추고 관련 변수를 리셋해주어야한다...
		if (climber->getActionContext()->isLower( CHARA_LOWER_JUMP) )
		{															// 현재 이 코드블록의 위치가 썩 좋지는 않은데, 가급적이면 LadderClimbStates.cpp 안에 가둬놓고 쓰는게 좋겠다....(2014.12.03.수빈)
			climber->Cmd_JumpEnd();
			//		climber->removeEnableStateMask( CHARA_STATE_DISABLE_MOVE);
			climber->removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

			CGameCharaMoveContext* move_context = climber->getMoveContext();
			i3Vector::Zero( move_context->getMoveDir() );
			move_context->ResetJumping();
		}

		m_LadderInfo->m_bJumping = false;
		return;
	}

	if( !i3::kind_of<i3PhysixShapeSet*, CGameCharaBase*>(pDest) )
		return;
	
	CGameCharaBase * climber = nullptr;

	if( i3::kind_of<i3PhysixShapeSet*>(pDest) )
	{
		i3PhysixShapeSet * pShapeSet = 	static_cast<i3PhysixShapeSet*>( pDest);
		climber = i3::kind_cast<CGameCharaBase*>( pShapeSet->getEventReceiver() );
	}
	else
	{
		climber = static_cast<CGameCharaBase*>( pDest );
	}
	
	if (!climber) return;
	if (climber->IsDino()) return;
	if (!climber->isLocal()) return;

	SetClimberIntoLadderInfo(m_LadderInfo, climber);

	if (climber->getActionContext()->isLower( CHARA_LOWER_JUMP) )
	{															// 현재 이 코드블록의 위치가 썩 좋지는 않은데, 가급적이면 LadderClimbStates.cpp 안에 가둬놓고 쓰는게 좋겠다....(2014.12.03.수빈)
		climber->Cmd_JumpEnd();
//		climber->removeEnableStateMask( CHARA_STATE_DISABLE_MOVE);
		climber->removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

		CGameCharaMoveContext* move_context = climber->getMoveContext();
		i3Vector::Zero( move_context->getMoveDir() );
		move_context->ResetJumping();
	}

	CLadderTrigger::UpdateTriggerImp();
}

void	CLadderTrigger::UpdateTriggerImp()
{
	if (IsClimberExistedFromLadderInfo(m_LadderInfo) == false )
		return;
	
	CGameCharaBase* climber = m_LadderInfo->m_localClimber;

	if ( m_LadderInfo->m_bJumping )		// 점프 진행중...
	{
		// CLIMB 상태는 셋팅하지 않고, 이전 상태와 동일하게 맞춤..
		climber->removeEnableStateMask(CHARA_STATE_DISABLE_GRAVITY);
		return;
	}

	if ( FaceEachOther(m_LadderInfo) == false)
	{
		climber->removeCharaStateMask(CHARA_STATE_CLIMB);
		climber->removeEnableStateMask(CHARA_STATE_DISABLE_GRAVITY);
		return;
	}
	
	UINT64 keyStroke = climber->getInputContext()->getKeyStrokeInput();
	if( keyStroke & GAME_KEY_ACTION_JUMP )
	{
		m_LadderInfo->m_bJumping = true;
		// CLIMB 상태는 셋팅하지 않고, 이전 상태와 동일한 상태로 맞춤.
		climber->removeEnableStateMask(CHARA_STATE_DISABLE_GRAVITY);
		climber->Cmd_JumpStart();
		return;
	}

	CGameCharaMoveContext* move_context = climber->getMoveContext();

	VEC3D* moveDir   = move_context->getMoveDir();	
	
	bool   isCharacterHeadedLower = IsCharacterHeadedToLower(climber);

//	VEC3D* moveAt	 = move_context->getMoveAt();
//	VEC3D* moveRight = move_context->getMoveRight();
//	VEC3D  inputVelAt = *moveAt;	inputVelAt *= moveDir->z;
//	VEC3D  inputVelRight = *moveRight;	inputVelRight *= moveDir->x;
//	VEC3D  inputVel = inputVelAt;		inputVel += inputVelRight;
		
	bool   bFloor    = climber->isCharaStateMask(CHARA_STATE_FLOOR);
	VEC3D playerPos;		CalcClimberPos(m_LadderInfo, &playerPos);
//	VEC3D playerPos = *i3Matrix::GetPos(climber->getSceneObject()->GetCacheMatrix());

	REAL32 playerRadius;	CalcClimberRadius(m_LadderInfo, playerRadius);
	playerPos.y += playerRadius;		// 미리 반지름만큼 더해서, 캡슐밑바닥이 아닌 아랫 스피어중심이 되도록 조정한다..

	// 좌우 평면 검사 벗어나면 그냥 out시킴..

	REAL32 dist_Left = CalcDist_LeftPlane(m_LadderInfo, &playerPos);

	if (dist_Left > playerRadius * 2.f)		// 측면 바깥이면 무조건 벗어난 것으로 간주..
	{
		ResetClimberFromLadderInfo(m_LadderInfo);
		return;
	}
	else
	if (dist_Left > 0.f)  // playerRadius
	{
		climber->removeCharaStateMask(CHARA_STATE_CLIMB);
		climber->removeEnableStateMask(CHARA_STATE_DISABLE_GRAVITY);
		return;
	}
	
	REAL32 dist_right = CalcDist_RightPlane(m_LadderInfo, &playerPos);

	if (dist_right > playerRadius * 2.f)
	{
		ResetClimberFromLadderInfo(m_LadderInfo);
		return;
	}
	else
	if (dist_right > 0.f)		//playerRadius 측면 바깥이면 무조건 벗어난 것으로 간주..
	{

		climber->removeCharaStateMask(CHARA_STATE_CLIMB);
		climber->removeEnableStateMask(CHARA_STATE_DISABLE_GRAVITY);

		return;
	}

	REAL32 dist_FrontOrtho = CalcDist_FrontOrthoPlane(m_LadderInfo, &playerPos);
	
	if (dist_FrontOrtho > playerRadius)
	{
		if (moveDir->z < 0.f )		// 사다리 아래에서 후진한 경우 트리거 종료
		{
			ResetClimberFromLadderInfo(m_LadderInfo);
			return;
		}

		climber->removeCharaStateMask(CHARA_STATE_CLIMB);
		climber->removeEnableStateMask(CHARA_STATE_DISABLE_GRAVITY);
		// 그렇지 않으면 오르지 않은 상태에서 트리거는 종료하지 않음..
		return;
	}
	

	climber->addEnableStateMask(CHARA_STATE_DISABLE_GRAVITY);

	REAL32 dist_BackOrtho	= CalcDist_BackOrthoPlane( m_LadderInfo, &playerPos);

	if ( dist_BackOrtho <= playerRadius )	// 사다리 상단 뒷면 직전 단계.... // CLIMB or Not // But Gravity Disabled..
	{
	
		REAL32 dist_TopOrtho = CalcDist_TopOrthoPlane(m_LadderInfo, &playerPos);

		if ( dist_TopOrtho >= playerRadius)		// 꼭대기까지 올라간 경우...
		{
			if (moveDir->z < 0.f)
			{
				REAL32 dist_Front = CalcDist_FrontPlane(m_LadderInfo, &playerPos);

				if ( dist_Front > 0.f )
				{
					// 윗쪽 사다리 맨 윗 꼭지점 근처에 다다르면, 정상적인 사다리 방향으로 변경..
					climber->addCharaStateMask(CHARA_STATE_CLIMB);

					// 꼭대기에서 내려가는 방향일때, 아래를 향하고 있다면 그냥 멈춰버리기로 기획과 협의...
					if ( !isCharacterHeadedLower )
						m_LadderInfo->m_CurrAccDir = GetTriggerAxis(m_LadderInfo, 1);	
					else
					{
						m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
						move_context->setVelocity(0.f,0.f,0.f);
					}

					return;
				}

			}
			else
			if (moveDir->z > 0.f)
			{
				if ( isCharacterHeadedLower )
				{
					REAL32 dist_Front = CalcDist_FrontPlane(m_LadderInfo, &playerPos);

					if ( dist_Front > 0.f )		// 꼭대기에서 전진중인데, 고개를 파묻고 있다면 대놓고 내려가도록 조정한다..
					{
						// 윗쪽 사다리 맨 윗 꼭지점 근처에 다다르면, 정상적인 사다리 방향으로 변경..
						climber->addCharaStateMask(CHARA_STATE_CLIMB);
						m_LadderInfo->m_CurrAccDir = &m_LadderInfo->m_vNegativeLadderAxisY;
						return;
					}
					else
					{
						climber->removeCharaStateMask(CHARA_STATE_CLIMB);
						move_context->setVelocity(0.f,0.f,0.f);
						return;
					}
				}
			}

		//	if (isCharacterHeadedLower)				// 정지중이라도 고개를 내리고있으면 못 움직이게 막아주지 않으면 조금씩조금씩 움직일수 있다...
		//	{
		//		climber->addCharaStateMask(CHARA_STATE_CLIMB);
		//		m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
		//		move_context->setVelocity(0.f,0.f,0.f);
		//		return;
		//	}

			climber->removeCharaStateMask(CHARA_STATE_CLIMB);
			move_context->setVelocity(0.f,0.f,0.f);
			return;
		}
	
		if ( dist_BackOrtho > -playerRadius )
		{
			
			if ( moveDir->z < 0.f  )		// 후진 중..
			{
				//
				if (dist_TopOrtho < playerRadius)		// 이 경우 뒷부분 사다리 모서리에 부딪친다..진행방향을 바꾼다...I3_YAXIS로 바꾼다....
				{
					climber->addCharaStateMask(CHARA_STATE_CLIMB);

					// 꼭대기에서 내려가는 방향일때, 아래를 향하고 있다면 그냥 멈춰버리기로 기획과 협의...
//					if ( !isCharacterHeadedLower )
						m_LadderInfo->m_CurrAccDir = &NEGATIVE_YAXIS;
//					else
//					{
//						m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
//						move_context->setVelocity(0.f,0.f,0.f);
//					}
					return;
				}
				else
				{
//					if ( !isCharacterHeadedLower )
//					{
						climber->removeCharaStateMask(CHARA_STATE_CLIMB);
						move_context->setVelocity(0.f,0.f,0.f);
//					}
//					else		// 고개를 내렸을때는 역시 못움직이게 막는다...
//					{
//						climber->addCharaStateMask(CHARA_STATE_CLIMB);
//						m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
//						move_context->setVelocity(0.f,0.f,0.f);
//					}
					return;
				}
			}
			else
			{

				if ( isCharacterHeadedLower && moveDir->z == 0.f )	// 고개를 내렸고, 멈춘 상태라면 강제로 ZEROACC를 만들고 멈추게 처리..
				{
					climber->addCharaStateMask(CHARA_STATE_CLIMB);
					m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
					move_context->setVelocity(0.f,0.f,0.f);
				}
				else
				{
					climber->removeCharaStateMask(CHARA_STATE_CLIMB);
					move_context->setVelocity(0.f,0.f,0.f);
				}
			}
			return;
		}
		
		if (bFloor)		// 아랫쪽 지면에 닿은 경우
		{
			// 전진시 고개를 숙이면 정지시켜야한다..
			if ( moveDir->z > 0.f && isCharacterHeadedLower)
			{
				climber->addCharaStateMask(CHARA_STATE_CLIMB);
				m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
				move_context->setVelocity(0.f,0.f,0.f);
				return;
			}			// 반면, 지면에 닿은 상태일때는 후진할때는 고개의 높낮이는 따지지 않는다....그냥 후진...
			else	
			if ( moveDir->z < 0.f)			// 후진중이라면 사다리를 오르지 않는 것으로 간주한다.
			{
				if (!isCharacterHeadedLower)
				{
					climber->removeCharaStateMask(CHARA_STATE_CLIMB);
					move_context->setVelocity(0.f,0.f,0.f);
				}
				else
				{
					climber->addCharaStateMask(CHARA_STATE_CLIMB);
					m_LadderInfo->m_CurrAccDir = &m_LadderInfo->m_vNegativeLadderAxisY;
				}

				return;
			}
			else
			if (moveDir->z == 0.f)		// 정지중일땐 아예 못움직이게 차단...
			{
				climber->addCharaStateMask(CHARA_STATE_CLIMB);
				m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
				move_context->setVelocity(0.f,0.f,0.f);
			}
		}
		
		REAL32 bottom_dist = CalcDist_BottomOrthoPlane(m_LadderInfo, &playerPos);

		if ( bottom_dist >= -playerRadius)			// 밑바닥 평면노멀은 음의 방향으로 잡아놓았다..
		{
			// 전진시 고개를 숙이면 정지시켜야한다..
			if ( moveDir->z > 0.f && isCharacterHeadedLower)
			{
				climber->addCharaStateMask(CHARA_STATE_CLIMB);
				m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
				move_context->setVelocity(0.f,0.f,0.f);
				return;
			}			// 반면, 지면에 닿은 상태일때는 후진할때는 고개의 높낮이는 따지지 않는다....그냥 후진...
			else	
			if ( moveDir->z < 0.f)			// 후진중이라면 사다리를 오르지 않는 것으로 간주한다.
			{
				if (!isCharacterHeadedLower)
				{
					climber->removeCharaStateMask(CHARA_STATE_CLIMB);
					move_context->setVelocity(0.f,0.f,0.f);
				}
				else
				{
					climber->addCharaStateMask(CHARA_STATE_CLIMB);
					m_LadderInfo->m_CurrAccDir = &m_LadderInfo->m_vNegativeLadderAxisY;
				}
				return;
			}
			else
			if (moveDir->z == 0.f)		// 정지중일땐 아예 못움직이게 차단...
			{
				climber->addCharaStateMask(CHARA_STATE_CLIMB);
				m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
				move_context->setVelocity(0.f,0.f,0.f);
			}
		}


		/*
		if ( dist_FrontOrtho > playerRadius * .5f )		//  트리거 앞면에 적당이 중간정도 걸친 경우엔 아직 올리지는 않는다..
		{
				// 전진시 고개를 숙이면 정지시켜야한다..
			if (moveDir->z > 0.f && isCharacterHeadedLower)
			{
				climber->addCharaStateMask(CHARA_STATE_CLIMB);
				m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
				move_context->setVelocity(0.f,0.f,0.f);
				return;
			}
			else	// 후진시 고개를 숙였다면 올려야한다..??
			if ( moveDir->z < 0.f && isCharacterHeadedLower)
			{
				climber->addCharaStateMask(CHARA_STATE_CLIMB);
				m_LadderInfo->m_CurrAccDir = &m_LadderInfo->m_vNegativeLadderAxisY;
			}
			else
			if (moveDir->z == 0.f)		// 정지중일땐 아예 못움직이게 차단...
			{
				climber->addCharaStateMask(CHARA_STATE_CLIMB);
				m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
				move_context->setVelocity(0.f,0.f,0.f);
			}
			else
			{
				climber->removeCharaStateMask(CHARA_STATE_CLIMB);
				move_context->setVelocity(0.f,0.f,0.f);
			}
		}
		else		// 그 외의 경우는 벽에서 사다리를 오르거나 내리는 상태로 간주한다..	// 벽에서 오르거나 내릴때는 고개를 숙인 여부에 따라 방향이 결정된다..
		*/
		{
			climber->addCharaStateMask(CHARA_STATE_CLIMB);
			if (!isCharacterHeadedLower)
				m_LadderInfo->m_CurrAccDir = GetTriggerAxis(m_LadderInfo, 1);		// 높이축이 디폴트 진행방향..
			else
				m_LadderInfo->m_CurrAccDir = &m_LadderInfo->m_vNegativeLadderAxisY;
		}
/*
		if ( (bFloor && moveDir->z < 0.f) || dist_FrontOrtho > playerRadius * .5f )
		{
			climber->removeCharaStateMask(CHARA_STATE_CLIMB);
			move_context->setVelocity(0.f,0.f,0.f);
		}
		else
		{
			climber->addCharaStateMask(CHARA_STATE_CLIMB);
			m_LadderInfo->m_CurrAccDir = GetTriggerAxis(m_LadderInfo, 1);		// 높이축이 디폴트 진행방향..
		}
*/
		return;
	}
	
	// 사다리 후면을 벗어난 상태..

	if ( moveDir->z > 0.f )		// 사다리 위에서 벗어나는 방향에 있다면 트리거 종료로 간주한다.
	{
		ResetClimberFromLadderInfo(m_LadderInfo);
		return;
	}
		
}


void	CLadderTrigger::OnUpdate( REAL32 rDeltaSeconds)
{
	i3Trigger::OnUpdate(rDeltaSeconds);

	CLadderTrigger::UpdateTriggerImp();
}
