#ifndef __BEX_UTILITY_RANGE_HPP__
#define __BEX_UTILITY_RANGE_HPP__

#include <type_traits>

//////////////////////////////////////////////////////////////////////////
/// range, 主要用于range for的辅助工具. @by C++11

namespace Bex
{
    template <typename T = std::size_t>
    struct range_t
    {
        static_assert(std::is_integral<T>::value, "range value_type must be integral!");
        typedef T value_type;

        value_type const min_;
        value_type const max_;

        class iterator
        {
            friend range_t;
            value_type value_;

            explicit iterator(value_type value)
                : value_(value)
            {}

        public:
            value_type const& operator*() const
            {
                return value_;
            }

            iterator& operator++()
            {
                ++value_;
                return (*this);
            }

            iterator operator++(int)
            {
                iterator tmp(*this);
                ++value_;
                return (tmp);
            }

            inline bool operator==(iterator const& other) const
            {
                return value_ == other.value_;
            }

            inline bool operator!=(iterator const& other) const
            {
                return !(*this == other);
            }
        };

        explicit range_t(T max)
            : min_(), max_(max)
        {}

        explicit range_t(T min, T max)
            : min_(min), max_(max)
        {}

        iterator begin() const
        {
            return iterator(min_);
        }

        iterator end() const
        {
            return iterator(max_);
        }

        T size() const
        {
            return (max_ - min_);
        }

        bool overflow() const
        {
            return (min_ >= max_);
        }
    };

    typedef range_t<> range;
} //namespace Bex


#endif //__BEX_UTILITY_RANGE_HPP__