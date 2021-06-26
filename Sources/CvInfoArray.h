#pragma once

#ifndef CvInfoArray_h__
#define CvInfoArray_h__

#include "CvGlobals.h"
#include "CvInfoReplacements.h"
#include "CvTaggedSaveFormatWrapper.h"
//#include "CvXMLLoadUtility.h"

template <class CvInfoClass_T>
struct CvInfoArray : bst::noncopyable
{
	~CvInfoArray()
	{
		deleteInfos();
	}

	CvInfoClass_T* operator[](int i) const
	{
		FASSERT_BOUNDS(0, size(), i)
		return m_infos[i];
	}

	int size() const
	{
		return (int)m_infos.size();
	}

	typedef typename std::vector<CvInfoClass_T*>::iterator        iterator;
	typedef typename std::vector<CvInfoClass_T*>::const_iterator  const_iterator;

	iterator begin() { return m_infos.begin(); }
	iterator end()   { return m_infos.end(); }

	const_iterator begin() const { return m_infos.begin(); }
	const_iterator end() const   { return m_infos.end(); }

	void writeClassMappingTable(FDataStreamBase* pStream)
	{
		class_mapping_table_entry entry;
		entry.escapeId = SAVE_ELEMENT_ID_CLASS_MAP;
		entry.classType = classType;
		entry.numClasses = size();
		pStream->Write(sizeof(class_mapping_table_entry), static_cast<uint8_t*>(&entry));
		foreach_(const CvInfoClass_T* info, m_infos)
			pStream->WriteString(info->getType());
	}

private:
	friend class cvInternalGlobals;
	friend class CvXMLLoadUtility;

	void deleteInfos()
	{
		foreach_(const CvInfoClass_T* info, m_infos)
			delete info;
		m_infos.clear();
	}

	void push_back(CvInfoClass_T* info)
	{
		return m_infos.push_back(info);
	}

	void insert(iterator& it, int i, CvInfoClass_T* info)
	{
		return m_infos.insert(it, i, info);
	}

	void load(CvXMLLoadUtility* pXML, const char* szFile, const char* szFolder, const wchar_t* szXmlPath, bool bTwoPass = false)
	{
		//FAssert(m_infos.empty())
		GC.addToInfoArrays(this);
		pXML->LoadGlobalClassInfo(m_infos, szFile, szFolder, szXmlPath, bTwoPass, &m_infoReplacements);
	}

	void readPass3()
	{
		foreach_(CvInfoClass_T* info, m_infos)
		{
			info->readPass3();
			m_infoReplacements.readPass3();
		}
	}

	void updateReplacements()
	{
		m_infoReplacements.updateReplacements(m_infos);
	}

	std::vector<CvInfoClass_T*> m_infos;
	CvInfoReplacements<CvInfoClass_T> m_infoReplacements;
};

#endif