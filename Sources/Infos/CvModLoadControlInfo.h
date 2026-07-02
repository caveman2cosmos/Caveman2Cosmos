#pragma once

#ifndef CV_MOD_LOAD_CONTROL_INFO_H
#define CV_MOD_LOAD_CONTROL_INFO_H

#include "CvInfoBase.h"

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL				 10/24/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
// MLF loading
class CvModLoadControlInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:

	CvModLoadControlInfo();
	virtual ~CvModLoadControlInfo();

	bool isLoad(int i) const;
	void setLoad(int i, bool bLoad = true);
	const std::string getModuleFolder(int i) const;
	const std::string getParentFolder() const;
	int getNumModules() const;
	int getDirDepth() const;
	bool read(CvXMLLoadUtility* pXML, CvString szDirDepth, int iDirDepth);

protected:
	bool* m_bLoad;
	int m_iNumModules;
	int m_iDirDepth;
	std::string* m_paszModuleFolder;
	std::string m_paszParentFolder;
};

#endif // CV_MOD_LOAD_CONTROL_INFO_H
