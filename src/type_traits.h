#ifndef B_TYPETRAITS_H
#define B_TYPETRAITS_H

#include <cstdint>
#include <string>

#include "hash.h"

namespace ByteT
{

	template<typename Type>
	struct isConst
	{
		inline static constexpr bool value{ false };
	};

	template<typename Type>
	struct isConst<const Type>
	{
		inline static constexpr bool value{ true };
	};

}

#endif