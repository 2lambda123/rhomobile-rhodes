#include "stdafx.h"

#include "NetRequest.h"
#include "common/AutoPointer.h"
#include "common/RhoFile.h"
#include "NetRequestImpl.h"

extern "C" char* HTTPResolveUrl(char* url);

namespace rho {
namespace net {
IMPLEMENT_LOGCLASS(CNetRequest,"Net");

INetResponse* CNetRequest::pullData(const String& strUrl )
{
    return doRequest("GET",strUrl,String());
}

INetResponse* CNetRequest::pushData(const String& strUrl, const String& strBody)
{
    return doRequest("POST",strUrl,strBody);
}

INetResponse* CNetRequest::pullCookies(const String& strUrl, const String& strBody)
{
    return doRequest("POST",strUrl,strBody);
}

//if strUrl.length() == 0 delete all cookies if possible
void CNetRequest::deleteCookie(const String& strUrl)
{
    if ( strUrl.length() > 0 )
        ::InternetSetCookieA(strUrl.c_str(), NULL, "");
}

String CNetRequest::resolveUrl(const String& strUrl)
{
    char* url = HTTPResolveUrl( strdup(strUrl.c_str()) );
    String res = url;
    free(url);
    return res;
}

void CNetRequest::cancel()
{
    m_bCancel = true;
    if ( m_pCurNetRequestImpl != null )
        m_pCurNetRequestImpl->close();
}

INetResponse* CNetRequest::pushFile(const String& strUrl, const String& strFilePath)
{
    common::CRhoFile oFile;
    if ( !oFile.open(strFilePath.c_str(),common::CRhoFile::OpenReadOnly) ) 
    {
        LOG(ERROR) + "pushFile: cannot find file :" + strFilePath;
        return new CNetResponseImpl();
    }

    int nTry = 0;
    m_bCancel = false;
    CNetResponseImpl* pResp = 0;
    do
    {
        if ( pResp )
            delete pResp;

        CNetRequestImpl oImpl(this, "POST",strUrl);
        pResp = oImpl.sendStream(oFile.getInputStream());
        nTry++;

    }while( !m_bCancel && !pResp->isResponseRecieved() && nTry < MAX_NETREQUEST_RETRY );

    return pResp;
}

INetResponse* CNetRequest::pullFile(const String& strUrl, const String& strFilePath)
{
    common::CRhoFile oFile;
    if ( !oFile.open(strFilePath.c_str(),common::CRhoFile::OpenForWrite) ) 
    {
        LOG(ERROR) + "pullFile: cannot create file :" + strFilePath;
        return new CNetResponseImpl();
    }

    int nTry = 0;
    m_bCancel = false;
    CNetResponseImpl* pResp = 0;
    do
    {
        if ( pResp )
            delete pResp;

        CNetRequestImpl oImpl(this, "GET",strUrl);
        pResp = oImpl.downloadFile(oFile);
        nTry++;

    }while( !m_bCancel && !pResp->isResponseRecieved() && nTry < MAX_NETREQUEST_RETRY );

    return pResp;
}

INetResponse* CNetRequest::doRequest( const char* method, const String& strUrl, const String& strBody )
{
    int nTry = 0;
    m_bCancel = false;
    CNetResponseImpl* pResp = 0;
    do
    {
        if ( pResp )
            delete pResp;

        CNetRequestImpl oImpl(this, method,strUrl);
        pResp = oImpl.sendString(strBody);
        nTry++;

    }while( !m_bCancel && !pResp->isResponseRecieved() && nTry < MAX_NETREQUEST_RETRY );

    return pResp;
}

}
}

extern "C" {

int g_rho_net_has_network = 1;

int rho_net_has_network()
{
	return g_rho_net_has_network > 0 ? 1 : 0;
}

}