#include "common/RhoPort.h"
#if (defined OS_WINCE || defined OS_WP8)
#include "../platform/wm/src/NetworkDetect.h"
#include <list>
#if (defined OS_WINCE) && !defined(OS_PLATFORM_MOTCE)
#include "../platform/wm/src/ConnectionManager.h"
#endif
#endif

#include "generated/cpp/NetworkBase.h"
#include "net/INetRequest.h"
#include "common/RhoAppAdapter.h"
#include "json/JSONIterator.h"
#include "common/RhodesApp.h"
#include "System.h"

#undef DEFAULT_LOGCATEGORY
#define DEFAULT_LOGCATEGORY "NetworkAcceess"

namespace rho {

using namespace apiGenerator;
using namespace common;
using namespace json;

class CNetworkImpl: public CNetworkSingletonBase
{
public:

    class CHttpCommand : public CGeneratorQueue::CGeneratorQueueCommand
    {
    public:

        NetRequest m_NetRequest;

        CHttpCommand(rho::common::CInstanceClassFunctorBase<CMethodResult>* pFunctor) : CGeneratorQueue::CGeneratorQueueCommand(pFunctor){}
        virtual void cancel()
        {
            m_NetRequest.cancel(); 
        }
    };

    CNetworkImpl(): CNetworkSingletonBase()
	{
#if (defined OS_WINCE) && !defined(OS_PLATFORM_MOTCE)
		CRhoExtData rhodesData = RHODESAPP().getExtManager().makeExtData();
		if (rhodesData.m_hBrowserWnd)
		{
			m_pConnectionManager = new CWAN(rhodesData.m_hBrowserWnd);
			m_pConnectionManager->Initialise();
		}
		else
			m_pConnectionManager = NULL;
#endif
	}

	~CNetworkImpl()
	{
#if (defined OS_WINCE) && !defined(OS_PLATFORM_MOTCE)
		if (m_pConnectionManager)
			delete m_pConnectionManager;
#endif
	}

    virtual rho::common::CThreadQueue::IQueueCommand* createQueueCommand(rho::common::CInstanceClassFunctorBase<CMethodResult>* pFunctor){ return new CHttpCommand(pFunctor); }

    virtual void cancel( rho::apiGenerator::CMethodResult& oResult);
    virtual void downloadFile( const rho::Hashtable<rho::String, rho::String>& propertyMap, rho::apiGenerator::CMethodResult& oResult);
    virtual void get( const rho::Hashtable<rho::String, rho::String>& propertyMap, rho::apiGenerator::CMethodResult& oResult);
    virtual void post( const rho::Hashtable<rho::String, rho::String>& propertyMap, rho::apiGenerator::CMethodResult& oResult);
    virtual void uploadFile( const rho::Hashtable<rho::String, rho::String>& propertyMap, rho::apiGenerator::CMethodResult& oResult);

    virtual void hasNetwork(rho::apiGenerator::CMethodResult& oResult);
    virtual void hasWifiNetwork(rho::apiGenerator::CMethodResult& oResult);
    virtual void hasCellNetwork(rho::apiGenerator::CMethodResult& oResult);
    virtual void startStatusNotify( int pollInterval, rho::apiGenerator::CMethodResult& oResult);
    virtual void stopStatusNotify(rho::apiGenerator::CMethodResult& oResult);
    virtual void detectConnection( const rho::Hashtable<rho::String, rho::String>& propertyMap, rho::apiGenerator::CMethodResult& oResult);
	virtual void stopDetectingConnection(rho::apiGenerator::CMethodResult& oResult);
    virtual void connectWan( const rho::String& connectionDestination, rho::apiGenerator::CMethodResult& oResult);
    virtual void disconnectWan(rho::apiGenerator::CMethodResult& oResult);
#if (defined OS_WINCE) && !defined(OS_PLATFORM_MOTCE)
    virtual bool onWndMsg(MSG& oMsg)
	{
		if (oMsg.message == WM_USER_CONNECTION_MANGER_STATUS)
		{
			if (!m_pConnectionManager) {return FALSE;}
				m_pConnectionManager->ConnectionManagerStatusUpdate();
		}
		return FALSE;
	}
#endif
private:

    NetRequest& getCurRequest(NetRequest& oNetRequest);

    void readHeaders( const rho::Hashtable<rho::String, rho::String>& propertyMap, Hashtable<String,String>& mapHeaders );
    void createResult( NetResponse& resp, Hashtable<String,String>& mapHeaders, rho::apiGenerator::CMethodResult& oResult );
	//  RE1 Network API
#if (defined OS_WINCE || defined OS_WP8)
	std::list<CNetworkDetection*> m_networkPollers;
#if (defined OS_WINCE) && !defined(OS_PLATFORM_MOTCE)
	CWAN *m_pConnectionManager;
#endif
#endif
};

NetRequest& CNetworkImpl::getCurRequest(NetRequest& oNetRequest)
{
    if (!getCommandQueue())
        return oNetRequest;

    CHttpCommand* pCmd = (CHttpCommand*)(getCommandQueue()->getCurCommand());
    if ( pCmd )
        return pCmd->m_NetRequest;

    return oNetRequest;
}

static String getStringProp(const rho::Hashtable<rho::String, rho::String>& propertyMap, const char* szKey, const char* szDefValue = "")
{
    String strRes;
    if (propertyMap.containsKey(szKey) )
        strRes = propertyMap.get(szKey);

    if (strRes.length() == 0 && szDefValue && *szDefValue)
        strRes = szDefValue;

    return strRes;
}

void CNetworkImpl::cancel( rho::apiGenerator::CMethodResult& oResult)
{
    if (!getCommandQueue())
        return;

    synchronized(getCommandQueue()->getCommandLock());
    CHttpCommand* pCmd = (CHttpCommand*)getCommandQueue()->getCurCommand();

    if ( pCmd != null && ( !oResult.hasCallback() || pCmd->isEqualCallback(oResult) ) )
        pCmd->cancel();

    if ( !oResult.hasCallback() )
        getCommandQueue()->getCommands().clear();
    else
    {
        for (int i = getCommandQueue()->getCommands().size()-1; i >= 0; i--)
        {
            CHttpCommand* pCmd1 = (CHttpCommand*)getCommandQueue()->getCommands().get(i);

            if ( pCmd1 != null && pCmd1->isEqualCallback(oResult) )
                getCommandQueue()->getCommands().remove(i);
        }

    }
}

void CNetworkImpl::get( const rho::Hashtable<rho::String, rho::String>& propertyMap, rho::apiGenerator::CMethodResult& oResult)
{
    Hashtable<String,String> mapHeaders;                                          
    readHeaders( propertyMap, mapHeaders );

    NetRequest oNetRequest;

    if ( propertyMap.containsKey("verifyPeerCertificate") )
        getCurRequest(oNetRequest).setSslVerifyPeer( propertyMap.get("verifyPeerCertificate") == "true" );

    NetResponse resp = getNetRequest(&getCurRequest(oNetRequest)).doRequest( getStringProp(propertyMap, "httpVerb", "GET").c_str(),
            propertyMap.get("url"), propertyMap.get("body"), null, &mapHeaders);

    if ( !getCurRequest(oNetRequest).isCancelled())
        createResult( resp, mapHeaders, oResult );
}

void CNetworkImpl::downloadFile( const rho::Hashtable<rho::String, rho::String>& propertyMap, rho::apiGenerator::CMethodResult& oResult)
{
    Hashtable<String,String> mapHeaders;                                          
    readHeaders( propertyMap, mapHeaders );

    NetRequest oNetRequest;

    if ( propertyMap.containsKey("verifyPeerCertificate") )
        getCurRequest(oNetRequest).setSslVerifyPeer( propertyMap.get("verifyPeerCertificate") == "true" );

    NetResponse resp = getNetRequest(&getCurRequest(oNetRequest)).pullFile( propertyMap.get("url"), propertyMap.get("filename"), null, &mapHeaders);

    if ( !getCurRequest(oNetRequest).isCancelled())
        createResult( resp, mapHeaders, oResult );
}

void CNetworkImpl::post( const rho::Hashtable<rho::String, rho::String>& propertyMap, rho::apiGenerator::CMethodResult& oResult)
{
    Hashtable<String,String> mapHeaders;                                          
    readHeaders( propertyMap, mapHeaders );

    NetRequest oNetRequest;

    if ( propertyMap.containsKey("verifyPeerCertificate") )
        getCurRequest(oNetRequest).setSslVerifyPeer( propertyMap.get("verifyPeerCertificate") == "true" );

    NetResponse resp = getNetRequest(&getCurRequest(oNetRequest)).doRequest( getStringProp(propertyMap, "httpVerb", "POST").c_str(),
            propertyMap.get("url"), propertyMap.get("body"), null, &mapHeaders);

    if ( !getCurRequest(oNetRequest).isCancelled())
        createResult( resp, mapHeaders, oResult );
}

void CNetworkImpl::uploadFile( const rho::Hashtable<rho::String, rho::String>& propertyMap, rho::apiGenerator::CMethodResult& oResult)
{
    Hashtable<String,String> mapHeaders;                                          
    readHeaders( propertyMap, mapHeaders );

    NetRequest oNetRequest;

    if ( propertyMap.containsKey("verifyPeerCertificate") )
        getCurRequest(oNetRequest).setSslVerifyPeer( propertyMap.get("verifyPeerCertificate") == "true" );

    VectorPtr<net::CMultipartItem*> arMultipartItems;
    if ( propertyMap.containsKey("multipart") )
    {
        for( CJSONArrayIterator oArray( propertyMap.get("multipart").c_str() ); !oArray.isEnd(); oArray.next() )
        {
            CJSONEntry oItem(oArray.getCurItem().getString());

            net::CMultipartItem* pItem = new net::CMultipartItem();
            String strFilePath = oItem.getString("filename", "");
            if ( strFilePath.length() == 0 )
            {
                pItem->m_strBody = oItem.getString("body", "");
                pItem->m_strContentType = oItem.getString("contentType", "");
            }
            else
            {
                pItem->m_strFilePath = strFilePath;
                pItem->m_strContentType = oItem.getString("contentType", "application/octet-stream");
            }

            pItem->m_strName = oItem.getString("name", "");
            pItem->m_strFileName = oItem.getString("filenameBase", "");
            arMultipartItems.addElement(pItem);
        }

    }else
    {
        net::CMultipartItem* pItem = new net::CMultipartItem();
        pItem->m_strFilePath = propertyMap.get("filename");
        pItem->m_strContentType = getStringProp(propertyMap, "fileContentType", "application/octet-stream");
        pItem->m_strName = propertyMap.get("name");
        pItem->m_strFileName = propertyMap.get("filenameBase");
        arMultipartItems.addElement(pItem);

        String strBody = propertyMap.get("body");
        if ( strBody.length() > 0 )
        {
            net::CMultipartItem* pItem2 = new net::CMultipartItem();
            pItem2->m_strBody = strBody;
            pItem2->m_strContentType = mapHeaders.get("content-type");
            arMultipartItems.addElement(pItem2);
        }
    }

    NetResponse resp = getNetRequest(&getCurRequest(oNetRequest)).pushMultipartData( propertyMap.get("url"), arMultipartItems, null, &mapHeaders );

    if ( !getCurRequest(oNetRequest).isCancelled())
        createResult( resp, mapHeaders, oResult );
}

void CNetworkImpl::readHeaders( const rho::Hashtable<rho::String, rho::String>& propertyMap, Hashtable<String,String>& mapHeaders )
{
    if ( propertyMap.get("headers").length() > 0 )
    {
        for( CJSONStructIterator oHashIter( propertyMap.get("headers").c_str() ); !oHashIter.isEnd(); oHashIter.next() )
        {
            mapHeaders[oHashIter.getCurKey()] = oHashIter.getCurString();
        }
    }

    if ( propertyMap.get("authType") == AUTH_BASIC )
    {
        int nLen = rho_base64_encode(propertyMap.get("authPassword").c_str(), -1, 0);
        char* szBuf = new char[nLen+1];
        rho_base64_encode(propertyMap.get("authPassword").c_str(), -1, szBuf );

        mapHeaders["Authorization"] = "Basic " + propertyMap.get("authUser") + ":" + szBuf;
        delete szBuf;
    }

    if ( mapHeaders.get("User-Agent").length() == 0 )
    {
        mapHeaders["User-Agent"] = "Mozilla-5.0 (" + rho::System::getPlatform() + "; " + rho::System::getDeviceName() + "; " + rho::System::getOsVersion() + ")";
    }

}

void CNetworkImpl::createResult( NetResponse& resp, Hashtable<String,String>& mapHeaders, rho::apiGenerator::CMethodResult& oResult )
{
/*
    String strJSON = "{";
    if ( resp.isSuccess() )
    {
        strJSON += "\"status\":" + CJSONEntry::quoteValue("ok");
        if ( resp.isResponseRecieved())
            strJSON += ",\"http_error\":" + CJSONEntry::quoteValue( convertToStringA(resp.getRespCode()) );
    }
    else
    {
        strJSON += "\"status\":" + CJSONEntry::quoteValue("error");
        strJSON += ",\"error_code\":" + CJSONEntry::quoteValue(convertToStringA(RhoAppAdapter.getErrorFromResponse(resp)));
        if ( resp.isResponseRecieved())
            strJSON += ",\"http_error\":" + CJSONEntry::quoteValue( convertToStringA(resp.getRespCode()) );
    }

    String cookies = resp.getCookies();
    if ( cookies.length() > 0 )
        strJSON += ",\"cookies\":" + CJSONEntry::quoteValue(cookies);

    strJSON += ",\"headers\":" + CJSONEntry::toJSON(mapHeaders);
    strJSON += ",\"body\":" + CJSONEntry::quoteValue(String(resp.getCharData(), resp.getDataSize()));

    strJSON += "}";

    oResult.setJSON(strJSON); */

    Hashtable<String,String>& mapRes = oResult.getStringHash();

    if ( resp.isSuccess() )
    {
        mapRes["status"] = "ok";
        if ( resp.isResponseRecieved())
            mapRes["http_error"] = convertToStringA(resp.getRespCode());
    }
    else
    {
        mapRes["status"] = "error";
        mapRes["error_code"] = convertToStringA(RhoAppAdapter.getErrorFromResponse(resp));;
        if ( resp.isResponseRecieved())
            mapRes["http_error"] = convertToStringA(resp.getRespCode());
    }

    String cookies = resp.getCookies();
    if (cookies.length()>0)
        mapRes["cookies"] = cookies;

    oResult.getStringHashL2()["headers"] = mapHeaders;

    //TODO: support "application/json" content-type
    mapRes["body"].assign( resp.getCharData(), resp.getDataSize() );

    oResult.set(mapRes);
}

static int g_rho_has_network = 1, g_rho_has_cellnetwork = 0;

extern "C" void rho_sysimpl_sethas_network(int nValue)
{
    g_rho_has_network = nValue > 1 ? 1 : 0;
}

extern "C" int rho_sysimpl_has_network()
{
    return g_rho_has_network;
}

extern "C" void rho_sysimpl_sethas_cellnetwork(int nValue)
{
    g_rho_has_cellnetwork = nValue;
}

void CNetworkImpl::hasNetwork(rho::apiGenerator::CMethodResult& oResult)
{
    oResult.set(g_rho_has_network!= 0 || g_rho_has_cellnetwork!= 0);
}

void CNetworkImpl::hasWifiNetwork(rho::apiGenerator::CMethodResult& oResult)
{
    oResult.set(g_rho_has_network!= 0);
}

void CNetworkImpl::hasCellNetwork(rho::apiGenerator::CMethodResult& oResult)
{
    oResult.set(g_rho_has_cellnetwork!= 0);
}

void CNetworkImpl::startStatusNotify( int pollInterval, rho::apiGenerator::CMethodResult& oResult)
{
    //TODO: startStatusNotify
}

void CNetworkImpl::stopStatusNotify(rho::apiGenerator::CMethodResult& oResult)
{
    //TODO: stopStatusNotify
}


//  RE1 Network API Implementation
void CNetworkImpl::detectConnection( const rho::Hashtable<rho::String, rho::String>& propertyMap, rho::apiGenerator::CMethodResult& oResult)
{
#if (defined WINCE || defined _WP8_LIB)
	CNetworkDetection* pNetworkDetection = new CNetworkDetection();
	pNetworkDetection->Initialise();
	m_networkPollers.push_back(pNetworkDetection);
	if (!pNetworkDetection->IsChecking())
	{
		typedef std::map<rho::String, rho::String>::const_iterator it_type;
		for (it_type iterator = propertyMap.begin(); iterator != propertyMap.end(); iterator++)
		{
			if (iterator->first.compare("host") == 0)
				pNetworkDetection->SetHost(iterator->second);
			else if (iterator->first.compare("port") == 0)
				pNetworkDetection->SetPort(atoi(iterator->second.c_str()));
			else if (iterator->first.compare("pollInterval") == 0)
				pNetworkDetection->SetNetworkPollInterval(atoi(iterator->second.c_str()));
			else if (iterator->first.compare("detectionTimeout") == 0)
				pNetworkDetection->SetConnectionTimeout(atoi(iterator->second.c_str()));
			else
				LOG(WARNING) + "Unrecognised parameter passed to detectConnection: " + iterator->first;
		}
		pNetworkDetection->SetCallback(oResult);
		pNetworkDetection->StartNetworkChecking();
	}
	else
		LOG(WARNING) + "Unable to start checking for a network connection, a check is already in progress.  First stop the in progress connection check";
#endif
}

void CNetworkImpl::stopDetectingConnection(rho::apiGenerator::CMethodResult& oResult)
{
#if (defined OS_WINCE || defined OS_WP8)
	//  Find the network detector which matches our callback
	CNetworkDetection *pNetworkDetection = NULL;
	std::list<CNetworkDetection*>::iterator i;
	for (i = m_networkPollers.begin(); i != m_networkPollers.end(); ++i)
	{
		if ((*i) && (*i)->GetCallback().isEqualCallback(oResult))
		{
			//  We've found the CNetworkDetection object with the callback we're looking for
			pNetworkDetection = (*i);
			break;
		}
	}
	if (pNetworkDetection && pNetworkDetection->IsChecking())
	{
		pNetworkDetection->StopNetworkChecking();
		m_networkPollers.remove(pNetworkDetection);
		delete pNetworkDetection;
	}
	else
		LOG(WARNING) + "Unable to stop detecting network connection, could not find specified callback";
#endif
}


void CNetworkImpl::connectWan( const rho::String& connectionDestination, rho::apiGenerator::CMethodResult& oResult)
{
#if (defined OS_WINCE)&& !defined(OS_PLATFORM_MOTCE) 
	//  Only applicable to WM/CE, specific to connection manager
	//  There is only a single object for connection manager access as you can only have
	//  one physical connection.
	m_pConnectionManager->SetWanCallback(oResult);
	m_pConnectionManager->Connect(convertToStringW(connectionDestination).c_str(), TRUE);
#endif
}

void CNetworkImpl::disconnectWan(rho::apiGenerator::CMethodResult& oResult)
{
#if (defined OS_WINCE) && !defined(OS_PLATFORM_MOTCE)
	//  Only applicable to WM/CE, specific to connection manager
	m_pConnectionManager->Disconnect(TRUE);
#endif
}


////////////////////////////////////////////////////////////////////////

class CNetworkFactory: public CNetworkFactoryBase
{
public:
    ~CNetworkFactory(){}

    INetworkSingleton* createModuleSingleton()
    { 
        return new CNetworkImpl(); 
    }
};

}

extern "C" void Init_Network()                                                                                      
{
    rho::CNetworkFactory::setInstance( new rho::CNetworkFactory() );
    rho::Init_Network_API();

    RHODESAPP().getExtManager().requireRubyFile("Network");
}
