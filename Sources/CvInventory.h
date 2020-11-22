#pragma once

#ifndef CVINVENTORY_H__
#define CVINVENTORY_H__

#include "CvGameCoreDLL.h"

class cvInternalGlobals;
class CvItemInfo;
class CvTaggedSaveFormatWrapper;

class CvInventory
{
public:
	struct CvItem
	{
		const CvItemInfo& getInfo() const { return GC.getItemInfo(type); }

		ItemTypes type;
		int16_t amount;
	};

	void addItem(ItemTypes type, int amount)
	{
		FASSERT_BOUNDS(0, GC.getNumItemInfos(), type)

		foreach_(CvItem& item, m_Inventory)
		{
			if (item.type == type)
			{
				item.amount = std::min(item.amount + amount, item.getInfo().getMaxAmount());
				return;
			}
		}
		if (m_Inventory.size() < 10)
		{
			CvItem newItem;
			newItem.type = type;
			newItem.amount = amount;
			m_Inventory.push_back(newItem);
		}
	}

	void transferItems(CvInventory& target)
	{
		foreach_(const CvItem& item, m_Inventory)
			target.addItem(item.type, item.amount);

		m_Inventory.clear();
	}

	int collectItem(ItemTypes e)
	{
		FASSERT_BOUNDS(0, GC.getNumItemInfos(), e)

		for (uint32_t i = 0, num = m_Inventory.size(); i < num; i++)
		{
			const CvItem& item = m_Inventory[i];
			if (item.type == e)
			{
				const int amount = item.amount;
				removeAt(i);
				return amount;
			}
		}
		return 0;
	}

	void removeAt(int index)
	{
		FASSERT_BOUNDS(0, numItems(), index)

		for (const int num = numItems() -1; index < num; index++)
			m_Inventory[index] = m_Inventory[index +1];

		m_Inventory.pop_back();
	}

	void buildDisplay(CvWStringBuffer& szString)
	{}

	void read(CvTaggedSaveFormatWrapper& wrapper)
	{
		int numItems = 0;
		WRAPPER_READ(wrapper, "CvInventory", &numItems)
		m_Inventory.resize(numItems);

		foreach_(CvItem& item, m_Inventory)
		{
			//WRAPPER_READ_CLASS_ENUM_ALLOW_MISSING(wrapper, "CvInventory", REMAPPED_CLASS_TYPE_ITEMS, (int*)&item.type)
			WRAPPER_READ(wrapper, "CvInventory", (int*)&item.type)
			WRAPPER_READ(wrapper, "CvInventory", &item.amount)
		}
	}

	void write(CvTaggedSaveFormatWrapper& wrapper)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvInventory", (int)m_Inventory.size(), "numItems")

		foreach_(CvItem& item, m_Inventory)
		{
			//WRAPPER_WRITE_CLASS_ENUM(wrapper, "CvInventory", REMAPPED_CLASS_TYPE_ITEMS, item.type)
			WRAPPER_WRITE(wrapper, "CvInventory", item.type)
			WRAPPER_WRITE(wrapper, "CvInventory", item.amount)
		}
	}

	const CvItem& getAt(int index) const
	{
		FASSERT_BOUNDS(0, numItems(), index)
		return m_Inventory[index];
	}

	int numItems() const
	{
		return m_Inventory.size();
	}

	python::list getPythonList() const
	{
		python::list list = python::list();
		foreach_(CvItem item, m_Inventory)
			list.append(item);
		return list;
	}

	//typedef std::vector<CvItem>::iterator iterator;

	//iterator begin() { return m_Inventory.begin(); }
	//iterator end()   { return m_Inventory.end(); }

protected:
	std::vector<CvItem> m_Inventory;
};

#endif
