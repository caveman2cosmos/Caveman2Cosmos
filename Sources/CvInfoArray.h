#pragma once

#ifndef CvInfoArray_h__
#define CvInfoArray_h__

#include "CvInfoReplacements.h"
#include "CvTaggedSaveFormatWrapper.h"

template <class T>
struct CvInfoArray
{
	void deleteInfos()
	{
		foreach_(const T* info, m_infos)
		{
			SAFE_DELETE(info);
		}
		m_infos.clear();
	}

	int size() const
	{
		return (int)m_infos.size();
	}

	T& operator[](int i) const
	{
		FASSERT_BOUNDS(0, (int)m_infos.size(), (int)i)
		return *m_infos[i];
	}

	void load(CvXMLLoadUtility* pXML, const char* szFile, const char* szFolder, const wchar_t* szXmlPath, bool bTwoPass = false)
	{
		pXML->LoadGlobalClassInfo(m_infos, szFile, szFolder, szXmlPath, bTwoPass, &m_infoReplacements);
	}

	void writeClassMappingTable(FDataStreamBase* pStream)
	{
		class_mapping_table_entry entry;
		entry.escapeId = SAVE_ELEMENT_ID_CLASS_MAP;
		entry.classType = classType;
		entry.numClasses = (int)m_infos.size();
		pStream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		foreach_(T* info, m_infos)
			pStream->WriteString(info.getType());
	}

	typedef typename std::vector<T>::iterator        iterator;
	typedef typename std::vector<T>::const_iterator  const_iterator;

	iterator begin() { return m_infos.begin(); }
	iterator end()   { return m_infos.end(); }

	const_iterator begin() const { return m_infos.begin(); }
	const_iterator end() const   { return m_infos.end(); }

private:
	std::vector<T*> m_infos;
	CvInfoReplacements<T> m_infoReplacements;
};

#endif