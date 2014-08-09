
namespace Bex
{
    struct win_svc_base::WinStructImpl
    {
        SERVICE_STATUS          m_ssStatus;
        SERVICE_STATUS_HANDLE   m_ssh;
    };

    win_svc_base::win_svc_base()
        : m_pWinStructImpl( new WinStructImpl )
    {
        memset(&m_pWinStructImpl->m_ssStatus, 0, sizeof(m_pWinStructImpl->m_ssStatus));
        m_pWinStructImpl->m_ssh = SERVICE_STATUS_HANDLE();
    }

    win_svc_base::~win_svc_base()
    {

    }

    /// 安装成功回调
    void win_svc_base::OnInstall( bool bAutoStart )
    {
        if ( bAutoStart )
        {
            SC_HANDLE schSCMgr = ::OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
            if (!schSCMgr)
            {
                std::cout << "服务管理数据库打开失败!" << std::endl;
                return ;
            }

            SC_HANDLE schService = ::OpenServiceA(schSCMgr, GetServiceName(), SERVICE_ALL_ACCESS);
            if (!schService)
            {
                std::cout << "服务打开失败!" << std::endl;
                ::CloseServiceHandle(schSCMgr);
                return ;
            }

            ::StartServiceA(schService, 0, NULL);
            ::CloseServiceHandle(schService);
            ::CloseServiceHandle(schSCMgr);
        }
    }

    int win_svc_base::Main( int argc, char * argv[] )
    {
        //////////////////////////////////////////////////////////////////////////
        /// 命令行参数解析
        boost::program_options::options_description opts;
        opts.add_options()
            ("help", "options: --install --uninstall")
            ("install", "")
            ("uninstall", "")
            ("auto-start", "");

        boost::program_options::store(
            boost::program_options::parse_command_line(argc, argv, opts)
            , m_vm );

        //////////////////////////////////////////////////////////////////////////
        /// 定义SERVICE_TABLE_ENTRY DispatchTable[] 结构
        SERVICE_TABLE_ENTRYA DispatchTable[2] = 
        {
            {(char*)GetServiceName(), (LPSERVICE_MAIN_FUNCTIONA)GetServiceStartFunction()},                  
            {NULL, NULL}
        };

        if ( m_vm.count("help") )
        {
            /// 帮助
            std::cout << opts << std::endl;
        }
        else if ( m_vm.count("install") )
        {
            bool bAutoStart = (m_vm.count("auto-start") > 0);
            /// 安装服务
            if ( Install(bAutoStart) )
            {
                std::cout << "安装成功!" << std::endl;
                OnInstall(bAutoStart);
            }
            else
                std::cout << "安装失败!" << std::endl;
        }
        else if ( m_vm.count("uninstall") )
        {
            /// 卸载服务
            if ( Uninstall() )
                std::cout << "卸载成功!" << std::endl;
            else
                std::cout << "卸载失败!" << std::endl;
        }
        else
        {
            /// OS启动该程序
            BOOL bSuccessed = ::StartServiceCtrlDispatcherA(DispatchTable);
            //if (bSuccessed)
            //    LOG << "服务入口函数注册成功!" << std::endl;
            //else
            //    LOG << "服务入口函数注册失败!" << std::endl;
        }
        return 0;
    }

    win_svc_base::PServiceStart win_svc_base::GetServiceStartFunction()
    {
        return (&win_svc_base::service_start);
    }

    win_svc_base::PServiceCtrl win_svc_base::GetServiceCtrlFunction()
    {
        return (&win_svc_base::service_ctrl);
    }

    bool win_svc_base::Install(bool bAutoStart)
    {
        SC_HANDLE schSCMgr, schService;
        char szPath[MAX_PATH] = {};
        if (!::GetModuleFileNameA(NULL, szPath, sizeof(szPath)))
            return false;

        /// 打开服务管理数据库
        schSCMgr = ::OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (!schSCMgr)
        {
            std::cout << "服务管理数据库打开失败!" << std::endl;
            return false;
        }

        schService = ::CreateServiceA(
            schSCMgr,                           ///< 服务管理数据库句柄
            GetServiceName(),                   ///< 服务名
            GetShowName(),                      ///< 用于显示服务的标识
            SERVICE_ALL_ACCESS,                 ///< 响应所有的访问请求
            SERVICE_WIN32_OWN_PROCESS,          ///< 服务类型(独占一个进程)
            bAutoStart ? SERVICE_AUTO_START
                : SERVICE_DEMAND_START,         ///< 启动类型(手动启动)
            SERVICE_ERROR_NORMAL,               ///< 错误控制类型
            szPath,                             ///< 服务程序路径
            NULL,                               ///< 服务不属于任何组
            NULL,                               ///< 没有tag标识符
            NULL,                               ///< 启动服务所依赖的服务或服务组,这里仅仅是一个空字符串
            NULL,                               ///< LocalSystem 帐号
            NULL );

        if (!schService)
        {
            std::cout << "服务创建失败!" << std::endl;
            ::CloseServiceHandle(schSCMgr);
            return false;
        }

        ::CloseServiceHandle(schService);
        ::CloseServiceHandle(schSCMgr);
        return true;
    }

    bool win_svc_base::Uninstall()
    {
        SC_HANDLE schSCMgr = ::OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (!schSCMgr)
        {
            std::cout << "服务管理数据库打开失败!" << std::endl;
            return false;
        }

        SC_HANDLE schService = ::OpenServiceA(schSCMgr, GetServiceName(), SERVICE_ALL_ACCESS);
        if (!schService)
        {
            std::cout << "服务打开失败!" << std::endl;
            ::CloseServiceHandle(schSCMgr);
            return false;
        }

        BOOL bDeleted = ::DeleteService(schService);
        ::CloseServiceHandle(schService);
        ::CloseServiceHandle(schSCMgr);
        return (bDeleted ? true : false);
    }

    /// 服务入口点函数
    void __stdcall win_svc_base::service_start( unsigned long argc, char ** argv )
    {
        WinService::ServicePtr pSvc = WinService::getInstance().GetServicePtr();

        //LOG << "call " << __FUNCTION__ << std::endl;

        /// 注册Ctrl函数
        pSvc->m_pWinStructImpl->m_ssh = ::RegisterServiceCtrlHandlerA(
            pSvc->GetServiceName(), pSvc->GetServiceCtrlFunction());
        if (!pSvc->m_pWinStructImpl->m_ssh)
        {
            //LOG << "Ctrl函数注册失败!" << std::endl;
            return ;
        }

        /// 初始化服务状态
        pSvc->ReportStatus(SERVICE_START_PENDING, 0, 0, 1000);

        /// 启动任务线程
        pSvc->m_taskThread = boost::thread(boost::bind(&win_svc_base::Run, pSvc));

        pSvc->ReportStatus(SERVICE_RUNNING);
    }

    /// 回报服务状态
    bool win_svc_base::ReportStatus(unsigned long dwCurrentState, unsigned long dwExitCode
        , unsigned long dwCheckPoint, unsigned long dwWaitHint)
    {
        if (dwCurrentState == SERVICE_START_PENDING)
            m_pWinStructImpl->m_ssStatus.dwControlsAccepted = 0;
        else
            m_pWinStructImpl->m_ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;

        // Initialize ss structure.
        m_pWinStructImpl->m_ssStatus.dwServiceType             = SERVICE_WIN32_OWN_PROCESS;
        m_pWinStructImpl->m_ssStatus.dwServiceSpecificExitCode = 0;
        m_pWinStructImpl->m_ssStatus.dwCurrentState            = dwCurrentState;
        m_pWinStructImpl->m_ssStatus.dwWin32ExitCode           = dwExitCode;
        m_pWinStructImpl->m_ssStatus.dwCheckPoint              = dwCheckPoint;
        m_pWinStructImpl->m_ssStatus.dwWaitHint                = dwWaitHint;

        bool bRet = (SetServiceStatus(m_pWinStructImpl->m_ssh, &m_pWinStructImpl->m_ssStatus) ? true : false);
        //LOG << "call ReportStatus dwCurrentState=" << dwCurrentState << " ret=" << bRet << std::endl;
        return bRet;
    }

    void __stdcall win_svc_base::service_ctrl( unsigned long ulCtrlCode )
    {
        WinService::ServicePtr pSvc = WinService::getInstance().GetServicePtr();

        //LOG << "call " << __FUNCTION__ << " uiCtrlCode=" << ulCtrlCode << std::endl;

        //处理控制请求码
        switch(ulCtrlCode)
        {
            //先更新服务状态为 SERVICDE_STOP_PENDING,再停止服务。
        case SERVICE_CONTROL_STOP:
            pSvc->ReportStatus(SERVICE_STOP_PENDING,NO_ERROR,0,500);
            pSvc->ServiceStop();     //由具体的服务程序实现
            pSvc->m_pWinStructImpl->m_ssStatus.dwCurrentState = SERVICE_STOPPED;
            break;

            //暂停服务
        case SERVICE_CONTROL_PAUSE:
            pSvc->ReportStatus(SERVICE_PAUSE_PENDING,NO_ERROR,0,500);
            pSvc->ServicePause();    //由具体的服务程序实现
            pSvc->m_pWinStructImpl->m_ssStatus.dwCurrentState = SERVICE_PAUSED;
            break;

            //继续服务
        case SERVICE_CONTROL_CONTINUE:
            pSvc->ReportStatus(SERVICE_CONTINUE_PENDING,NO_ERROR,0,500);
            pSvc->ServiceContinue(); //由具体的服务程序实现
            pSvc->m_pWinStructImpl->m_ssStatus.dwCurrentState = SERVICE_RUNNING;
            break;

            //更新服务状态
        case SERVICE_CONTROL_INTERROGATE:
            pSvc->OnServiceUpdateStatus();
            break;

            //无效或自定义控制码
        default:
            pSvc->OnCtrlCode(ulCtrlCode);
            break;
        }

        pSvc->ReportStatus(pSvc->m_pWinStructImpl->m_ssStatus.dwCurrentState);
    }

}

#endif //_WIN32