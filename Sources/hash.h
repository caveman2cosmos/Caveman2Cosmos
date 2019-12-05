#pragma once

#ifndef hash_h__
#define hash_h__

namespace Cv {
	inline size_t hash_combine(const size_t seed, const size_t with)
	{
		return seed ^ (with + 0x9e3779b9 + (seed << 6) + (seed >> 2));
	}
}

#endif // hash_h__
