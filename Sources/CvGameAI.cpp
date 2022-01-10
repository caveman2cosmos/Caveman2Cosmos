// gameAI.cpp

#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"

// Public Functions...

CvGameAI::CvGameAI()
{
	AI_reset();
}


CvGameAI::~CvGameAI()
{
	AI_uninit();
}


void CvGameAI::AI_init()
{
	AI_reset();

	//--------------------------------
	// Init other game data
}


void CvGameAI::AI_uninit()
{
}


void CvGameAI::AI_reset()
{
	AI_uninit();

	m_iPad = 0;

	CvUnitAI::AI_clearCaches();
}


void CvGameAI::AI_makeAssignWorkDirty()
{
	int iI;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			GET_PLAYER((PlayerTypes)iI).AI_makeAssignWorkDirty();
		}
	}
}


void CvGameAI::AI_updateAssignWork()
{
	PROFILE_FUNC();

	int iI;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (GET_TEAM(kLoopPlayer.getTeam()).isHuman() && kLoopPlayer.isAlive())
		{
			kLoopPlayer.AI_updateAssignWork();
		}
	}
}


int CvGameAI::AI_combatValue(const UnitTypes eUnit) const
{
	int iValue = 100;

	if (GC.getUnitInfo(eUnit).getDomainType() == DOMAIN_AIR)
	{
		iValue *= GC.getUnitInfo(eUnit).getAirCombat();
	}
	else
	{
		iValue *= GC.getUnitInfo(eUnit).getCombat();
		//TB Combat Mods Begin
		iValue += (((100 + GC.getUnitInfo(eUnit).getArmor())/100)/5);
		iValue += (((100 + GC.getUnitInfo(eUnit).getPuncture())/100)/5);
		iValue += (((100 + GC.getUnitInfo(eUnit).getPrecisionModifier())/100)/5);
		iValue += (((100 + GC.getUnitInfo(eUnit).getDodgeModifier())/100)/5);
		iValue += (((100 + GC.getUnitInfo(eUnit).getDamageModifier())/100)/5);
		//TB Combat Mods End

		iValue *= ((((GC.getUnitInfo(eUnit).getFirstStrikes() * 2) + GC.getUnitInfo(eUnit).getChanceFirstStrikes()) * (GC.getCOMBAT_DAMAGE() / 5)) + 100);
		iValue /= 100;
	}
	if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		iValue = CvUnit::applySMRank(iValue, GC.getUnitInfo(eUnit).getSMRankTotal() - 15, GC.getSIZE_MATTERS_MOST_MULTIPLIER());
	}

	iValue /= getBestLandUnitCombat();

	return iValue;
}


int CvGameAI::AI_turnsPercent(int iTurns, const int iPercent) const
{
	FAssert(iPercent > 0);
	if (iTurns != MAX_INT)
	{
		iTurns *= (iPercent);
		iTurns /= 100;
	}

	return std::max(1, iTurns);
}


void CvGameAI::read(FDataStreamBase* pStream)
{
	CvGame::read(pStream);

	pStream->Read(&m_iPad);
}


void CvGameAI::write(FDataStreamBase* pStream)
{
	CvGame::write(pStream);

	pStream->Write(m_iPad);
}

// Protected Functions...

// Private Functions...
