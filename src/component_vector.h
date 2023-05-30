#ifndef B_COMPONENTMAP_H
#define B_COMPONENTMAP_H

#include <memory>
#include <cstdint>
#include <algorithm>

#include "config.h"
#include "stair_map.h"

namespace ByteE
{
	
	class IComponentVector;
	using ComponentVectorBase = std::unique_ptr<IComponentVector>;

	class IComponentVector
	{
	public:
		virtual ~IComponentVector() = default;

		virtual void popBack() = 0;

		virtual void swap(size_t left, size_t right) = 0;

		virtual size_t size() const = 0;
		
		virtual ComponentVectorBase copy() const = 0;
	};

	template <typename Type>
	class ComponentVector: public ByteC::StairVector<Type>, public IComponentVector
	{
	public:
		using ComponentType = Type;
		using ContainerBase = ByteC::StairVector<ComponentType>;

	public:
		ComponentVector() = default;

		~ComponentVector() = default;

		void popBack() override
		{
			ContainerBase::popBack();
		}

		void swap(size_t left, size_t right) override
		{
			std::swap(ContainerBase::at(left), ContainerBase::at(right));
		}

		virtual size_t size() const
		{
			return ContainerBase::size();
		}

		ComponentVectorBase copy() const override
		{
			return std::make_unique<ComponentVector<ComponentType>>((*this));
		}
	};

}

#endif
