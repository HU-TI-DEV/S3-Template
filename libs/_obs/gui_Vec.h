#pragma once
#include <assert.h>

namespace crt
{
	template <size_t NofMembers, typename Type, Type defaultValue>
	class Vec
	{
	protected:
		Type arMembers[NofMembers];

	public:
		Vec()
		{
			for (int i = 0; i < NofMembers; i++)
			{
				arMembers[i] = defaultValue;
			}
		}

		Vec(const Vec& other)
		{
			operator=(other);
		}

		Vec& operator=(const Vec& other)
		{
			for (int i = 0; i < NofMembers; i++)
			{
				arMembers[i] = other.arMembers[i];
			}

			return *this;
		}

		bool operator==(const Vec& other)
		{
			for (int i = 0; i < NofMembers; i++)
			{
				if (arMembers[i] != other.arMembers[i])
				{
					return false;
				}
			}

			return true;
		}

		bool operator!=(const Vec& other)
		{
			return !operator==(other);
		}

		// hadamard - product: picewise multiplication
		Vec& operator*=(const Vec& other)
		{
			Vec newVec;
			for (int i = 0; i < NofMembers; i++)
			{
				arMembers[i] *= other.arMembers[i];
			}

			return *this;
		}

		Vec& operator+=(const Vec& other)
		{
			for (int i = 0; i < NofMembers; i++)
			{
				arMembers[i] += other.arMembers[i];
			}

			return *this;
		}

		Vec& operator-=(const Vec& other)
		{
			Vec newVec;
			for (int i = 0; i < NofMembers; i++)
			{
				arMembers[i] -= other.arMembers[i];
			}

			return *this;
		}

		Vec& operator*=(Type scalar)
		{
			for (int i = 0; i < NofMembers; i++)
			{
				arMembers[i] *= scalar;
			}

			return *this;
		}

		Vec& operator/=(Type scalar)
		{
			for (int i = 0; i < NofMembers; i++)
			{
				arMembers[i] /= scalar;
			}

			return *this;
		}

		// hadamard - product: picewise multiplication
		Vec operator*(const Vec& other)
		{
			Vec newVec;
			for (int i = 0; i < NofMembers; i++)
			{
				newVec.arMembers[i] = arMembers[i] * other.arMembers[i];
			}

			return newVec;
		}

		Vec operator+(const Vec& other)
		{
			Vec newVec;
			for (int i = 0; i < NofMembers; i++)
			{
				newVec.arMembers[i] = arMembers[i] + other.arMembers[i];
			}

			return newVec;
		}

		Vec operator-(const Vec& other)
		{
			Vec newVec;
			for (int i = 0; i < NofMembers; i++)
			{
				newVec.arMembers[i] = arMembers[i] - other.arMembers[i];
			}

			return newVec;
		}

		Vec operator*(int scalar)
		{
			Vec newVec;
			for (int i = 0; i < NofMembers; i++)
			{
				newVec.arMembers[i] = arMembers[i] * scalar;
			}

			return newVec;
		}

		Vec operator/(int scalar)
		{
			Vec newVec;
			for (int i = 0; i < NofMembers; i++)
			{
				newVec.arMembers[i] = arMembers[i] / scalar;
			}

			return newVec;
		}
	};
};
