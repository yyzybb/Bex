#ifndef __BEX_IO_SESSION_LIST_MGR_HPP__
#define __BEX_IO_SESSION_LIST_MGR_HPP__

//////////////////////////////////////////////////////////////////////////
/// 连接管理器(侵入式双向链表存储)
/*
* @ 不校验session归属, 使用者要当心!
*/
#include "bexio_fwd.hpp"
#include "intrusive_list.hpp"

namespace Bex { namespace bexio
{
    template <class Session, typename LockType = inter_lock>
    class session_list_mgr
    {
    public:
        typedef Session session_type;
        typedef shared_ptr<session_type> session_ptr;
        typedef intrusive_list<session_type> list_type;
        typedef typename list_type::hook hook;
        typedef LockType lock_type;

        class session_id
            : boost::totally_ordered<session_id>
        {
            explicit session_id(shared_ptr<this_type> const& spointer)
                : id_(spointer ? spointer->id_ : 0), wpointer_(spointer)
            {}

            friend bool operator<(id const& lhs, id const& rhs)
            {
                return lhs.id_ < rhs.id_;
            }

            shared_ptr<this_type> get()
            {
                return wpointer_.lock();
            }

            long id_;
            weak_ptr<this_type> wpointer_;
        };

        session_list_mgr() : size_(0) {}
        ~session_list_mgr()
        {
            list_.clear();
            size_ = 0;
        }

        /// 创建连接id
        static session_id create_id(session_ptr sp)
        {
            return session_id(sp);
        }

        /// 插入
        void insert(session_ptr sp)
        {
            if (!sp) return ;

            BOOST_INTERLOCKED_INCREMENT(size_);
            lock_type::scoped_lock lock(lock_);
            list_.push_back(sp.get());
        }

        /// 删除
        void erase(session_id id)
        {
            session_ptr sp = id.get();
            if (!sp) return ;

            erase(sp.get());
        }

        /// 删除
        void erase(session_type * sp)
        {
            BOOST_INTERLOCKED_DECREMENT(size_);
            lock_type::scoped_lock lock(lock_);
            list_.erase(sp);
        }

        /// 查找
        session_ptr find(session_id id) const
        {
            return id.get();
        }

        /// 数量
        std::size_t size() const
        {
            return size_;
        }

        template <typename F>
        void for_each(F f)
        {
            lock_type::scoped_lock lock(lock_);
            typename list_type::iterator it = list_.begin();
            for (; it != list_.end(); ++it)
                f(*it);
        }

    private:
        list_type list_;
        volatile long size_;
        lock_type lock_;
    };

} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_SESSION_LIST_MGR_HPP__