#pragma once
#include "../ComLight/hresult.h"

namespace details
{
	constexpr size_t defaultAlign = 16;

	// Unfortunately, VC++ doesn't support std::aligned_alloc from C++/17 spec:
	// https://developercommunity.visualstudio.com/content/problem/468021/c17-stdaligned-alloc缺失.html

	// Allocate aligned block of memory
	inline void* alignedMalloc( size_t size, size_t alignment = defaultAlign )
	{
#ifdef _MSC_VER
		return _aligned_malloc( size, alignment );
#else
		return aligned_alloc( alignment, size );
#endif
	}

	// Free aligned block of memory
	inline void alignedFree( void* pointer )
	{
#ifdef _MSC_VER
		_aligned_free( pointer );
#else
		free( pointer );
#endif
	}

	inline void* alignedRealloc( void *memblock, size_t size, size_t alignment = defaultAlign )
	{
#ifdef _MSC_VER
		return _aligned_realloc( memblock, size, alignment );
#else
		// Unfortunately, aligned_realloc is only available on Windows. Implementing a workaround:
		// https://stackoverflow.com/a/9078627/126995
		void* const reallocated = realloc( memblock, size );
		const bool isAligned = ( 0 == ( ( (size_t)reallocated ) % alignment ) );
		if( isAligned || nullptr == reallocated )
			return reallocated;

		void* const copy = aligned_alloc( alignment, size );
		if( nullptr == copy )
		{
			free( reallocated );
			return nullptr;
		}
		memcpy( copy, reallocated, size );
		free( reallocated );
		return copy;
#endif
	}

	struct AlignedDeleter
	{
		inline void operator()( void* pointer )
		{
			alignedFree( pointer );
		}
	};
}

// Similar to std::vector, doesn't call constructors, alignes memory by 16.
template<class E>
class DynamicArray
{
	E* m_pointer = nullptr;
	uint32_t m_length = 0;
	uint32_t m_capacity = 0;

public:
	DynamicArray() = default;
	~DynamicArray() = default;

	bool resize( uint32_t count )
	{
		if( count == m_length )
			return false;
		using namespace details;

		if( count <= 0 )
		{
			alignedFree( m_pointer );
			m_pointer = nullptr;
			m_length = m_capacity = 0;
			return true;
		}

		if( count <= m_capacity )
		{
			m_length = count;
			return true;
		}

		const size_t newSize = count * sizeof( E );
		if( nullptr == m_pointer )
			m_pointer = (E*)alignedMalloc( newSize );
		else
			m_pointer = (E*)alignedRealloc( m_pointer, newSize );
		if( !m_pointer )
			throw E_OUTOFMEMORY;
		m_length = m_capacity = count;
		return true;
	}

	operator bool() const { return nullptr != m_pointer; }
	operator E*( ) { return m_pointer; }
	E* data() { return m_pointer; }
	E* operator->() { return m_pointer; }
};

// Similar to std::unique_ptr<E> but support more operators for interop with C code, e.g. automatic cast to raw pointers
template<class E>
class unique_ptr
{
	E* m_pointer = nullptr;

public:

	unique_ptr() = default;
	unique_ptr( const unique_ptr<E>& ) = delete;
	unique_ptr( unique_ptr<E>&& that ) :
		m_pointer(that.m_pointer)
	{
		that.m_pointer = nullptr;
	}
	unique_ptr( E* ptr ) :
		m_pointer( ptr ) { }
	~unique_ptr()
	{
		delete m_pointer;
	}

	operator E*( ) { return m_pointer; }
	E* operator->() { return m_pointer; }

	void operator=( const unique_ptr<E>& that ) = delete;
	void operator=( unique_ptr<E>&& that )
	{
		std::swap( m_pointer, that.m_pointer );
	}

	void reset()
	{
		delete m_pointer;
		m_pointer = nullptr;
	}
};

template<class E>
inline unique_ptr<E> makeUnique()
{
	try
	{
		return std::move( unique_ptr<E>( new E() ) );
	}
	catch( const std::bad_alloc& )
	{
		throw E_OUTOFMEMORY;
	}
}