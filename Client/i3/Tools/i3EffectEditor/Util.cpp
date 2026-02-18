#include "stdafx.h"

void	REAL2STR( REAL32 val, char * pszStr)
{
	char * pszTemp;

	sprintf( pszStr, "%f", val);

	pszTemp = pszStr;
	while( *pszTemp)
		pszTemp++;

	do
	{
		pszTemp--;

		if( *pszTemp == '0')
			*pszTemp = 0;

	} while( *pszTemp == 0);

	if( *pszTemp == '.')
		*pszTemp = 0;
}

i3KeyframeTable *	CreateKeyframeTable(INT32 count)
{
	i3KeyframeTable * pNew;
	INT32 i;
	REAL32 * pTable;

	pNew = i3KeyframeTable::new_object_ref();

	pNew->Create( count, TRUE);

	pTable = pNew->GetTable();

	for( i = 0; i < count; i++)
	{
		pTable[i] = 0.5f;
	}

	return pNew;
}

i3KeyframeTable *	CreateColorKeyframeTable( INT32 count)
{
	i3KeyframeTable * pNew;
	INT32 i;
	REAL32 * pTable;
	REAL32 Accm, step;

	pNew = i3KeyframeTable::new_object_ref();

	pNew->Create( count, FALSE);

	pTable = pNew->GetTable();

	Accm = 255.0f;
	step = 255.0f / (count - 1);

	for( i = 0; i < count; i++)
	{
		pTable[i] = Accm;
		pNew->SetTime( i, (REAL32) i / (count - 1));

		Accm -= step;
	}

	return pNew;
}