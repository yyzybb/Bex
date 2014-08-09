#ifndef __BEX_TEST_COMMON__
#define __BEX_TEST_COMMON__

#include <memory>
#include <Bex/locale/charset_cvt.h>

#define DumpC(x) do { std::cout << x << std::flush; } while(0)
#define DumpR do { std::cout << std::endl; } while(0)
#define Dump(x) do { std::cout << x << std::endl; } while(0)
#define DumpX(x) do { std::cout << #x << " = " << x << std::endl; } while(0)
#define XDump(x) do { std::cout << std::string(20, '-') << conv::u82a(x) << std::string(20, '-') << std::endl; } while(0)

#define FOR(n) for( int i = 0; i < n; ++i )
#define UFOR(n) for( unsigned int ui = 0; ui < n; ++ui )

typedef  boost::mpl::list<int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t>  int_types;
typedef  boost::mpl::list<char, wchar_t>  char_types;

#pragma pack(push, 1)
struct buildin_type_struct
{
    int8_t          _s8;
    uint8_t         _u8;
    int16_t         _s16;
    uint16_t        _u16;
    int32_t         _s32;
    uint32_t        _u32;
    int64_t         _s64;
    uint64_t        _u64;
    float32_t       _f32;
    float64_t       _f64;

    buildin_type_struct()
    {
        reset();
    }

    template <typename T>
    explicit buildin_type_struct( T _Val )
    {
        reset(_Val);
    }

    template <typename T>
    void reset( T _Val )
    {
        _s8 = (int8_t)_Val, _u8 = (uint8_t)_Val;
        _s16 = (int16_t)_Val, _u16 = (uint16_t)_Val;
        _s32 = (int32_t)_Val, _u32 = (uint32_t)_Val;
        _s64 = (int64_t)_Val, _u64 = (uint64_t)_Val;
        _f32 = (float32_t)_Val, _f64 = (float64_t)_Val;
    }

    void reset()
    {
        this->reset(0);
    }

    template <typename T>
    bool check( T _Val )
    {
        buildin_type_struct  _Other;
        _Other.reset(_Val);
        return (*this == _Other);
    }

    bool operator==(buildin_type_struct const& _Other) const
    {
        return (memcmp(this, &_Other, sizeof(*this)) == 0);
    }
};
#pragma pack(pop)

#endif //__BEX_TEST_COMMON__