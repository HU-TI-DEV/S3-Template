#pragma once
#include <assert.h>

namespace crt
{	
	class Vec2
	{
	public:
		int x;
		int y;

	public:
		Vec2():x(0),y(0)
		{
		}

		Vec2(int xValue, int yValue) : x(xValue), y(yValue)
		{
		}

		Vec2(const Vec2& other):x(other.x), y(other.y)
		{
		}

		Vec2& operator=(const Vec2& other)
		{
			x=other.x;
			y=other.y;

			return *this;
		}

		void set(int xValue, int yValue)
		{
			x = xValue;
			y = yValue;
		}

		bool operator==(const Vec2& other)
		{
			return (x==other.x)&&(y==other.y);
		}

		bool operator!=(const Vec2& other)
		{
			return !operator==(other);
		}


		Vec2& operator*=(const Vec2& other)
		{
			x*=other.x;
			y*=other.y;
			return *this;
		}

		Vec2& operator+=(const Vec2& other)
		{
			x+=other.x;
			y+=other.y;
			return *this;
		}

		Vec2& operator-=(const Vec2& other)
		{
			x-=other.x;
			y-=other.y;
			return *this;
		}

		Vec2& operator*=(int scalar)
		{
			x*=scalar;
			y*=scalar;
			return *this;
		}

		Vec2& operator/=(int scalar)
		{
			x/=scalar;
			y/=scalar;
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
			newVec.x = x*other.x;
			newVec.y = y*other.y;

			return newVec;
		}
		
		Vec2 operator+(const Vec2& other)
		{
			Vec2 newVec;
			newVec.x = x+other.x;
			newVec.y = y+other.y;
			return newVec;
		}

		Vec2 operator-(const Vec2& other)
		{
			Vec2 newVec;
			newVec.x = x-other.x;
			newVec.y = y-other.y;

			return newVec;
		}

		Vec2 operator*(int scalar)
		{
			Vec2 newVec;
			newVec.x = x*scalar;
			newVec.y = y*scalar;

			return newVec;
		}

		Vec2 operator/(int scalar)
		{
			Vec2 newVec;
			newVec.x = x/scalar;
			newVec.y = y/scalar;

			return newVec;
		}
	};
};
