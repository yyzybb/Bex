//////////////////////////////////////////////////////////////////////////
/// stream支持stl容器类型的适配器

// size
struct sized_adapter
{
    template <class Container, class Ar>
    inline static bool do_save(Container const& container, Ar & ar)
    {
        cn32 size(container.size());
        serialize_adl(ar, size);
        return ar.good();
    }

    template <class Ar>
    inline static bool do_load(std::size_t & out_size, Ar & ar)
    {
        cn32 size;
        serialize_adl(ar, size);
        out_size = size.get();
        return ar.good();
    }
};

//////////////////////////////////////////////////////////////////////////
/// Inserters
struct Pushback
{
    template <class Container, typename ValueType>
    inline void operator()(Container & c, ValueType const& v) const
    { c.push_back(v); }
};
struct Insert
{
    template <class Container, typename ValueType>
    inline void operator()(Container & c, ValueType const& v) const
    { c.insert(v); }
};

/// container
template <class Ar, class Container, class Inserter>
struct container_adapter
{
    inline static bool do_save(Container const& c, Ar & ar)
    {
        if (!sized_adapter::do_save(c, ar)) return false;
        if (c.empty()) return true;

        BOOST_AUTO(it, c.begin());
        for (; it != c.end(); ++it)
            if (!ar.do_save(*it))
                return false;

        return true;
    }

    inline static bool do_load(Container & c, Ar & ar)
    {
        c.clear();
        std::size_t size = 0;
        if (!sized_adapter::do_load(size, ar)) return false;

        typename Container::value_type v;
        for (std::size_t ui = 0; ui < size; ++ui)
        {
            if (!ar.do_load(v))
                return false;

            Inserter()(c, v);
        }
        return true;
    }
};
    
//////////////////////////////////////////////////////////////////////////
/// vector
template <class Ar, typename T, typename Alloc>
struct adapter<Ar, std::vector<T, Alloc> >
{
    typedef std::vector<T, Alloc> Vector;

    inline static bool do_save(Vector const& vecT, Ar & ar)
    {
        if (!sized_adapter::do_save(vecT, ar)) return false;
        if (vecT.empty()) return true;
            
        if (is_optimize<T, Ar>::value)
            return ar.do_save((char*)&vecT[0], vecT.size() * sizeof(T));
        
        for (std::size_t ui = 0; ui < vecT.size(); ++ui)
            if (!ar.do_save(vecT[ui]))
                return false;

        return true;
    }

    inline static bool do_load(Vector & vecT, Ar & ar)
    {
        vecT.clear();
        std::size_t size = 0;
        if (!sized_adapter::do_load(size, ar)) return false;

        std::size_t count = size;
        while (count)
        {
            std::size_t once = std::min<std::size_t>(count, 128);
            count -= once;
            std::size_t start = vecT.size();
            vecT.resize(vecT.size() + once);
            if (is_optimize<T, Ar>::value)
            {
                if (!ar.do_load((char*)&vecT[start], once * sizeof(T)))
                    return false;
            }
            else
                for (std::size_t i = 0; i < once; ++i)
                    if (!ar.do_load(vecT[start + i]))
                        return false;
        }

        return true;
    }
};

//////////////////////////////////////////////////////////////////////////
/// vector<bool>
template <class Ar, typename Alloc>
struct adapter<Ar, std::vector<bool, Alloc> >
{
    typedef std::vector<bool, Alloc> Vector;

    inline static bool do_save(Vector const& vecT, Ar & ar)
    {
        if (!sized_adapter::do_save(vecT, ar)) return false;
        if (vecT.empty()) return true;

        unsigned char byte[1024] = {};
        for (std::size_t i = 0; i < vecT.size(); ++i)
        {
            if (vecT[i])
                byte[(i >> 3) & (sizeof(byte) - 1)] |= (unsigned char)(1 << (i & 0x7));

            if (((i + 1) & (8 * sizeof(byte) - 1)) == 0)
            {
                if (!ar.do_save((char*)byte, sizeof(byte)))
                    return false;

                memset(byte, 0, sizeof(byte));
            }
        }

        std::size_t avail = bit_to_byte(vecT.size() & (8 * sizeof(byte) - 1));
        if (avail)
            return ar.do_save((char*)byte, avail);

        return true;
    }

    inline static bool do_load(Vector & vecT, Ar & ar)
    {
        std::size_t bit_c;
        if (!sized_adapter::do_load(bit_c, ar)) return false;

        vecT.clear();
        if (!bit_c) return true;

        unsigned char byte[1024] = {};
        while (bit_c)
        {
            std::size_t n = (std::min<std::size_t>)(1024 * 8, bit_c);
            if (!ar.do_load((char*)byte, bit_to_byte(n))) return false;
            bit_c -= n;

            vecT.reserve(vecT.size() + n);
            for (std::size_t i = 0; i < n; ++i)
                vecT.push_back((byte[i >> 3] & (1 << (i & 0x7))) != 0);
        }

        return true;
    }

    inline static std::size_t bit_to_byte(std::size_t bit_c)
    {
        return (bit_c >> 3) + ((bit_c & 0x7) ? 1 : 0);
    }
};

/*
template <class Ar, typename Alloc>
struct adapter<Ar, std::vector<bool, Alloc> >
{
    typedef std::vector<bool, Alloc> Vector;

    inline static bool do_save(Vector const& vecT, Ar & ar)
    {
        if (!sized_adapter::do_save(vecT, ar)) return false;

        if (!vecT.empty())
        {
            char * pBase = get_base(vecT);
            std::size_t bits = vecT.size();
            std::size_t bytes = (bits >> 3) + ((bits & 0x7) > 0 ? 1 : 0);
            if (!ar.do_save(pBase, bytes)) return false;
        }
        
        return true;
    }

    inline static bool do_load(Vector & vecT, Ar & ar)
    {
        std::size_t bits;
        if (!sized_adapter::do_load(bits, ar)) return false;

        vecT.clear();
        if (bits)
        {
            std::size_t bytes = (bits >> 3) + ((bits & 0x7) > 0 ? 1 : 0);
            const std::size_t once = (bytes > 4096 ? 4096 : 128);
            std::size_t size = 0;
            while (bytes)
            {
                std::size_t ls = (std::min)(bytes, once);
                vecT.resize( (std::min)((ls + size) * 8, bits) );
                char * pNext = get_base(vecT) + size;
                if (!ar.do_load(pNext, ls)) return false;

                bytes -= ls;
                size += ls;
            }
        }

        return true;
    }

    inline static char * get_base(Vector const& vecT)
    {
#if defined(_MSC_VER)
        return (char*)(vecT.begin()._Myptr);
#endif //_MSC_VER
    }
};
*/

//////////////////////////////////////////////////////////////////////////
/// string
template <class Ar, typename T, typename Traits, typename Alloc>
struct adapter<Ar, std::basic_string<T, Traits, Alloc> >
{
    typedef std::basic_string<T, Traits, Alloc> String;

    inline static bool do_save(String const& stringT, Ar & ar)
    {
        BOOST_STATIC_ASSERT((boost::is_same<T, char>::value || boost::is_same<T, wchar_t>::value));

        if (!sized_adapter::do_save(stringT, ar)) return false;
        if (stringT.empty()) return true;
        return ar.do_save((char *)&stringT[0], stringT.size() * sizeof(T));
    }

    inline static bool do_load(String & stringT, Ar & ar)
    {
        BOOST_STATIC_ASSERT((boost::is_same<T, char>::value || boost::is_same<T, wchar_t>::value));

        stringT.clear();
        std::size_t size = 0;
        if (!sized_adapter::do_load(size, ar)) return false;

        T buf[128];
        while (size)
        {
            std::size_t ls = (std::min)(size, sizeof(buf) / sizeof(T));
            if (!ar.do_load((char *)&buf[0], ls * sizeof(T))) return false;
            size -= ls;
            stringT.append(&buf[0], ls);
        }
        return true;
    }
};

//////////////////////////////////////////////////////////////////////////
/// list
template <class Ar, typename T, typename Alloc>
struct adapter<Ar, std::list<T, Alloc> >
    : public container_adapter<Ar, std::list<T, Alloc>, Pushback>
{};

//////////////////////////////////////////////////////////////////////////
/// deque
template <class Ar, typename T, typename Alloc>
struct adapter<Ar, std::deque<T, Alloc> >
    : public container_adapter<Ar, std::deque<T, Alloc>, Pushback>
{};

//////////////////////////////////////////////////////////////////////////
/// set
template <class Ar, typename T, typename Pr, typename Alloc>
struct adapter<Ar, std::set<T, Pr, Alloc> >
    : public container_adapter<Ar, std::set<T, Pr, Alloc>, Insert>
{};

//////////////////////////////////////////////////////////////////////////
/// multiset
template <class Ar, typename T, typename Pr, typename Alloc>
struct adapter<Ar, std::multiset<T, Pr, Alloc> >
    : public container_adapter<Ar, std::multiset<T, Pr, Alloc>, Insert>
{};

//////////////////////////////////////////////////////////////////////////
/// map
template <class Ar, typename K, typename T, typename Pr, typename Alloc>
struct adapter<Ar, std::map<K, T, Pr, Alloc> >
    : public container_adapter<Ar, std::map<K, T, Pr, Alloc>, Insert>
{};

//////////////////////////////////////////////////////////////////////////
/// multimap
template <class Ar, typename K, typename T, typename Pr, typename Alloc>
struct adapter<Ar, std::multimap<K, T, Pr, Alloc> >
    : public container_adapter<Ar, std::multimap<K, T, Pr, Alloc>, Insert>
{};

//////////////////////////////////////////////////////////////////////////
/// pair
template <class Ar, typename T1, typename T2>
struct adapter<Ar, std::pair<T1, T2> >
{
    typedef std::pair<T1, T2> Pair;

    inline static bool do_save(Pair & pairT, Ar & ar)
    {
        return ar.do_save(pairT.first) && ar.do_save(pairT.second);
    }

    inline static bool do_load(Pair & pairT, Ar & ar)
    {
        return ar.do_load((typename boost::remove_cv<T1>::type&)pairT.first) 
            && ar.do_load((typename boost::remove_cv<T2>::type&)pairT.second);
    }
};

#if defined(_MSC_VER)
//////////////////////////////////////////////////////////////////////////
/// hash_map
template <class Ar, typename K, typename T, typename Tr, typename Alloc>
struct adapter<Ar, stdext::hash_map<K, T, Tr, Alloc> >
    : public container_adapter<Ar, stdext::hash_map<K, T, Tr, Alloc>, Insert>
{};
#endif //defined(_MSC_VER)


//////////////////////////////////////////////////////////////////////////
/// unordered_map
template <class Ar, typename K, typename T, typename H, typename Eq, typename Alloc>
struct adapter<Ar, std::unordered_map<K, T, H, Eq, Alloc> >
    : public container_adapter<Ar, std::unordered_map<K, T, H, Eq, Alloc>, Insert>
{};
//////////////////////////////////////////////////////////////////////////
/// unordered_multimap
template <class Ar, typename K, typename T, typename H, typename Eq, typename Alloc>
struct adapter<Ar, std::unordered_multimap<K, T, H, Eq, Alloc> >
    : public container_adapter<Ar, std::unordered_multimap<K, T, H, Eq, Alloc>, Insert>
{};

//////////////////////////////////////////////////////////////////////////
/// unordered_set
template <class Ar, typename T, typename H, typename Eq, typename Alloc>
struct adapter<Ar, std::unordered_set<T, H, Eq, Alloc> >
    : public container_adapter<Ar, std::unordered_set<T, H, Eq, Alloc>, Insert>
{};
//////////////////////////////////////////////////////////////////////////
/// unordered_multiset
template <class Ar, typename T, typename H, typename Eq, typename Alloc>
struct adapter<Ar, std::unordered_multiset<T, H, Eq, Alloc> >
    : public container_adapter<Ar, std::unordered_multiset<T, H, Eq, Alloc>, Insert>
{};

//////////////////////////////////////////////////////////////////////////
/// std::tuple
template <class Ar, typename ... CArgs>
struct adapter<Ar, std::tuple<CArgs...> >
{
    typedef std::tuple<CArgs...> Tuple;

private:
    template <int I, typename ... Args>
    inline static typename boost::enable_if_c<(I < sizeof...(Args)), bool>::type
        do_save_one(std::tuple<Args...> const& c, Ar & ar, int index)
    {
        if (!index) return ar.do_save(std::get<I>(c));
        return do_save_one<I + 1>(c, ar, --index);
    }

    template <int I, typename ... Args>
    inline static typename boost::disable_if_c<(I < sizeof...(Args)), bool>::type
        do_save_one(std::tuple<Args...> const&, Ar &, int)
    {
        return false;
    }

    template <int I, typename ... Args>
    inline static typename boost::enable_if_c<(I < sizeof...(Args)), bool>::type
        do_load_one(std::tuple<Args...> & c, Ar & ar, int index)
    {
        if (!index) return ar.do_load(std::get<I>(c));
        return do_load_one<I + 1>(c, ar, --index);
    }

    template <int I, typename ... Args>
    inline static typename boost::disable_if_c<(I < sizeof...(Args)), bool>::type
        do_load_one(std::tuple<Args...> &, Ar &, int)
    {
        return false;
    }

public:
    inline static bool do_save(Tuple const& c, Ar & ar)
    {
        for (int i = 0; i < std::tuple_size<Tuple>::value; ++i)
            if (!do_save_one<0>(c, ar, i))
                return false;

        return true;
    }

    template <typename T, typename ... Args>
    inline static bool do_load(std::tuple<T, Args...> & c, Ar & ar)
    {
        for (int i = 0; i < std::tuple_size<Tuple>::value; ++i)
            if (!do_load_one<0>(c, ar, i))
                return false;

        return true;
    }
};