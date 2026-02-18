#include "pch.h"

#include "SentrygunBehavior.h"

namespace
{
	inline REAL32 CellDegree(REAL32 rad)
	{
		if (rad >= 0.0f)
		{
			if (I3_RAD2DEG(rad) >= 360.0f)
				return I3_RAD2DEG(rad) - 360.0f;

			return I3_RAD2DEG(rad);
		}

		if (rad < 0.0f)
		{
			if ((360.0f+I3_RAD2DEG(rad)) < 0.0f)
				return 360.0f+(360.0f+I3_RAD2DEG(rad));

			return 360.0f+I3_RAD2DEG(rad);
		}

		return 0.0f;
	}

	// 총기를 지속적으로 좌우/상하 회전
	inline void RotateFireArmSideToSide(WeaponSentryGun& sg)
	{
		i3SceneObject* sceneObj = sg.getSceneObject();
		sceneObj->AddFlag(I3_NODEFLAG_MANUAL_UPDATE);

		i3Body* body = sceneObj->GetBody();
		body->addStyle(I3BS_MANUAL_ANIM_UPDATE | I3BS_MANUAL_TRANSFORM);
		body->UpdateAnim(g_pFramework->GetViewer()->GetSceneTracer());

		// 총기 상체 좌우 회전(-45.0f ~ 45.0f)
		INT32 thetaBone = body->FindBoneByName("GunRotateDummy");
		MATRIX* matThetaLocal = body->getBone(thetaBone)->getMatrix();
		MATRIX* matThetaWorld = body->getCacheMatrix(thetaBone);

		FireArmAngle* faa = sg.GetFireArmAngle();

		REAL32 angle = CellDegree(faa->theta);
		if ((0.0f <= angle && angle <= 45.0f) || (315.0f <= angle && angle <= 360.0f))
		{
			if (44.0f <= angle && angle <= 45.0f)
				faa->thetaIncr = -0.005f;
			else if (315.0f <= angle && angle <= 316.0f)
				faa->thetaIncr = 0.005f;

			faa->theta += faa->thetaIncr;
		}
		else
			faa->theta += i3Math::abs(faa->thetaIncr);

		i3Matrix::SetRotateZ(matThetaLocal, faa->theta);
		i3Matrix::Mul(matThetaWorld, matThetaLocal, matThetaWorld);

		// 총기 상체 상하 회전
		INT32 rhoBone = body->FindBoneByName("GunAniDummy");
		MATRIX* matRhoLocal	= body->getBone(rhoBone)->getMatrix();
		MATRIX* matRhoWorld = body->getCacheMatrix(rhoBone);
		
		if (faa->rho < 0.0f)
			faa->rho += faa->rhoIncr;
		else if (faa->rho > 0.0f)
			faa->rho -= faa->rhoIncr;

		i3Matrix::SetRotateX(matRhoLocal, faa->rho);
		i3Matrix::Mul(matRhoWorld , matRhoLocal, matRhoWorld);

		sceneObj->OnUpdate(g_pFramework->GetViewer()->GetSceneTracer());
	}

	// 공격 회전
	inline void RotateFireArmForAttack(WeaponSentryGun& sg)
	{
		i3SceneObject* sgSceneObj = sg.getSceneObject();
		sgSceneObj->AddFlag(I3_NODEFLAG_MANUAL_UPDATE);

		i3Body* body = sgSceneObj->GetBody();
		body->addStyle(I3BS_MANUAL_ANIM_UPDATE | I3BS_MANUAL_TRANSFORM);
		body->UpdateAnim(g_pFramework->GetViewer()->GetSceneTracer());

		FireArmAngle* faa = sg.GetFireArmAngle();
		VEC3D* targetDir = sg.CollideeLine()->GetDir();

		// 상체 좌우 회전
		INT32 thetaBone = body->FindBoneByName("GunRotateDummy");
		MATRIX* matLocalTheta = body->getBone(thetaBone)->getMatrix();
		MATRIX* matWorldTheta = body->getCacheMatrix(thetaBone);

		REAL32 targetTheta	= i3Math::atan2(getX(targetDir), getZ(targetDir));
		REAL32 diff = abs(faa->recv_theta - targetTheta);
		if( diff >= 0.785f) //45도
		{
			faa->ReceiveTheta( targetTheta);
			FireArmAngle::LERP_TIME = 0.1f;
		}

		REAL32 cur_theta = faa->interpolate_theta();
		i3Matrix::SetRotateZ(matLocalTheta, cur_theta);
		i3Matrix::Mul(matWorldTheta, matLocalTheta, matWorldTheta);
		
			
		// 상체 상하 회전
		//INT32 rhoBone = body->FindBoneByName("GunAniDummy");
		//MATRIX* matLocalRho = body->getBone(thetaBone)->getMatrix();
		//MATRIX* matWorldRho = body->getCacheMatrix(rhoBone);

		//REAL32 length = i3Math::Sqrt((getX(targetDir)*getX(targetDir))+(getZ(targetDir)*getZ(targetDir)));
		//REAL32 rho	  = i3Math::atan2(getY(targetDir), length);

		//i3Matrix::SetRotateX(matLocalRho, rho);
		//i3Matrix::Mul(matWorldRho, matLocalRho, matWorldRho);
		//faa->rho = rho;

		sgSceneObj->OnUpdate(g_pFramework->GetViewer()->GetSceneTracer());
	}

	// 조준 : 서버에서 주는 theta값 적용
	inline void TakeAim(WeaponSentryGun& sg)
	{
		i3SceneObject* sceneObj = sg.getSceneObject();
		sceneObj->AddFlag(I3_NODEFLAG_MANUAL_UPDATE);

		i3Body* body = sceneObj->GetBody();
		body->addStyle(I3BS_MANUAL_ANIM_UPDATE | I3BS_MANUAL_TRANSFORM);
		body->UpdateAnim(g_pFramework->GetViewer()->GetSceneTracer());

		FireArmAngle* faa = sg.GetFireArmAngle();
		REAL32 cur_theta = faa->interpolate_theta();

		//I3TRACE(">> sentry gun(%d) theta(%.3f)\n", sg.getNetworkIdx(), cur_theta);

		// theta 적용
		INT32 thetaBone = body->FindBoneByName("GunRotateDummy");
		MATRIX* matThetaLocal = body->getBone(thetaBone)->getMatrix();
		MATRIX* matThetaWorld = body->getCacheMatrix(thetaBone);

		i3Matrix::SetRotateZ(matThetaLocal, cur_theta);
		i3Matrix::Mul(matThetaWorld, matThetaLocal, matThetaWorld);

		sceneObj->OnUpdate(g_pFramework->GetViewer()->GetSceneTracer());

		//debug...
		//VEC3D* sgDir = i3Matrix::GetAt(sg.GetMatrix());
		//VEC3D* temp_dir = i3Matrix::GetAt(matThetaWorld);

		//REAL32 sgTheta		= i3Math::atan2(getX(sgDir), getZ(sgDir));
		//REAL32 temp_sgTheta		= i3Math::atan2(getX(temp_dir), getZ(temp_dir));
		//I3TRACE(">> sentry gun(%d) dir theta(%.3f)    temp_dir theta(%.3f)\n", sg.getNetworkIdx(), sgTheta, temp_sgTheta);
	}

	

	// 설치
	inline void SetupOnWorld(WeaponSentryGun& sg)
	{
		i3SceneObject* sceneObj = sg.getSceneObject();
		sceneObj->AddFlag(I3_NODEFLAG_MANUAL_UPDATE);

		i3Body* body = sceneObj->GetBody();
		body->addStyle(I3BS_MANUAL_ANIM_UPDATE | I3BS_MANUAL_TRANSFORM);
		body->UpdateAnim(g_pFramework->GetViewer()->GetSceneTracer());

		sceneObj->OnUpdate(g_pFramework->GetViewer()->GetSceneTracer());

		FireArmAngle* faa = sg.GetFireArmAngle();
		faa->Reset();
	}

	// 파괴 이펙트
	inline void DestroyEffect(WeaponSentryGun& sg)
	{
		INT32 effectIdx = g_pEffectManager->FindEffect("Exp_C5_01");
		g_pEffectManager->AddEffect(effectIdx, i3Matrix::GetPos(sg.getSceneObject()->GetCacheMatrix()), &I3_YAXIS);
		g_pSndMng->StartWeapon_C4StateSnd(GTFW_C4_EXPLOSION, sg.GetPos(), false);
	}




	struct None : Behavior
	{
		void operator()(WeaponSentryGun& sg) {}
	}g_none;

	struct UnderSurveillance : Behavior
	{
		void operator()(WeaponSentryGun& sg)
		{
			TakeAim(sg); // 적 조준(서버 theta 적용)
		}
	}g_underSurveillance;

	struct Attack : Behavior
	{
		void operator()(WeaponSentryGun& sg)
		{
			RotateFireArmForAttack(sg);
		}
	}g_attack;

	struct Setup : Behavior
	{
		void operator()(WeaponSentryGun& sg)
		{
			SetupOnWorld(sg);
		}
	}g_setup;

	struct Destroy : Behavior
	{
		void operator()(WeaponSentryGun& sg)
		{
			DestroyEffect(sg);

			sg.Reset();
		}
	}g_destroy;

	Behavior* g_behaviors[SG_BEHAVIOR_MAX] = 
		{&g_attack, &g_destroy, &g_none, &g_setup, &g_underSurveillance};
}

Behavior* GetSentrygunBehavior(INT32 idx)
{
#ifdef DEBUG
	I3ASSERT(0 <= idx && idx < SG_BEHAVIOR_MAX);
#endif

	return g_behaviors[idx];
}

