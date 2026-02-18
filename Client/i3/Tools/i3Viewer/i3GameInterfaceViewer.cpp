#include "stdafx.h"
#include "i3GameInterfaceViewer.h"

void OmitQuatation( char * pszStr)
{
	char * pSrc, * pDest;

	pSrc = pDest = pszStr;

	while( *pSrc)
	{
		if( *pSrc != '"')
		{
			*pDest = *pSrc;
			pDest ++;
		}

		pSrc++;
	}

	*pDest = 0;
}

bool i3GameInterfaceViewer::OnInitFramework( i3Framework * pFramework, const char * pszCmdLine)
{
	if( strlen( pszCmdLine) > 0)
	{
		char szTemp[MAX_PATH];

		strcpy( szTemp, pszCmdLine);
		OmitQuatation( szTemp);

		((i3ViewerFramework *)pFramework)->LoadI3S( szTemp);
	}

	return true;
}
