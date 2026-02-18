#pragma once

// AI전에서만 사용하는 어시스터 정보 보관 Context
// 원래 서버에서 어시스트 정보를 보관하지만 현재 AI전 데미지 계산을 따로 해주지 않으므로
// AI전을 서버로 이전하기 전까지 임시로 사용합니다.
// 차후 AI전이 서버로 이전하면 제거해야함...

class AssistContext : public i3::shared_ginst<AssistContext>
{
public:
	AssistContext();

	// AI전에서 방장이 어시스트 판별하여 전달함
	void				SetAssistor(INT32 idx, INT32 assistor);		// Hit할 때 마지막 타격자를 어시스터로 저장 
	INT32				GetAssistor(INT32 idx, INT32 killer);		// Kill이 발생했을 경우 어시스터를 얻어옴

	void				AllResetAssistor();
	void				UpdateAssistorInfo();

private:
	INT32				m_Assistor[16][2];				// [initialize at constructor]
};
