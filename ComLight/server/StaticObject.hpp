#pragma once

namespace ComLight
{
	// Outer class of objects, implements IUnknown for statically-allocated ones. The type argument must be your class implementing your interfaces, inherited from ObjectRoot<I>
	template<class T>
	class StaticObject : public T
	{
	public:
		template <typename ...Args>
		StaticObject( Args && ...args ):
			T( std::forward<Args>( args )... )
		{ }
		inline virtual ~StaticObject() override { }

		// Implement IUnknown methods
		HRESULT COMLIGHTCALL QueryInterface( REFIID riid, void **ppvObject ) override
		{
			static_assert( details::has_member_implQueryInterface<T>::value, "Your object class must inherit from ComLight::ObjectRoot" );

			if( nullptr == ppvObject )
				return E_POINTER;

			if( T::implQueryInterface( riid, ppvObject ) )
				return S_OK;
			if( T::queryExtraInterfaces( riid, ppvObject ) )
				return S_OK;

			if( riid == IUnknown::iid() )
			{
				ComLight::IUnknown* unk = T::getUnknown();
				unk->AddRef();
				*ppvObject = unk;
				return S_OK;
			}
			return E_NOINTERFACE;
		}

		uint32_t COMLIGHTCALL AddRef() override
		{
			return 1;
		}

		uint32_t COMLIGHTCALL Release() override
		{
			return 1;
		}
	};
}