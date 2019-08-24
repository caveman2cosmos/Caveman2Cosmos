#pragma once

#include "FDataStreamBase.h"
#include "CvTaggedSaveFormatWrapper.h"

#include <hash_map>

template <class T>
struct FItemIDMap
{
	typedef stdext::hash_map<int, T*> map_type;
	typedef typename map_type::iterator iterator;
	typedef typename map_type::const_iterator const_iterator;
	typedef typename map_type::reverse_iterator reverse_iterator;
	typedef typename map_type::const_reverse_iterator const_reverse_iterator;
	typedef typename map_type::key_type key_type;

	FItemIDMap() : m_nextId(0) {}
	
	void clear();

	int size() const { return m_items.size(); }
	int nextId() const { return m_nextId; }

	const_iterator begin() const { return m_items.begin(); }
	const_iterator end() const { return m_items.end(); }
	iterator begin() { return m_items.begin(); }
	iterator end() { return m_items.end(); }

	const_reverse_iterator rbegin() const { return m_items.rbegin(); }
	const_reverse_iterator rend() const { return m_items.rend(); }
	reverse_iterator rbegin() { return m_items.rbegin(); }
	reverse_iterator rend() { return m_items.rend(); }

	const_iterator find(int id) const { return m_items.find(id); }
	iterator find(int id) { return m_items.find(id); }

	T* findAt(int id) const 
	{
		// FAssertMsg(m_items.find(id) != m_items.end(), "FItemIDMap::getAt id not found");
		const_iterator fItr = m_items.find(id); 
		if (fItr != m_items.end())
		{
			return fItr->second;
		}
		return NULL;
	}

	//T* operator[](int id) const
	//{
	//	FAssertMsg(m_items.find(id) != m_items.end(), "FItemIDMap::getAt id not found");
	//	return m_items.find(id)->second; 
	//}

	T*& operator[](int id)
	{
		// FAssertMsg(m_items.find(id) != m_items.end(), "FItemIDMap::getAt id not found");
		return m_items[id];
	}

	T* add();
	void insert(T* item);

	bool remove(T* item) { return removeAt(item->getID()); }
	bool removeAt(int id);

	void Read(FDataStreamBase* pStream);
	void Write(FDataStreamBase* pStream);

private:
	key_type m_nextId;
	map_type m_items;
};

template <class T>
T* FItemIDMap<T>::add()
{
	MEMORY_TRACK_EXEMPT();
	int newId = m_nextId++;
	FAssertMsg(m_items.find(newId) == m_items.end(), "Duplicate item being added to FFreeListTrashArray, start index was not valid");
	T* newItem = new T();
	newItem->setID(newId);
	m_items.insert(typename map_type::value_type(newId, newItem));
	return newItem;
}

template <class T>
bool FItemIDMap<T>::removeAt(int id)
{
	iterator itr = m_items.find(id);
	if (itr == m_items.end())
	{
		return false;
	}
	delete itr->second;
	m_items.erase(itr);
	return true;
}


template <class T>
void FItemIDMap<T>::clear()
{
	for (iterator itr = m_items.begin(); itr != m_items.end(); ++itr)
	{
		delete itr->second;
	}
	m_items.clear();
	m_nextId = 0;
}


template <class T>
void FItemIDMap<T>::insert(T* item)
{
	FAssertMsg(item != NULL, "Null item being added to FFreeListTrashArray");
	int id = item->getID();
	FAssertMsg(m_items.find(id) == m_items.end(), "Duplicate item being added to FFreeListTrashArray");
	m_items.insert(typename map_type::value_type(id, item));
	m_nextId = std::max<int>(id + 1, m_nextId);
}

//
// use when list contains non-streamable types
//
template < class T >
inline void FItemIDMap<T>::Read(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	int iTemp, numSlots;
	WRAPPER_READ_DECORATED(wrapper, "FFreeListTrashArray", &numSlots, "numSlots");
	WRAPPER_READ_DECORATED(wrapper, "FFreeListTrashArray", &iTemp, "lastIndex");
	WRAPPER_READ_DECORATED(wrapper, "FFreeListTrashArray", &iTemp, "freeListHead");
	WRAPPER_READ_DECORATED(wrapper, "FFreeListTrashArray", &iTemp, "freeListCount");
	WRAPPER_READ_DECORATED(wrapper, "FFreeListTrashArray", &iTemp, "currentID");

	for (int i = 0; i < numSlots; i++)
	{
		WRAPPER_READ_DECORATED(wrapper, "FFreeListTrashArray", &iTemp, "nextFreeIndex");
	}

	int iCount;
	WRAPPER_READ_DECORATED(wrapper, "FFreeListTrashArray", &iCount, "count");

	for (int i = 0; i < iCount; i++)
	{
		T* pData = new T;
		WRAPPER_READ_ARRAY_DECORATED(wrapper, "FFreeListTrashArray", sizeof (T), (byte*)pData, "pData");
		insert(pData);
	}

	WRAPPER_READ_OBJECT_END(wrapper);
}

template < class T >
inline void FItemIDMap<T>::Write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	int dummy = 0;
	WRAPPER_WRITE_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", dummy, "flistNumSlots");
	WRAPPER_WRITE_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", dummy, "flistLastIndex");
	WRAPPER_WRITE_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", dummy, "flistFreeListHead");
	WRAPPER_WRITE_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", dummy, "flistFreeListCount");
	WRAPPER_WRITE_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", dummy, "flistCurrentId");

	WRAPPER_WRITE_DECORATED(wrapper, "FFreeListTrashArray", size(), "count");

	for (const_iterator itr = m_items.begin(); itr != m_items.end(); ++itr)
	{
		WRAPPER_WRITE_ARRAY_DECORATED(wrapper, "FFreeListTrashArray", sizeof (T), (byte*)(itr->second), "pData");
	}

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

//-------------------------------
// Serialization helper templates:
//-------------------------------

//
// use when list contains streamable types
//
template < class T >
inline void ReadStreamableFFreeListTrashArray(FItemIDMap<T>& flist, FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	int iTemp, numSlots;
	WRAPPER_READ_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", &numSlots, "flistNumSlots");
	WRAPPER_READ_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", &iTemp, "flistLastIndex");
	WRAPPER_READ_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", &iTemp, "flistFreeListHead");
	WRAPPER_READ_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", &iTemp, "flistFreeListCount");
	WRAPPER_READ_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", &iTemp, "flistCurrentId");

	for (int i = 0; i < numSlots; i++)
	{
		WRAPPER_READ_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", &iTemp, "flistNextFree");
	}

	int iCount;
	WRAPPER_READ_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", &iCount, "flistCount");

	for (int i = 0; i < iCount; i++)
	{
		T* pData = new T;
		pData->read(pStream);
		flist.insert(pData);
	}

	WRAPPER_READ_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", &iTemp, "flistCorruptedAdjustment");

	WRAPPER_READ_OBJECT_END(wrapper);
}

template < class T >
inline void WriteStreamableFFreeListTrashArray(FItemIDMap<T>& flist, FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	int dummy = 0;
	WRAPPER_WRITE_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", dummy, "flistNumSlots");
	WRAPPER_WRITE_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", dummy, "flistLastIndex");
	WRAPPER_WRITE_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", dummy, "flistFreeListHead");
	WRAPPER_WRITE_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", dummy, "flistFreeListCount");
	WRAPPER_WRITE_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", dummy, "flistCurrentId");

	WRAPPER_WRITE_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", flist.size(), "flistCount");

	for (FItemIDMap<T>::const_iterator itr = flist.begin(); itr != flist.end(); ++itr)
	{
		itr->second->write(pStream);
	}

	WRAPPER_WRITE_OBJECT_END(wrapper);
}
