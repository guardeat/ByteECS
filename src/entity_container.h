#ifndef B_ENTITYCONTAINER_H
#define B_ENTITYCONTAINER_H

#include "stair_map.h"
#include "cluster.h"

#include "config.h"

namespace ByteE
{

	class EntityContainer
	{
	public:
		using ClusterPointer = Cluster*;
		using Container = ByteC::StairMap<EntityID,ClusterPointer>;

	private:
		Container dataContainer;
		EntityID nextID{1};

	public:
		EntityID createEntity()
		{
			EntityID id{ nextID++ };
			dataContainer.insert(id, nullptr);
			return id;
		}

		void removeEntity(EntityID id)
		{
			dataContainer.erase(id);
		}

		ClusterPointer getCluster(EntityID id)
		{
			return dataContainer.at(id);
		}

		const ClusterPointer getCluster(EntityID id) const
		{
			return dataContainer.at(id);
		}

		void setCluster(EntityID id, Cluster& cluster)
		{
			dataContainer[id] = &cluster;
		}
	};

}

#endif