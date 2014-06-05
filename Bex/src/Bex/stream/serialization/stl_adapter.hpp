
//////////////////////////////////////////////////////////////////////////
/// stream支持stl容器类型的适配器

template <class Ar, class Container>
struct stl_adapter_base
{
    inline static bool save_size(Container & container, Ar & ar)
    {
        cn32 size(container.size());
        try
        {
            serialize_adl(ar, size, 0);
            return true;
        }
        catch(std::exception &)
        {
            return false;
        }
    }

    inline static bool load_size(std::size_t & out_size, Ar & ar)
    {
        cn32 size;
        try
        {
            serialize_adl(ar, size, 0);
            out_size = size.get();
            return true;
        }
        catch(std::exception &)
        {
            return false;
        }
    }
};

    
//////////////////////////////////////////////////////////////////////////
/// vector
template <class Ar, typename T, typename Alloc>
struct adapter<Ar, std::vector<T, Alloc> >
    : public stl_adapter_base<Ar, std::vector<T, Alloc> >
{
    typedef std::vector<T, Alloc> Vector;

    inline static bool save(Vector & vecT, Ar & ar)
    {
        if (!save_size(vecT, ar)) return false;
        if (vecT.empty()) return true;
            
        if (is_optimize<T, Ar>::value)
        {
            return ar.save((char*)&vecT[0], vecT.size() * sizeof(T));
        }
        
        for (std::size_t ui = 0; ui < vecT.size(); ++ui)
            if (!ar.save(vecT[ui]))
                return false;

        return true;
    }

    inline static bool load(Vector & vecT, Ar & ar)
    {
        vecT.clear();
        std::size_t size = 0;
        if (!load_size(size, ar)) return false;

        T t;
        for (std::size_t ui = 0; ui < size; ++ui)
        {
            if (!ar.load(t)) 
                return false;

            vecT.push_back(t);
        }
        return true;
    }
};

//////////////////////////////////////////////////////////////////////////
/// vector<bool>
template <class Ar, typename Alloc>
struct adapter<Ar, std::vector<bool, Alloc> >
    : public stl_adapter_base<Ar, std::vector<bool, Alloc> >
{
    typedef std::vector<bool, Alloc> Vector;

    inline static bool save(Vector & vecT, Ar & ar)
    {
        if (!save_size(vecT, ar)) return false;

        if (!vecT.empty())
        {
            char * pBase = get_base(vecT);
            std::size_t bits = vecT.size();
            std::size_t bytes = (bits >> 3) + ((bits & 0x7) > 0 ? 1 : 0);
            if (!ar.save(pBase, bytes)) return false;
        }
        
        return true;
    }

    inline static bool load(Vector & vecT, Ar & ar)
    {
        std::size_t bits;
        if (!load_size(bits, ar)) return false;

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
                if (!ar.load(pNext, ls)) return false;

                bytes -= ls;
                size += ls;
            }
        }

        return true;
    }

    inline static char * get_base(Vector & vecT)
    {
#if defined(_MSC_VER)
        return (char*)(vecT.begin()._Myptr);
#endif //_MSC_VER
    }
};

//////////////////////////////////////////////////////////////////////////
/// list
template <class Ar, typename T, typename Alloc>
struct adapter<Ar, std::list<T, Alloc> >
    : public stl_adapter_base<Ar, std::list<T, Alloc> >
{
    typedef std::list<T, Alloc> List;

    inline static bool save(List & listT, Ar & ar)
    {
        if (!save_size(listT, ar)) return false;
        if (listT.empty()) return true;

        BOOST_AUTO(it, listT.begin());
        for (; it != listT.end(); ++it)
            if (!ar.save(*it))
                return false;

        return true;
    }

    inline static bool load(List & listT, Ar & ar)
    {
        listT.clear();
        std::size_t size = 0;
        if (!load_size(size, ar)) return false;

        T t;
        for (std::size_t ui = 0; ui < size; ++ui)
        {
            if (!ar.load(t))
                return false;

            listT.push_back(t);
        }
        return true;
    }
};

//////////////////////////////////////////////////////////////////////////
/// string
template <class Ar, typename T, typename Traits, typename Alloc>
struct adapter<Ar, std::basic_string<T, Traits, Alloc> >
    : public stl_adapter_base<Ar, std::basic_string<T, Traits, Alloc> >
{
    typedef std::basic_string<T, Traits, Alloc> String;

    inline static bool save(String & stringT, Ar & ar)
    {
        BOOST_STATIC_ASSERT((boost::is_same<T, char>::value || boost::is_same<T, wchar_t>::value));

        if (!save_size(stringT, ar)) return false;
        if (stringT.empty()) return true;
        return ar.save((char *)&stringT[0], stringT.size() * sizeof(T));
    }

    inline static bool load(String & stringT, Ar & ar)
    {
        BOOST_STATIC_ASSERT((boost::is_same<T, char>::value || boost::is_same<T, wchar_t>::value));

        stringT.clear();
        std::size_t size = 0;
        if (!load_size(size, ar)) return false;

        T buf[128];
        while (size)
        {
            std::size_t ls = (std::min)(size, sizeof(buf) / sizeof(T));
            if (!ar.load((char *)&buf[0], ls * sizeof(T))) return false;
            size -= ls;
            stringT.append(&buf[0], ls);
        }
        return true;
    }
};

//////////////////////////////////////////////////////////////////////////
/// map
template <class Ar, typename K, typename T, typename Pr, typename Alloc>
struct adapter<Ar, std::map<K, T, Pr, Alloc> >
    : public stl_adapter_base<Ar, std::map<K, T, Pr, Alloc> >
{
    typedef std::map<K, T, Pr, Alloc> Map;
    
    inline static bool save(Map & mapT, Ar & ar)
    {
        if (!save_size(mapT, ar)) return false;
        if (mapT.empty()) return true;

        BOOST_AUTO(it, mapT.begin());
        for (; it != mapT.end(); ++it)
            if (!ar.save(it->first) || !ar.save(it->second))
                return false;

        return true;
    }

    inline static bool load(Map & mapT, Ar & ar)
    {
        mapT.clear();
        std::size_t size = 0;
        if (!load_size(size, ar)) return false;

        K k;
        T t;
        for (std::size_t ui = 0; ui < size; ++ui)
        {
            if (!ar.load(k) || !ar.load(t))
                return false;

            mapT[k] = t;
        }

        return true;
    }
};

//////////////////////////////////////////////////////////////////////////
/// pair
template <class Ar, typename T1, typename T2>
struct adapter<Ar, std::pair<T1, T2> >
    : public stl_adapter_base<Ar, std::pair<T1, T2> >
{
    typedef std::pair<T1, T2> Pair;

    inline static bool save(Pair & pairT, Ar & ar)
    {
        return ar.save(pairT.first) && ar.save(pairT.second);
    }

    inline static bool load(Pair & pairT, Ar & ar)
    {
        return ar.load(pairT.first) && ar.load(pairT.second);
    }
};