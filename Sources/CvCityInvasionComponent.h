#pragma once

// @SAVEBREAK - flabbert - 16.07.2022
// finish this implementation in next savebreak
// we want to add this to city as composable component and remove existing variables for invasion
// it has been removed from city, because it currently is not in use (AI doesnt understand it at all)
// SAVEBREAK@

#ifndef CvCityInvasionComponent_h__
#define CvCityInvasionComponent_h__

#include "CvUnit.h"
#include "CvPlot.h"
#include "CvTeamAI.h"
#include "CvPlayerAI.h"
#include "CvPlayer.h"
#include "CvGameAI.h"
#include "CvPopupInfo.h"
#include "CvDLLInterfaceIFaceBase.h"

class CvCityInvasionComponent
{
public:
	CvCityInvasionComponent(CvCity* pCity) {
		thisCity = pCity;
	}
	void reset() {
		m_iInvasionChance = 0;
		m_iInvasionTimer = 0;
	}
	int getInvasionChance() const
	{
		return m_iInvasionChance;
	}
	void changeInvasionChance(int iChange)
	{
		m_iInvasionChance += iChange;
	}
	int getInvasionTimer() const
	{
		return m_iInvasionTimer;
	}
	void changeInvasionTimer(int iChange)
	{
		m_iInvasionTimer += iChange;
	}
	bool isInvaded() const
	{
		return m_iInvasionTimer > 0;
	}
	void doInvasion(CvPlayerAI* pPlayer)
	{
		PROFILE_FUNC();

		PlayerTypes ePlayer = NO_PLAYER;
		if (!isInvaded())
		{
			bool bTestInvasion = false;
			if (getInvasionChance() > 0)
			{
				foreach_(const CvPlot* adjacentPlot, thisCity->plot()->adjacent())
				{
					bst::optional<CvUnit*> unit = algo::find_if(adjacentPlot->units(), bind(unitCouldInvade, _1, thisCity->getTeam()));
					if (unit)
					{
						bTestInvasion = true;
						ePlayer = (*unit)->getOwner();
						break;
					}
				}
			}
			if (bTestInvasion)
			{
				if (GC.getGame().getSorenRandNum(100, "Enemy Invades City Chance") < getInvasionChance())
				{
					int iTurns = getInvasionChance() / 2;
					changeInvasionTimer(iTurns);
					//Alert the Player
					if (pPlayer->isHuman())
					{
						CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_INVASION);
						pInfo->setData1(thisCity->getID());
						gDLL->getInterfaceIFace()->addPopup(pInfo, thisCity->getOwner());
					}
					//Alert the invader
					if (GET_PLAYER(ePlayer).isHuman())
					{
						CvWString szBuffer = gDLL->getText("TXT_KEY_INVASION_SUCCESSFUL", thisCity->getNameKey());
						AddDLLMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BUILD_BARRACKS", MESSAGE_TYPE_INFO, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), GC.getCOLOR_GREEN(), getX(), getY(), true, true);
					}
				}
			}
		}
		else
		{
			changeInvasionTimer(-1);
			if (!isInvaded())
			{

				CvWString szBuffer = gDLL->getText("TXT_KEY_INVASION_ENDED", thisCity->getNameKey());
				AddDLLMessage(thisCity->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BUILD_BARRACKS", MESSAGE_TYPE_INFO, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), GC.getCOLOR_GREEN(), getX(), getY(), true, true);
			}
		}
	}
	bool unitCouldInvade(const CvUnit* unit, const TeamTypes& testTeam)
	{
		return GET_TEAM(unit->getTeam()).isAtWar(testTeam) && !unit->isAnimal() && !unit->isOnlyDefensive() && !unit->canAttackOnlyCities();
	}
private:
	CvCity* thisCity = NULL;
	int m_iInvasionChance = 0;
	int m_iInvasionTimer = 0;
};

#endif
