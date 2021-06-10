#pragma once
// CvUnitComponents.h
#ifndef UNIT_COMPONENTS_H
#define UNIT_COMPONENTS_H
//------------------------------------------------------------------------------------------------------
//
//  CLASS: UnitCompCommander
//
//------------------------------------------------------------------------------------------------------
class UnitCompCommander
{
public:
	UnitCompCommander();
	~UnitCompCommander();

	void changeControlPoints(const int iChange);
	void changeControlPointsLeft(const int iChange);
	inline short getControlPoints() const { return m_iControlPoints; }
	inline short getControlPointsLeft() const { return m_iControlPointsLeft; }
	void restoreControlPoints();

	void changeCommandRange(const int iChange);
	inline short getCommandRange() const { return m_iCommandRange; }

private:
	short m_iControlPoints;
	short m_iControlPointsLeft;
	short m_iCommandRange;
};
//------------------------------------------------------------------------------------------------------
//
//  CLASS: X
//
//------------------------------------------------------------------------------------------------------

#endif