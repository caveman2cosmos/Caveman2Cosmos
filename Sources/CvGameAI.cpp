// gameAI.cpp

#include "CvGameCoreDLL.h"

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


int CvGameAI::AI_combatValue(UnitTypes eUnit)
{
	int iValue;

	iValue = 100;

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

		// UncutDragon
		// original
		//iValue *= ((((GC.getUnitInfo(eUnit).getFirstStrikes() * 2) + GC.getUnitInfo(eUnit).getChanceFirstStrikes()) * (GC.getDefineINT("COMBAT_DAMAGE") / 5)) + 100);
		// modified
			iValue *= ((((GC.getUnitInfo(eUnit).getFirstStrikes() * 2) + GC.getUnitInfo(eUnit).getChanceFirstStrikes()) * (GC.getCOMBAT_DAMAGE() / 5)) + 100);
		// /UncutDragon
		iValue /= 100;
	}
	if (GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		int iOffset = GC.getUnitInfo(eUnit).getSMRankTotal() - 15;
		int iSMMultiplier = GC.getDefineINT("SIZE_MATTERS_MOST_MULTIPLIER");
		bool bPositive = ((iOffset > 0) ? true : false);
		iValue *= 100;
		if (bPositive)
		{
			for (int iI = 0; iI < iOffset; iI++)
			{
				iValue *= iSMMultiplier;
				iValue /= 100;
			}
		}
		else if (!bPositive)
		{
			for (int iI = 0; iI < -iOffset; iI++)
			{
				iValue *= 100;
				iValue /= iSMMultiplier;
			}
		}
		iValue /= 100;
	}


	iValue /= getBestLandUnitCombat();

	return iValue;
}


int CvGameAI::AI_turnsPercent(int iTurns, int iPercent)
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

	uint uiFlag=0;
	pStream->Read(&uiFlag);	// flags for expansion

	pStream->Read(&m_iPad);
}


void CvGameAI::write(FDataStreamBase* pStream)
{
	CvGame::write(pStream);

	uint uiFlag=0;
	pStream->Write(uiFlag);		// flag for expansion

	pStream->Write(m_iPad);
}

// Protected Functions...

// Private Functions...
