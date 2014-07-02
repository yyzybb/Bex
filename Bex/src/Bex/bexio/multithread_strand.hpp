#ifndef __BEX_IO_MULTITHREAD_STRAND_HPP__
#define __BEX_IO_MULTITHREAD_STRAND_HPP__

//////////////////////////////////////////////////////////////////////////
// 多线程->单线程串行器
/*
* @par concept:
*   io_service, Strand
*/

#include "bexio_fwd.hpp"
#include "multithread_strand_fwd.hpp"
#include "allocator.hpp"

namespace Bex { namespace bexio 
{
    using namespace boost::asio;

    template <class Strand, class Allocator>
    class multithread_strand
    {
    public:
        // next layer type
        typedef typename boost::remove_reference<Strand>::type next_layer_type;

        // lowest layer type
        typedef typename next_layer_t<Strand>::type lowest_layer_type;

    public:
        template <typename Arg>
        explicit multithread_strand(BEX_IO_MOVE_ARG(Arg) arg)
            : next_layer_(BEX_IO_MOVE_CAST(Arg)(arg))
        {
        }

        // 获取下层strand引用
        next_layer_type & next_layer();

        // 获取最底层strand引用
        lowest_layer_type & lowest_layer();

        // 获取当前层的io_service
        io_service & actor();

        // post完成回调
        template <typename Handler>
        void post(BEX_IO_MOVE_ARG(Handler) handler);

        // 处理完成回调
        std::size_t run(error_code & ec);

        // 处理完成回调(noexcept)
        std::pair<std::size_t, error_code> run();

        // 启动线程池工作
        // @thread_count: 工作线程数量, 当thread_count==0时, 将thread_count改写为当前cpu线程数.
        // @remarks: 如果当前已有工作线程在工作, 且工作线程数量小于thread_count, 则增加工作线程数至thread_count.
        void startup(unsigned int thread_count);

        // 停止线程池工作
        void stop();

        // 重启线程池
        void reset();

        // 优雅地关闭
        void shutdown();

        // 强制终止
        void terminate();

        // 线程池是否已停止工作
        bool stopped();

    private:
        // 下层strand
        Strand next_layer_;

        // 串行多个工作线程的回调
        io_service actor_;

        // 工作线程数量
        unsigned int thread_count_;

        // 用于阻塞工作线程的worker
        boost::shared_ptr<io_service::work> worker_;

        // 工作线程列表
        typedef boost::shared_ptr<boost::thread> thread_ptr;
        typedef std::list<thread_ptr, Allocator> thread_list;
        thread_list threads_;

        // 工作线程列表锁
        boost::recursive_mutex threads_mutex_;
    };

} //namespace bexio
} //namespace Bex

#include "detail/multithread_strand.ipp"

#endif //__BEX_IO_MULTITHREAD_STRAND_HPP__
