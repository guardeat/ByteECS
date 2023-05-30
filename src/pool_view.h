#ifndef	B_POOLVIEW_H
#define B_POOLVIEW_H

#include "config.h"

#include "cluster.h"
#include "cluster_container.h"
#include "pool.h"

namespace ByteE
{

	template<typename... Types>
	class PoolIterator
	{
	public:
		using ComponentGroup = ComponentGroup<Types...>;

		using Cache = Cluster::Cache<Types...>;

	private:
		Index index;
		ClusterGroup* clusters;
		Cache cache;
		Index cacheIndex;

	public:
		PoolIterator(Index index, ClusterGroup& clusterGroup, Index cacheIndex)
			:index{ index }, clusters{ &clusterGroup }, cacheIndex{cacheIndex}
		{
			if (cacheIndex < clusters->size())
			{
				cache = Cache(*clusters->at(cacheIndex));
			}
		}

		ComponentGroup operator*()
		{
			return cache.getComponents(index);
		}

		PoolIterator& operator++()
		{
			++index;

			if (index == cache.size())
			{
				index = 0;
				++cacheIndex;
				if (cacheIndex != clusters->size())
				{
					cache = Cache(*clusters->at(cacheIndex));
				}
			}

			return *this;
		}

		bool operator==(const PoolIterator& left) const
		{
			return cacheIndex == left.cacheIndex;
		}

		bool operator!=(const PoolIterator& left) const
		{
			return !(*this == left);
		}
	};

	template<typename... Types>
	class PoolView
	{
	public:
		using Iterator = PoolIterator<Types...>;

	private:
		ClusterGroup clusters;

	public:
		PoolView(Pool& pool,const Signature& without=Signature{})
			:clusters(pool.clusters.getClusters(SignatureCreator<Types...>(), without))
		{
		}

		Iterator begin()
		{
			return Iterator{0,clusters,0};
		}

		Iterator end()
		{
			return Iterator{ 0,clusters,static_cast<Index>(clusters.size())};
		}
	};

}

#endif
