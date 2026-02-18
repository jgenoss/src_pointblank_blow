#include "i3GfxType.h"
#include "i3PostProcess.h"
#include "i3PostProcessManager.h"

I3_CLASS_INSTANCE( i3PostProcess);


bool i3PostProcess::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{	
	bool bResult = false;

	bResult = i3GfxResource::OnSaveXML(pFile, pXML);

	if(bResult == false)
		return false;

	pXML->addAttr("IsEnabled", (INT32) m_bIsEnable ? 1 : 0);

	return true;
}

bool i3PostProcess::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3GfxResource::OnLoadXML(pFile, pXML);

	if(bResult == false)
		return false;

	INT32 val;
	pXML->getAttr("IsEnabled", &val);
	
	m_bIsEnable = (val == 1);

	return true;
}