#include "i3SceneDef.h"
#include "i3IKBoneInfo.h"

bool i3IKBoneInfo::SaveToXML( const char * pszField, i3XMLElement * pXML)
{
	i3XMLElement * pNewChild = new i3XMLElement( pszField);

	pNewChild->addAttr( "IKFlag", (INT32)m_BoneFlag);
	pNewChild->addAttr( "IKBoneIndex", m_BoneIndex);
	pNewChild->addAttr( "IKJointCount", m_iIKJointCount);
	
	pNewChild->addAttr( "Swing1_Low", m_rSwing1_Low);
	pNewChild->addAttr( "Swing1_High", m_rSwing1_High);
	pNewChild->addAttr( "Swing1_Tension", m_rSwing1Tension);

	pNewChild->addAttr( "Swing2_Low", m_rSwing2_Low);
	pNewChild->addAttr( "Swing2_High", m_rSwing2_High);
	pNewChild->addAttr( "Swing2_Tension", m_rSwing2Tension);

	pNewChild->addAttr( "Twist_Low", m_rTwist_Low);
	pNewChild->addAttr( "Twist_High", m_rTwist_High);
	pNewChild->addAttr( "Twist_Tension", m_rTwistTension);

	pNewChild->addAttr( "Limit", m_rRotateLimit);

	pNewChild->addAttr( "PickedBoneIndex", m_PickedBoneIndex);

	pXML->LinkEndChild( pNewChild);

	return true;
}

bool i3IKBoneInfo::LoadFromXML( const char * pszField, i3XMLElement * pXML)
{
	i3XMLElement * pChild = (i3XMLElement*) pXML->FirstChild( pszField);

	if( pChild != NULL)
	{
		pChild->getAttr( "IKFlag", (INT32*) &m_BoneFlag);
		pChild->getAttr( "IKBoneIndex", &m_BoneIndex);
		pChild->getAttr( "IKJointCount", &m_iIKJointCount);
		
		pChild->getAttr( "Swing1_Low", &m_rSwing1_Low);
		pChild->getAttr( "Swing1_High", &m_rSwing1_High);
		pChild->getAttr( "Swing1_Tension", &m_rSwing1Tension);

		pChild->getAttr( "Swing2_Low", &m_rSwing2_Low);
		pChild->getAttr( "Swing2_High", &m_rSwing2_High);
		pChild->getAttr( "Swing2_Tension", &m_rSwing2Tension);

		pChild->getAttr( "Twist_Low", &m_rTwist_Low);
		pChild->getAttr( "Twist_High", &m_rTwist_High);
		pChild->getAttr( "Twist_Tension", &m_rTwistTension);

		pChild->getAttr( "Limit", &m_rRotateLimit);
		pChild->getAttr( "PickedBoneIndex", &m_PickedBoneIndex);
	}

	return true;
}
