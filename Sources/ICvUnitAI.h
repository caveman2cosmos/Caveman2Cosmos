#pragma once

// ICvUnitAI.h

#ifndef I_CV_UNIT_AI_H
#define I_CV_UNIT_AI_H

class ICvUnitAI
{
public:
	virtual void read(FDataStreamBase* pStream) = 0;
	virtual void write(FDataStreamBase* pStream) = 0;

	virtual void init(UnitAITypes eUnitAI, int iBirthmark) = 0;
	virtual void uninit() = 0;
	virtual void reset(UnitAITypes eUnitAI = NO_UNITAI, bool bConstructor = false) = 0;
	virtual bool update() = 0;
	virtual bool follow() = 0;
	virtual bool upgrade() = 0;
	virtual bool promote() = 0;
	virtual int groupFirstVal() const = 0;
	virtual int groupSecondVal() const = 0;
	virtual int attackOdds(const CvPlot* pPlot, bool bPotentialEnemy, CvUnit** ppDefender = NULL, bool bAssassinate = false) = 0;
	//	Variant to test a specific defender AS IF it was in the specified plot
	virtual int attackOddsAtPlot(const CvPlot* pPlot, CvUnit* pDefender, bool modifyPredictedResults = false) = 0;
	virtual bool bestCityBuild(const CvCity* pCity, CvPlot** ppBestPlot = NULL, BuildTypes* peBestBuild = NULL, const CvPlot* pIgnorePlot = NULL, const CvUnit* pUnit = NULL) = 0;
	virtual bool isCityAIType() const = 0;
	virtual UnitAITypes getUnitAIType() const = 0;
	virtual void setUnitAIType(UnitAITypes eNewValue) = 0;
	virtual int sacrificeValue(const CvPlot* pPlot) const = 0;
	virtual bool isAwaitingContract() const = 0;
	virtual bool isCityGarrison(const CvCity* pCity) const = 0;
	virtual void setAsGarrison(const CvCity* pCity) = 0;
	virtual bool load(UnitAITypes eUnitAI, MissionAITypes eMissionAI, UnitAITypes eTransportedUnitAI = NO_UNITAI, int iMinCargo = -1, int iMinCargoSpace = -1, int iMaxCargoSpace = -1, int iMaxCargoOurUnitAI = -1, int iFlags = 0, int iMaxPath = MAX_INT, int iMaxTransportPath = MAX_INT) = 0;
	virtual void flushValueCache() = 0;
	virtual int genericUnitValueTimes100(UnitValueFlags eFlags) const = 0;
	virtual int getBirthmark() const = 0;
	virtual void setToWaitOnUnitAI(UnitAITypes eUnitAI, bool bAdd) = 0;
	virtual bool isWaitingOnUnitAI(int iIndex) const = 0;
	virtual bool isWaitingOnUnitAIAny() const = 0;
	virtual bool processContracts(int iMinPriority = 0) = 0;
	virtual bool exposedToDanger(const CvPlot* pPlot, int acceptableOdds, bool bConsiderOnlyWorstThreat = false) 
		const = 0;

	//	KOSHLING - inform the AI of unit losses so that it can adjust internal counts
	virtual void killed() = 0;
	//	Set an override to the group head priority for this unit, to allow a unit that
	//	would not nomally lead a stack to do so - priority will be preserved when other groups
	//	joing one lead by a unit with an override, but reset if we join another group
	virtual void setLeaderPriority(int iPriority) = 0; //	 -1 means reset to default
	virtual int getPredictedHitPoints() const = 0;
	virtual void setPredictedHitPoints(int iPredictedHitPoints) = 0;
	virtual bool getHasAttacked() const = 0;
	virtual int beneficialPropertyValueToCity(const CvCity* pCity, PropertyTypes eProperty) const = 0;

	virtual int searchRange(int iRange = 1) const = 0;
};

#endif