#ifndef __BEX_WIN32_SERVICE__
#define __BEX_WIN32_SERVICE__

#include <Bex/config/windows.h>

#if defined(BEX_WINDOWS_API)

#include <boost/program_options.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <Bex/utility/singleton.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <iostream>

//////////////////////////////////////////////////////////////////////////
/// Win32服务程序框架
namespace Bex
{
    class WinService;
    class win_svc_base
    {
        friend class WinService;

    protected:
        typedef void (__stdcall * PServiceStart)(unsigned long, char **);
        typedef void (__stdcall * PServiceCtrl)(unsigned long);

    public:
        win_svc_base();
        virtual ~win_svc_base();

        /// 获取服务名
        virtual const char* GetServiceName() = 0;

        /// 获取服务名(用于显示)
        virtual const char* GetShowName() = 0;

    protected:
        /// 任务
        virtual void Run() = 0;

    protected:
        //////////////////////////////////////////////////////////////////////////
        /// ------ 回调处理

        /// 安装成功回调
        virtual void OnInstall(bool bAutoStart);

        /// 服务停止回调
        virtual void ServiceStop() {}

        /// 服务暂停回调
        virtual void ServicePause() {}

        /// 服务继续回调
        virtual void ServiceContinue() {}

        /// 更新服务状态回调
        virtual void OnServiceUpdateStatus() {}

        /// 其他控制码处理
        virtual void OnCtrlCode(unsigned long ulCtrlCode) { ulCtrlCode; }
        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        /// ------ 以下函数如果没有特殊需求, 不建议重载.
    protected:
        /// 获取服务入口点函数指针
        virtual PServiceStart GetServiceStartFunction();

        /// 获取服务控制函数指针
        virtual PServiceCtrl GetServiceCtrlFunction();

        /// 安装服务
        // @param bAutoStart 是否自动启动
        virtual bool Install(bool bAutoStart);

        /// 卸载服务
        virtual bool Uninstall();

        /// 回报服务状态
        bool ReportStatus(unsigned long dwCurrentState, unsigned long dwExitCode = 0
            , unsigned long dwCheckPoint = 0, unsigned long dwWaitHint = 0);
        //////////////////////////////////////////////////////////////////////////

    private:
        /// 入口函数
        int Main(int argc, char * argv[]);

        /// 默认服务入口点函数
        static void __stdcall service_start(unsigned long argc, char ** argv);

        /// 默认服务控制函数
        static void __stdcall service_ctrl(unsigned long ulCtrlCode);

    private:
        boost::program_options::variables_map m_vm;         ///< 命令行参数map
        boost::thread           m_taskThread;

        struct WinStructImpl;
        boost::shared_ptr<WinStructImpl> m_pWinStructImpl;
    };

    class WinService
        : public singleton<WinService>
    {
    public:
        typedef boost::shared_ptr<win_svc_base> ServicePtr;

        /// 绑定服务对象
        // @param pSvc 指向堆内存的指针, 交由WinService负责在析构时释放内存.
        bool BindService(win_svc_base * pSvc)
        {
            BOOST_ASSERT( !m_pService );
            if (m_pService) 
                return false;

            m_pService.reset(pSvc);
            return true;
        }

        /// 入口函数
        int Main(int argc, char * argv[])
        {
            if (m_pService)
                return m_pService->Main(argc, argv);

            return 0;
        }

        /// 获取服务对象指针
        inline ServicePtr GetServicePtr()
        {
            return m_pService;
        }

    private:
        ServicePtr m_pService;
    };
}

#include "win32_service.ipp"

#endif //defined(BEX_WINDOWS_API)

#endif //__BEX_WIN32_SERVICE__