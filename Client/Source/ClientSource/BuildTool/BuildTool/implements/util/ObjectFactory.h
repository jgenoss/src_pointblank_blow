#pragma once

#include <map>
#include <functional>
#include <exception>
#include "ThreadPolicy.h"


template <class IdentifierType, class ProductType>
class DefaultFactoryError
{
public:
	class Exception : public std::exception
	{
	public:
		Exception(const IdentifierType& id) : m_id(id) {}

		virtual const char* what() const { return "Unknown object type passed to Factory"; }
		const IdentifierType id() const { return m_id; }

	private:
		IdentifierType m_id;
	};

	static ProductType* UnknownType(const IdentifierType& id)
	{
		throw Exception(id);
	}
};

template <class IdentifierType, class ProductType>
class CreateDefaultProduct
{
public:
	static std::shared_ptr<ProductType> CreateDefault()
	{
		static std::shared_ptr<ProductType> s_objPtr(new ProductType);
		return s_objPtr;
	}
};

template
<
	class AbstractProduct,
	typename IdentifierType,
	class NullProduct = AbstractProduct,	// 널 객체 또는 기본으로 설정한 객체.
	typename ProductCreator = AbstractProduct* (*)(),
	class MT_Policy = _DEFAULT_MT_POLICY,
	template <typename, class> class FactoryErrorPolicy = CreateDefaultProduct
>
class Factory : public MT_Policy
{
public:
	bool Regist(const IdentifierType& id, ProductCreator creator)
	{
		lock_block<MT_Policy> lock(this);

		return m_map.insert(ProductMap::value_type(id, creator)).second;
	}

	bool Unregist(const IdentifierType& id)
	{
		lock_block<MT_Policy> lock(this);

		return (m_map.erase(id) == 1);
	}

	bool IsExist(const IdentifierType& id)
	{
		lock_block<MT_Policy> lock(this);

		return m_map.find(id) != m_map.end();
	}

	virtual void Clear()
	{
		lock_block<MT_Policy> lock(this);

		m_map.clear();
	}

	template <typename ...Args>
	std::shared_ptr<AbstractProduct> Create(const IdentifierType& id, const Args& ...args)
	{
		lock_block<MT_Policy> lock(this);

		typename ProductMap::const_iterator it = m_map.find(id);

		if (it == m_map.end())
			return FactoryErrorPolicy<IdentifierType, NullProduct>().CreateDefault();
		else
		{
			AbstractProduct* obj = (it->second)(args...);
			std::shared_ptr<AbstractProduct> objPtr(obj);
			return objPtr;
		}
	}

private:
	using ProductMap = std::map<IdentifierType, ProductCreator>;
	ProductMap m_map;
};

template
<
	class AbstractProduct,
	typename IdentifierType,
	class NullProduct = AbstractProduct,
	typename ProductCreator = AbstractProduct* (*)(),
	class MT_Policy = _DEFAULT_MT_POLICY,
	template <typename, class> class FactoryErrorPolicy = CreateDefaultProduct
>
class FlyweightFactory 
	: protected Factory<AbstractProduct, IdentifierType, NullProduct, ProductCreator, MT_Policy, FactoryErrorPolicy>
{
public:
	template <typename ...Args>
	std::shared_ptr<AbstractProduct> RequestObj(const IdentifierType& id, const Args& ...args)
	{
		lock_block<MT_Policy> lock(this);

		auto it = m_sharedObjMap.find(id);
		if (it == m_sharedObjMap.end())
		{
			std::shared_ptr<AbstractProduct> objPtr = Factory::Create(id, args...);

			m_sharedObjMap.insert(SharedObjMap::value_type(id, objPtr));

			return objPtr;
		}

		return (it->second);
	}

	virtual void Clear() override
	{
		Factory::Clear();
		m_sharedObjMap.clear();
	}

private:
	using SharedObjMap = std::map < IdentifierType, std::shared_ptr<AbstractProduct> > ;
	SharedObjMap m_sharedObjMap;
};

