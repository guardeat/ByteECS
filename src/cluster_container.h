#ifndef B_CLUSTERCONTAINER_H
#define B_CLUSTERCONTAINER_H

#include <memory>
#include <set>
#include <vector>
#include <unordered_map>

#include "config.h"
#include "stair_map.h"
#include "signature.h"
#include "cluster.h"
#include "component.h"

namespace ByteE
{
	using ClusterPointer = Cluster*;
	using ClusterGroup = std::vector<Cluster*>;

	class ClusterContainer
	{
	public:
		using Container = ByteC::StairMap<Signature, std::unique_ptr<Cluster>>;

	private:
		Container clusters;
		ClusterID nextID{1};

	public:
		Cluster& getCluster(const Signature& signature)
		{
			auto result{clusters.find(signature)};

			if (result.valid())
			{
				return *result.get();
			}

			auto cluster{ std::make_unique<Cluster>(nextID++,signature) };
			Cluster& out{*cluster};
			clusters.insert(signature, std::move(cluster));

			return out;
		}

		void checkCluster(Cluster& cluster)
		{
			if (cluster.empty())
			{
				clusters.erase(cluster.getSignature());
			}
		}

		ClusterGroup getClusters(const Signature& with, const Signature& without)
		{
			std::vector<ClusterPointer> out;
			for (auto& pair : clusters)
			{
				if (with.subset(pair.first) && !without.match(pair.first))
				{
					out.push_back(pair.second.get());
				}
			}
			return out;
		}
	};


}

#endif