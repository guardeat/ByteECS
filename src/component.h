#ifndef B_COMPONENTTRAITS_H
#define B_COMPONENTTRAITS_H

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <type_traits>

#include "config.h"
#include "component_vector.h"

namespace ByteE
{

	class IComponentMask
	{
	public:
		virtual ~IComponentMask() = default;

		virtual void arrayCarry(
			Index index, 
			IComponentVector& old, 
			IComponentVector& dest) const = 0;

		virtual void arrayCopy(
			Index index, 
			const IComponentVector& old, 
			IComponentVector& dest) const = 0;

		virtual ComponentVectorBase createArray() const = 0;
	};

	template<typename Type>
	class ComponentMask : public IComponentMask
	{
	public:
		void arrayCarry(
			Index index, 
			IComponentVector& old,
			IComponentVector& dest) const override
		{
			auto& castedOld{ static_cast<ComponentVector<Type>&>(old) };
			auto& castedDest{ static_cast<ComponentVector<Type>&>(dest) };
			castedDest.pushBack(std::move(castedOld.at(index)));
		}

		void arrayCopy(
			Index index, 
			const IComponentVector& old, 
			IComponentVector& dest) const override
		{
			auto& castedOld{ static_cast<const ComponentVector<Type>&>(old) };
			auto& castedDest{ static_cast<ComponentVector<Type>&>(dest) };
			castedDest.pushBack(castedOld.at(index));
		}

		ComponentVectorBase createArray() const override
		{
			return std::make_unique<ComponentVector<Type>>();
		}
	};

	class MaskContainer
	{
	private:
		using ComponentBase = std::unique_ptr<IComponentMask>;
		using Container = std::vector<ComponentBase>;
		using Accessor = IComponentMask&;

		friend struct ComponentAccessor;
		friend struct ComponentIndexer;

		inline static Container accessors;

		MaskContainer() = delete;

		static void pushAccessor(ComponentBase accessorBase)
		{
			accessors.push_back(std::move(accessorBase));
		}

		static const Accessor getAccessor(ComponentID id)
		{
			return *accessors.at(id);
		}
	};

	struct ComponentAccessor
	{
		static void arrayCarry(
			ComponentID id, 
			Index index, 
			IComponentVector& old,
			IComponentVector& dest)
		{
			MaskContainer::getAccessor(id).arrayCarry(index, old, dest);
		}

		static void arrayCopy(
			ComponentID id, 
			Index index, 
			const IComponentVector& old, 
			IComponentVector& dest)
		{
			MaskContainer::getAccessor(id).arrayCopy(index, old, dest);
		}

		static ComponentVectorBase createArray(ComponentID id)
		{
			return MaskContainer::getAccessor(id).createArray();
		}
	};

	struct ComponentIndexer
	{
	private:
		inline static ComponentID next{};

		template<typename Type>
		friend struct Component;

		template<typename Type>
		static ComponentID create()
		{
			MaskContainer::pushAccessor(std::make_unique<ComponentMask<Type>>());
			return ComponentIndexer::next++;
		}
	};

	template<typename Type>
	struct Component
	{
	private:
		inline static ComponentName name{ typeid(Type).name() };
		inline static const ComponentID id{ ComponentIndexer::create<Type>() };

	public:
		static ComponentID getID()
		{
			return id;
		}

		static const ComponentName& getName()
		{
			return name;
		}

		static void setName(const ComponentName& newName)
		{
			name = newName;
		}
	};
}

#endif