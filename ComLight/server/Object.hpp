#pragma once
#include <type_traits>
#include "../comLightClient.h"
#include "../utils/typeTraits.hpp"
#include "../Exception.hpp"

namespace ComLight
{
	namespace details
	{
		GENERATE_HAS_MEMBER( implQueryInterface );
		GENERATE_HAS_MEMBER( implAddRef );
		GENERATE_HAS_MEMBER( implRelease );
	}

	// Outer class of objects, implements IUnknown methods, also the class factory. The type argument must be your class implementing your interfaces, inherited from ObjectRoot<I>
	template<class T>
	class Object : public T
	{
	public:
		template<typename... Args>
		Object( Args&&... args )
			: T( std::forward<Args>( args )... )
		{  }

		inline virtual ~Object() override { }

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
			static_assert( details::has_member_implAddRef<T>::value, "Your object class must inherit from ComLight::ObjectRoot" );
			return T::implAddRef();
		}

		uint32_t COMLIGHTCALL Release() override
		{
			static_assert( details::has_member_implRelease<T>::value, "Your object class must inherit from ComLight::ObjectRoot" );
			const uint32_t ret = T::implRelease();
			if( 0 == ret )
			{
				T::FinalRelease();
				delete this;
			}
			return ret;
		}

		// Create a new object on the heap, store in smart pointer.
		// Catches exceptions from the constructor, as long as they are HRESULT, please don't throw std::exception there, they will crash your app.
		template<typename... Args>
		static inline HRESULT create( ComLight::CComPtr<Object<T>>& result, Args&&... args ) noexcept
		{
			CComPtr<Object<T>> ptr;
			try
			{
				ptr = new Object<T>( std::forward<Args>( args )... );
				// The RefCounter constructor creates it with ref.counter 0. But then CComPtr constructor calls AddRef so we have RC=1 after the above line.

				HRESULT hr = ptr->internalFinalConstruct();
				if( FAILED( hr ) )
					return hr;

				hr = ptr->FinalConstruct();
				if( FAILED( hr ) )
					return hr;

				ptr.swap( result );
				return S_OK;
			}
			catch( HRESULT hr )
			{
				return hr;
			}
			catch( const Exception& ex )
			{
				return ex.code();
			}
		}

		// Create a new object on the heap, return one of it's interfaces. The caller is assumed to take ownership of the new object.
		template<class I, typename... Args>
		static inline HRESULT create( I** pp, Args&&... args ) noexcept
		{
			if( pp == nullptr )
				return E_POINTER;

			static_assert( details::pointersAssignable<I, T>(), "Object::create can't cast object to the requested interface" );
			CComPtr<Object<T>> ptr;
			const HRESULT hr = create( ptr, std::forward<Args>( args )... );
			if( FAILED( hr ) )
				return hr;
			ptr.detach( pp );
			return S_OK;
		}
	};
}