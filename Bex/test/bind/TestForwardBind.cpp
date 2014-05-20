#include "TestPCH.h"
#if defined(BEX_SUPPORT_CXX11)

#include <Bex/bind.hpp>
#include <functional>
#include <Bex/timer/timer.hpp>
using namespace Bex;


static int g_Aconstruct = 0;
static int g_Acopy = 0;
static int g_Amove = 0;
static int g_MoveAmove = 0;
static int g_MoveAcopy = 0;

void reset_counter()
{
    g_Aconstruct = g_Acopy = g_Amove = g_MoveAmove = g_MoveAcopy = 0;
}

struct A
{
    explicit A(int i) : m_i(i) { ++g_Aconstruct; /*Dump("A::construct " << m_i);*/ };
    A(const A& other) : m_i(other.m_i) { ++g_Acopy; /*Dump("A::copy " << m_i);*/ }
    A(A&& other) : m_i(other.m_i) { other.m_i = 0; ++g_Amove; /*Dump("A::move " << m_i);*/ }
    ~A() = default;

    mutable int m_i{ 0 };
    int f() const
    {
        //DumpX(m_i);
        return (m_i *= 10);
    }

    int mem_func(int a, int b)
    {
        return a + b + m_i;
    }

    int mem_func_const(int a, int b) const
    {
        return a + b + m_i;
    }

    int mem_func_volatile(int a, int b) volatile
    {
        return a + b + m_i;
    }
};


int func(A construct, A copy, A & lref, A const& clref, A && rref)
{
    construct.f();
    copy.f();
    lref.f();
    clref.f();
    rref.f();
    A temp(std::move(rref));
    return 0;
}

struct object_func
{
    inline void operator()(int& a, int b) const
    {
        a += b;
    }
};

struct MoveA
{
    MoveA() = default;
    MoveA(MoveA&&) { ++g_MoveAmove; /*Dump("MoveA::move");*/ }
    ~MoveA() = default;

    /// TODO: 支持vs2013 ctp
    MoveA(MoveA const&)
    {
        ++g_MoveAcopy;
    }
    //MoveA& operator=(MoveA const&) = default;
    //MoveA& operator=(MoveA&&) = default;
};

int move_func(MoveA const& a)
{
    return 0;
}

void void_move_func(MoveA const& a)
{
}

void test()
{
    {
        /// 自由函数转发测试 (形参: 值传递, 左值引用, 常量左值引用, 右值引用)
        reset_counter();
        A a(1), b(2), c(3), d(4);
        auto f = Bex::bind(&func, Bex::_1, Bex::_2, Bex::_3, /*Bex::_5*/std::ref(c), Bex::_4);
        f(A(9), a, b, /*c,*/ std::move(d));
        BOOST_CHECK(a.m_i == 1);
        BOOST_CHECK(b.m_i == 20);
        BOOST_CHECK(c.m_i == 30);
        BOOST_CHECK(d.m_i == 0);

        BOOST_CHECK(g_Aconstruct == 5);
        BOOST_CHECK(g_Acopy == 1);
        BOOST_CHECK(g_Amove == 2);
    }

    //////////////////////////////////////////////////////////////////////////
    /// @{ 非静态成员函数 (cv)
    {
        reset_counter();
        A a(1);
        auto f = Bex::bind(&A::mem_func, Bex::_1, 1, Bex::_2);
        BOOST_CHECK(f(a, 2) == 4);
        BOOST_CHECK(g_Aconstruct == 1);
        BOOST_CHECK(g_Acopy == 0);
        BOOST_CHECK(g_Amove == 0);
    }
    {
        reset_counter();
        const A a(1);
        auto f = Bex::bind(&A::mem_func_const, Bex::_1, 1, Bex::_2);
        BOOST_CHECK(f(a, 2) == 4);
        BOOST_CHECK(g_Aconstruct == 1);
        BOOST_CHECK(g_Acopy == 0);
        BOOST_CHECK(g_Amove == 0);
    }
    {
        reset_counter();
        volatile A a(1);
        auto f = Bex::bind(&A::mem_func_volatile, Bex::_1, 1, Bex::_2);
        BOOST_CHECK(f(a, 2) == 4);
        BOOST_CHECK(g_Aconstruct == 1);
        BOOST_CHECK(g_Acopy == 0);
        BOOST_CHECK(g_Amove == 0);
    }
    {
        reset_counter();
        A a(2);
        auto f = Bex::bind(&A::f, std::ref(a));
        f();
    }
    /// @}
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// @{ 仿函数
    {
        object_func obj;
        auto f = Bex::bind(obj, Bex::_1, 3);
        int r = 1;
        f(r);
        BOOST_CHECK(r == 4);
    }
    /// @}
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// @{ 绑定非静态成员变量
    {
        reset_counter();
        A a(2);
        auto f = Bex::bind(&A::m_i, Bex::_1);
        int r = f(a);
        BOOST_CHECK(r == 2);
        BOOST_CHECK(g_Aconstruct == 1);
        BOOST_CHECK(g_Acopy == 0);
        BOOST_CHECK(g_Amove == 0);
    }
    /// @}
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// @{ 移动语义绑定参数
    {
        reset_counter();
        MoveA a;
        auto f = Bex::bind(&move_func, std::move(a));
        f();
        BOOST_CHECK(g_MoveAmove == 1);
    }
    {
        reset_counter();
        MoveA a;
        auto f = Bex::bind(&void_move_func, std::move(a));
        f();
        BOOST_CHECK(g_MoveAmove == 1);
    }
    /// @}
    //////////////////////////////////////////////////////////////////////////
}

#if defined(_DEBUG)
static const int optimized_multi = 1;
#else
static const int optimized_multi = 10;
#endif

void empty_function() {}
void multi_light_function(int, double, float, void*) {}

struct weight_param 
{ 
    char x[4096]; 

    weight_param() {}
    ~weight_param() {}
};

void weight_ref_function(weight_param &) {}
void weight_cref_function(weight_param const&) {}
void weight_function(weight_param) {}

template <int>
struct select;

enum { bind_bex, bind_std, bind_boost };

template <>
struct select<bind_bex>    // bex bind
{
    template <typename Arg>
    inline static Arg && invoke(Arg && arg)
    {
        return std::forward<Arg>(arg);
    }
};

template <>
struct select<bind_std>    // std bind
{
    template <typename Arg>
    inline static Arg && invoke(Arg && arg)
    {
        return std::forward<Arg>(arg);
    }

    template <int I>
    inline static std::_Ph<I> invoke(Bex::forward_bind::placeholder<I>)
    {
        return std::_Ph<I>();
        //std::placeholders::_1
    }
};

template <>
struct select<bind_boost>    // boost bind
{
    template <typename Arg>
    inline static Arg && invoke(Arg && arg)
    {
        return std::forward<Arg>(arg);
    }

    template <int I>
    inline static boost::arg<I> invoke(Bex::forward_bind::placeholder<I>)
    {
        return boost::arg<I>();
    }
};

template <class F, typename ... Args>
static void do_one_test(int tc, std::string const& msg, F && f, Args && ... args)
{
    //DumpC(msg << ":");
    //std::chrono
    high_resolution_timer bpt;
    for (int i = 0; i < tc * optimized_multi; ++i)
    {
        f(std::forward<Args>(args)...);
    }
    DumpC("\t" << bpt.elapsed());
}

template <typename ... BArgs, unsigned int ... S, typename ... CArgs>
static void do_test_(int tc, std::string const& msg, std::tuple<BArgs...> & bargs, Bex::forward_bind::seq<S...>, CArgs && ... cargs)
{
    auto bex_f = Bex::bind(select<bind_bex>::invoke(std::forward<BArgs>(std::get<S>(bargs)))...);
    auto std_f = std::bind(select<bind_std>::invoke(std::forward<BArgs>(std::get<S>(bargs)))...);
    auto boost_f = boost::bind(select<bind_boost>::invoke(std::forward<BArgs>(std::get<S>(bargs)))...);
    

    do_one_test(tc, std::string("Bex") + msg, bex_f, std::forward<CArgs>(cargs)...);
    do_one_test(tc, std::string("std") + msg, std_f, std::forward<CArgs>(cargs)...);
    do_one_test(tc, std::string("boost") + msg, boost_f, std::forward<CArgs>(cargs)...);
}

template <typename ... BArgs, typename ... CArgs>
static void do_test(int tc, std::string const& msg, std::tuple<BArgs...> & bargs, CArgs && ... cargs)
{
    typedef typename Bex::forward_bind::gen<std::tuple_size<std::tuple<BArgs...>>::value>::type seq_type;
    do_test_(tc, msg, bargs, seq_type(), std::forward<CArgs>(cargs)...);
}

static void test_property()
{
    XDump("轻量级性能测试");

    //////////////////////////////////////////////////////////////////////////
    /// @{ free function
    Dump("free_func\t\t\tBex\tstd\tboost")
    {
        /// empty function       
        DumpC("empty_function\t\t");
        static const int tc = 10000000;
        auto tu = std::make_tuple(&empty_function);
        do_test(tc, " empty_function", tu);
        DumpR;
    }

    {
        /// multi light params function
        DumpC("multi_light(bind)\t");
        static const int tc = 1000000;
        auto tu = std::make_tuple(&multi_light_function, 1, 2.0, 3.0f, nullptr);
        do_test(tc, "", tu);
        DumpR;
    }

    {
        /// multi light params function
        DumpC("multi_light(call)\t");
        static const int tc = 1000000;
        auto tu = std::make_tuple(&multi_light_function, Bex::_1, Bex::_2, Bex::_3, Bex::_4);
        do_test(tc, "", tu, 1, 2.0, 3.0f, nullptr);
        DumpR;
    }
    /// @}
    //////////////////////////////////////////////////////////////////////////

    XDump("重量级性能测试");
    //////////////////////////////////////////////////////////////////////////
    /// @{ free function
    Dump("free_func\t\t\tBex\tstd\tboost")
    {
        /// weight function
        DumpC("weight(bind)\t\t");
        static const int tc = 1000000;
        weight_param a;
        auto tu = std::make_tuple(&weight_function, a);
        do_test(tc, " ", tu);
        DumpR;
    }
    {
        /// weight function
        DumpC("weight(call)\t\t");
        static const int tc = 1000000;
        weight_param a;
        auto tu = std::make_tuple(&weight_function, Bex::_1);
        do_test(tc, " weight_function(call)", tu, a);
        DumpR;
    }

    {
        /// weight_ref_function
        DumpC("weight_ref(bind)\t");
        static const int tc = 1000000;
        weight_param a;
        auto tu = std::make_tuple(&weight_ref_function, a);
        do_test(tc, " weight_ref_function(bind)", tu);
        DumpR;
    }
    {
        /// weight_ref_function
        DumpC("weight_ref(call)\t");
        static const int tc = 1000000;
        weight_param a;
        auto tu = std::make_tuple(&weight_ref_function, Bex::_1);
        do_test(tc, " weight_ref_function(call)", tu, a);
        DumpR;
    }

    {
        /// weight_cref_function
        DumpC("weight_cref(bind)\t");
        static const int tc = 1000000;
        weight_param a;
        auto tu = std::make_tuple(&weight_cref_function, a);
        do_test(tc, " weight_cref_function(bind)", tu);
        DumpR;
    }
    {
        /// weight_cref_function
        DumpC("weight_cref(call)\t");
        static const int tc = 1000000;
        weight_param a;
        auto tu = std::make_tuple(&weight_cref_function, Bex::_1);
        do_test(tc, " weight_cref_function(call)", tu, a);
        DumpR;
    }
    /// @}
    //////////////////////////////////////////////////////////////////////////
}

BOOST_AUTO_TEST_SUITE(s_forward_bind)

/// 正确性测试
BOOST_AUTO_TEST_CASE(t_forward_bind)
{
    XDump("开始测试 forward_bind");
    test();
    test_property();
    XDump("结束测试 forward_bind");
}

BOOST_AUTO_TEST_SUITE_END()

#endif //defined(BEX_SUPPORT_CXX11)