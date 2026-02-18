#include "StdAfx.h"
#include "i3UIEMeta.h"

#include "UIProject.h"

#include "Control/UIGripObjHandler.h"
#include "Control/UIGripCtrl.h"
#include "Control/UIGripCtrlPoint.h"
#include "Control/UIGripCtrlRect.h"

#include "UndoSystem2/UIUndoInfo.h"
#include "UndoSystem2/UIUndoInfoCreate.h"
#include "UndoSystem2/UIUndoInfoDelete.h"
#include "UndoSystem2/UIUndoInfoTransform.h"
#include "UndoSystem2/UIUndoInfoEdit.h"
#include "UndoSystem2/UIUndoSystem.h"

#define PAGE_SIZE 4096

void	SetMemPool( i3ClassMeta * pMeta)
{
//	i3FixedMemoryPool * pPool;

//	pPool = new i3FixedMemoryPool;

//	pPool->Create( pMeta->GetClassName(), pMeta->GetClassSize(), PAGE_SIZE);

//	pMeta->SetMemoryPool( pPool);
	i3::v_pool_ptr p = i3::create_v_pool_ptr<i3::v_storage_pool_mt>();
	pMeta->set_pool(p);
}

void	RegisterUIEMetas(void)
{
//	//CUIProject::RegisterMeta();

//	CUIGripObjHandler::RegisterMeta();
//	CUIGripCtrlPoint::RegisterMeta();
//	CUIGripCtrl::RegisterMeta();
//	CUIGripCtrlRect::RegisterMeta();

	// UI Undo System
//	UIUndoInfo::RegisterMeta();
//	UIUndoInfoCreate::RegisterMeta();
//	UIUndoInfoDelete::RegisterMeta();
	// //UIUndoInfoTransform::RegisterMeta();
	// //UIUndoInfoEdit::RegisterMeta();
//	UIUndoSystem::RegisterMeta();

	//SetMemory Pool
	{
		SetMemPool( CUIGripCtrl::static_meta());

		i3::vector<i3ClassMeta*> list;
		
		{
			CUIGripCtrl::static_meta()->get_all_derived_metas( list);
			for(size_t i = 0; i < list.size(); i++)
			{
				i3ClassMeta * pMeta = list[i];

				if(	pMeta->get_class_meta_style() == i3::cms_abstract ) //  pMeta->IsAbstractClass())
					continue;

				SetMemPool( pMeta);
			}
		}
	}
}
