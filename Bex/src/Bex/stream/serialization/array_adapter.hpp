//////////////////////////////////////////////////////////////////////////
/// stream支持数组类型的适配器

// array size
template <class Ar, typename Array>
struct array_adapter
{
    template <typename A>
    struct vt
    {
        typedef typename A::value_type type;
    };

    template <typename T, std::size_t N>
    struct vt<T[N]>
    {
        typedef T type;
        static const std::size_t size = N;
    };

    template <typename A>
    inline static typename boost::enable_if<boost::is_array<A>, std::size_t>::type 
        size(A const& arr)
    {
        return vt<A>::size;
    }

    template <typename A>
    inline static typename boost::disable_if<boost::is_array<A>, std::size_t>::type 
        size(A const& arr)
    {
        return arr.size();
    }

    inline static bool do_save(Array const& arrayT, Ar & ar)
    {
        std::size_t N = size(arrayT);
        typedef typename vt<Array>::type T;
        if (is_optimize<T, Ar>::value)
            return ar.do_save((char*)&arrayT[0], N * sizeof(T));
        
        for (std::size_t i = 0; i < N; ++i)
            if (!ar.do_save(arrayT[i]))
                return false;

        return true;
    }

    inline static bool do_load(Array & arrayT, Ar & ar)
    {
        std::size_t N = size(arrayT);
        typedef typename vt<Array>::type T;
        if (is_optimize<T, Ar>::value)
            return ar.do_load((char*)&arrayT[0], N * sizeof(T));
        
        for (std::size_t i = 0; i < N; ++i)
            if (!ar.do_load(arrayT[i]))
                return false;

        return true;
    }
};

//////////////////////////////////////////////////////////////////////////
/// array
template <class Ar, typename T, int N>
struct adapter<Ar, T[N]>
    : public array_adapter<Ar, T[N]>
{
    BOOST_STATIC_ASSERT(N > 0);
};

/// std::array
template <class Ar, typename T, std::size_t N>
struct adapter<Ar, std::array<T, N> >
    : public array_adapter<Ar, std::array<T, N> >
{
    BOOST_STATIC_ASSERT(N > 0);
};

/// boost::array
template <class Ar, typename T, std::size_t N>
struct adapter<Ar, boost::array<T, N> >
    : public array_adapter<Ar, boost::array<T, N> >
{
    BOOST_STATIC_ASSERT(N > 0);
};
