﻿/*------------------------------------------------------------------------
* (The MIT License)
* 
* Copyright (c) 2008-2011 Rhomobile, Inc.
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
* 
* http://rhomobile.com
*------------------------------------------------------------------------*/

#include "pch_rhoruntime.h"
#include "rhoruntime.h"

//#include "../../shared/sqlite/sqlite3.h"
//#include "logging/RhoLogConf.h"
#include "common/RhoStd.h"
#include "common/RhodesApp.h"
#include "common/RhoFilePath.h"
#include "rubyext/WebView.h"

using namespace rhoruntime;
using namespace Platform;

CRhoRuntime^ CRhoRuntime::m_instance = nullptr;

CRhoRuntime::CRhoRuntime(IMainPage^ mainPage):
	m_MainPage(mainPage)
{
}

CRhoRuntime^ CRhoRuntime::getInstance(IMainPage^ mainPage)
{
	if (m_instance == nullptr)
		m_instance = ref new CRhoRuntime(mainPage);
    return m_instance;
}

CRhoRuntime^ CRhoRuntime::getInstance()
{
    return m_instance;
}

IMainPage^ CRhoRuntime::getMainPage()
{
	return m_MainPage;
}

// rhodes executed in a separate thread
void CRhoRuntime::Execute()
{
	rho::String m_strRootPath = rho_native_rhopath(), m_logPort, m_strRuntimePath;
    rho_logconf_Init(m_strRootPath.c_str(), m_strRootPath.c_str(), m_logPort.c_str());

    rho::common::CRhodesApp::Create(m_strRootPath, m_strRootPath, m_strRuntimePath);
    //RHODESAPP().setExtManager( &m_oExtManager );

	//Create Main window
	createCustomMenu();

	RHODESAPP().startApp();

	// wait for 5 seconds
	//m_MainPage->DoWait(10000);
	
	//m_MainPage->tabbarInitialize();
	//m_MainPage->tabbarAddTab(ref new String(L"Tab 1"), ref new String(L""), ref new String(L"/app"), false, ref new String(L""), ref new String(L""), false, false);
	//m_MainPage->tabbarAddTab(ref new String(L"Tab 2"), ref new String(L""), ref new String(L"/app"), false, ref new String(L""), ref new String(L""), false, false);
	//m_MainPage->tabbarAddTab(ref new String(L"Tab 3"), ref new String(L""), ref new String(L"/app"), false, ref new String(L""), ref new String(L""), false, false);
	//m_MainPage->tabbarShow();

	//m_MainPage->fullscreenCommand(1);
	//m_MainPage->exitCommand();
}


// *** CALLBACKS from MainPage object ***

void CRhoRuntime::updateSizeProperties(int width, int height)
{
	// TODO: implement
}

void CRhoRuntime::onActivate(int active)
{
	//rho_webview_navigate("system/uicreated", 0); //HOTFIX, should remove after threadqueue fix  
	RHODESAPP().callUiCreatedCallback();
    //rho_rhodesapp_callAppActiveCallback(active);
    //if (!active)
    //    rho_geoimpl_turngpsoff();
}

void CRhoRuntime::logEvent(::Platform::String^ message)
{
	LOG(INFO) + rho::common::convertToStringW(message->Data());
}

void CRhoRuntime::createCustomMenu()
{
	IMainPage^ mainPage = CRhoRuntime::getInstance()->getMainPage();
	rho::Vector<rho::common::CAppMenuItem> m_arAppMenuItems;
    RHODESAPP().getAppMenu().copyMenuItems(m_arAppMenuItems);
#ifdef ENABLE_DYNAMIC_RHOBUNDLE
	rho::String index = "index"RHO_ERB_EXT;
    rho::String strIndexPage = rho::common::CFilePath::join(RHODESAPP().getStartUrl(), index);
    if ( RHODESAPP().getCurrentUrl().compare(RHODESAPP().getStartUrl()) == 0 ||
         RHODESAPP().getCurrentUrl().compare(strIndexPage) == 0 )
        m_arAppMenuItems.addElement(rho::common::CAppMenuItem("Reload RhoBundle","reload_rhobundle"));
#endif //ENABLE_DYNAMIC_RHOBUNDLE

    //update UI with custom menu items
	mainPage->menuClear();
	RHODESAPP().setAppBackUrl("");
    for ( unsigned int i = 0; i < m_arAppMenuItems.size(); i++)
    {
        rho::common::CAppMenuItem& oItem = m_arAppMenuItems.elementAt(i);
        if (oItem.m_eType == rho::common::CAppMenuItem::emtSeparator) 
            mainPage->menuAddSeparator();
        else {
 			rho::StringW labelW;
			rho::common::convertToStringW(oItem.m_strLabel.c_str(), labelW);
			if ((oItem.m_eType == rho::common::CAppMenuItem::emtBack) && (oItem.m_strLink.compare("back") != 0))
				RHODESAPP().setAppBackUrl(oItem.m_strLink);
			mainPage->menuAddAction((oItem.m_eType == rho::common::CAppMenuItem::emtClose ? "Exit" : ref new Platform::String(labelW.c_str())), i);
        }
    }
}

void CRhoRuntime::onCustomMenuItemCommand(int nItemPos)
{
	rho::Vector<rho::common::CAppMenuItem> m_arAppMenuItems;
    RHODESAPP().getAppMenu().copyMenuItems(m_arAppMenuItems);

	if ( nItemPos < 0 || nItemPos >= (int)m_arAppMenuItems.size() )
        return;

    rho::common::CAppMenuItem& oMenuItem = m_arAppMenuItems.elementAt(nItemPos);
    if ( oMenuItem.m_eType == rho::common::CAppMenuItem::emtUrl )
    {
		// TODO: implement ReloadRhoBundle
        if ( oMenuItem.m_strLink == "reload_rhobundle" )
        {
			#ifdef ENABLE_DYNAMIC_RHOBUNDLE
            //if ( RHODESAPP().getRhobundleReloadUrl().length()>0 ) {
            //    CAppManager::ReloadRhoBundle(m_hWnd,RHODESAPP().getRhobundleReloadUrl().c_str(), NULL);
            //} else {
            //    MessageBox(_T("Path to the bundle is not defined."),_T("Information"), MB_OK | MB_ICONINFORMATION );
            //}
			#endif
			return;
        }
    }
    oMenuItem.processCommand();
}

void CRhoRuntime::onToolbarAction(::Platform::String^ action)
{
	if ( action->Equals("forward") )
		rho_webview_navigate_forward();
	else
		RHODESAPP().loadUrl(rho::common::convertToStringA(action->Data()));
}

void CRhoRuntime::onTabbarCurrentChanged(int index, ::Platform::String^ action)
{
	// TODO: complete implementation

	//if (tbrp["on_change_tab_callback"].toString().length() > 0) {
	//	QString body = QString("&rho_callback=1&tab_index=") + QVariant(index).toString();
	//	rho::String* cbStr = new rho::String(tbrp["on_change_tab_callback"].toString().toStdString());
	//	rho::String* bStr = new rho::String(body.toStdString());
	//	const char* b = bStr->c_str();
	//	rho_net_request_with_data(RHODESAPP().canonicalizeRhoUrl(*cbStr).c_str(), b);
	//}

    //if (tbrp["reload"].toBool() || (ui->webView && (ui->webView->history()->count()==0))) {
	RHODESAPP().loadUrl(rho::common::convertToStringA(action->Data()));
    //}
}


void CRhoRuntime::onWindowClose(void)
{
	// TODO: implement
}

void CRhoRuntime::onWebViewUrlChanged(::Platform::String^ url)
{
	RHODESAPP().keepLastVisitedUrl(rho::common::convertToStringA(url->Data()));
}

bool CRhoRuntime::onBackKeyPress()
{
	// TODO: implement check for history size (return false if there's no history)
	RHODESAPP().navigateBack();
	return true;
}


// *** PUBLIC METHODS ***

bool CRhoRuntime::Initialize(::Platform::String^ title)
{
	return true;
}

void CRhoRuntime::DestroyUi(void)
{
	rho_rhodesapp_callUiDestroyedCallback();
}

extern "C" void rho_sys_app_exit()
{
	// exit application
	CRhoRuntime::getInstance()->getMainPage()->exitCommand();
}

#ifdef OS_WP8
extern "C" void rho_net_impl_network_indicator(int active)
{
	 //TODO: rho_net_ping_network
    RAWLOGC_INFO("rhoruntime.cpp", "rho_net_impl_network_indicator() has no implementation on WP8.");
}

extern "C" int rho_net_ping_network(const char* szHost)
{
    //TODO: rho_net_ping_network
    RAWLOGC_INFO("rhoruntime.cpp", "rho_net_ping_network() has no implementation on WP8.");
    return 1;
}
#endif

extern "C" void rho_wm_impl_performOnUiThread(rho::common::IRhoRunnable* pTask)
{
	//TODO: rho_wm_impl_performOnUiThread
}


extern "C" rho::String GetLocalFolderFullPath()
{
	return  rho::common::convertToStringA(Windows::Storage::ApplicationData::Current->LocalFolder->Path->Data());
}