#pragma once
#include <assert.h>
#include "crt_Vec.h"

namespace crt
{
	typedef Vec<2,int,0> Vec2spec;
	
	class Vec2 : public Vec2spec
	{
	public:
		int& x;
		int& y;

	public:
		Vec2():Vec2spec(),x(arMembers[0]),y(arMembers[1])
		{
		}

		Vec2(int xValue, int yValue) : Vec2spec(), x(arMembers[0]), y(arMembers[1])
		{
			arMembers[0] = xValue;
			arMembers[1] = yValue;
		}

		Vec2(const Vec2& other):Vec2spec(other),x(arMembers[0]), y(arMembers[1])
		{
		}

		Vec2& operator=(const Vec2& other)
		{
			Vec2spec::operator=(other);

			return *this;
		}

		bool operator==(const Vec2& other)
		{
			return Vec2spec::operator==(other);
		}

		bool operator!=(const Vec2& other)
		{
			return !operator==(other);
		}


		Vec2& operator*=(const Vec2& other)
		{
		
			Vec2spec::operator*=(other);
			return *this;
		}

		Vec2& operator+=(const Vec2& other)
		{
			Vec2spec::operator+=(other);
			return *this;
		}

		Vec2& operator-=(const Vec2& other)
		{
			Vec2spec::operator-=(other);
			return *this;
		}

		Vec2& operator*=(int scalar)
		{
			Vec2spec::operator*=(scalar);
			return *this;
		}

		Vec2& operator/=(int scalar)
		{
			Vec2spec::operator/=(scalar);
			return *this;
		}

		// hadamard - product: picewise multiplication
		// In the operators below, the same operators
		// in the base class are not called, because
		// they return a new object of type Vec instead of Vec2.
		// (Using the base class operators would require 
		// an additional type conversion step for each call.)
		Vec2 operator*(const Vec2& other)
		{
			Vec2 newVec;
			for (int i = 0; i < 2; i++)
			{
				newVec.arMembers[i] = arMembers[i] * other.arMembers[i];
			}

			return newVec;
		}
		
		Vec2 operator+(const Vec2& other)
		{
			Vec2 newVec;
			for (int i = 0; i < 2; i++)
			{
				newVec.arMembers[i] = arMembers[i] + other.arMembers[i];
			}

			return newVec;
		}

		Vec2 operator-(const Vec2& other)
		{
			Vec2 newVec;
			for (int i = 0; i < 2; i++)
			{
				newVec.arMembers[i] = arMembers[i] - other.arMembers[i];
			}

			return newVec;
		}

		Vec2 operator*(int scalar)
		{
			Vec2 newVec;
			for (int i = 0; i < 2; i++)
			{
				newVec.arMembers[i] = arMembers[i] * scalar;
			}

			return newVec;
		}

		Vec2 operator/(int scalar)
		{
			Vec2 newVec;
			for (int i = 0; i < 2; i++)
			{
				newVec.arMembers[i] = arMembers[i] / scalar;
			}

			return newVec;
		}
	};
};
