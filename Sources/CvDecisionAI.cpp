#include "CvGameCoreDLL.h"
#include "CvDecisionAI.h"

#include "BetterBTSAI.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvPlayerAI.h"

// ---------------------------------------------------------------------------
// Lifecycle.
// ---------------------------------------------------------------------------
CvDecisionAI::CvDecisionAI(PlayerTypes owner)
	: m_owner(owner)
	, m_lastTurn(-1)
{
}

void CvDecisionAI::onTurnBegin(int gameTurn)
{
	m_lastTurn = gameTurn;

	if (gPlayerLogLevel < 1 || m_owner == NO_PLAYER)
	{
		return;
	}

	const CvPlayerAI& kPlayer = GET_PLAYER(m_owner);

	// Baseline is only meaningful for AI players -- the decision functions all
	// early-out for humans / NPCs, so logging their flavours would be noise.
	if (kPlayer.isHumanPlayer() || kPlayer.isNPC() || !kPlayer.isAlive())
	{
		return;
	}

	logDecisionAI(1, "[DAI/begin] player=%d (%S) turn=%d era=%d",
		(int)m_owner, kPlayer.getCivilizationDescription(0), gameTurn, (int)kPlayer.getCurrentEra());

	for (int iI = 0; iI < GC.getNumFlavorTypes(); iI++)
	{
		logDecisionAI(1, "[DAI/flavors] player=%d flavor=%s value=%d",
			(int)m_owner, GC.getFlavorTypes((FlavorTypes)iI).c_str(),
			kPlayer.AI_getFlavorValue((FlavorTypes)iI));
	}
}
