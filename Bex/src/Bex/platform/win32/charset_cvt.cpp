#if defined(_WIN32) || defined(_WIN64)

#include <Windows.h>
#include <Bex/platform/charset_cvt.h>

namespace Bex {
    namespace conv
    {
        std::string locale::generate_id(std::string const& lc)
        {

            int cp = ::GetACP();
            for (int i = 0; i < sizeof(all_windows_encodings) / sizeof(windows_encoding); ++i)
                if (all_windows_encodings[i].codepage == cp)
                {
                    return lc + all_windows_encodings[i].name;
                }
 
            return lc + "utf8";
        }
    }
}

#endif 