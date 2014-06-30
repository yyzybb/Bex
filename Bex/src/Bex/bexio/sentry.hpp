#ifndef __BEX_IO_SENTRY_HPP__
#define __BEX_IO_SENTRY_HPP__

//////////////////////////////////////////////////////////////////////////
/// 标记

namespace Bex { namespace bexio
{
    template <typename NativeType>
    class sentry
    {
    public:
        explicit sentry(bool bset)
        {
            if (bset) set();
        }

        // 设置标记
        inline bool set()
        {
            return native_.try_lock();
        }

        // 清除标记
        inline bool reset()
        {
            return native_.unlock();
        }

        // 是否已设置
        inline bool is_set() const
        {
            return native_.is_locked();
        }

    private:
        NativeType native_;
    };

    template <>
    class sentry<bool>
    {
    public:
        explicit sentry(bool bset)
            : native_(bset)
        {
        }

        // 设置标记
        inline bool set()
        {
            bool ret = (native_ != true);
            native_ = true;
            return ret;
        }

        // 清除标记
        inline bool reset()
        {
            bool ret = (native_ != false);
            native_ = false;
            return ret;
        }

        // 是否已设置
        inline bool is_set() const
        {
            return native_;
        }

    private:
        volatile bool native_;
    };

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_SENTRY_HPP__