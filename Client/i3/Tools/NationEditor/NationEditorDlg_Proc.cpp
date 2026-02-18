
// NationEditorDlg.cpp : 구현 파일
//

#include "stdafx.h"

#include "NationEditorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



/*************************//*************************/
//단일 Spread

static void _Spread_(i3RegKey * pKey, size_t Index1, size_t Index2 , Spread::Proc Proc);
static void _Spread2_(i3RegKey * pKey, void* pData, Spread::Proc2 Proc);
static void _Spread3_(i3RegKey * pKey, void* pData, size_t Index1, size_t Index2, Spread::Proc3 Proc);

static void _AddSpreadProc(i3RegKey * pKey, size_t Index1, size_t Index2);	//Index1(From)
static void _RemoveSpreadProc(i3RegKey * pKey, size_t Index1, size_t Index2);	//Index1(Dest)
static void _CloneSpreadProc(i3RegKey * pKey, void* pData, size_t Index1, size_t Index2);	//Index1(From)

static void _SwapSpreadProc(i3RegKey * pKey, void* pData);	//Index1(From)


/*************************//*************************/
void Spread::AddReg(i3RegKey * pKey, size_t Index1, size_t Index2)
{
	_Spread_(pKey, Index1, Index2, _AddSpreadProc);
}

void Spread::RemoveReg(i3RegKey * pKey, size_t Index1, size_t Index2)
{
	_Spread_(pKey, Index1, Index2, _RemoveSpreadProc);
}

void Spread::SwapReg(i3RegKey * pKey, void* pData)
{
	_Spread2_(pKey, pData, _SwapSpreadProc);
}

void Spread::CloneReg(i3RegKey * pKey, void* pData, size_t Index1, size_t Index2)
{
	_Spread3_(pKey, pData, Index1, Index2, _CloneSpreadProc);
}




/*************************//*************************/
static void _Spread_(i3RegKey * pKey, size_t Index1, size_t Index2 , Spread::Proc Proc)
{
	(*Proc)(pKey, Index1, Index2);

	for(INT32 i=0; i<pKey->getChildCount(); i++)
	{
		_Spread_( (i3RegKey*)pKey->getChild(i), Index1, Index2, Proc);
	}
}

static void _Spread2_(i3RegKey * pKey, void* pData, Spread::Proc2 Proc)
{
	(*Proc)(pKey, pData);

	for(INT32 i=0; i<pKey->getChildCount(); i++)
	{
		_Spread2_( (i3RegKey*)pKey->getChild(i), pData, Proc);
	}
}

static void _Spread3_(i3RegKey * pKey, void* pData, size_t Index1, size_t Index2, Spread::Proc3 Proc)
{
	(*Proc)(pKey, pData, Index1, Index2);

	for(INT32 i=0; i<pKey->getChildCount(); i++)
	{
		_Spread3_( (i3RegKey*)pKey->getChild(i), pData, Index1, Index2, Proc);
	}
}


/*************************//*************************/
void _AddSpreadProc(i3RegKey * pKey, size_t Index1, size_t Index2)
{
	if( pKey != NULL)
	{
		INT32	nItemCount = pKey->GetDataCount();
		for( INT32 i = 0; i < nItemCount; ++i)
		{
			i3RegData* pReg = (i3RegData*)pKey->GetData(i);
			if( pReg->getDataType() != I3REG_TYPE_ARRAY )
				continue;

			i3RegArray* pArrayReg = (i3RegArray*)pReg;
			pArrayReg->addRegData( pArrayReg->createRegData() );

			Index2 = pArrayReg->getSize() -1;

			switch( pArrayReg->getElementType() )
			{
			case I3REG_TYPE_INT32:	
				{
					INT32 val;
					//_Blank_ 일 경우
					if( Index1 == (size_t)-1 )
						val = 0;
					else
						(*pArrayReg)[Index1]->GetValue( &val );
					 pArrayReg->replaceRegData( Index2, val );
				}
				break;
			case I3REG_TYPE_REAL32:	
				{
					REAL32 val;
					if( Index1 == (size_t)-1 )
						val = 0.0f;
					else
						(*pArrayReg)[Index1]->GetValue( &val );
					 pArrayReg->replaceRegData( Index2, val );
				}
				break;
			case I3REG_TYPE_STRING:	
				{
					i3::rc_wstring	wstr;
					if( Index1 != (size_t)-1 )
						(*pArrayReg)[Index1]->GetValue( wstr );
					 pArrayReg->replaceRegData( Index2, wstr );
				}
				break;
			case I3REG_TYPE_VEC2D:	
				{
					VEC2D val;
					if( Index1 == (size_t)-1 )
					{
						val.x = 0.0f;
						val.y = 0.0f;
					}
					else
						(*pArrayReg)[Index1]->GetValue( &val );
					 pArrayReg->replaceRegData( Index2, val );
				}
				break;
			case I3REG_TYPE_VEC3D:
				{
					VEC3D val;
					if( Index1 == (size_t)-1 )
					{
						val.x = 0.0f;
						val.y = 0.0f;
						val.z = 0.0f;
					}
					else
						(*pArrayReg)[Index1]->GetValue( &val );
					 pArrayReg->replaceRegData( Index2, val );
				}
				break;
			case I3REG_TYPE_VEC4D:
				{
					VEC4D val;
					if( Index1 == (size_t)-1 )
					{
						val.x = 0.0f;
						val.y = 0.0f;
						val.z = 0.0f;
						val.w = 0.0f;
					}
					else
						(*pArrayReg)[Index1]->GetValue( &val );
					 pArrayReg->replaceRegData( Index2, val );
				}
				break;
			case I3REG_TYPE_COLOR:
				{
					I3COLOR val;

					if( Index1 == (size_t)-1 )
					{
						val.r = 0;
						val.g = 0;
						val.b = 0;
						val.a = 0;
					}
					else
						(*pArrayReg)[Index1]->GetValue( &val );
					 pArrayReg->replaceRegData( Index2, val );
				}
				break;
			case I3REG_TYPE_MATRIX:	
				{
				}
				break;
			}
		}
	}
}

void _RemoveSpreadProc(i3RegKey * pKey, size_t Index1, size_t Index2)
{
	if( pKey != NULL)
	{
		INT32	nItemCount = pKey->GetDataCount();

		TRACE("%s\n", pKey->GetName() );

		for( INT32 i = 0; i < nItemCount; ++i)
		{
			i3RegData* pReg = (i3RegData*)pKey->GetData(i);
			if( pReg->getDataType() != I3REG_TYPE_ARRAY )
				continue;

			i3RegArray* pArrayReg = (i3RegArray*)pReg;

			if( pArrayReg->getSize() == Index2 )
			{
				pArrayReg->removeRegData(Index1);
			}
			else
			{
				TRACE("%s\n", pReg->GetName() );
			}
		}
	}
}


/*************************//*************************/
void _SwapSpreadProc(i3RegKey * pKey, void* pData)
{
	const OrderList *pList = (OrderList*)pData;
	size_t size = pList->size();
	if( pKey != NULL)
	{
		INT32	nItemCount = pKey->GetDataCount();

		TRACE1("%s\n", pKey->GetName() );

		for( INT32 i = 0; i < nItemCount; ++i)
		{
			i3RegArray* pArrayReg = (i3RegArray*)pKey->GetData(i);

			//Array Registry가 아니면 패스
			if( pArrayReg->getDataType() != I3REG_TYPE_ARRAY )
				continue;

			std::vector<i3RegData* >NewDataList( pArrayReg->getSize() );
			std::vector<i3RegData* >* pDataList = pArrayReg->getValue();

			for(size_t j = 0; j < size; ++j)
			{
				NewDataList[j] = (*pDataList)[ (*pList)[j] ];
			}

			pArrayReg->setValue( NewDataList );
		}
	}
}


/*************************//*************************/
void _CloneSpreadProc(i3RegKey * pKey, void* pData, size_t Index1, size_t Index2)
{
	if( pKey != NULL)
	{
		INT32	nItemCount = pKey->GetDataCount();

		StringVector* pStringVector = (StringVector*)pData;


		for( INT32 i = 0; i < nItemCount; ++i)
		{
			i3RegData* pReg = (i3RegData*)pKey->GetData(i);
			if( pReg->getDataType() != I3REG_TYPE_ARRAY )
				continue;

			i3RegArray* pArrayReg = (i3RegArray*)pReg;

			bool matched = false;
			size_t size = pStringVector->size();
			for(size_t j = 0; j < size; ++j)
			{
				if( (*pStringVector)[j] == pArrayReg->GetName() )
				{
					matched = true;
					break;
				}
			}
			
			if( matched == false )
				continue;

			switch( pArrayReg->getElementType() )
			{
			case I3REG_TYPE_INT32:	
				{
					INT32 val;
					(*pArrayReg)[Index2]->GetValue( &val );
					 pArrayReg->replaceRegData( Index1, val );
				}
				break;
			case I3REG_TYPE_REAL32:	
				{
					REAL32 val;
					(*pArrayReg)[Index2]->GetValue( &val );
					 pArrayReg->replaceRegData( Index1, val );
				}
				break;
			case I3REG_TYPE_STRING:	
				{
					i3::rc_wstring wstr;
					(*pArrayReg)[Index2]->GetValue( wstr );
					 pArrayReg->replaceRegData( Index1, wstr );
				}
				break;
			case I3REG_TYPE_VEC2D:	
				{
					VEC2D val;
					(*pArrayReg)[Index2]->GetValue( &val );
					 pArrayReg->replaceRegData( Index1, val );
				}
				break;
			case I3REG_TYPE_VEC3D:
				{
					VEC3D val;
					(*pArrayReg)[Index2]->GetValue( &val );
					 pArrayReg->replaceRegData( Index1, val );
				}
				break;
			case I3REG_TYPE_VEC4D:
				{
					VEC4D val;
					(*pArrayReg)[Index2]->GetValue( &val );
					 pArrayReg->replaceRegData( Index1, val );
				}
				break;
			case I3REG_TYPE_COLOR:
				{
					I3COLOR val;
					(*pArrayReg)[Index2]->GetValue( &val );
					 pArrayReg->replaceRegData( Index1, val );
				}
				break;
			case I3REG_TYPE_MATRIX:	
				{
				}
				break;
			}
		}
	}
}
