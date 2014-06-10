#ifndef __BEX_UTILITY_LEXICAL_CAST_HPP__
#define __BEX_UTILITY_LEXICAL_CAST_HPP__

#include <boost/lexical_cast.hpp>

namespace Bex
{
    template <typename Target, typename Source>
    Target lexical_cast_noexcept(Source const& arg, Target const& def)
    {
        try
        {
            return boost::lexical_cast<Target>(arg);
        }
        catch (boost::bad_lexical_cast&)
        {
            return def;
        }
    }

    template <typename Target, typename Source>
    inline Target lexical_cast_noexcept_d(Source const& arg)
    {
        return lexical_cast_noexcept(arg, Target());
    }

    template <typename Target, typename CharType>
    Target lexical_cast_noexcept(CharType const* arg, std::size_t len, Target const& def)
    {
        try
        {
            return boost::lexical_cast<Target>(arg, len);
        }
        catch (boost::bad_lexical_cast&)
        {
            return def;
        }
    }

    template <typename Target, typename CharType>
    inline Target lexical_cast_noexcept_d(CharType const* arg, std::size_t len)
    {
        return lexical_cast_noexcept(arg, len, Target());
    }

} //namespace Bex

#endif //__BEX_UTILITY_LEXICAL_CAST_HPP__