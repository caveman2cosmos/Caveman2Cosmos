#pragma once

#ifndef nullptr_t_h__
#define nullptr_t_h__

const // It is a const object...
	class nullptr_t
{
public:
	template<class T>
	inline operator T* () const // convertible to any type of null non-member pointer...
	{
		return 0;
	}

	template<class C, class T>
	inline operator T C::* () const   // or any type of null member pointer...
	{
		return 0;
	}

private:
	void operator&() const;  // Can't take address of nullptr

} nullptr = {};

namespace std {
	typedef ::nullptr_t nullptr_t;
}
#endif // nullptr_t_h__