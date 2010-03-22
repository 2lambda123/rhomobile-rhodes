#pragma once

namespace rho {
namespace net{
    struct INetRequest;
    struct ISSL;
}

struct ICallbackObject
{
    virtual unsigned long getObjectValue() = 0;
};

namespace common {
struct IRhoThreadImpl;
struct ISystemInfo;

struct IRhoClassFactory
{
    virtual ~IRhoClassFactory(void){;}

    virtual net::INetRequest* createNetRequest() = 0;
    virtual IRhoThreadImpl* createThreadImpl() = 0;
    virtual ISystemInfo* createSystemInfo() = 0;
    virtual net::ISSL* createSSLEngine() = 0;
};

IRhoClassFactory* createClassFactory();

}
}
