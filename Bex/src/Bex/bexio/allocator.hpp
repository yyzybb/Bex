#ifndef __BEX_IO_ALLOCATOR_HPP__
#define __BEX_IO_ALLOCATOR_HPP__

//////////////////////////////////////////////////////////////////////////
// 内存分配器
/*
* @Concept:
*   Allocator
*/

// \get: new delete
#include <new>

// \get: malloc free
#include <cstdlib>

#include <boost/shared_ptr.hpp>
#include <Bex/config.hpp>

namespace Bex { namespace bexio
{
    template <typename T>
    class allocator
    {
    public:
        typedef T value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef value_type const& const_reference;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        template <typename U>
        struct rebind
        {
            typedef allocator<U> other;
        };

        //////////////////////////////////////////////////////////////////////////
        /// allocator construct
        allocator() throw() 
        {
        }

        allocator(const allocator<T>&) throw()
        {
        }
        
        template <typename U>
        allocator(const allocator<U>&) throw()
        {
        }

        template <typename U>
        allocator& operator=(const allocator<U>&) throw()
        {
            return (*this);
        }
        //////////////////////////////////////////////////////////////////////////

        pointer allocate(size_type size) const throw()
        {
            return (pointer)(::operator new(size * sizeof(value_type)));
        }

        pointer allocate(size_type size, const void *) const throw()
        {
            return (this->allocate(size));
        }

        void deallocate(void * ptr) const throw()
        {
            ::operator delete(ptr);
        }

        void deallocate(void * ptr, size_type) const throw()
        {
            this->deallocate(ptr);
        }

        template <typename ... Args>
        void construct(T * ptr, Args && ... args) const
        {
            ::new ((void*)ptr) T(std::forward<Args>(args)...);
        }

        template <class U>
        void destroy(U * ptr) const throw()
        {
            ptr->~U();
        }
    };

    template <typename T, typename U>
    inline bool operator==(::Bex::bexio::allocator<T> const&, ::Bex::bexio::allocator<U> const&)
    {
        return true;
    }

    template <typename T, typename U>
    inline bool operator!=(::Bex::bexio::allocator<T> const& lhs, ::Bex::bexio::allocator<U> const& rhs)
    {
        return !(lhs == rhs);
    }

    template <typename T, class Allocator, typename ... Args>
    T * allocate(Args && ... args)
    {
        typedef typename Allocator::template rebind<T>::other alloc_t;
        alloc_t alloc;
        T * pointer = alloc.allocate(1);
        ::new ((void*)pointer) T(std::forward<Args>(args)...);
        return pointer;
    }

    template <class Allocator, typename T>
    void deallocate(T * pointer)
    {
        typedef typename Allocator::template rebind<T>::other alloc_t;
        alloc_t alloc;
        alloc.destroy(pointer);
        alloc.deallocate(pointer);
    }

    template <typename T, class Allocator>
    struct deallocator
    {
        inline void operator()(T * pointer) const
        {
            deallocate<Allocator>(pointer);
        }
    };

    template <typename T, class Allocator, typename ... Args>
    boost::shared_ptr<T> make_shared_ptr(Args && ... args)
    {
        return boost::shared_ptr<T>(allocate<T, Allocator>(std::forward<Args>(args)...),
            deallocator<T, Allocator>(), Allocator());
    }

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_ALLOCATOR_HPP__