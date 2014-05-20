

//////////////////////////////////////////////////////////////////////////
/// stream支持数组类型的适配器


//////////////////////////////////////////////////////////////////////////
/// array
template <class Ar, typename T, int N>
struct adapter<Ar, T[N] >
{
    BOOST_STATIC_ASSERT(N > 0);

    inline static bool save(T (& arrayT)[N], Ar & ar)
    {
        if (is_optimize<T, Ar>::value)
        {
            return ar.save((char*)&arrayT[0], N * sizeof(T));
        }
        
        for (int i = 0; i < N; ++i)
            if (!ar.save(arrayT[i]))
                return false;

        return true;
    }

    inline static bool load(T (& arrayT)[N], Ar & ar)
    {
        if (is_optimize<T, Ar>::value)
        {
            return ar.load((char*)&arrayT[0], N * sizeof(T));
        }
        
        for (int i = 0; i < N; ++i)
            if (!ar.load(arrayT[i]))
                return false;

        return true;
    }
};