#pragma once

#ifndef CV_GAME_TEXT_H
#define CV_GAME_TEXT_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGameText
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameText
	: public CvInfoBase
{
public:
	DllExport CvGameText();

	DllExport static int getNumLanguages()               { return NUM_LANGUAGES; }
	DllExport static void setNumLanguages(int iNum)      { }
	static void setLanguage(const CvWString& szLanguage) { m_szLanguage = szLanguage; }

	const wchar_t* getText() const   { return m_szText; }
	const wchar_t* getGender() const { return m_szGender; }
	const wchar_t* getPlural() const { return m_szPlural; }

	std::wstring pyGetText() const   { return getText(); }	// for Python

	bool read(CvXMLLoadUtility* pXML);

protected:
	static int NUM_LANGUAGES;
	static CvWString m_szLanguage;

	CvWString m_szText;
	CvWString m_szGender;
	CvWString m_szPlural;
};

#endif // CV_GAME_TEXT_H
