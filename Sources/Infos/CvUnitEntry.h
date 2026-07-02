#pragma once

#ifndef CV_UNIT_ENTRY_H
#define CV_UNIT_ENTRY_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// class	: CvUnitFormationInfo
//
// \brief	: Holds information relating to the formation of sub-units within a unit
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CvUnitEntry
{
public:
	CvUnitEntry()
		: m_position()
		, m_fRadius(0)
		, m_fFacingDirection(0)
		, m_fFacingVariance(0)
	{
	}

	CvUnitEntry(const NiPoint2 &position, float radius, float facingDirection, float facingVariance)
		: m_position(position)
		, m_fRadius(radius)
		, m_fFacingDirection(facingDirection)
		, m_fFacingVariance(facingVariance)
	{
	}

	NiPoint2 m_position;
	float m_fRadius;
	float m_fFacingDirection;
	float m_fFacingVariance;
};

#endif // CV_UNIT_ENTRY_H
