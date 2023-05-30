#ifndef B_CONFIG_H
#define	B_CONFIG_H

#include <cstdint>
#include <string>

namespace ByteE
{

	using Index = uint32_t;

	using ComponentID = uint32_t;
	using EntityID = uint64_t;
	using ClusterID = uint64_t;

	using ComponentName = std::string;

	inline constexpr EntityID nullEntity{ 0 };
	inline constexpr ClusterID nullCluster{ 0 };

	inline constexpr size_t MAX_COMPONENT_COUNT{ 1024 };

}

#endif