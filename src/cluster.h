#ifndef B_CLUSTER_H
#define B_CLUSTER_H

#include <type_traits>
#include <tuple>
#include <vector>
#include <memory>

#include "stair_map.h"
#include "component.h"
#include "component_vector.h"
#include "signature.h"

#include "config.h"

namespace ByteE
{
	template<typename... Types>
	using ComponentGroup = std::tuple<EntityID, Types&...>;

	class Cluster
	{
	public:
		using Container = ByteC::StairMap<ComponentID, ComponentVectorBase>;
		using IndexContainer = ByteC::StairMap<EntityID, Index>;
		using IDContainer = ByteC::StairVector<EntityID>;

	private:
		ClusterID id;
		Signature signature;
		Container components;
		IndexContainer indices;
		IDContainer ids;

		friend struct ClusterTraits;

	public:
		template<typename... Types>
		class Cache
		{
		public:
			using VectorPointer = IComponentVector*;
			using Container = std::vector<VectorPointer>;
			using MapPointer = IDContainer*;
			using ComponentGroup = ComponentGroup<Types...>;

		private:
			Container components;
			MapPointer ids;

		public:
			Cache() = default;

			Cache(Cluster& cluster)
				:ids{&cluster.ids}
			{
				((components.push_back(cluster.components.at(Component<Types>::getID()).get())),...);
			}

			ComponentGroup getComponents(Index index)
			{
				return ComponentGroup(ids->at(index),(getComponent<Types>(index))...);
			}

			size_t size() const
			{
				if (components.empty())
				{
					return 0;
				}
				return components[0]->size();
			}

		private:
			template<typename Type>
			Type& getComponent(Index index)
			{
				return static_cast<ComponentVector<Type>*>(components[Component<Type>::getID()])->at(index);
			}
		};

	public:
		Cluster(ClusterID id, const Signature& signature)
			:id{ id }, signature{signature}
		{
			const auto& componentList{ signature.query() };

			for (ComponentID id : componentList)
			{
				components.insert(id, ComponentAccessor::createArray(id));
			}
		}

		ClusterID getID() const
		{
			return id;
		}

		const Signature& getSignature()
		{
			return signature;
		}

		void pushEntity(EntityID id)
		{
			indices.insert(id, static_cast<Index>(ids.size()));
			ids.pushBack(id);
		}

		void removeEntity(EntityID id)
		{
			EntityID lastEntity{ids.back()};
			Index lastIndex{static_cast<Index>(ids.size() - 1)};
			Index entityIndex{indices.at(id)};

			if (id != lastEntity)
			{
				for (auto& pair : components)
				{
					pair.second->swap(entityIndex, lastIndex);
					pair.second->popBack();
				}

				indices[lastEntity] = entityIndex;
				ids[entityIndex] = lastEntity;
			}
			else
			{
				for (auto& pair : components)
				{
					pair.second->popBack();
				}
			}

			indices.erase(id);
			ids.popBack();
		}

		template<typename Type>
		void pushComponent(Type&& component)
		{
			getVector<Type>().pushBack(std::move(component));
		}

		template<typename Type>
		Type& getComponent(EntityID id)
		{
			return getVector<Type>().at(indices.at(id));
		}

		template<typename Type>
		const Type& getComponent(EntityID id) const
		{
			return getVector<Type>().at(indices.at(id));
		}

		template<typename Type>
		void setComponent(EntityID id, Type&& component)
		{
			getVector<Type>().at(indices.at(id)) = std::move(component);
		}

		size_t size() const
		{
			return ids.size();
		}

		bool empty() const
		{
			return size() == 0;
		}

	private:
		template<typename Type>
		ComponentVector<Type>& getVector()
		{
			return static_cast<ComponentVector<Type>&>(*components.at(Component<Type>::getID()));
		}

		template<typename Type>
		const ComponentVector<Type>& getVector() const
		{
			return static_cast<const ComponentVector<Type>&>(*components.at(Component<Type>::getID()));
		}
	};

	struct ClusterTraits
	{
		static Index carryEntity(Cluster& current, Cluster& destination, EntityID id)
		{
			destination.pushEntity(id);
			Index index{current.indices.at(id)};

			for (auto& pair : current.components)
			{
				auto destitationArray{destination.components.find(pair.first)};
				if (destitationArray.valid())
				{
					ComponentAccessor::arrayCarry(
						pair.first,
						index,
						*current.components[pair.first].get(),
						*destitationArray->get());
				}
			}

			return static_cast<Index>(destination.size() - 1);
		}

		static Index copyEntity(const Cluster& current, Cluster& destination, EntityID id, Index index)
		{
			destination.pushEntity(id);

			for (auto& pair : current.components)
			{
				ComponentAccessor::arrayCopy(
					pair.first,
					index,
					*current.components[pair.first].get(),
					*destination.components[pair.first].get());
			}

			return static_cast<Index>(destination.size() - 1);
		}
	};

	template<typename... Types>
	class ClusterIterator
	{
	public:
		using ClusterPointer = Cluster*;
		using Cache = Cluster::Cache<Types...>;
		using ComponentGroup = ComponentGroup<Types...>;

	private:
		ClusterPointer cluster;
		Index index;
		Cache cache;

	public:
		ClusterIterator(ClusterPointer cluster, Index index)
			:cluster{cluster},index{index},cache{Cache{*cluster}}
		{
		}

		ClusterIterator& operator++()
		{
			++index;
		}

		ComponentGroup operator*()
		{
			return cache.getComponents(index);
		}

		bool operator==(const ClusterIterator& left) const
		{
			return index == left.index;
		}

		bool operator!=(const ClusterIterator& left) const
		{
			return index != left.index;
		}
	};

	template<typename... Types>
	class ClusterView
	{
	public:
		using ClusterPointer = Cluster*;
		using Iterator = ClusterIterator<Types...>;

	private:
		ClusterPointer cluster;

	public:
		ClusterView(ClusterPointer cluster)
			:cluster{cluster}
		{
		}

		Iterator begin()
		{
			return Iterator{ cluster,0 };
		}

		Iterator end()
		{
			return Iterator{ cluster,cluster->size() };
		}

	};
}

#endif