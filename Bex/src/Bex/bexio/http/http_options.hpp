#ifndef __BEX_IO_HTTP_HTTP_OPTIONS_HPP__
#define __BEX_IO_HTTP_HTTP_OPTIONS_HPP__

#include <Bex/bexio/bexio_fwd.hpp>
#include <boost/algorithm/string.hpp>

namespace Bex { namespace bexio { namespace http
{ 
    
// 如果没有定义最大重定向次数, 则默认为5次最大重定向.
#ifndef BEXIO_HTTP_MAX_REDIRECTS
#define BEXIO_HTTP_MAX_REDIRECTS 5
#endif

// 常用http选项.
namespace option
{
	// 下面为bexio内置选项(必须以"_"开头).
	static const std::string request_method("_request_method"); // 请求方式(GET/POST)
	static const std::string http_version("_http_version");		// HTTP/1.0|HTTP/1.1
	static const std::string request_body("_request_body");		// 一般用于POST一些数据如表单之类时使用.
	static const std::string status_code("_status_code");	    // HTTP状态码.
	static const std::string status_str("_status_str");	        // HTTP状态字符串(跟在状态码后面的, 比如:OK).
	static const std::string path("_path");		// 请求的path, 如http://abc.ed/v2/cma.txt中的/v2/cma.txt.
	static const std::string url("_url");		// 在启用keep-alive的时候, 请求host上不同的url时使用.

	// 以下是常用的标准http head选项.
	static const std::string host("Host");
	static const std::string accept("Accept");
	static const std::string range("Range");
	static const std::string cookie("Cookie");
	static const std::string referer("Referer");
	static const std::string user_agent("User-Agent");
	static const std::string content_type("Content-Type");
	static const std::string content_length("Content-Length");
	static const std::string content_range("Content-Range");
	static const std::string connection("Connection");
	static const std::string proxy_connection("Proxy-Connection");
	static const std::string accept_encoding("Accept-Encoding");
	static const std::string transfer_encoding("Transfer-Encoding");
	static const std::string content_encoding("Content-Encoding");

    // 分隔符
    static const std::string line_end("\r\n");
    static const std::string opt_end("\r\n\r\n");

    // 检测是否是内置选项
    inline bool is_builtin_option(std::string const& key)
    {
        return !key.empty() && key[0] == '_';
    }
} // namespace option


// http的选项存储.
class options
{
public:
    typedef std::string opt_key_type;
    typedef std::string opt_value_type;
    typedef std::pair<opt_key_type, opt_value_type> opt_type;
    typedef std::vector<opt_type> opts;
    typedef opts::iterator iterator;
    typedef opts::const_iterator const_iterator;

    options() : fake_continue_(false) {}

    // 获取指定选项的值
    opt_value_type get(opt_key_type const& _key) const
    {
        opt_key_type key = boost::to_lower_copy(_key);
        const_iterator cit = std::find_if(opts_.begin(), opts_.end(), [](opt_type const& opt, opt_key_type const& key) {
            return opt.first == key;
        });
        return (opts_.end() == cit) ? opt_value_type() : (cit->second);
    }

    // 设置指定选项的值
    void set(opt_key_type const& _key, opt_value_type const& value)
    {
        opt_key_type key = boost::to_lower_copy(_key);
        iterator it = std::find_if(opts_.begin(), opts_.end(), [](opt_type const& opt, opt_key_type const& key) {
            return opt.first == key;
        });
        if (opts_.end() == it)
            opts_.push_back(opt_type(key, value));
        else
            it->second = value;
    }

    std::string to_string() const
    {
        using namespace option;

        std::string s;
        opt_value_type v_method = get(request_method);
        opt_value_type v_version = get(http_version);
        opt_value_type v_path = get(path);
        opt_value_type v_status_code = get(status_code);
        opt_value_type v_status_str = get(status_str);
        if (v_method.empty())
        {
            // response
            s += v_version, s += " ";
            s += v_status_code, s += " ";
            s += v_status_str, s += line_end;
        }
        else
        {
            // request
            s += v_method, s += " ";
            s += v_path, s += " ";
            s += v_version, s += line_end;
        }
    }

protected:
	// 选项列表.
	opts opts_;

	// 是否启用假100 continue消息, 如果启用, 则在发送完成http request head
	// 之后, 返回一个fake continue消息.
	bool fake_continue_;
};

// 请求时的http选项.
// _http_version, 取值 "HTTP/1.0" / "HTTP/1.1", 默认为"HTTP/1.1".
// _request_method, 取值 "GET/POST/HEAD", 默认为"GET".
// _request_body, 请求中的body内容, 取值任意, 默认为空.
// Host, 取值为http服务器, 默认为http服务器.
// Accept, 取值任意, 默认为"*/*".
// 这些比较常用的选项被定义在http_options中.
typedef option request_opts;

// http服务器返回的http选项.
// 一般会包括以下几个选项:
// _http_version, 取值 "HTTP/1.0" / "HTTP/1.1", 默认为"HTTP/1.1".
// _status_code, http返回状态.
// Server, 服务器名称.
// Content-Length, 数据内容长度.
// Connection, 连接状态标识.
typedef option response_opts;



// Http请求的代理设置.

struct proxy_settings
{
	proxy_settings()
		: type (none)
	{}

	std::string hostname;
	int port;

	std::string username;
	std::string password;

	enum proxy_type
	{
		// 没有设置代理.
		none,
		// socks4代理, 需要username.
		socks4,
		// 不需要用户密码的socks5代理.
		socks5,
		// 需要用户密码认证的socks5代理.
		socks5_pw,
		// http代理, 不需要认证.
		http,
		// http代理, 需要认证.
		http_pw,
	};

	proxy_type type;
};


// 一些默认的值.
static const int default_request_piece_num = 10;
static const int default_time_out = 11;
static const int default_connections_limit = 5;
static const int default_buffer_size = 1024;

// multi_download下载设置.

struct settings
{
	settings ()
		: download_rate_limit(-1)
		, connections_limit(default_connections_limit)
		, piece_size(-1)
		, time_out(default_time_out)
		, request_piece_num(default_request_piece_num)
		, allow_use_meta_url(true)
		, disable_multi_download(false)
		, check_certificate(true)
		, storage(NULL)
	{}

	// 下载速率限制, -1为无限制, 单位为: byte/s.
	int download_rate_limit;

	// 连接数限制, -1为默认.
	int connections_limit;

	// 分块大小, 默认根据文件大小自动计算.
	int piece_size;

	// 超时断开, 默认为11秒.
	int time_out;

	// 每次请求的分片数, 默认为10.
	int request_piece_num;

	// meta_file路径, 默认为当前路径下同文件名的.meta文件.
	fs::path meta_file;

	// 允许使用meta中保存的url, 默认为允许. 针对一些变动的url, 我们应该禁用.
	bool allow_use_meta_url;

	// 禁止使用并发下载.
	// NOTE: 比如用于动态页面下载, 因为动态页面不能使用并发下载, 如果还想继续使用
	// multi_download进行下载, 则需要设置这个参数, 否则并发下载动态数据的行为, 是
	// 未定义的, 其结果可能因为数据长度不一至导致断言, 也可能数据错误.
	// NOTE: 不推荐使用multi_download进行下载, 而应该使用http_stream进行下载,
	// multi_download主要应用在大文件, 静态页面下载!
	bool disable_multi_download;

	// 下载文件路径, 默认为当前目录.
	fs::path save_path;

	// 设置是否检查证书, 默认检查证书.
	bool check_certificate;

	// 存储接口创建函数指针, 默认为multi_download提供的file.hpp实现.
	storage_constructor_type storage;

	// 请求选项.
	request_opts opts;

	// 代理设置.
	proxy_settings proxy;
};

} //namespace http
} //namespace bexio
} //namespace Bex

#endif //__BEX_IO_HTTP_HTTP_OPTIONS_HPP__