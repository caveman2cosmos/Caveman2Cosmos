#pragma once
// CvUnitComponents.h
#ifndef UNIT_COMPONENTS_H
#define UNIT_COMPONENTS_H

//------------------------------------------------------------------------------------------------------
//  CLASS: UnitCompCommander
//------------------------------------------------------------------------------------------------------
class UnitCompCommander
{
public:
	UnitCompCommander();
	~UnitCompCommander();
	UnitCompCommander(CvUnitInfo* unitInfo);

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
//  CLASS: UnitCompWorker
//------------------------------------------------------------------------------------------------------
class UnitCompWorker
{
public:
	UnitCompWorker();
	~UnitCompWorker();
	UnitCompWorker(CvUnitInfo* unitInfo);

	void changeWorkModifier(const int iChange);
	void changeHillsWorkModifier(const int iChange);
	void changePeaksWorkModifier(const int iChange);
	inline short getWorkModifier() const { return m_iWorkModifier; }
	inline short getHillsWorkModifier() const { return m_iHillsWorkModifier; }
	inline short getPeaksWorkModifier() const { return m_iPeaksWorkModifier; }

private:
	short m_iWorkModifier;
	short m_iHillsWorkModifier;
	short m_iPeaksWorkModifier;
};
//------------------------------------------------------------------------------------------------------
//  CLASS: X
//------------------------------------------------------------------------------------------------------

#endif