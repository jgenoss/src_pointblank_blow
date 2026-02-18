#include "i3FrameworkPCH.h"
#include "i3UIListView_Box.h"
#include "i3UIListView_Item.h"
#include "i3UIListView_Data.h"
#include "i3UIListView_Header.h"
#include "i3UIListView_Cell.h"
#include "i3UIManager.h"
#include "i3UIListView_Defines.h"
#include "i3UITemplate_ImageBox.h"
#include "i3UIButtonImageSet.h"
#include "i3UITemplate_ButtonImageSet.h"
#include "i3UIControl_Dummy.h"
#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

#include "i3Base/string/ext/utf8_to_utf16.h"

extern "C"
{
	int i3UILV_NewItem( lua_State* L)
	{
		//LVITEM_DATA*	NewItem( void);
		//i3UIControl* pControl = (i3UIControl*) i3Lua::GetUserDataArg(L, 1);
		//char conv[256];
		//i3::string_ncopy_nullpad( conv, pControl->GetClassMeta()->GetClassNameA(), sizeof( conv));
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			LVITEM_DATA* pLVITEM = pLVBox->NewItem();
			i3Lua::PushUserData( L, pLVITEM);
		}
		else
		{
			i3Lua::PushUserData( L, nullptr);
		}
		
		return 1;
	}

	int i3UILV_AddItem( lua_State* L)
	{
		//void			AddItem( LVITEM_DATA* pData);
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			LVITEM_DATA* pLVITEM = (LVITEM_DATA*)i3Lua::GetUserDataArg( L, 2);
			pLVBox->AddItem( pLVITEM);
		}
		
		return 0;
	}

	int i3UILV_InsertItem( lua_State* L)
	{
		//void			InsertItem( INT32 nIdx, LVITEM_DATA* pData);
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			INT32 nIdx = i3Lua::GetIntegerArg( L, 2, -1);
			LVITEM_DATA* pLVITEM = (LVITEM_DATA*)i3Lua::GetUserDataArg( L, 3);
			pLVBox->InsertItem( nIdx, pLVITEM);
		}
		
		return 0;
	}

	int i3UILV_DeleteItem( lua_State* L)
	{
		//void			DeleteItem( INT32 nIdx);
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			INT32 nIdx = i3Lua::GetIntegerArg( L, 2, -1);
			pLVBox->DeleteItem( nIdx);
		}
		
		return 0;
	}

	int i3UILV_DeleteAllItems(LuaState *L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			pLVBox->DeleteAllItems();
		}
		return 0;
	}

	int i3UILV_GetCellControl( LuaState *L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);

		if( pLVBox != nullptr)
		{
			INT32 nItemIdx = i3Lua::GetIntegerArg( L, 2, -1);
			INT32 nCellIdx = i3Lua::GetIntegerArg( L, 3, -1);

			i3UIControl * pCtrl = pLVBox->getCellChild( nItemIdx, nCellIdx);

			i3Lua::PushUserData( L, pCtrl);

			return 1;
		}
		
		return 0;
	}

	int i3UILV_GetHeaderControl(LuaState *L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);

		if( pLVBox != nullptr)
		{
			INT32 nCol = i3Lua::GetIntegerArg( L, 2, -1);

			i3UIControl * pCtrl = pLVBox->getHeaderButton(nCol);

			i3Lua::PushUserData( L, pCtrl);

			return 1;
		}

		return 0;
	}

	int i3UILV_SetCell( lua_State* L)
	{
		char szData[ 1024];

		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);

		if( pLVBox != nullptr)
		{
			INT32 nItemIdx = i3Lua::GetIntegerArg( L, 2, -1);
			INT32 nCellIdx = i3Lua::GetIntegerArg( L, 3, -1);

			if( i3Lua::GetStringArg( L, 4, szData, sizeof( szData), "DEFAULT") != nullptr)
				pLVBox->SetItemCell( nItemIdx, nCellIdx, szData);
		}
		
		return 0;
	}

	int i3UILV_SetCellDataTable( lua_State* L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);

		if( pLVBox != nullptr)
		{
			INT32 nItemIdx = i3Lua::GetIntegerArg( L, 2, -1);
			INT32 nCellIdx = i3Lua::GetIntegerArg( L, 3, -1);

			i3::vector<i3::string> List;
			List.reserve( 32);

			size_t size = i3Lua::GetTableSize( L, -1);
			for( size_t i = 1; i <= size; i++)
			{
				i3Lua::PushInteger( L, i);
				i3Lua::Rawget( L, -2);

				if( i3Lua::StackIsString( L, -1))
				{
					char szData[MAX_PATH] = {0,};
					i3Lua::GetStringArg( L, -1, szData, MAX_PATH);	

					List.push_back( szData);
				}
				i3Lua::StackPop( L, 1);
			}
			pLVBox->SetItemCell( nItemIdx, nCellIdx, List);
		}

		return 0;	
	}
	
	int i3UILV_SetCellDCStyle( lua_State* L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);

		if( pLVBox != nullptr)
		{
			INT32 nItemIdx = i3Lua::GetIntegerArg( L, 2, -1);
			INT32 nCellIdx = i3Lua::GetIntegerArg( L, 3, -1);

			i3::vector<i3UIListView_Item*> ItmeList = pLVBox->getItemList();
			if( nItemIdx >= 0 && nItemIdx < static_cast<INT32>(ItmeList.size()))
			{
				i3UIListView_Cell* pCell = ItmeList[nItemIdx]->getCell( nCellIdx);
				if( pCell != nullptr)
				{
					if( i3UIControl_Dummy* pDummy = i3::same_cast<i3UIControl_Dummy*>(pCell->getFirstChild()))
					{
						INT32 nChildIdx = i3Lua::GetIntegerArg( L, 4, -1);
						INT32 nAlignStyle = i3Lua::GetIntegerArg( L, 5, -1);
	
						pDummy->SetChildAlignStyle( nChildIdx, nAlignStyle);
					}
				}
			}
		}
		return 0;
	}

	int i3UILV_SetCellDCPos( lua_State* L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);

		if( pLVBox != nullptr)
		{
			INT32 nItemIdx = i3Lua::GetIntegerArg( L, 2, -1);
			INT32 nCellIdx = i3Lua::GetIntegerArg( L, 3, -1);

			i3::vector<i3UIListView_Item*> ItmeList = pLVBox->getItemList();
			if( nItemIdx >= 0 && nItemIdx < static_cast<INT32>(ItmeList.size()))
			{
				i3UIListView_Cell* pCell = ItmeList[nItemIdx]->getCell( nCellIdx);
				if( pCell != nullptr)
				{
					if( i3UIControl_Dummy* pDummy = i3::same_cast<i3UIControl_Dummy*>(pCell->getFirstChild()))
					{
						INT32 nChildIdx = i3Lua::GetIntegerArg( L, 4, -1);
						REAL32 x = i3Lua::GetNumberArg( L, 5, -1);
						REAL32 y = i3Lua::GetNumberArg( L, 6, -1);

						pDummy->SetChildPos( nChildIdx, x, y);
					}
				}
			}
		}

		return 0;
	}


	int i3UILV_SetItemCell( lua_State* L)
	{
		char szStr[ 260];
		bool bResult = false;
		ECELLTYPE eType = ECELLTYPE_TEXT;
		
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			LVITEM_DATA* pData = (LVITEM_DATA*)i3Lua::GetUserDataArg( L, 2);
			if( pData != nullptr)
			{
				INT32 nCellIdx = i3Lua::GetIntegerArg( L, 3, -1);
				
				if( i3Lua::GetStringArg( L, 4, szStr, sizeof( szStr), "DEFAULT") != nullptr)
				{
					i3UIListView_Item* pLVItem = pLVBox->getBoundItem();
					if( pLVItem != nullptr)
					{
						eType = pLVItem->getCellType( nCellIdx);
						if( eType == ECELLTYPE_IMAGE)
						{
							i3UITemplate* pTemplate = i3UI::FindTemplateByName( szStr);

							if( pTemplate != nullptr &&
								(i3::same_of<i3UITemplate_ImageBox*>(pTemplate) ||
								 i3::same_of<i3UITemplate_ButtonImageSet*>(pTemplate) ) )
							{
								pData->SetCellData( nCellIdx, szStr);
								bResult = true;
							}
						}
						else if(eType == ECELLTYPE_TEMPLATE)
						{
							//i3UITemplate* pTemplate = i3UI::FindTemplateByName( szData);
							//if( pTemplate != nullptr)
							{
								pData->SetCellData( nCellIdx, szStr);
								bResult = true;
							}
						}
						else if( eType == ECELLTYPE_TEXT)
						{
							pData->SetCellData( nCellIdx, szStr);
							bResult = true;
						}
					}
				}
			}
		}

		i3Lua::PushBoolean( L, bResult);					//Result
		i3Lua::PushString( L, s_pszCellType[eType]);		//CellType
		
		return 2;
	}

	int i3UILV_SetCells( lua_State* L)
	{
		char szStr[ 260];
		bool bResult = false;
		INT32 idx = -1;
		ECELLTYPE eType = ECELLTYPE_TEXT;


		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			LVITEM_DATA* pData = (LVITEM_DATA*)i3Lua::GetUserDataArg( L, 2);
			if( pData != nullptr)
			{
				i3UIListView_Item* pLVItem = pLVBox->getBoundItem();
				if( pLVItem != nullptr)
				{
					bResult = true;
					
					for( INT32 i=0; i< pData->getCellCount(); i++)
					{
						eType = pLVItem->getCellType( i);

						if( i3Lua::GetStringArg( L, i+3, szStr, sizeof( szStr), "DEFAULT") != nullptr)
						{
							if( eType == ECELLTYPE_IMAGE )
							{
								i3UITemplate* pTemplate = i3UI::FindTemplateByName( szStr);

								if( pTemplate == nullptr || 
									(i3::same_of<i3UITemplate_ImageBox*>(pTemplate) == false &&
									 i3::same_of<i3UITemplate_ButtonImageSet*>(pTemplate) == false ) )
								{
									bResult = false;
									idx = i;
									break;
								}
							}
							
							pData->SetCellData( i, szStr);
						}
					}
				}
			}
		}

		i3Lua::PushBoolean( L, bResult);					//Result
		i3Lua::PushInteger( L, idx);						//idx
		i3Lua::PushString( L, s_pszCellType[eType]);		//CellType
		
		return 3;
	}

	int i3UILV_SetHeader( lua_State* L)
	{
		char szStr[ 260];
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			i3UIListView_Item* pBoundItem = pLVBox->getBoundItem();
			if( pBoundItem != nullptr)
			{
				i3::vector<i3::rc_wstring> strList;

				for( INT32 i=0; i< pBoundItem->getCellCount(); i++)
				{
					if( i3Lua::GetStringArg( L, i+2, szStr, sizeof( szStr), "DEFAULT") != nullptr)
					{
						i3::rc_wstring wstrHeaderItemName;
						i3::utf8_to_utf16(szStr, wstrHeaderItemName);
						strList.push_back( wstrHeaderItemName);
					}
				}

				pLVBox->SetHeaderText( strList);
			}
		}
		
		return 0;
	}

	int i3UILV_GetDataCount( lua_State* L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			INT32 nCount = pLVBox->getDataCount();
			i3Lua::PushInteger( L, nCount);
		}
		return 1;
	}

	int i3UILV_GetItemCount( lua_State* L)
	{
		INT32 nCount = -1;
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			nCount = pLVBox->getItemCount();
		}
		i3Lua::PushInteger( L, nCount);
		return 1;
	}

	int i3UILV_GetItem( LuaState * L)
	{
		i3UIListView_Item * pItem = nullptr;
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);

		if( pLVBox != nullptr)
		{
			i3::vector<i3UIListView_Item*>& List = pLVBox->getItemList();

			INT32 item = i3Lua::GetIntegerArg( L, 2);

			if( item < (INT32)List.size())
				pItem = List[item];
		}

		i3Lua::PushUserData( L, pItem);

		return 1;
	}

	int i3UILV_GetCellCount( lua_State* L)
	{
		INT32 nCellCount = -1;
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			if( pLVBox->getBoundItem() != nullptr)
			{
				nCellCount = pLVBox->getBoundItem()->getCellCount();
			}
		}

		i3Lua::PushInteger( L, nCellCount);
		return 1;
	}


	int i3UILV_OnPreviewMode( lua_State* L)
	{
		bool bResult = false;

		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			bResult = pLVBox->OnPreviewMode();
		}
		i3Lua::PushBoolean( L, bResult);
		return 1;
	}

	int i3UILV_OnEditMode( lua_State* L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			pLVBox->OnEditMode();
		}
		return 0;
	}

	// All, Header, Item, Col 만 있으면 됨
	int i3UILV_SetTextAlign( lua_State* L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			INT32 nXAlign = i3Lua::GetIntegerArg( L, 2, 1);
			INT32 nYAlign = i3Lua::GetIntegerArg( L, 3, 1);
			ECOMMAND_SCOPE eScope = (ECOMMAND_SCOPE)i3Lua::GetIntegerArg( L, 4, 0);		//param1

			i3UIListView_Header* pHeader = pLVBox->getHeader();
			
			switch( eScope)
			{
			case ECOMMAND_SCOPE_ALL:
				{
					if( pHeader != nullptr)
					{
						pHeader->SetHeaderAlign( nXAlign, nYAlign);
					}

					for( INT32 i=0; i< pLVBox->getCellCount(); i++)	
					{
						pLVBox->SetCellTextAlign( i, nXAlign, nYAlign);
					}
				}
				break;
			case ECOMMAND_SCOPE_HEADER:
				{
					if( pHeader != nullptr)
					{
						INT32 IsIndividual = i3Lua::GetIntegerArg( L, 6, 0);
						if( IsIndividual == 0 )
						{
							pHeader->SetHeaderAlign( nXAlign, nYAlign);
						}
						else
						{
							INT32 nColIdx = i3Lua::GetIntegerArg( L, 5, -1);
							pHeader->SetIndividualHeaderAlign( nColIdx, nXAlign, nYAlign);
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_ITEM:
				{
					for( INT32 i=0; i< pLVBox->getCellCount(); i++)
					{
						pLVBox->SetCellTextAlign( i, nXAlign, nYAlign);
					}
				}
				break;
			case ECOMMAND_SCOPE_COLUMN:
				{
					INT32 nColIdx = i3Lua::GetIntegerArg( L, 5, -1);
					
					pLVBox->SetCellTextAlign( nColIdx, nXAlign, nYAlign);
				}
				break;
			default:
				break;
			}

			pLVBox->Redraw();
		}
		return 0;
	}

	// 이건 특정 Cell의 텍스트 컬러를 설정할 때만 쓰임
	int i3UILV_SetTextColor( lua_State* L)
	{
		INT32 j;
		char szStr[ 260];
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		
		if( pLVBox != nullptr)
		{
			INT32 r = i3Lua::GetIntegerArg(L, 2, 255);
			INT32 g = i3Lua::GetIntegerArg(L, 3, 255);
			INT32 b = i3Lua::GetIntegerArg(L, 4, 255);
			
			COLOR col;
			i3Color::Set(&col, (UINT8)r, (UINT8)g, (UINT8)b, 255);
			
			ECOMMAND_SCOPE eScope = (ECOMMAND_SCOPE)i3Lua::GetIntegerArg( L, 5, 0);		//param1

			i3::vector<LVITEM_DATA*>& DataList = pLVBox->getDataList();
			i3UIListView_Header* pHeader = pLVBox->getHeader();

			switch( eScope)
			{
			case ECOMMAND_SCOPE_ALL:
				{
					if( pHeader != nullptr)
					{
						pHeader->SetHeaderColor( &col);
					}

					for(size_t i=0; i< DataList.size(); i++)
					{
						LVITEM_DATA* pItem = DataList[i];
						for( j=0; j< pItem->getCellCount(); j++)
						{
							pItem->setCellColor( j, &col);
						}
					}

					//pLVBox->setDefColor(&col);
				}
				break;
			case ECOMMAND_SCOPE_HEADER:
				{
					if( pHeader != nullptr)
					{
						pHeader->SetHeaderColor( &col);
					}
				}
				break;
			case ECOMMAND_SCOPE_ITEM:
				{
					for(size_t i=0; i< DataList.size(); i++)
					{
						LVITEM_DATA* pItem = DataList[i];
						for( j=0; j< pItem->getCellCount(); j++)
						{
							pItem->setCellColor( j, &col);
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_ROW:
				{
					INT32 nRowIdx = i3Lua::GetIntegerArg( L, 6, -1);
					
					if( nRowIdx > -1 && nRowIdx < (INT32)DataList.size())
					{
						LVITEM_DATA* pItem = DataList[nRowIdx];
						for( j=0; j< pItem->getCellCount(); j++)
						{
							pItem->setCellColor( j, &col);
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_COLUMN:
				{
					INT32 nColIdx = i3Lua::GetIntegerArg( L, 6, -1);
					for(size_t i=0; i< DataList.size(); i++)
					{
						LVITEM_DATA* pItem = DataList[i];
						if( nColIdx > -1 && nColIdx < pItem->getCellCount())
						{
							pItem->setCellColor( nColIdx, &col);
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_CELL:
				{
					INT32 nRowIdx = i3Lua::GetIntegerArg( L, 6, -1);
					INT32 nColIdx = i3Lua::GetIntegerArg( L, 7, -1);

					if( nRowIdx > -1 && nRowIdx < (INT32)DataList.size())
					{
						LVITEM_DATA* pItem = DataList[nRowIdx];
						if( nColIdx > -1 && nColIdx < pItem->getCellCount())
						{
							pItem->setCellColor( nColIdx, &col);
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_DATA:
				{
					if( i3Lua::GetStringArg( L, 6, szStr, sizeof( szStr), "DEFAULT") != nullptr)
					{
						for(size_t i=0; i< DataList.size(); i++)
						{
							LVITEM_DATA* pItem = DataList[i];

							for( j=0; j< pItem->getCellCount(); j++)
							{
								char buf[MAX_PATH +1] = "";
								pItem->GetCellData( j, buf, MAX_PATH);

								if( i3::generic_is_iequal( buf, szStr) )
								{
									pItem->setCellColor( j, &col);
								}
							}
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_BOX:
				{
					//pLVBox->setDefColor(&col);
				}
				break;
			default:
				break;
			}
			pLVBox->Redraw();
		}
		return 0;
	}

	int i3UILV_SetTextColor4Byte(lua_State* L)
	{
		INT32 j;
		char szStr[ 260];
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);

		if( pLVBox != nullptr)
		{
			INT32 rgba = i3Lua::GetIntegerArg(L, 2, 255);

			COLOR col;	col.SetRGBA(rgba);

			ECOMMAND_SCOPE eScope = (ECOMMAND_SCOPE)i3Lua::GetIntegerArg( L, 3, 0);		//param1

			i3::vector<LVITEM_DATA*>& DataList = pLVBox->getDataList();
			i3UIListView_Header* pHeader = pLVBox->getHeader();

			switch( eScope)
			{
			case ECOMMAND_SCOPE_ALL:
				{
					if( pHeader != nullptr)
					{
						pHeader->SetHeaderColor( &col);
					}

					for( size_t i=0; i< DataList.size(); i++)
					{
						LVITEM_DATA* pItem = DataList[i];
						for( j=0; j< pItem->getCellCount(); j++)
						{
							pItem->setCellColor( j, &col);
						}
					}

					//pLVBox->setDefColor(&col);
				}
				break;
			case ECOMMAND_SCOPE_HEADER:
				{
					if( pHeader != nullptr)
					{
						pHeader->SetHeaderColor( &col);
					}
				}
				break;
			case ECOMMAND_SCOPE_ITEM:
				{
					for(size_t i=0; i< DataList.size(); i++)
					{
						LVITEM_DATA* pItem = DataList[i];
						for( j=0; j< pItem->getCellCount(); j++)
						{
							pItem->setCellColor( j, &col);
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_ROW:
				{
					INT32 nRowIdx = i3Lua::GetIntegerArg( L, 4, -1);

					if( nRowIdx > -1 && nRowIdx < (INT32)DataList.size())
					{
						LVITEM_DATA* pItem = DataList[nRowIdx];
						for( j=0; j< pItem->getCellCount(); j++)
						{
							pItem->setCellColor( j, &col);
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_COLUMN:
				{
					INT32 nColIdx = i3Lua::GetIntegerArg( L, 4, -1);
					for( size_t i=0; i< DataList.size(); i++)
					{
						LVITEM_DATA* pItem = DataList[i];
						if( nColIdx > -1 && nColIdx < pItem->getCellCount())
						{
							pItem->setCellColor( nColIdx, &col);
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_CELL:
				{
					INT32 nRowIdx = i3Lua::GetIntegerArg( L, 4, -1);
					INT32 nColIdx = i3Lua::GetIntegerArg( L, 5, -1);

					if( nRowIdx > -1 && nRowIdx < (INT32)DataList.size())
					{
						LVITEM_DATA* pItem = DataList[nRowIdx];
						if( nColIdx > -1 && nColIdx < pItem->getCellCount())
						{
							pItem->setCellColor( nColIdx, &col);
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_DATA:
				{
					if( i3Lua::GetStringArg( L, 4, szStr, sizeof( szStr), "DEFAULT") != nullptr)
					{
						for( size_t i=0; i< DataList.size(); i++)
						{
							LVITEM_DATA* pItem = DataList[i];

							for( j=0; j< pItem->getCellCount(); j++)
							{
								char buf[MAX_PATH +1] = "";
								pItem->GetCellData( j, buf, MAX_PATH);

								if( i3::generic_is_iequal( buf, szStr) )
								{
									pItem->setCellColor( j, &col);
								}
							}
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_BOX:
				{
					//pLVBox->setDefColor(&col);
				}
				break;
			default:
				break;
			}
			pLVBox->Redraw();
		}
		return 0;
	}

	int i3UILV_RestoreTextColor( lua_State* L)
	{
		INT32 j;
		char szStr[ 260];
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);

		if( pLVBox != nullptr)
		{
			ECOMMAND_SCOPE eScope = (ECOMMAND_SCOPE)i3Lua::GetIntegerArg( L, 2, 0);		//param1

			i3::vector<LVITEM_DATA*>& DataList = pLVBox->getDataList();
		//	i3UIListView_Header* pHeader = pLVBox->getHeader();

			switch( eScope)
			{
			case ECOMMAND_SCOPE_ALL:
				{
					//
					// 헤더쪽 색상은 그냥 스킵함..
					//

					for(size_t i=0; i< DataList.size(); i++)
					{
						LVITEM_DATA* pItem = DataList[i];
						for( j=0; j< pItem->getCellCount(); j++)
						{
							pItem->restoreCellColor( j);
						}
					}

					//pLVBox->setDefColor(&col);
				}
				break;
			case ECOMMAND_SCOPE_HEADER:
				{
					//
					// 헤더쪽 색상은 그냥 스킵함..
					//
				}
				break;
			case ECOMMAND_SCOPE_ITEM:
				{
					for(size_t i=0; i< DataList.size(); i++)
					{
						LVITEM_DATA* pItem = DataList[i];
						for( j=0; j< pItem->getCellCount(); j++)
						{
							pItem->restoreCellColor( j);
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_ROW:
				{
					INT32 nRowIdx = i3Lua::GetIntegerArg( L, 3, -1);

					if( nRowIdx > -1 && nRowIdx < (INT32)DataList.size())
					{
						LVITEM_DATA* pItem = DataList[nRowIdx];
						for( j=0; j< pItem->getCellCount(); j++)
						{
							pItem->restoreCellColor( j);
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_COLUMN:
				{
					INT32 nColIdx = i3Lua::GetIntegerArg( L, 3, -1);
					for(size_t i=0; i< DataList.size(); i++)
					{
						LVITEM_DATA* pItem = DataList[i];
						if( nColIdx > -1 && nColIdx < pItem->getCellCount())
						{
							pItem->restoreCellColor( nColIdx);
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_CELL:
				{
					INT32 nRowIdx = i3Lua::GetIntegerArg( L, 3, -1);
					INT32 nColIdx = i3Lua::GetIntegerArg( L, 4, -1);

					if( nRowIdx > -1 && nRowIdx < (INT32)DataList.size())
					{
						LVITEM_DATA* pItem = DataList[nRowIdx];
						if( nColIdx > -1 && nColIdx < pItem->getCellCount())
						{
							pItem->restoreCellColor( nColIdx);
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_DATA:
				{
					if( i3Lua::GetStringArg( L, 3, szStr, sizeof( szStr), "DEFAULT") != nullptr)
					{
						for(size_t i=0; i< DataList.size(); i++)
						{
							LVITEM_DATA* pItem = DataList[i];

							for( j=0; j< pItem->getCellCount(); j++)
							{
								char buf[MAX_PATH +1] = "";
								pItem->GetCellData( j, buf, MAX_PATH);

								if( i3::generic_is_iequal( buf, szStr) )
								{
									pItem->restoreCellColor( j);
								}
							}
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_BOX:
				{
					//pLVBox->setDefColor(&col);
				}
				break;
			default:
				break;
			}
			pLVBox->Redraw();
		}
		return 0;
	}

	int i3UILV_SetTextFont( LuaState * L)
	{
		INT32 j;
		char szStr[ 260];
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);

		if( pLVBox != nullptr)
		{
			I3UI_FONTINFO info;

			ECOMMAND_SCOPE eScope = (ECOMMAND_SCOPE)i3Lua::GetIntegerArg( L, 2, 0);		//param1
			
			{
				char tempFontName[MAX_PATH];
				i3Lua::GetStringArg( L, 3, tempFontName, _countof( tempFontName ));
				info.m_strName = tempFontName;
			}

			info.m_Size = i3Lua::GetIntegerArg( L, 4, 10);
			info.m_Quality = (I3UI_FONT_QUALITY) i3Lua::GetIntegerArg( L, 5, 0);
			info.m_Style = i3Lua::GetIntegerArg( L, 6, 0);

			i3::vector<LVITEM_DATA*>& DataList = pLVBox->getDataList();
			i3UIListView_Header* pHeader = pLVBox->getHeader();

			switch( eScope)
			{
			case ECOMMAND_SCOPE_ALL:
				{	
					if( pHeader != nullptr)
					{
						pHeader->SetHeaderFont( info.m_strName.c_str(), info.m_Size, info.m_Quality, info.m_Style);
					}

					for(size_t i=0; i< DataList.size(); i++)
					{
						LVITEM_DATA* pItem = DataList[i];
						for( j=0; j< pItem->getCellCount(); j++)
						{
							pItem->setFontInfo( j, &info);
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_HEADER:
				{
					if( pHeader != nullptr)
					{
						pHeader->SetHeaderFont( info.m_strName.c_str(), info.m_Size, info.m_Quality, info.m_Style);
					}
				}
				break;
			case ECOMMAND_SCOPE_ITEM:
				{
					for( size_t i=0; i< DataList.size(); i++)
					{
						LVITEM_DATA* pItem = DataList[i];
						for( j=0; j< pItem->getCellCount(); j++)
						{
							pItem->setFontInfo( j, &info);
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_ROW:
				{
					INT32 nRowIdx = i3Lua::GetIntegerArg( L, 4, -1);

					if( nRowIdx > -1 && nRowIdx < (INT32)DataList.size())
					{
						LVITEM_DATA* pItem = DataList[nRowIdx];
						for( j=0; j< pItem->getCellCount(); j++)
						{
							pItem->setFontInfo( j, &info);
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_COLUMN:
				{
					INT32 nColIdx = i3Lua::GetIntegerArg( L, 4, -1);
					for( size_t i=0; i< DataList.size(); i++)
					{
						LVITEM_DATA* pItem = DataList[i];
						if( nColIdx > -1 && nColIdx < pItem->getCellCount())
						{
							pItem->setFontInfo( nColIdx, &info);
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_CELL:
				{
					INT32 nRowIdx = i3Lua::GetIntegerArg( L, 4, -1);
					INT32 nColIdx = i3Lua::GetIntegerArg( L, 5, -1);

					if( nRowIdx > -1 && nRowIdx < (INT32)DataList.size())
					{
						LVITEM_DATA* pItem = DataList[nRowIdx];
						if( nColIdx > -1 && nColIdx < pItem->getCellCount())
						{
							pItem->setFontInfo( nColIdx, &info);
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_DATA:
				{
					if( i3Lua::GetStringArg( L, 4, szStr, sizeof( szStr), "DEFAULT") != nullptr)
					{
						for(size_t i=0; i< DataList.size(); i++)
						{
							LVITEM_DATA* pItem = DataList[i];

							for( j=0; j< pItem->getCellCount(); j++)
							{
								char buf[MAX_PATH +1] = "";
								pItem->GetCellData( j, buf, MAX_PATH);

								if( i3::generic_is_iequal( buf, szStr ) )
								{
									pItem->setFontInfo( j, &info);
								}
							}
						}
					}
				}
				break;
			case ECOMMAND_SCOPE_BOX:
				{
					//pLVBox->setDefColor(&col);
				}
				break;
			default:
				break;
			}
			pLVBox->Redraw();
		}
		return 0;
	}


	int i3UILV_GetCurSel( lua_State* L)
	{
		INT32 n = 0;

		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			i3::vector<LVITEM_DATA*>& DataList = pLVBox->getDataList();

			for( size_t i=0; i< DataList.size(); i++)
			{
				LVITEM_DATA* pData = DataList[i];
				if( pData->isSelected())
				{
					i3Lua::PushInteger( L, i);
					n++;
				}
			}
		}

		return n;
	}

	int i3UILV_SetCurSel( lua_State* L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			INT32 idx = i3Lua::GetIntegerArg( L, 2, -1);
			bool bEnable = i3Lua::GetBooleanArg(L, 3, true);			// bEnable을 디폴트인자로 true로 하여 인수를 추가합니다..( 2011.07.17.수빈)
			pLVBox->SelectData( idx, bEnable);
		}

		return 0;
	}


	int i3UILV_UnsetCurSel( lua_State* L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			pLVBox->UnSelectAllData();
		}

		return 0;
	}

	int i3UILV_ScrollToIndex( lua_State* L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			INT32 idx = i3Lua::GetIntegerArg( L, 2, -1);
			pLVBox->ScrollToIndex( idx);
		}

		return 0;
	}

	int i3UILV_SetComboItems( lua_State* L)
	{
		char szStr[ 260];

		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);

		if( pLVBox != nullptr)
		{
			INT32 nColIdx = i3Lua::GetIntegerArg( L, 2, -1);
			if( i3Lua::GetStringArg( L, 3, szStr, sizeof( szStr), "") != nullptr)
			{
				i3::vector<i3UIListView_Item*>& ItemList = pLVBox->getItemList();
				for( size_t i=0; i< ItemList.size(); i++)
				{
					i3UIListView_Item* pLVItem = ItemList[i];
					pLVItem->SetComboItems( nColIdx, szStr);
				}
			}
		}

		return 0;
	}

	int i3UILV_SetButtonName( lua_State* L)
	{
		char szStr[ 260];

		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);

		if( pLVBox != nullptr)
		{
			INT32 nColIdx = i3Lua::GetIntegerArg( L, 2, -1);
			if( i3Lua::GetStringArg( L, 3, szStr, sizeof( szStr), "") != nullptr)
			{
				i3::vector<i3UIListView_Item*>& ItemList = pLVBox->getItemList();
				for( size_t i=0; i< ItemList.size(); i++)
				{
					i3UIListView_Item* pLVItem = ItemList[i];
					pLVItem->SetButtonName( nColIdx, szStr);
				}
			}
		}

		return 0;
	}

	int i3UILV_GetData( lua_State* L)
	{
		INT32 n = 0;

		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			INT32 nIdx = i3Lua::GetIntegerArg( L, 2, -1);
			LVITEM_DATA* pData = pLVBox->getData( nIdx);

			if( pData != nullptr)
			{
				for( INT32 i=0; i< pData->getCellCount(); i++)
				{
					char pBuf[MAX_PATH +1] = "";
					pData->GetCellData( i, pBuf, MAX_PATH);
					i3Lua::PushString( L, pBuf);
					n++;
				}
			}
		}

		return n;
	}


	int i3UILV_SetData( lua_State* L)
	{
		//uiLV_SetData( self.ctrlID, nDataIdx, ...)

		char szStr[ 260];
		bool bRes = false;
		char szMsg[MAX_PATH] = {0,};
		i3::string_ncopy_nullpad( szMsg, "No Error", MAX_PATH);

		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox == nullptr)
		{
			i3::string_ncopy_nullpad( szMsg, "pLVBox == nullptr", MAX_PATH);
			i3Lua::PushBoolean( L, bRes); 		i3Lua::PushString( L, szMsg);		return 2;
		}

		INT32 nDataIdx = i3Lua::GetIntegerArg( L, 2, -1);
		if( nDataIdx < 0)
		{
			i3::string_ncopy_nullpad( szMsg, "nDataIdx < 0", MAX_PATH);
			i3Lua::PushBoolean( L, bRes); 		i3Lua::PushString( L, szMsg);		return 2;
		}

		INT32 nCellCount = pLVBox->getCellCount();
		INT32 nArgCnt = i3Lua::GetIntegerArg( L, 3, -1);
		if( nArgCnt != nCellCount)
		{
			i3::string_ncopy_nullpad( szMsg, "nArgCnt != nCellCount", MAX_PATH);
			i3Lua::PushBoolean( L, bRes); 		i3Lua::PushString( L, szMsg);		return 2;
		}

		for( INT32 i=0; i< nArgCnt; i++)
		{
			if( i3Lua::GetStringArg( L, 4+i, szStr, sizeof( szStr), "DEFAULT") != nullptr)
				pLVBox->SetItemCell( nDataIdx, i, szStr);
		}
		
		bRes = true;

		i3Lua::PushBoolean( L, bRes); 		
		i3Lua::PushString( L, szMsg);		
		return 2;
	}

	int i3UILV_SetDataEnable( lua_State* L)
	{
		INT32 nDataIdx;
		bool bError = true;

		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		
		if( pLVBox != nullptr)
		{
			nDataIdx = i3Lua::GetIntegerArg( L, 2, -1);

			LVITEM_DATA* pData = pLVBox->getData( nDataIdx);
			
			if( pData != nullptr)
			{
				bool bEnable = i3Lua::GetBooleanArg( L, 3, true);
				pData->setEnable( bEnable);

				pLVBox->ApplyChanges();

				bError = false;
			}
		}

		if( bError)
		{
			I3PRINTLOG(I3LOG_WARN,  "function named 'i3UILV_SetDataEnable' is misused");
		}

		return 0;
	}


	int i3UILV_SetChildRect( lua_State* L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			i3UIListView_Item* pLVItem = pLVBox->getBoundItem();
			if( pLVItem != nullptr)
			{
				INT32 nCellIdx = i3Lua::GetIntegerArg( L, 2, -1);
				i3UIListView_Cell* pCell = pLVItem->getCell( nCellIdx);

				if( pCell != nullptr)
				{
					ECELLTYPE eType = pCell->getType();
					if( eType == ECELLTYPE_IMAGE || eType == ECELLTYPE_TEMPLATE)
					{
						INT32 x = i3Lua::GetIntegerArg( L, 3, -1);
						INT32 y = i3Lua::GetIntegerArg( L, 4, -1);
						INT32 w = i3Lua::GetIntegerArg( L, 5, -1);
						INT32 h = i3Lua::GetIntegerArg( L, 6, -1);
						pCell->setChildRect( x, y, w, h);
					}
				}

				i3::vector<i3UIListView_Item*>& ItemList = pLVBox->getItemList();

				for( size_t i=0; i< ItemList.size(); i++)
				{
					i3UIListView_Item* pItem = ItemList[i];
					pCell = pItem->getCell( nCellIdx);

					if( pCell != nullptr)
					{
						ECELLTYPE eType = pCell->getType();
						if( eType == ECELLTYPE_IMAGE || eType == ECELLTYPE_TEMPLATE)
						{
							INT32 x = i3Lua::GetIntegerArg( L, 3, -1);
							INT32 y = i3Lua::GetIntegerArg( L, 4, -1);
							INT32 w = i3Lua::GetIntegerArg( L, 5, -1);
							INT32 h = i3Lua::GetIntegerArg( L, 6, -1);
							pCell->setChildRect( x, y, w, h);
						}
					}
				}
			}
		}

		return 0;
	}

	int i3UILV_GetInstancePointer( lua_State* L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			INT32 nRow = i3Lua::GetIntegerArg( L, 2, -1);
			INT32 nCol = i3Lua::GetIntegerArg( L, 3, -1);

			i3UIControl* pCellChild = pLVBox->getCellChild( nRow, nCol);
			
			i3Lua::PushUserData( L, pCellChild);

			return 1;
		}

		return 0;
	}

	int i3UILV_SetTemplateName( lua_State* L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			INT32 nCol = (INT32)i3Lua::GetNumberArg( L, 2, -1);
			
			char szName[MAX_PATH];
			i3Lua::GetStringArg( L, 3, szName, MAX_PATH, "");

			pLVBox->SetTemplateInfo( nCol, szName);
		}
		return 0;
	}

	int i3UILV_SetTemplateResource( lua_State* L)
	{ 
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			INT32 nCol = (INT32)i3Lua::GetNumberArg( L, 2, -1);
			i3::vector<i3::rc_string> List;
			
			size_t size = i3Lua::GetTableSize( L, -1);
			for( UINT32 i = 1; i <= size; ++i)
			{
				i3Lua::PushInteger( L, i);
				i3Lua::Rawget( L, -2);
				
				if( i3Lua::StackIsString( L, -1))
				{
					char szName[MAX_PATH] = {0,};
					i3Lua::GetStringArg( L, -1, szName, MAX_PATH);

					List.push_back( szName);
				}
				i3Lua::StackPop( L, 1);
			}

			pLVBox->SwapTemplateInfo( nCol, List);	//한번에 설정.
		}
		return 0;
	}

	int i3UILV_IsMouseOver( lua_State* L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			i3Lua::PushBoolean( L, pLVBox->getIsMouseOverValidItem());

			return 1;
		}
		return 0;
	}

	int i3UILV_SetMouseOver( lua_State* L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			bool	bEnable = i3Lua::GetBooleanArg( L, 2, 0);

			pLVBox->setIsMouseOverValidItem( bEnable);
		}
		return 0;
	}

	int i3UILV_ResetButtonState( lua_State* L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			i3::vector<i3UIListView_Item*>& List = pLVBox->getItemList();

			for( INT32 i=0; i<pLVBox->getItemCount(); i++) {
				i3UIListView_Item* pItem = List[i];
				if(pItem == nullptr)	continue;
				
				i3::vector<i3UIListView_Cell*>& List = pItem->getCellList();

				for( INT32 j=0; j<pItem->getCellCount(); j++) {
					i3UIListView_Cell* pCell = List[j];
					if( pCell == nullptr)	continue;
					i3UIControl* pCtrl = pCell->getCellChild();
					if( pCtrl == nullptr)	continue;
					pCtrl->removeState( I3UI_STATE_SELECTED);
					pCtrl->removeState( I3UI_STATE_ONMOUSE);
				}
			}
		}
		return 0;
	}

	int i3UILV_GetShapeValueIdx( lua_State* L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			i3UIListView_Cell* pCell = pLVBox->getCurrentOverCell();
			if( pCell != nullptr) {
				i3UIButtonImageSet* pSet = (i3UIButtonImageSet*)pCell->getCellChild();
				if( pSet != nullptr) {
					INT32 nIdx = pSet->GetShapeIdx();
					i3Lua::PushInteger( L, nIdx);

					return 1;
				}
			}
		}
		return 0;
	}

	int i3UILV_GetShapeValueString( lua_State* L)
	{
		i3UIListView_Box* pLVBox = (i3UIListView_Box*)i3Lua::GetUserDataArg( L, 1);
		if( pLVBox != nullptr)
		{
			i3UIListView_Cell* pCell = pLVBox->getCurrentOverCell();
			if( pCell != nullptr) {
				i3UIButtonImageSet* pSet = (i3UIButtonImageSet*)pCell->getCellChild();
				if( pSet != nullptr) {
					const char * pszValue = pSet->GetShapeString();
					i3Lua::PushString( L, pszValue);

					return 1;
				}
			}
		}
		return 0;
	}

	int i3UILV_SetCellCallback( LuaState * L)
	{
		i3UIListView_Box * pLVBox = (i3UIListView_Box*) i3Lua::GetUserDataArg( L, 1);

		if( pLVBox != nullptr)
		{
			INT32 nItemIdx = i3Lua::GetIntegerArg( L, 2, -1);
			INT32 nCellIdx = i3Lua::GetIntegerArg( L, 3, -1);

			void * pFunc = i3Lua::GetUserDataArg( L, 4);

			pLVBox->SetCellCallback(nItemIdx, nCellIdx, pFunc);
		}

		return 0;
	}

	int i3UILV_SetToolTip( LuaState * L)
	{
		i3UIListView_Box * pLVBox = (i3UIListView_Box*) i3Lua::GetUserDataArg( L, 1);

		if( pLVBox != nullptr)
		{
			char szToolTip[ 1024];

			INT32 nItemIdx = i3Lua::GetIntegerArg( L, 2, -1);
			INT32 nCellIdx = i3Lua::GetIntegerArg( L, 3, -1);

			if( i3Lua::GetStringArg( L, 4, szToolTip, sizeof( szToolTip)) != nullptr)
			{
				LVITEM_DATA * pData = pLVBox->getData( nItemIdx);

				pData->setToolTip_utf8( nCellIdx, szToolTip);
			}
		}
		return 0;
	}

	int i3UILV_SetUserCallback( LuaState * L)
	{
		i3UIListView_Box * pLVBox = (i3UIListView_Box*) i3Lua::GetUserDataArg( L, 1);

		if( pLVBox != nullptr)
		{
			I3UI_USER_FILTER_CALLBACK pFunc = (I3UI_USER_FILTER_CALLBACK) i3Lua::GetUserDataArg( L, 2);
			pLVBox->setUserFilterFunc(pFunc);
		}

		return 0;
	}
}


LuaFuncReg i3UIListView_Glue[] = 
{
	{"uiLV_NewItem",			i3UILV_NewItem				},
	{"uiLV_AddItem",			i3UILV_AddItem				},
	{"uiLV_InsertItem",			i3UILV_InsertItem			},
	{"uiLV_DeleteItem",			i3UILV_DeleteItem			},
	{"uiLV_DeleteAllItems",		i3UILV_DeleteAllItems		},
	{"uiLV_SetItemCell",		i3UILV_SetItemCell			},
	{"uiLV_GetCellControl",		i3UILV_GetCellControl		},
	{"uiLV_GetHeaderControl",	i3UILV_GetHeaderControl		},
	{"uiLV_SetCell",			i3UILV_SetCell				},
	{"uiLV_SetCells",			i3UILV_SetCells				},
	{"uiLV_SetHeader",			i3UILV_SetHeader			},
	
	{"uiLV_SetCellDataTable",	i3UILV_SetCellDataTable		},
	{"uiLV_SetCellDCStyle",		i3UILV_SetCellDCStyle		},
	{"uiLV_SetCellDCPos",		i3UILV_SetCellDCPos			},

	{"uiLV_GetData",			i3UILV_GetData				},
	{"uiLV_SetData",			i3UILV_SetData				},

	{"uiLV_GetCellCount",		i3UILV_GetCellCount			},
	
	{"uiLV_GetDataCount",		i3UILV_GetDataCount			},
	{"uiLV_GetItemCount",		i3UILV_GetItemCount			},
	{"uiLV_GetItem",			i3UILV_GetItem				},
	
	{"uiLV_UpdateToPreviewMode",	i3UILV_OnPreviewMode	},
	{"uiLV_UpdateToEditMode",		i3UILV_OnEditMode	},

	{"uiLV_SetComboItems",		i3UILV_SetComboItems	},
	{"uiLV_SetButtonName",		i3UILV_SetButtonName	},

	{"uiLV_SetTextAlign",		i3UILV_SetTextAlign		},
	{"uiLV_SetTextColor",		i3UILV_SetTextColor		},
	{"uiLV_SetTextColor4Byte",  i3UILV_SetTextColor4Byte },
	{"uiLV_RestoreTextColor",	i3UILV_RestoreTextColor },

	{"uiLV_SetTextFont",		i3UILV_SetTextFont		},
	{"uiLV_GetCurSel",			i3UILV_GetCurSel		},
	{"uiLV_SetCurSel",			i3UILV_SetCurSel		},
	{"uiLV_UnsetCurSel",		i3UILV_UnsetCurSel      },
	{"uiLV_ScrollToIndex",		i3UILV_ScrollToIndex		},

	{"uiLV_SetChildRect",		i3UILV_SetChildRect	},

	{"uiLV_GetInstancePointer",		i3UILV_GetInstancePointer	},

	{"uiLV_SetTemplateName",		i3UILV_SetTemplateName	},
	{"uiLV_SetTemplateResource",	i3UILV_SetTemplateResource	},			//Cell에 여러개의 탬플릿을 등록

	{"uiLV_GetIsMouseOver",			i3UILV_IsMouseOver			},
	{"uiLV_SetMouseOver",			i3UILV_SetMouseOver			},
	{"uiLV_ResetButtonState",		i3UILV_ResetButtonState		},

	{"uiLV_GetShapeValueIdx",		i3UILV_GetShapeValueIdx		},
	{"uiLV_GetShapeValueString",	i3UILV_GetShapeValueString	},

	{"uiLV_SetDataEnable",			i3UILV_SetDataEnable	},

	{"uiLV_SetCellCallback",		i3UILV_SetCellCallback	},		// Cell의 Callback을 등록한다. (SetCell, SetData가 호출될시)
	{"uiLV_SetToolTip",				i3UILV_SetToolTip		},		// Data에 ToolTip을 설정합니다.
	{"uiLV_SetUserCallback",		i3UILV_SetUserCallback	},	

	{nullptr, nullptr}
};

void i3UIListView_Box::RegisterLuaFunction(LuaState * pState)
{
	if( pState == nullptr)
		return;

	for(int i=0; i3UIListView_Glue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UIListView_Glue[i].name, i3UIListView_Glue[i].func);
	}
}


