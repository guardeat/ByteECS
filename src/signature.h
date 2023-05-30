#ifndef SIGNATURE_H
#define SIGNATURE_H

#include <bitset>
#include <vector>

#include "stair_map.h"
#include "hash.h"

#include "config.h"
#include "component.h"

namespace ByteE
{

	class Signature
	{
	public:
		using Container = size_t[(MAX_COMPONENT_COUNT / 64) + 1];
		using Bitset = std::bitset<64>;

	private:
		Container data{};

		friend struct ByteA::Hash<Signature>;

	public:
		void set(size_t index)
		{
			size_t bitIndex{ index % 64 };
			data[index / 64] |= 1ULL << bitIndex;
		}

		void set(size_t index, bool value)
		{
			size_t bitIndex{ index % 64 };
			if (value)
			{
				data[index / 64] |= 1ULL << bitIndex;
			}
			else
			{
				data[index / 64] &= ~(1ULL << bitIndex);
			}
		}

		bool test(size_t index) const
		{
			return (data[index / 64]) & (1ULL << (index%64));
		}

		bool subset(const Signature& left) const
		{
			constexpr size_t elementCount{ (MAX_COMPONENT_COUNT / 64) + 1 };

			for (size_t i{}; i < elementCount; ++i)
			{
				if ((data[i] | left.data[i]) != data[i])
				{
					return false;
				}
			}
			return true;
		}

		bool match(const Signature& left) const
		{
			constexpr size_t elementCount{ (MAX_COMPONENT_COUNT / 64) + 1 };

			for (size_t i{}; i < elementCount; ++i)
			{
				if ((data[i] & left.data[i]) > 0)
				{
					return true;
				}
			}
			return false;
		}

		bool any() const
		{
			constexpr size_t elementCount{ (MAX_COMPONENT_COUNT / 64) + 1 };

			for (size_t i{}; i < elementCount; ++i)
			{
				if (data[i] > 0)
				{
					return true;
				}
			}
			return false;
		}

		void operator+=(const Signature& left)
		{
			constexpr size_t elementCount{ (MAX_COMPONENT_COUNT / 64) + 1 };

			for (size_t i{}; i < elementCount; ++i)
			{
				data[i] |= left.data[i];
			}
		}

		bool operator==(const Signature& left) const
		{
			constexpr size_t elementCount{ (MAX_COMPONENT_COUNT / 64) + 1 };

			for (size_t i{}; i < elementCount; ++i)
			{
				if (data[i] != left.data[i])
				{
					return false;
				}
			}
			return true;
		}

		bool operator!=(const Signature& left) const
		{
			return !(*this == left);
		}

		std::vector<ComponentID> query() const
		{
			std::vector<ComponentID> out;
			
			constexpr size_t elementCount{ (MAX_COMPONENT_COUNT / 64) + 1 };

			for (size_t i{}; i < elementCount; ++i)
			{
				if (data[i])
				{
					for (size_t i2{}; i2 < 64; ++i2)
					{
						if ((data[i]) & (1ULL << (i2)))
						{
							out.push_back(static_cast<ComponentID>((i * 64) + i2));
						}
					}
				}
			}

			return out;
		}
	};

	template<typename... Types>
	struct SignatureCreator
	{
		operator Signature() const
		{
			Signature out;
			(out.set(Component<Types>::getID()), ...);
			return out;
		}
	};
}

namespace ByteA
{
	template<>
	struct Hash<ByteE::Signature>
	{
		size_t operator()(const ByteE::Signature& signature)
		{
			constexpr size_t elementCount{(ByteE::MAX_COMPONENT_COUNT/64) + 1};
			
			size_t hash{};
			for (size_t i{}; i < elementCount; ++i)
			{
				hash += (i + 1) * signature.data[i];
			}
			return hash;
		}
	};
}

#endif