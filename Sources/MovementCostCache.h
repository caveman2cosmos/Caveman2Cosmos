#pragma once

#include <cstddef>
#include <vector>

// Hashes select buckets only. Equality compares all mechanical inputs, so
// neither a hash collision nor query/allocator order can change a move cost.
class MovementCostCache
{
public:
	struct Key
	{
		enum Field { MODE, TERRAIN, FEATURE, FLAGS, DISCOUNT, MOVES, DENOMINATOR,
			FROM_ROUTE, TO_ROUTE, FROM_ROAD, TO_ROAD, FROM_CHANGE, TO_CHANGE,
			FROM_ROAD_CHANGE, TO_ROAD_CHANGE, HILLS_EXTRA, RIVER_EXTRA, PEAK_EXTRA, COUNT };
		int data[COUNT];
		Key() { for (int i=0;i<COUNT;++i) data[i]=0; }
		bool operator==(const Key& other) const
		{ for (int i=0;i<COUNT;++i) if(data[i]!=other.data[i]) return false; return true; }
		unsigned int hash() const
		{
			unsigned int h=2166136261U;
			for(int i=0;i<COUNT;++i) h=(h^static_cast<unsigned int>(data[i]))*16777619U;
			return h;
		}
	};
	MovementCostCache() : m_size(0), m_generation(1) {}

	int get(const Key& key) const
	{
		if (m_entries.empty()) return -1;
		const Entry& entry = m_entries[findSlot(key)];
		return entry.generation == m_generation ? entry.value : -1;
	}

	void set(const Key& key, int value)
	{
		if (m_entries.empty()) grow(1024);
		std::size_t slot = findSlot(key);
		if (m_entries[slot].generation != m_generation)
		{
			// Always leave empty slots so unsuccessful probes terminate.
			if (m_size >= m_entries.size() * 3 / 4)
			{
				grow(m_entries.size() * 2);
				slot = findSlot(key);
			}
			m_entries[slot].key = key;
			m_entries[slot].generation = m_generation;
			++m_size;
		}
		m_entries[slot].value = value;
	}

	void clear()
	{
		m_size = 0;
		// Reuse allocation without walking every retained slot at every flush.
		if (++m_generation == 0)
		{
			for (std::size_t i = 0; i < m_entries.size(); ++i)
				m_entries[i].generation = 0;
			m_generation = 1;
		}
	}

	std::size_t size() const { return m_size; }
	std::size_t capacity() const { return m_entries.size(); }

private:
	struct Entry
	{
		Key key;
		int value;
		unsigned int generation;
		Entry() : value(0), generation(0) {}
	};
	std::vector<Entry> m_entries;
	std::size_t m_size;
	unsigned int m_generation;

	static unsigned int bucketHash(unsigned int key)
	{
		// Mix low bits too: the original key is already a characteristics hash,
		// but power-of-two indexing must also handle structured/repeated keys.
		unsigned int value = static_cast<unsigned int>(key);
		value ^= value >> 16;
		value *= 0x7feb352dU;
		value ^= value >> 15;
		value *= 0x846ca68bU;
		return value ^ (value >> 16);
	}

	std::size_t findSlot(const Key& key) const
	{
		const std::size_t mask = m_entries.size() - 1;
		std::size_t slot = bucketHash(key.hash()) & mask;
		while (m_entries[slot].generation == m_generation && !(m_entries[slot].key == key))
			slot = (slot + 1) & mask;
		return slot;
	}

	void grow(std::size_t capacity)
	{
		std::vector<Entry> entries(capacity);
		entries.swap(m_entries);
		for (std::size_t i = 0; i < entries.size(); ++i)
		{
			if (entries[i].generation == m_generation)
				m_entries[findSlot(entries[i].key)] = entries[i];
		}
	}

	MovementCostCache(const MovementCostCache&);
	MovementCostCache& operator=(const MovementCostCache&);
};
