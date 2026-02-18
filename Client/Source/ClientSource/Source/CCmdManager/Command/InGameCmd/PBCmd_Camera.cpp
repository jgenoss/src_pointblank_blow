#include "pch.h"
#include "PBCmd_Camera.h"
#include "../../../Cheatkey_LimitPred.h"
#include "../../../UtilBattle.h"

namespace Cheatkey
{
	//--------------------------------------------------
	// 3인칭 자유 관점
	// ex) "/camera3pfree"
	//--------------------------------------------------
	bool Toggle3pCamera::_Execute()
	{
		ub::toggle_3p_camera();

		return true;
	}

	ICommand* Toggle3pCamera::Creator()
	{
		return new Toggle3pCamera(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//--------------------------------------------------
	// 플라이 카메라
	// ex) "/fly"
	//--------------------------------------------------
	ICommand * ToggleFlyCamera::Creator()
	{
		return new ToggleFlyCamera(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	bool ToggleFlyCamera::_Execute()
	{
		ub::toggle_flying_camera_mode();
		return true;
	}

	//--------------------------------------------------
	// 탑뷰
	// ex) "/topview"
	//--------------------------------------------------
	ICommand * TopViewCamera::Creator()
	{
		return new TopViewCamera(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	bool TopViewCamera::_Execute()
	{
		ub::toggle_topview_camera();
		return true;
	}
}

