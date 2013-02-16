#include "SystemImplBase.h"

#include "common/RhoConf.h"
#include "logging/RhoLog.h"
#include "common/RhodesApp.h"
#include "sync/RhoconnectClientManager.h"
#include "common/RhoFilePath.h"
#include "common/RhoFile.h"
#include "unzip/zip.h"
#include "db/DBAdapter.h"

#undef DEFAULT_LOGCATEGORY
#define DEFAULT_LOGCATEGORY "System"

using namespace rho::common;
int rho_sys_zip_files_with_path_array_ptr(const char* szZipFilePath, const char *base_path, const rho::Vector<rho::String>& arFiles, const char* psw)
{
	ZRESULT res;
	HZIP hz = 0;
	
#if defined(UNICODE) && defined(WIN32)
	hz = CreateZip( convertToStringW(szZipFilePath).c_str(), psw);
#else
	hz = CreateZip(szZipFilePath, psw);
#endif
	
	if ( !hz )
		return -1;
	
	for ( int i = 0; i < (int)arFiles.size(); i++ )
	{
		rho::String filePath = arFiles.elementAt(i);
		bool isDir = CRhoFile::isDirectory(filePath.c_str());
		rho::String zipPath = base_path ? filePath.substr(strlen(base_path)) : filePath;
		
#if defined(UNICODE) && defined(WIN32)
		if ( isDir )
			res = ZipAddFolder( hz, convertToStringW(zipPath).c_str(), convertToStringW(filePath).c_str() );
		else
			res = ZipAdd( hz, convertToStringW(zipPath).c_str(), convertToStringW(filePath).c_str() );
#else
		if ( isDir )
			res = ZipAddFolder( hz, zipPath.c_str(), filePath.c_str() );
		else
			res = ZipAdd( hz, zipPath.c_str(), filePath.c_str() );
#endif
		
		if (res != 0)
			LOG(ERROR) + "Zip file failed: " + res + "; " + filePath;
	}
	
	res = CloseZip(hz);
	
	return res;
}

namespace rho {

using namespace apiGenerator;

void CSystemImplBase::getPlatform(CMethodResult& oResult)
{
#ifdef RHODES_EMULATOR
    rho::String strPlatform = RHOSIMCONF().getString("platform");

    if ( strPlatform.compare("bb") == 0 || strPlatform.compare("bb6") == 0 )
        oResult.set( L"Blackberry" );
    else if ( strPlatform.compare("wm") == 0 )
        oResult.set( L"WINDOWS" );
    else if ( strPlatform.compare("win32") == 0 )
        oResult.set( L"WINDOWS_DESKTOP" );
    else if ( strPlatform.compare("wp") == 0 )
        oResult.set( L"WP7" );
    else if ( strPlatform.compare("wp8") == 0 )
        oResult.set( L"WP8");
    else if ( strPlatform.compare("android") == 0 )
        oResult.set( L"ANDROID" );
    else if ( strPlatform.compare("iphone") == 0 )
        oResult.set( L"APPLE" );
    else if ( strPlatform.compare("symbian") == 0 )
        oResult.set( L"SYMBIAN" );
    else
        oResult.set( L"UNKNOWN" );

#else //!RHODES_EMULATOR

#if defined(OS_MACOSX)
	oResult.set( PLATFORM_IOS );
#elif defined(OS_WINDOWS_DESKTOP)
	oResult.set( PLATFORM_WINDOWS_DESKTOP );
#elif defined(WINDOWS_PLATFORM)
#if defined(OS_WP8)
	oResult.set( PLATFORM_WP8 );
#else
	oResult.set( PLATFORM_WM_CE );
#endif
#elif defined(OS_SYMBIAN)
	oResult.set( L"SYMBIAN" );
#elif defined(OS_ANDROID)
    oResult.set( PLATFORM_ANDROID );
#elif defined(OS_LINUX)
    oResult.set( L"LINUX" );
#else
	oResult.set( L"UNKNOWN" );
#endif			

#endif
}

void CSystemImplBase::getHasCamera(CMethodResult& oResult)
{
#ifdef OS_WINDOWS_DESKTOP
    oResult.set(false);
#else
    oResult.set(true);
#endif

}

void CSystemImplBase::getPhoneNumber(CMethodResult& oResult)
{
    oResult.set(L"");
}

void CSystemImplBase::getDevicePushId(CMethodResult& oResult)
{
    rho::String strDeviceID;
	if ( rho::sync::RhoconnectClientManager::haveRhoconnectClientImpl() ) 
		strDeviceID = rho::sync::RhoconnectClientManager::clientRegisterGetDevicePin();

    oResult.set( strDeviceID );
}

void CSystemImplBase::getOsVersion(CMethodResult& oResult)
{
#ifdef RHODES_EMULATOR
    oResult.set( RHOSIMCONF().getString("os_version") );
#endif
}

void CSystemImplBase::getIsMotorolaDevice(CMethodResult& oResult)
{
    oResult.set(false);
}

void CSystemImplBase::getLocalServerPort(CMethodResult& oResult)
{
    oResult.set( (int64)atoi(RHODESAPP().getFreeListeningPort()) );
}

void CSystemImplBase::setLocalServerPort( __int64 value, CMethodResult& oResult)
{
    //Local port can be set only in confuguration file
}

void CSystemImplBase::getFreeServerPort(rho::apiGenerator::CMethodResult& oResult)
{
    oResult.set( (int64)RHODESAPP().determineFreeListeningPort() );
}

void CSystemImplBase::getHasTouchscreen(rho::apiGenerator::CMethodResult& oResult)
{
    oResult.set(true);
}

void CSystemImplBase::getSecurityTokenNotPassed(rho::apiGenerator::CMethodResult& oResult)
{
    oResult.set( RHODESAPP().isSecurityTokenNotPassed() );
}

void CSystemImplBase::getHasSqlite(rho::apiGenerator::CMethodResult& oResult)
{
    oResult.set(true);
}

void CSystemImplBase::getRealScreenWidth(CMethodResult& oResult)
{
    getScreenWidth(oResult);
}

void CSystemImplBase::getRealScreenHeight(CMethodResult& oResult)
{
    getScreenHeight(oResult);
}

void CSystemImplBase::getDeviceOwnerEmail(CMethodResult& oResult)
{
    oResult.set("");
}

void CSystemImplBase::getDeviceOwnerName(CMethodResult& oResult)
{
    oResult.set("");
}

void CSystemImplBase::getApplicationIconBadge(CMethodResult& oResult)
{
    oResult.set(0);
}

void CSystemImplBase::setApplicationIconBadge( __int64 value, CMethodResult& oResult)
{
}

void CSystemImplBase::getStartParams(rho::apiGenerator::CMethodResult& oResult)
{
    oResult.set( RHODESAPP().getStartParameters() );
}

//TODO: move rho_sys_unzip_file here
extern "C" int rho_sys_unzip_file(const char* szZipPath, const char* psw);
void CSystemImplBase::unzipFile( const rho::String& localPathToZip, const rho::String& password, rho::apiGenerator::CMethodResult& oResult)
{
    oResult.set( rho_sys_unzip_file( localPathToZip.c_str(), password.c_str()) );
}

void CSystemImplBase::zipFile( const rho::String& localPathToZip,  const rho::String& localPathToFile,  const rho::String& password, CMethodResult& oResult)
{
    ZRESULT res;

#if defined(UNICODE) && defined(WIN32)
    rho::StringW strZipFilePathW;
    convertToStringW(localPathToZip.c_str(), strZipFilePathW);

    CFilePath oPath(localPathToFile);
    rho::StringW strFileNameW;
    convertToStringW(oPath.getBaseName(), strFileNameW);

    rho::StringW strToZipPathW;
    convertToStringW(localPathToFile.c_str(), strToZipPathW);

    HZIP hz = CreateZip(strZipFilePathW.c_str(), password.c_str());
    if ( !hz )
        res = -1;
    else
    {
        res = ZipAdd( hz, strFileNameW.c_str(), strToZipPathW.c_str() );

        res = CloseZip(hz);
    }

#else

    HZIP hz = CreateZip(szZipFilePath, psw);
    if ( !hz )
        res = -1;
    else
    {
        CFilePath oPath(szToZipPath);

        res = ZipAdd( hz, oPath.getBaseName(), szToZipPath );

        res = CloseZip(hz);
    }
#endif

    oResult.set(res);
}

void CSystemImplBase::zipFiles( const rho::String& localPathToZip,  const rho::String& basePath,  const rho::Vector<rho::String>& filePathsToZip,  const rho::String& password, CMethodResult& oResult)
{
    int nRes = rho_sys_zip_files_with_path_array_ptr( localPathToZip.c_str(), basePath.c_str(), filePathsToZip, password.c_str() );
    oResult.set(nRes);
}

struct rho_param;
extern "C" void rho_sys_replace_current_bundleEx(const char* path, const char* finish_callback, bool do_not_restart_app, bool not_thread_mode );
void CSystemImplBase::replaceCurrentBundle( const rho::String& pathToBundle,  const rho::Hashtable<rho::String, rho::String>& params, rho::apiGenerator::CMethodResult& oResult)
{
    bool do_not_restart_app = false, not_thread_mode = false;
    if( params.containsKey("do_not_restart_app") )
        convertFromStringA( params.get("do_not_restart_app").c_str(), do_not_restart_app );
    if( params.containsKey("not_thread_mode") )
        convertFromStringA( params.get("not_thread_mode").c_str(), not_thread_mode );

    rho_sys_replace_current_bundleEx( pathToBundle.c_str(), params.containsKey("callback") ? params.get("callback").c_str():0, do_not_restart_app, not_thread_mode );
}

void CSystemImplBase::deleteFolder( const rho::String& pathToFolder, rho::apiGenerator::CMethodResult& oResult)
{
    CRhoFile::deleteFolder( pathToFolder.c_str() );
}

void CSystemImplBase::setDoNotBackupAttribute( const rho::String& pathToFile, rho::apiGenerator::CMethodResult& oResult)
{
    //iOS only
}

void CSystemImplBase::setRegistrySetting( int64 hive,  int64 type,  const rho::String& subKey,  const rho::String& setting,  const rho::String& value, rho::apiGenerator::CMethodResult& oResult)
{
    //Windows only
}

void CSystemImplBase::getRegistrySetting( int64 hive,  const rho::String& subKey,  const rho::String& setting, rho::apiGenerator::CMethodResult& oResult)
{
    //Windows only
    oResult.set("");
}

//TODO: move to Database
void CSystemImplBase::isBlobAttr( const rho::String& partition,  int64 sourceID,  const rho::String& attrName, rho::apiGenerator::CMethodResult& oResult)
{
    bool bRes = db::CDBAdapter::getDB( partition.c_str()).getAttrMgr().isBlobAttr((int)sourceID, attrName.c_str());
    oResult.set(bRes);
}

//TODO: move to Database
extern "C" void rho_sys_update_blob_attribs(const char* szPartition, int source_id);
void CSystemImplBase::updateBlobAttribs( const rho::String& partition,  int64 sourceID, rho::apiGenerator::CMethodResult& oResult)
{
    rho_sys_update_blob_attribs( partition.c_str(), (int)sourceID );
}

void CSystemImplBase::get_locale(rho::apiGenerator::CMethodResult& oResult)
{
    getLocale(oResult);    
}

void CSystemImplBase::setPushNotification( const rho::String& url,  const rho::String& url_params,  const rho::String& push_client, rho::apiGenerator::CMethodResult& oResult)
{
    String strTypes = push_client.length() > 0 ? push_client : "default";

    String item;
    String::size_type pos = 0;

    while (String_getline(strTypes, item, pos, ';')) {
        RHODESAPP().setPushNotification( url, url_params, item);
    }
}

static int g_rho_has_network = 1, g_rho_has_cellnetwork = 0;

extern "C" void rho_sysimpl_sethas_network(int nValue)
{
    g_rho_has_network = nValue > 1 ? 1 : 0;
}

extern "C" void rho_sysimpl_sethas_cellnetwork(int nValue)
{
    g_rho_has_cellnetwork = nValue;
}

void CSystemImplBase::getHasNetwork(rho::apiGenerator::CMethodResult& oResult)
{
    oResult.set(g_rho_has_network!= 0 || g_rho_has_cellnetwork!= 0);
}

void CSystemImplBase::getHasWifiNetwork(rho::apiGenerator::CMethodResult& oResult)
{
    oResult.set(g_rho_has_network!= 0);
}

void CSystemImplBase::getHasCellNetwork(rho::apiGenerator::CMethodResult& oResult)
{
    oResult.set(g_rho_has_cellnetwork!= 0);
}

void CSystemImplBase::setScreenRotationNotification( const rho::String& url,  const rho::String& url_params, rho::apiGenerator::CMethodResult& oResult)
{
	RHODESAPP().setScreenRotationNotification( url, url_params );
}

extern "C" void rho_sys_app_exit();
void CSystemImplBase::exit(rho::apiGenerator::CMethodResult& oResult)
{
    rho_sys_app_exit();
}

void CSystemImplBase::set_sleeping( bool enable, rho::apiGenerator::CMethodResult& oResult)
{
    setScreenSleeping(enable, oResult);
}

void CSystemImplBase::set_application_icon_badge( int64 badgeNumber, rho::apiGenerator::CMethodResult& oResult)
{
    setApplicationIconBadge(badgeNumber, oResult);
}

void CSystemImplBase::startTimer( int64 interval,  const rho::String& url,  const rho::String& url_params, rho::apiGenerator::CMethodResult& oResult)
{
    RHODESAPP().getTimer().addTimer( (int)interval, url.c_str(), url_params.c_str() );
}

void CSystemImplBase::stopTimer( const rho::String& url, rho::apiGenerator::CMethodResult& oResult)
{
    RHODESAPP().getTimer().stopTimer( url.c_str());
}

void CSystemImplBase::setNetworkStatusNotify( const rho::String& url, int64 poll_interval, rho::apiGenerator::CMethodResult& oResult)
{
    RHODESAPP().setNetworkStatusNotify( url, (int)poll_interval );
}

void CSystemImplBase::clearNetworkStatusNotify(rho::apiGenerator::CMethodResult& oResult)
{
    RHODESAPP().clearNetworkStatusNotify();
}

void CSystemImplBase::set_http_proxy_url( const rho::String& proxyURI, rho::apiGenerator::CMethodResult& oResult)
{
    //windows only
}

void CSystemImplBase::unset_http_proxy(rho::apiGenerator::CMethodResult& oResult)
{
    //windows only
}

}