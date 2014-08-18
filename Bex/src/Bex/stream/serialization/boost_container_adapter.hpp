//////////////////////////////////////////////////////////////////////////
/// stream支持boost提供的容器的适配器

/// boost::bimap
template <class Ar, typename K1, typename K2, typename AP1, typename AP2, typename AP3>
struct adapter<Ar, boost::bimap<K1, K2, AP1, AP2, AP3> >
{
    typedef boost::bimap<K1, K2, AP1, AP2, AP3> BoostBimap;
    typedef typename BoostBimap::left_map Container;
    typedef typename Container::value_type Pair;
    typedef typename Pair::first_type first_type;
    typedef typename Pair::second_type second_type;
    typedef typename boost::remove_reference<first_type>::type first_ct;
    typedef typename boost::remove_reference<second_type>::type second_ct;
    typedef typename boost::remove_cv<first_ct>::type first_t;
    typedef typename boost::remove_cv<second_ct>::type second_t;

    inline static bool do_save(BoostBimap const& bm, Ar & ar)
    {
        if (!sized_adapter::do_save(bm.left, ar)) return false;
        if (bm.left.empty()) return true;

        BOOST_AUTO(it, bm.left.begin());
        for (; it != bm.left.end(); ++it)
            if (!ar.do_save(it->first) || !ar.do_save(it->second))
                return false;

        return true;
    }

    inline static bool do_load(BoostBimap & bm, Ar & ar)
    {
        bm.left.clear();
        std::size_t size = 0;
        if (!sized_adapter::do_load(size, ar)) return false;

        first_t f;
        second_t s;
        for (std::size_t ui = 0; ui < size; ++ui)
        {
            if (!ar.do_load(f) || !ar.do_load(s))
                return false;

            Insert()(bm.left, Pair(std::make_pair(f, s)));
        }
        return true;
    }
};