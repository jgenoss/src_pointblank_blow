#include "pch.h"
#include "UIClanFunction.h"

namespace ucf
{

	i3UIImageBoxEx* CreateImageBoxExUV( i3UIControl* pParent, i3Texture *pTex, INT32 u, INT32 v, INT32 w, INT32 h )
	{
		if (pParent == nullptr || pTex == nullptr )
			return nullptr;

		i3UIImageBoxEx* pImage = i3UIImageBoxEx::new_object();
		pImage->AddTexture( pTex, u, v, w, h);
		pImage->CreateBaseSceneObjects();
		pParent->AddControl(pImage);
		pImage->setPos(0,0,0);						// 아..이걸 안해주면 문제가 되었다..
		I3_MUST_RELEASE(pImage);						// 한때 삭제되었는데, 다시 넣었습니다..
		pImage->SetCurrentImage(0);
		
		return pImage;
	}

	bool			AdjustControlPosToParentCenter(i3UIControl* ctrl)
	{
		if (ctrl == 0) return false;
		i3UIControl* parent = static_cast<i3UIControl*>(ctrl->getParent());
		if (parent == 0) return false;
		
		VEC2D* size_parent = parent->getSize();
		VEC2D* size_ctrl   = ctrl->getSize();

		REAL32 offset_x = (size_parent->x - size_ctrl->x) * .5f;
		REAL32 offset_y = (size_parent->y - size_ctrl->y) * .5f;
		ctrl->setPos(offset_x, offset_y);

		return true;
	}

	void	SelectClanMark(i3UIControl* parent, CLAN_MARK_SIZE type, CLAN_DETAIL_INFO* info, i3UIImageBoxEx*& inout)
	{
		inout = ucf::RemoveImageBoxExFromParent(inout);
		inout = ucf::CreateClanMark(parent, type, info);
		ucf::AdjustControlPosToParentCenter(inout);		
	}
	void	SelectClanMark(i3UIControl* parent, CLAN_MARK_SIZE type, UINT32 nClanMarkFlag, i3UIImageBoxEx*& inout)
	{
		inout = ucf::RemoveImageBoxExFromParent(inout);
		inout = ucf::CreateClanMark(parent, type, nClanMarkFlag);
		ucf::AdjustControlPosToParentCenter(inout);
	}

	i3UIImageBoxEx* CreateClanMark( i3UIControl* pParent, CLAN_MARK_SIZE type, CLAN_DETAIL_INFO* info)
	{
		return (info) ? CreateClanMark(pParent, type, info->m_mark) : nullptr;
	}

	i3UIImageBoxEx*	CreateClanMark(	i3UIControl* pParent, CLAN_MARK_SIZE type, UINT32 nClanMarkFlag)
	{
		return CreateClanMark( pParent, type, CLANMARK_FRAME_IMAGE_IDX(nClanMarkFlag),
			CLANMARK_CONTENT_IMAGE_IDX(nClanMarkFlag), CLANMARK_FRAME_COLOR_IDX(nClanMarkFlag),
			CLANMARK_CONTENT_COLOR_IDX(nClanMarkFlag));
	}

	i3UIImageBoxEx*	CreateClanMark(	i3UIControl* pParent, CLAN_MARK_SIZE type, INT32 nBackImageIdx, INT32 nShapeImageIdx, 
		INT32 nBackColorIdx, INT32 nShapeColorIdx)
	{
		I3_BOUNDCHK(type, MAX_CLAN_MARK_SIZE);
		
		i3Texture* pTex = nullptr;
		INT32 texU=0, texV=0, texW = 0, texH = 0;
		
		if( nBackImageIdx == 0 && nShapeImageIdx == 0 && nBackColorIdx == 0 && nShapeColorIdx == 0)
		{// 클랜마크가 없을 경우  Default Mark 사용
			pTex = g_pClanMarkManager->GetDefaultClanMarkTex(type);
			texU = g_pClanMarkManager->GetDefaultClanMarkU(type);
			texV = g_pClanMarkManager->GetDefaultClanMarkV(type);
			texW = texH = g_pClanMarkManager->GetDefaultClanMarkWH(type);
			return ucf::CreateImageBoxExUV(pParent, pTex, texU, texV, texW, texH);
		}
		else if( MAKE_CLANMARK_FLAG(nBackImageIdx, nShapeImageIdx, nBackColorIdx, nShapeColorIdx) == CLAN_MARK_FLAG_NONE)
		{	//	비클랜원이다. 각 Index가 255를 넘어서면 안된다.
			return nullptr;
		}
		else if( nBackImageIdx == 0 || nShapeImageIdx == 0 || nBackColorIdx == 0 || nShapeColorIdx == 0)
		{	//	인덱스는 1 base 이다. 하나라도 0이면 인자가 잘못된 것입니다.
			return nullptr;
		}
		
		//	클랜 마크 이미지
		ClanMarkInfo * pClanMarkInfo = g_pClanMarkManager->GetClanMarkTex(type, nBackImageIdx, nShapeImageIdx, nBackColorIdx, nShapeColorIdx);
		I3ASSERT( pClanMarkInfo != nullptr);
				
		pTex = pClanMarkInfo->_image;	texU = pClanMarkInfo->_texU;	texV = pClanMarkInfo->_texV;	
		texW = pClanMarkInfo->_texW;	texH = pClanMarkInfo->_texH;

		return  ucf::CreateImageBoxExUV(pParent, pTex, texU, texV, texW, texH);
	}

	i3UIImageBoxEx*	RemoveImageBoxExFromParent( i3UIImageBoxEx* ctrl )		// ctrl이 완전이 메모리 소거될수 있습니다.
	{
		i3UIImageBoxEx* ret = ctrl;
		if (ctrl == nullptr) return nullptr;
		if ( ctrl->GetRefCount() == 1) ret = 0;
		
		i3UIControl* parent = static_cast<i3UIControl*>(ctrl->getParent());
		if ( parent != nullptr)
			parent->RemoveControl(ctrl);
		else
		{
			I3_MUST_RELEASE(ctrl);
		}

		return ret;
	}
	
	void	MakeComboboxString(i3::wstring& inout, const i3::wliteral_range& input)
	{
		if (!inout.empty()) 
			inout += L';';
		
		inout.append(input.begin(), input.end());
	}
	
}
