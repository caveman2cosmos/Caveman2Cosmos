#pragma once

//
// GameBryo
//
class NiColor
{
public:
	float r, g, b;
};
class NiColorA
{
public:
	NiColorA(float fr, float fg, float fb, float fa) : r(fr), g(fg), b(fb), a(fa) {}
	NiColorA() {}
	float r, g, b, a;
};
class NiPoint2
{
public:
	NiPoint2() {}
	NiPoint2(float fx, float fy) : x(fx), y(fy) {}

	float x, y;
};
class NiPoint3
{
public:
	NiPoint3() : x(0.0f), y(0.0f), z(0.0f) {}
	NiPoint3(float fx, float fy, float fz) : x(fx), y(fy), z(fz) {}

	bool operator== (const NiPoint3& pt) const
	{
		return (x == pt.x && y == pt.y && z == pt.z);
	}

	inline NiPoint3 operator+ (const NiPoint3& pt) const
	{
		return NiPoint3(x + pt.x, y + pt.y, z + pt.z);
	}

	inline NiPoint3 operator- (const NiPoint3& pt) const
	{
		return NiPoint3(x - pt.x, y - pt.y, z - pt.z);
	}

	inline float operator* (const NiPoint3& pt) const
	{
		return x * pt.x + y * pt.y + z * pt.z;
	}

	inline NiPoint3 operator* (float fScalar) const
	{
		return NiPoint3(fScalar * x, fScalar * y, fScalar * z);
	}

	inline NiPoint3 operator/ (float fScalar) const
	{
		float fInvScalar = 1.0f / fScalar;
		return NiPoint3(fInvScalar * x, fInvScalar * y, fInvScalar * z);
	}

	inline NiPoint3 operator- () const
	{
		return NiPoint3(-x, -y, -z);
	}

	inline float Length() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	inline float Unitize()
	{
		float length = Length();
		if (length != 0)
		{
			x /= length;
			y /= length;
			z /= length;
		}
		return length;
	}

	//	inline NiPoint3 operator* (float fScalar, const NiPoint3& pt)
	//	{	return NiPoint3(fScalar*pt.x,fScalar*pt.y,fScalar*pt.z);	}
	float x, y, z;
};

namespace NiAnimationKey
{
	enum KeyType
	{
		NOINTERP,
		LINKEY,
		BEZKEY,
		TCBKEY,
		EULERKEY,
		STEPKEY,
		NUMKEYTYPES
	};
};
