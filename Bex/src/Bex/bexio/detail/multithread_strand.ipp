#ifndef __BEX_IO_DETAIL_MULTITHREAD_STRAND_IPP__
#define __BEX_IO_DETAIL_MULTITHREAD_STRAND_IPP__

namespace Bex { namespace bexio 
{
    template <class Strand, class Allocator>
    typename multithread_strand<Strand, Allocator>::next_layer_type & multithread_strand<Strand, Allocator>::next_layer()
    {
        return next_layer_;
    }


    template <class Strand, class Allocator>
    typename multithread_strand<Strand, Allocator>::lowest_layer_type & multithread_strand<Strand, Allocator>::lowest_layer()
    {
        return ::Bex::bexio::lowest_layer(next_layer_);
    }


    template <class Strand, class Allocator>
    io_service & multithread_strand<Strand, Allocator>::actor()
    {
        return actor_;
    }

    template <class Strand, class Allocator>
    template <typename Handler>
    void multithread_strand<Strand, Allocator>::post(BEX_IO_MOVE_ARG(Handler) handler)
    {
        actor_.post(BEX_IO_MOVE_CAST(Handler)(handler));
    }


    template <class Strand, class Allocator>
    std::size_t multithread_strand<Strand, Allocator>::run(error_code & ec)
    {
        return actor_.run(ec);
    }


    template <class Strand, class Allocator>
    std::pair<std::size_t, error_code> multithread_strand<Strand, Allocator>::run()
    {
        error_code ec;
        std::size_t n = run(ec);
        return std::pair<std::size_t, error_code>(n, ec);
    }


    template <class Strand, class Allocator>
    void multithread_strand<Strand, Allocator>::startup(unsigned int thread_count)
    {
        boost::recursive_mutex::scoped_lock lock(threads_mutex_);
        thread_count_ = (thread_count == 0) ? boost::thread::hardware_concurrency() : thread_count;
        if (stopped())
            reset();
        else
        {
            unsigned int add_count = (thread_count_ > threads_.size()) ? (thread_count_ - threads_.size()) : 0;
            for (unsigned int ui = 0; ui < add_count; ++ui)
                threads_.push_back(make_shared_ptr<boost::thread>(
                    boost::bind(&next_layer_type::run, &next_layer())));
        }
    }


    template <class Strand, class Allocator>
    void multithread_strand<Strand, Allocator>::stop()
    {
        boost::recursive_mutex::scoped_lock lock(threads_mutex_);
        worker_.reset();
        for (thread_list::iterator it = threads_.begin(); 
            it != threads_.end(); ++it)
        {
            boost::thread & bthread = **it;
            bthread.join();
        }

        threads_.clear();
    }


    template <class Strand, class Allocator>
    void multithread_strand<Strand, Allocator>::reset()
    {
        boost::recursive_mutex::scoped_lock lock(threads_mutex_);
        stop();
        worker_ = make_shared_ptr<io_service::work, Allocator>(Bex::bexio::actor(next_layer()));
        for (unsigned int ui = 0; ui < thread_count_; ++ui)
            threads_.push_back(make_shared_ptr<boost::thread>(
                boost::bind(&next_layer_type::run, &next_layer())));
    }

    template <class Strand, class Allocator>
    void multithread_strand<Strand, Allocator>::shutdown()
    {
        stop();
    }


    template <class Strand, class Allocator>
    void multithread_strand<Strand, Allocator>::terminate()
    {
        stop();
    }


    template <class Strand, class Allocator>
    bool multithread_strand<Strand, Allocator>::stopped()
    {
        return !(bool)worker_;
    }

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_DETAIL_MULTITHREAD_STRAND_IPP__