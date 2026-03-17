#include "pch.h"
#include "SpeedState.h"

// Maximum allowed character speed (units/sec) with 20% tolerance
// Original: DS_CHARA_DEFAULT_FRONT_SPEED = 290, max with boost ~400
#define SPEED_HACK_THRESHOLD	450.0f

SpeedState::SpeedState()
	: m_ui32Count(0)
{
	memset(m_afSpeedData, 0, sizeof(m_afSpeedData));
}

SpeedState::~SpeedState()
{
}

void SpeedState::Reset()
{
	memset(m_afSpeedData, 0, sizeof(m_afSpeedData));
	m_ui32Count = 0;
}

void SpeedState::InsertSpeed(float fSpeed)
{
	// Circular buffer
	uint32_t idx = m_ui32Count % MAX_SPEED_SAMPLES;
	m_afSpeedData[idx] = fSpeed;
	m_ui32Count++;
}

bool SpeedState::IsSpeedHack(float* pfAvgSpeed) const
{
	float avg = GetAverageSpeed();
	if (pfAvgSpeed)
		*pfAvgSpeed = avg;

	// Need at least MAX_SPEED_SAMPLES before declaring hack
	if (m_ui32Count < MAX_SPEED_SAMPLES)
		return false;

	return (avg > SPEED_HACK_THRESHOLD);
}

float SpeedState::GetAverageSpeed() const
{
	uint32_t samples = (m_ui32Count < MAX_SPEED_SAMPLES) ? m_ui32Count : MAX_SPEED_SAMPLES;
	if (samples == 0)
		return 0.0f;

	float sum = 0.0f;
	for (uint32_t i = 0; i < samples; i++)
		sum += m_afSpeedData[i];

	return sum / (float)samples;
}
