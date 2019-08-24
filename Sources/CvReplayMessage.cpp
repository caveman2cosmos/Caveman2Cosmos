#include "CvGameCoreDLL.h"
#include "CvReplaymessage.h"

CvReplayMessage::CvReplayMessage(int iTurn, ReplayMessageTypes eType, PlayerTypes ePlayer) :
	m_iTurn(iTurn),
	m_ePlayer(ePlayer),
	m_eType(eType),
	m_iPlotX(-1),
	m_iPlotY(-1),
	m_eColor(NO_COLOR)
{
}

CvReplayMessage::~CvReplayMessage()
{
}

const CvReplayMessage& CvReplayMessage::operator=(const CvReplayMessage& other)
{
	m_iTurn = other.m_iTurn;
	m_eType = other.m_eType;
	m_iPlotX = other.m_iPlotX;
	m_iPlotY = other.m_iPlotY;
	m_ePlayer = other.m_ePlayer;
	m_szText = other.m_szText;
	m_eColor = other.m_eColor;

	return (*this);
}

void CvReplayMessage::setTurn(int iTurn)
{
	m_iTurn = iTurn;
}

int CvReplayMessage::getTurn() const
{
	return m_iTurn;
}

void CvReplayMessage::setType(ReplayMessageTypes eType)
{
	m_eType = eType;
}

ReplayMessageTypes CvReplayMessage::getType() const
{
	return m_eType;
}

void CvReplayMessage::setPlot(int iX, int iY)
{
	m_iPlotX = iX;
	m_iPlotY = iY;
}

int CvReplayMessage::getPlotX() const
{
	return m_iPlotX;
}

int CvReplayMessage::getPlotY() const
{
	return m_iPlotY;
}

void CvReplayMessage::setPlayer(PlayerTypes ePlayer)
{
	m_ePlayer = ePlayer;
}

PlayerTypes CvReplayMessage::getPlayer() const
{
	return m_ePlayer;
}

void CvReplayMessage::setText(CvWString pszText)
{
	m_szText = pszText;
}

const CvWString& CvReplayMessage::getText() const
{
	return m_szText;
}

void CvReplayMessage::setColor(ColorTypes eColor)
{
	m_eColor = eColor;
}

ColorTypes CvReplayMessage::getColor() const
{
	return m_eColor;
}


void CvReplayMessage::read(FDataStreamBase& stream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(&stream);

	WRAPPER_READ_OBJECT_START(wrapper);

	WRAPPER_READ(wrapper, "CvReplayMessage" ,&m_iTurn);
	WRAPPER_READ(wrapper, "CvReplayMessage" ,(int*)&m_eType);

	WRAPPER_READ(wrapper, "CvReplayMessage" ,&m_iPlotX);
	WRAPPER_READ(wrapper, "CvReplayMessage" ,&m_iPlotY);

	WRAPPER_READ(wrapper, "CvReplayMessage" ,(int*)&m_ePlayer);

	WRAPPER_READ_STRING(wrapper, "CvReplayMessage" ,m_szText);
	WRAPPER_READ(wrapper, "CvReplayMessage" ,(int*)&m_eColor);

	WRAPPER_READ_OBJECT_END(wrapper);
}

void CvReplayMessage::write(FDataStreamBase& stream) const
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(&stream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE(wrapper, "CvReplayMessage", m_iTurn);
	WRAPPER_WRITE(wrapper, "CvReplayMessage", m_eType);
	WRAPPER_WRITE(wrapper, "CvReplayMessage", m_iPlotX);
	WRAPPER_WRITE(wrapper, "CvReplayMessage", m_iPlotY);
	WRAPPER_WRITE(wrapper, "CvReplayMessage", m_ePlayer);
	WRAPPER_WRITE_STRING(wrapper, "CvReplayMessage", m_szText);
	WRAPPER_WRITE(wrapper, "CvReplayMessage", m_eColor);

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

