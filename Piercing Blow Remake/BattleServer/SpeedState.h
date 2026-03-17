#ifndef __SPEEDSTATE_H__
#define __SPEEDSTATE_H__

#pragma once

// ============================================================================
// SpeedState - Speed hack detection via averaging
// Port of original CSpeedState from Dedication/Dedi/SpeedState.h
// ============================================================================

#define MAX_SPEED_SAMPLES	5

class SpeedState
{
public:
	SpeedState();
	~SpeedState();

	void	Reset();

	// Record a new speed sample (units per second)
	void	InsertSpeed(float fSpeed);

	// Check if speed hack detected, returns avg speed via out param
	bool	IsSpeedHack(float* pfAvgSpeed) const;

	// Get current average speed
	float	GetAverageSpeed() const;

	// Get sample count
	int		GetSampleCount() const { return (int)m_ui32Count; }

private:
	float		m_afSpeedData[MAX_SPEED_SAMPLES];
	uint32_t	m_ui32Count;
};

#endif // __SPEEDSTATE_H__
