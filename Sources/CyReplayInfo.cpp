#include "CvGameCoreDLL.h"
#include "CyReplayInfo.h"


CyReplayInfo::CyReplayInfo() { m_pHoF = &m_replay; }

CyReplayInfo::CyReplayInfo(CvReplayInfo* pReplayInfo) : m_pHoF(pReplayInfo) {}


void CyReplayInfo::createInfo(int iPlayer)
{
	if (m_pHoF) m_pHoF->createInfo((PlayerTypes)iPlayer);
}

int CyReplayInfo::getActivePlayer() const
{
	return m_pHoF ? m_pHoF->getActivePlayer() : -1;
}

int CyReplayInfo::getLeader(int iPlayer) const
{
	return m_pHoF ? m_pHoF->getLeader(iPlayer) : -1;
}

int CyReplayInfo::getColor(int iPlayer) const
{
	return m_pHoF ? m_pHoF->getColor(iPlayer) : -1;
}

int CyReplayInfo::getDifficulty() const
{
	return m_pHoF ? m_pHoF->getDifficulty() : -1;
}

std::wstring CyReplayInfo::getLeaderName() const
{
	return m_pHoF ? m_pHoF->getLeaderName().GetCString() : L"";
}

std::wstring CyReplayInfo::getCivDescription() const
{
	return m_pHoF ? m_pHoF->getCivDescription().GetCString() : L"";
}

std::wstring CyReplayInfo::getShortCivDescription() const
{
	return m_pHoF ? m_pHoF->getShortCivDescription().GetCString() : L"";
}

std::wstring CyReplayInfo::getCivAdjective() const
{
	return m_pHoF ? m_pHoF->getCivAdjective().GetCString() : L"";
}

std::wstring CyReplayInfo::getMapScriptName() const
{
	return m_pHoF ? m_pHoF->getMapScriptName().GetCString() : L"";
}

int CyReplayInfo::getWorldSize() const
{
	return m_pHoF ? m_pHoF->getWorldSize() : -1;
}

int CyReplayInfo::getClimate() const
{
	return m_pHoF ? m_pHoF->getClimate() : -1;
}

int CyReplayInfo::getSeaLevel() const
{
	return m_pHoF ? m_pHoF->getSeaLevel() : -1;
}

int CyReplayInfo::getEra() const
{
	return m_pHoF ? m_pHoF->getEra() : -1;
}

int CyReplayInfo::getGameSpeed() const
{
	return m_pHoF ? m_pHoF->getGameSpeed() : -1;
}

bool CyReplayInfo::isGameOption(int iOption) const
{
	return m_pHoF ? m_pHoF->isGameOption((GameOptionTypes)iOption) : false;
}

bool CyReplayInfo::isVictoryCondition(int iVictory) const
{
	return m_pHoF ? m_pHoF->isVictoryCondition((VictoryTypes)iVictory) : false;
}

int CyReplayInfo::getVictoryType() const
{
	return m_pHoF ? m_pHoF->getVictoryType() : -1;
}

bool CyReplayInfo::isMultiplayer() const
{
	return m_pHoF ? m_pHoF->isMultiplayer() : false;
}

int CyReplayInfo::getInitialTurn() const
{
	return m_pHoF ? m_pHoF->getInitialTurn() : -1;
}

int CyReplayInfo::getStartYear() const
{
	return m_pHoF ? m_pHoF->getStartYear() : -1;
}

int CyReplayInfo::getFinalTurn() const
{
	return m_pHoF ? m_pHoF->getFinalTurn() : -1;
}

const std::wstring CyReplayInfo::getFinalDate() const
{
	return m_pHoF ? m_pHoF->getFinalDate() : L"";
}

int CyReplayInfo::getCalendar() const
{
	return m_pHoF ? m_pHoF->getCalendar() : -1;
}

int64_t CyReplayInfo::getFinalScore() const
{
	return m_pHoF ? m_pHoF->getFinalScore() : -1;
}

int64_t CyReplayInfo::getFinalEconomy() const
{
	return m_pHoF ? m_pHoF->getFinalEconomy() : -1;
}

int64_t CyReplayInfo::getFinalIndustry() const
{
	return m_pHoF ? m_pHoF->getFinalIndustry() : -1;
}

int64_t CyReplayInfo::getFinalAgriculture() const
{
	return m_pHoF ? m_pHoF->getFinalAgriculture() : -1;
}

int CyReplayInfo::getNormalizedScore() const
{
	return m_pHoF ? m_pHoF->getNormalizedScore() : -1;
}

int CyReplayInfo::getMapWidth() const
{
	return m_pHoF ? m_pHoF->getMapWidth() : -1;
}

int CyReplayInfo::getMapHeight() const
{
	return m_pHoF ? m_pHoF->getMapHeight() : -1;
}

int CyReplayInfo::getNumPlayers() const
{
	return m_pHoF ? m_pHoF->getNumPlayers() : -1;
}

int64_t CyReplayInfo::getPlayerScore(int iPlayer, int iTurn) const
{
	return m_pHoF ? m_pHoF->getPlayerScore(iPlayer, iTurn) : -1;
}

int64_t CyReplayInfo::getPlayerEconomy(int iPlayer, int iTurn) const
{
	return m_pHoF ? m_pHoF->getPlayerEconomy(iPlayer, iTurn) : -1;
}

int64_t CyReplayInfo::getPlayerIndustry(int iPlayer, int iTurn) const
{
	return m_pHoF ? m_pHoF->getPlayerIndustry(iPlayer, iTurn) : -1;
}

int64_t CyReplayInfo::getPlayerAgriculture(int iPlayer, int iTurn) const
{
	return m_pHoF ? m_pHoF->getPlayerAgriculture(iPlayer, iTurn) : -1;
}

int CyReplayInfo::getReplayMessageTurn(int i) const
{
	return m_pHoF ? m_pHoF->getReplayMessageTurn((uint)i) : -1;
}

int CyReplayInfo::getReplayMessageType(int i) const
{
	return m_pHoF ? m_pHoF->getReplayMessageType((uint)i) : -1;
}

int CyReplayInfo::getReplayMessagePlotX(int i) const
{
	return m_pHoF ? m_pHoF->getReplayMessagePlotX((uint)i) : -1;
}

int CyReplayInfo::getReplayMessagePlotY(int i) const
{
	return m_pHoF ? m_pHoF->getReplayMessagePlotY((uint)i) : -1;
}

int CyReplayInfo::getReplayMessagePlayer(int i) const
{
	return m_pHoF ? m_pHoF->getReplayMessagePlayer((uint)i) : -1;
}

const std::wstring CyReplayInfo::getReplayMessageText(int i) const
{
	std::wstring szText;
	if (m_pHoF)
	{
		szText = m_pHoF->getReplayMessageText((uint)i);
	}
	return szText;
}

int CyReplayInfo::getNumReplayMessages() const
{
	return m_pHoF ? m_pHoF->getNumReplayMessages() : -1;
}

int CyReplayInfo::getReplayMessageColor(int i) const
{
	return m_pHoF ? m_pHoF->getReplayMessageColor((uint)i) : -1;
}

const char* CyReplayInfo::getModName() const
{
	return m_pHoF ? m_pHoF->getModName() : "";
}
