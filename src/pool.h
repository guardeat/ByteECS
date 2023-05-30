#ifndef B_POOL_H
#define B_POOL_H

#include <functional>
#include <tuple>

#include "config.h"
#include "entity_container.h"
#include "cluster_container.h"
#include "component.h"

namespace ByteE
{

	class Pool
	{
	private:
		EntityContainer entities;
		ClusterContainer clusters;

		template<typename... Types>
		friend class PoolView;

	public:
		EntityID createEntity()
		{
			return entities.createEntity();
		}

		void destroyEntity(EntityID id)
		{
			Cluster* cluster{ entities.getCluster(id) };
			if (cluster)
			{
				cluster->removeEntity(id);
				clusters.checkCluster(*cluster);
			}
			entities.removeEntity(id);
		}

		template<typename Type>
		void addComponent(EntityID id, Type&& component)
		{
			Cluster* oldCluster{entities.getCluster(id)};

			Signature newSignature;

			if (oldCluster)
			{
				newSignature = oldCluster->getSignature();
			}

			newSignature.set(Component<Type>::getID());

			Cluster& newCluster{clusters.getCluster(newSignature)};

			if (oldCluster)
			{
				ClusterTraits::carryEntity(*oldCluster, newCluster,id);
				oldCluster->removeEntity(id);
			}
			else
			{
				newCluster.pushEntity(id);
			}

			newCluster.pushComponent<Type>(std::move(component));
			entities.setCluster(id, newCluster);

			if (oldCluster)
			{
				clusters.checkCluster(*oldCluster);
			}
		}

		template<typename Type>
		Type& getComponent(EntityID id)
		{
			return entities.getCluster(id)->getComponent<Type>(id);
		}

		template<typename Type>
		const Type& getComponent(EntityID id) const
		{
			return entities.getCluster(id)->getComponent<Type>(id);
		}

		template<typename Type>
		void setComponent(EntityID id, Type&& component)
		{
			entities.getCluster(id)->setComponent<Type>(id,std::move(component));
		}

		template<typename Type>
		void removeComponent(EntityID id)
		{
			Cluster* oldCluster{ entities.getCluster(id)};
			
			Signature newSignature{oldCluster->getSignature()};

			newSignature.set(Component<Type>::getID(), false);

			if (!newSignature.any())
			{
				oldCluster->removeEntity(id);
			}
			else
			{
				Cluster& newCluster{ clusters.getCluster(newSignature) };

				ClusterTraits::carryEntity(*oldCluster, newCluster, id);
				oldCluster->removeEntity(id);
				entities.setCluster(id, newCluster);
			}
			clusters.checkCluster(*oldCluster);
		}

		template<typename... Types>
		void addComponents(EntityID id, Types... components)
		{
			Signature newSignature{ SignatureCreator<Types...>{} };

			Cluster* oldCluster{entities.getCluster(id)};

			if (oldCluster)
			{
				newSignature += oldCluster->getSignature();
			}

			Cluster& newCluster{ clusters.getCluster(newSignature) };

			if (oldCluster)
			{
				ClusterTraits::carryEntity(*oldCluster, newCluster, id);
				oldCluster->removeEntity(id);
			}
			else
			{
				newCluster.pushEntity(id);
			}

			(newCluster.pushComponent<Types>(std::move(components)),...);

			entities.setCluster(id, newCluster);

			if (oldCluster)
			{
				clusters.checkCluster(*oldCluster);
			}
		}

		template<typename Type>
		bool hasComponent(EntityID id) const
		{
			return entities.getCluster(id)->getSignature().test(Component<Type>::getID());
		}
	};

}

#endif