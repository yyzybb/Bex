#ifndef __BEX_IO_INTRUSIVE_LIST_HPP__
#define __BEX_IO_INTRUSIVE_LIST_HPP__

//////////////////////////////////////////////////////////////////////////
// 侵入式双向链表容器

#include <boost/operators.hpp>
#include <boost/utility/swap.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/static_assert.hpp>

namespace Bex { namespace bexio
{
    struct default_tag {};

    template <typename T, typename Tag = default_tag>
    class intrusive_list
    {
    public:
        typedef intrusive_list<T, Tag> this_type;
        typedef T value_type;
        typedef T* pointer;

        struct hook
        {
            hook * prev;
            hook * next;

            hook()
                : prev(0), next(0)
            {}
        };

        BOOST_STATIC_ASSERT((boost::is_base_of<hook, T>::value));

        struct iterator
            : boost::bidirectional_iteratable<iterator, pointer>
        {
            hook * pointer_;

            explicit iterator(hook * ptr)
                : pointer_(ptr)
            {
            }

            iterator operator++()
            {
                if (pointer_ && pointer_->next)
                    pointer_ = pointer_->next;
                return (*this);
            }

            iterator operator--()
            {
                if (pointer_ && pointer_->prev)
                    pointer_ = pointer_->prev;
                return (*this);
            }

            T& operator*()
            {
                if (invalid())
                    throw std::logic_error("intrusive_list iterator dereference error!");

                return *static_cast<T*>(pointer_);
            }

            bool invalid() const
            {
                return !pointer_ || !pointer_->next || !pointer_->prev;
            }

            friend bool operator==(iterator const& lhs, iterator const& rhs)
            {
                if (lhs.invalid() && rhs.invalid())
                    return true;

                return (lhs.pointer_ == rhs.pointer_);
            }
        };

    public:
        intrusive_list()
        {
            reset();
        }

        iterator begin()
        {
            return iterator(front_.next);
        }

        iterator end()
        {
            return iterator(&back_);
        }

        iterator find(pointer ptr)
        {
            return iterator(ptr);
        }

        void swap(this_type & other)
        {
            boost::swap(front_, other.front_);
            boost::swap(back_, other.back_);
        }

        void push_back(pointer ptr)
        {
            hook * hook_ptr = static_cast<hook*>(ptr);
            hook_ptr->prev = back_.prev;
            hook_ptr->next = &back_;
            back_.prev->next = hook_ptr;
            back_.prev = hook_ptr;
        }

        void push_back(this_type & other)
        {
            back_.prev->next = other.front_.next;
            back_.prev = other.back_.prev;
            other.reset();
        }

        void push_front(pointer ptr)
        {
            hook * hook_ptr = static_cast<hook*>(ptr);
            hook_ptr->prev = &front_;
            hook_ptr->next = front_.next;
            front_.next->prev = hook_ptr;
            front_.next = hook_ptr;
        }

        void push_front(this_type & other)
        {
            front_.next->prev = other.back_.prev;
            front_.next = other.front_.next;
            other.reset();
        }

        static void erase(iterator where)
        {
            if (!where.invalid())
            {
                where.pointer_->next.prev = where.pointer_->prev;
                where.pointer_->prev.next = where.pointer_->next;
                where.pointer_->next = where.pointer_->prev = 0;
            }
        }

        static void erase(pointer ptr)
        {
            ptr->next.prev = ptr->prev;
            ptr->prev.next = ptr->next;
            ptr->next = ptr->prev = 0;
        }

        bool empty() const
        {
            return (front_.next == &back_);
        }

        std::size_t size() const
        {
            std::size_t result = 0;
            for (hook * pos = front_.next; pos->next; pos = pos->next, ++result)
                ;
            return result;
        }

        void clear()
        {
            while (!empty())
                erase(begin());
        }

    private:
        void reset()
        {
            front_.next = &back_;
            back_.prev = &front_;
        }

    private:
        hook front_;
        hook back_;
    };

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_INTRUSIVE_LIST_HPP__