# Bex
===

* 通用程序库, 作为boost的补充库而存在, 依赖于boost并与boost搭配使用.

===

## 编译

* 库中很多组件是header only的, 但仍有一部分组件是需要编译才能使用的.

* Windows平台:

  - MSVC12.0编译方法:
  - 1.打开Bex/_build/Bex.sln解决方案直接编译即可.
  - 2.生成的lib文件在_output/Bex目录中.

  - Mingw+GCC4.8编译方法:
  - 1.安装cmake并将cmake.exe所在路径加入系统环境变量PATH中.
  - 2.执行Bex/cmake/win32-mingw.bat批处理即可完成编译.
  
* 其他平台和编译器后续会慢慢支持, 如有需要请联系作者或pull request.

===

## Repository目录结构

    > Bex           - Bex库主体
    
        |---- _build    - MSVC构建目录
        |---- cmake     - 其他编译环境构建目录, 使用cmake.
        |---- src       - 源码目录
        |---- test      - 测试代码目录
        |---- test_cobwebs  - Bex.network.cobwebs测试工程源码
        
    > boost         - 安装boost1.55+的目录, 需要用户自行下载并解压至此目录, 然后使用其中的编译批处理进行编译.
    
    > openssl       - 安装openssl的目录, 仓库中自带以编译完成的openssl, 解压出来后即可使用.(暂未启用)

===

## 源码目录结构

* Bex/src/Bex (按字母顺序排列, 带^标记的是还不成熟的或以后可能会放弃维护的组件):

    > base          - 提供内置类型定义
    
    > bind          - 完美转发的bind库, 功能类似于标准库中的bind, 基于C++11, 优势在于可以正确处理带右值引用形参的函数调用.
    
    > config        - Bex配置工具, 用于区分当前系统\编译器等环境信息.
  
    > filesystem    - 标准库fstream的扩展
  
    > math          - 提供了一个可以压缩存储的自定义整数类型
  
    > network       - 网络库.
  
        |---- cobwebs   - 基于asio封装的网络库, 追求简单易用且高效.
        |---- ^ http    - 简易的同步读写的http客户端.
      
    > platform      - 封装不得不用平台SDK实现的功能
  
        |---- win32     - 在Win32下的实现
        |-- TODO: 其他平台下的实现
      
    > signals       - 基于boost.signals2封装的消息框架(可以作为项目框架使用).
  
    > stream        - 流处理库.
  
        |---- serialization    -- 序列化库, 功能类似boost.serialization但更节省空间, 支持二进制和文本格式两种形式.
        |---- convert          -- 搭配serialization库使用的, 可以在序列化的同时进行各种转换的辅助类. 
        |                       比如: 字符编码转换(内存中用ansi, 序列化格式中用utf16).
        |---- static_streambuf -- 固定长度的streambuf, 主要用于处理定长或有上限的缓冲区.
        |---- ring_streambuf   -- 固定缓冲区大小的环形streambuf, 同一时刻只有一读一写可以无锁.
        |---- ring_buf         -- 固定缓冲区大小的环形缓冲区, 同一时刻只有一读一写可以无锁, 自带读写者线程检测机制.
        |---- multi_ringbuf    -- 基于ring_buf实现的可以动态增长长度的环形缓冲区.

    > thread        - 线程库
  
        |---- lock          - 原子锁, 断言锁, 线程锁包装器
        |---- thread_pool   - 线程池
      
    > timer         - 功能类似boost.timer, 但精度是纳秒级的, 接口功能更加丰富. 基于C++11
  
    > type_traits -- boost.type_traits的扩展
  
        |---- class_info       -- C++中的反射! (未完善, 在C++11下可以发挥更强大的威力)
        |---- remove_all       -- 迭代式剥掉类型的所有修饰
        |---- signature_traits -- 自由函数类型转换为成员函数类型的traits.

    > utility       -- 一些超轻量级的实用小工具
  
        |---- exception.h      -- 异常基类
        |---- format.hpp       -- 基于boost.format封装的格式化库, noexcept.
        |---- lexical_cast.hpp -- 基于boost.lexical_cast封装的转换库, noexcept.
        |---- lock_ptr.hpp     -- 带锁的指针, 析构时释放锁.
        |---- operators.h      -- 各种C++运算符的宏定义
        |---- range.hpp        -- 转为C++11中的range for实现的辅助工具. 功能类似于Python中的range
        |---- singleton.hpp    -- 单件模式基类.











      
      

