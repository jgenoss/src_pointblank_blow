#include "pch.h"
#include "TestAutomationInterface.h"

#if defined( TEST_AUTOMATION)

#include "UI/UIDefine.h"
#include "UI/UIMainFrame.h"

#include "UI/UIShopInfoMgr.h"

#include "IGShopContext.h"
#include "ShopContext.h"
#include "BattleSlotContext.h"
#include "GameCharaBoneContext.h"
#include "StageBattle/C4Context.h"

#include "i3Base/string/ext/contain_string_head.h"

static VEC3D 	ParseVEC3D( wstring & str)
{
	i3TLineLexer<wchar_t> lexer;
	wchar_t token[256], sep;
	VEC3D vec;
	INT32 idx = 0;

	lexer.SetMasterString(str.c_str());

	while ( (lexer.GetNextToken(token, &sep, L", \t\n\r") != -1) && (idx < 3))
	{
		vec.vec[idx] = i3::atof(token);
		idx++;
	}

	return vec;
}

VEC3D TestAutomationInterface::GetBombInstallPos(BOMB_AREA area)
{
	const BombInstallAreaProp* pProp = C4Context::i()->FindAreaProp( area);

	return pProp->pos;
}

TESTRESULT	TestAutomationInterface::OnMoveTo( float tmLocal, LOCTYPE type, wstring & value)
{
	if (g_pFramework->IsBattleStage() == false)
	{
		if( tmLocal > 15.0f)
			return TESTRESULT_FAIL;
	}
	else
	{
		if (tmLocal == 0.0f)
		{
			CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
			VEC3D pos;

			pos = VEC3D(0.0f, 0.0f, 0.0f);

			switch (type)
			{
			case LOC_CHARA:
				break;

			case LOC_OBJECT:
				if (i3::generic_compare(value, L"BombA") == 0)
					pos = GetBombInstallPos(BOMB_AREA_A);
				else if (i3::generic_compare(value, L"BombB") == 0)
					pos = GetBombInstallPos(BOMB_AREA_B);
				break;

			default:
				pos = ParseVEC3D(value);
				break;
			}

			pPlayer->WarpChara(&pos);
		}

		if (tmLocal > 0.3f)
			return TESTRESULT_PASS;
	}

	return TESTRESULT_CONTINUE;
}

TESTRESULT	TestAutomationInterface::OnLookAt( float tmLocal, DIRTYPE type, wstring & value)
{
	if (g_pFramework->IsBattleStage() == false)
	{
		return TESTRESULT_FAIL;
	}
	else
	{
		if (tmLocal == 0.0f)
		{
			VEC3D dir;

			dir = VEC3D(0.0f, 0.0f, 0.0f);
			CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());

			switch (type)
			{
			case DIR_CHARA:
				break;

			case DIR_OBJECT:			// 특정 Object를 바라보도록 합니다.
				{
					VEC3D target = VEC3D(0.0f, 0.0f, 0.0f);

					if (i3::generic_compare(value, L"BombA") == 0)
						target = GetBombInstallPos(BOMB_AREA_A);
					else if (i3::generic_compare(value, L"BombB") == 0)
						target = GetBombInstallPos(BOMB_AREA_B);

					VEC3D pos = *pPlayer->GetPos();
					pos.y += 2.0f;

					VEC3D at =  target - pos;					// 바라봐야 할 방향
					
					// Theta
					{
						dir.x = i3Math::atan2(-at.z, at.x);
					}

					// Rho
					{
						VEC3D theta = VEC3D(at.x, 0.0f, at.z);		// 바라 볼 높이 계산을 위해...
						theta.Normalize();
						at.Normalize();

						float c = VEC3D::Dot( at, theta);
						dir.y = - i3Math::NormalizeAngle( acosf(c));
					}
				}
				break;

			default:
				dir = ParseVEC3D(value);
				break;
			}

			if (pPlayer != nullptr)
			{
				CGameCharaBoneContext * pBoneCtx = pPlayer->getBoneContext();

				pBoneCtx->setTheta( dir.x);
				pBoneCtx->setRho(dir.y);

				return TESTRESULT_PASS;
			}
		}
	}

	return TESTRESULT_CONTINUE;
}

#endif