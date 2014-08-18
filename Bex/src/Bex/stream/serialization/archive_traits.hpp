#ifndef __BEX_STREAM_SERIALIZATION_UTILITY__
#define __BEX_STREAM_SERIALIZATION_UTILITY__

#include "serialization_fwd.h"

namespace Bex { namespace serialization
{
    /// archive traits
    template <class Archive>
    struct archive_traits_base
    {
        typedef Archive archive_type;

        //////////////////////////////////////////////////////////////////////////
        /// archive mode info
        static const bool is_binary_mode = boost::is_base_of<binary_base, Archive>::value;
        static const bool is_text_mode = boost::is_base_of<text_base, Archive>::value;
        static const bool is_unkown_mode = !is_binary_mode && !is_text_mode;

        static const archive_mode_enum mode = (is_binary_mode ? archive_mode_enum::archive_mode_binary :
            (is_text_mode ? archive_mode_enum::archive_mode_text : archive_mode_enum::archive_mode_unkown));

        typedef typename boost::mpl::if_c<is_binary_mode, binary_mode_tag,
            typename boost::mpl::if_c<is_text_mode, text_mode_tag, unkown_mode_tag>::type>::type
            mode_tag;

        //////////////////////////////////////////////////////////////////////////
        /// archive oper info
        static const bool is_load = boost::is_base_of<input_archive_base, Archive>::value;
        static const bool is_save = boost::is_base_of<output_archive_base, Archive>::value;;
        static const bool is_unkown = (!is_load && !is_save);

        static const archive_oper_enum oper = is_unkown ? archive_oper_enum::archive_oper_unkown : 
            (is_load ? archive_oper_enum::archive_oper_load  : archive_oper_enum::archive_oper_save);

        typedef typename boost::mpl::if_c<is_load, load_oper_tag,
            typename boost::mpl::if_c<is_save, save_oper_tag, unkown_oper_tag>::type>::type
            oper_tag;
    };

    template <class Archive>
    struct archive_traits
        : archive_traits_base<typename remove_all<Archive>::type>
    {};

} //namespace serialization

namespace {
    using serialization::binary_wrapper;
    using serialization::archive_traits;

} //namespace serialization
} //namespace Bex


#endif //__BEX_STREAM_SERIALIZATION_UTILITY__