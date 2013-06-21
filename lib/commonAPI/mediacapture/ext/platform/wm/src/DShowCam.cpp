//------------------------------------------------------------------
// FILENAME: DShowCam.cpp
//
// MODULE NAME: DirectShow Camera class
//
// Copyright(c) 2003 Symbol Technologies Inc. All rights reserved.
//
// DESCRIPTION:	
//
// NOTES:
//
// %IF Symbol_Internal
// AUTHOR: Wajra Attale	
// CREATION DATE:	01/25/2007
// DERIVED FROM:	New File
//
// EDIT HISTORY: No PVCS history for this file
// $Log:   T:/MPA2.0_sandbox/archives/SymbolValueAdd_mpa2/CCL/Camera/DShowCam/test/DShowTestApp/DShowCam.cpp-arc  $
//
//   Rev 1.0   Apr 18 2008 10:17:30   cheung
//Initial revision.
//
//   Rev 1.0   Dec 14 2007 07:20:58   sepalas
//Initial revision.
//
//%End
//------------------------------------------------------------------
#include "DShowCam.h"
#include "DShowStruct.h"
#include "DShowDef.h"
#include "PropertyBag.h"

//extern TCHAR m_tcImagerSound[MAX_PATH];
#ifdef LOG_TO_FILE
FILE *LogFile;
#endif
//CDShowCam::CDShowCam(PBModule* pModule)
CDShowCam::CDShowCam(void)
{
	// SPS
	// m_pModule = pModule;
	//DEBUGIT(LogFile,"CDShowCam constructor \n");
	LOG(INFO) + __FUNCTION__ + "4100:Calling CDShowCam constructor ...";
	// SPS
	//Initialize the COM library
	/*HRESULT hr = CoInitialize(NULL);
	if(FAILED(hr))
	{
		m_lErrno = 0x001;
		// SPS
		//DEBUGIT(LogFile,"\n Error Initializing COM !!!\n");
		LOG(ERROR) + __FUNCTION__ + "4410:Error Initializing COM";
		// SPS
		exit(1);
	}*/

	//Initializing class members
	m_hCamHdl = NULL;
	m_pGraphBuilder = NULL;
	m_pCaptureGraphBuilder = NULL;
	m_pMediaControl = NULL;
	m_pViewWindow = NULL;
	m_pMediaEventEx = NULL;
	m_pVideoCaptureFilter = NULL;
	m_pAudioCaptureFilter = NULL;
	m_pImgCtrl = NULL;
	m_pCamCtrl = NULL;
	m_pStrConf = NULL;
	m_pMux = NULL;
	m_pSink = NULL;
	m_pVideoEncoder = NULL;
	m_pVideoWrapperFilter = NULL;
	m_pStillSink = NULL;
	m_pVideoCtrl = NULL;
	m_pPrvPin = NULL;
	m_wsVFName = DFT_VDO_FN;
	m_pVideoRenderer=NULL;
	m_nFileAutoCnt=0;

	InitializeCriticalSection(&m_DSCamCriticalSection);
	EnterCriticalSection(&m_DSCamCriticalSection);
	LeaveCriticalSection(&m_DSCamCriticalSection);
	m_bCaptureSoundRegKeyExists = FALSE;
	
	/*HKEY hKey = 0;
	WCHAR buf[255] = {0};
	DWORD dwType = 0;
	DWORD dwBufSize = sizeof(buf);
	LPCWSTR subkey = L"Drivers\\BuiltIn\\DShowCam";

	if( RegOpenKey(HKEY_LOCAL_MACHINE,subkey,&hKey) == ERROR_SUCCESS)
	{
		dwType = REG_SZ;
		if( RegQueryValueEx(hKey,L"ShutterSound",0, &dwType, (BYTE*)buf, &dwBufSize) == ERROR_SUCCESS)
		{	// if the key exists assume that DirectShow will handle the shutter sound
			//int len;

			m_bCaptureSoundRegKeyExists = TRUE;
			//if (m_tcImagerSound[0] && (wcslen(m_tcImagerSound) > 0))
			//{
			//	// write the shutter click wav to the registry
			//	len = wcslen(m_tcImagerSound);
			//	if (wcscmp(buf, m_tcImagerSound) != 0)
			//	{
			//		RegSetValueEx(hKey, L"ShutterSound", 0, dwType, (BYTE*)m_tcImagerSound, len*sizeof(TCHAR));
			//	}
			//}
			RegCloseKey(hKey);
		}
	}*/
	LOG(INFO) + __FUNCTION__ + "4199:CDShowCam constructor called.";
}

CDShowCam::~CDShowCam(void)
{
	// SPS
	//DEBUGIT(LogFile,"CDShowCam destructor \n");
	LOG(INFO) + __FUNCTION__ + "4200:Calling CDShowCam destructor ...";
	// SPS
	
	EnterCriticalSection(&m_DSCamCriticalSection);
	DeleteCriticalSection(&m_DSCamCriticalSection);
	CloseHandle(m_hCamHdl);
	m_hCamHdl = NULL;

	//Closes the COM library 
	CoUninitialize();

	LOG(INFO) + __FUNCTION__ + "4299:CDShowCam destructor called.";
}

//------------------------------------------------------------------
//
// Prototype:	BOOL FindFirstCam(wstring* pwsCamID)
//
// Description:	This is used to enumerate First DirectShow camera driver.
//
// Parameters:	pwsCamID is unicode string pointer. 
//
// Returns:	Boolean "TRUE" if function succeeds otherwise Boolean "FALSE".
// 
// Notes:
//------------------------------------------------------------------
BOOL CDShowCam::FindFirstCam(wstring* pwsCamID)
{
	DEVMGR_DEVICE_INFORMATION di;
	*pwsCamID = L"";
	
	LOG(INFO) + "CDShowCam::FindFirstCam-1 calling ...";

	// The driver material doesn't ship as part of the SDK. This GUID is hardcoded
	// here to be able to enumerate the camera drivers and pass the name of the driver to the video capture filter
	GUID guidCamera = {0xCB998A05, 0x122C, 0x4166, 0x84, 0x6A, 0x93, 0x3E, 0x4D, 0x7E, 0x3C, 0x86};
	
	di.dwSize = sizeof(di);
	m_hCamHdl = FindFirstDevice(DeviceSearchByGuid, &guidCamera, &di);
	if((m_hCamHdl == INVALID_HANDLE_VALUE) ||(m_hCamHdl == NULL) || (di.hDevice == NULL))
	{
		// SPS
		//DEBUGIT(LogFile,"\n!!! No Camera found !!!!\n");
		LOG(ERROR) + "CDShowCam::FindFirstCam-2 !!! No Camera found !!!!";
		// SPS
		m_lErrno = 0x003;
	}
	else 
	{
		wcscpy((wchar_t* )pwsCamID->c_str(), di.szLegacyName);
		return TRUE;
	}

	LOG(INFO) + "CDShowCam::FindFirstCam-3 called.";
	return FALSE;
} //end FindFirstCam

//------------------------------------------------------------------
//
// Prototype:	BOOL FindNextCam(wstring* pwsCamID)
//
// Description:	This is used to enumerate Next available 
//			 	DirectShow camera driver.
//
// Parameters:	pwsCamID is unicode string pointer. 
//
// Returns:	Boolean "TRUE" if function succeeds otherwise Boolean "FALSE".
// 
// Notes:
//------------------------------------------------------------------
BOOL CDShowCam::FindNextCam(wstring* pwsCamID)
{
	DEVMGR_DEVICE_INFORMATION di;

	LOG(INFO) + __FUNCTION__ + "CDShowCam::FindNextCam-1 Calling ...";

	*pwsCamID = L"";
	if(m_hCamHdl == INVALID_HANDLE_VALUE)
	{
		m_lErrno = 0x003;
	}
	else 
	{
		if(FindNextDevice(m_hCamHdl, &di))
		{
			wcscpy((wchar_t* )pwsCamID->c_str(), di.szLegacyName);
			return TRUE;
		}
			
	}
	LOG(INFO) + __FUNCTION__ + "CDShowCam::FindNextCam-2 called.";
	return FALSE;

} //end FindNextCam

//------------------------------------------------------------------
//
// Prototype:	HRESULT InitFilterGrp()
//
// Description:	This is used to create the Filter Graph Manager(FGM) and
//				initialize several filter graph related components.
//
// Parameters:	none
//
// Returns:	S_OK if all required components are created successfully.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::InitFilterGrp()
{
	HRESULT hr = 0;
	// Create the Filter Graph Manager.
	LOG(INFO) + "CDShowCam::InitFilterGrp-1 Calling InitFilterGrp ...";
	
    if (m_pGraphBuilder==NULL)
	{	
		hr = CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&m_pGraphBuilder);
	}
	
    if(SUCCEEDED(hr))
	{
		// Create the Capture Graph Builder.
		if (m_pCaptureGraphBuilder == NULL)
		{	hr = CoCreateInstance(CLSID_CaptureGraphBuilder, NULL, CLSCTX_INPROC_SERVER,\
														IID_ICaptureGraphBuilder2, (void**)&m_pCaptureGraphBuilder);      
		}
  		
		if(SUCCEEDED(hr))
		{
            //Specifies the Filter Graph for the Capture Graph Builder to use.
			hr = m_pCaptureGraphBuilder->SetFiltergraph(m_pGraphBuilder);
			
			if(SUCCEEDED(hr))
			{
				//Obtain the interfaces for media control, video and media event interfaces
				hr = m_pGraphBuilder->QueryInterface (IID_IMediaControl, (void **)&m_pMediaControl);
				
				if(FAILED(hr))
				{
					LOG(ERROR) + __FUNCTION__ + "CDShowCam::InitFilterGrp-2 QueryInterface error";
					return hr;
				}

				hr = m_pGraphBuilder->QueryInterface (IID_IVideoWindow, (void **)&m_pViewWindow);
				
				if(FAILED(hr))
				{
					LOG(ERROR) + __FUNCTION__ + "CDShowCam::InitFilterGrp-3 QueryInterface error";
					return hr;
				}

				hr = m_pGraphBuilder->QueryInterface (IID_IMediaEventEx, (void **)&m_pMediaEventEx);
				
				if(FAILED(hr))
				{
					LOG(ERROR) + __FUNCTION__ + "CDShowCam::InitFilterGrp-4 QueryInterface error";
					return hr;
				}

				hr = m_pGraphBuilder->QueryInterface (IID_IMediaSeeking, (void **)&m_pMediaSeeking);
					
				if(FAILED(hr))
				{
					LOG(ERROR) + __FUNCTION__ + "CDShowCam::InitFilterGrp-5 QueryInterface error";
					return hr;
				}
			}	
		}
        else
		{
			//free resources
			LOG(ERROR) + __FUNCTION__ + "CDShowCam::InitFilterGrp-6 CoCreateInstance error";
		    m_pCaptureGraphBuilder->Release();
			m_pCaptureGraphBuilder = NULL;
			m_pGraphBuilder->Release();
			m_pGraphBuilder = NULL;
		}
    }
	else
	{
		//free resources
		LOG(ERROR) + __FUNCTION__ + "CDShowCam::InitFilterGrp-7 CoCreateInstance error";
		m_pGraphBuilder->Release();
		m_pGraphBuilder = NULL;
	}
		
	LOG(INFO) + __FUNCTION__ + "CDShowCam::InitFilterGrp-8 InitFilterGrp called.";
	return hr; 
} //end InitFilterGrp

//------------------------------------------------------------------
//
// Prototype:	BOOL BuildFilterGrp(CamConfig* ptCamcfg)
//
// Description:	This is used to combine all requested objects in a Filter Graph.
//
// Parameters:	Pointer to CamConfig structure.
//
// Returns:	Boolean "TRUE" if Filter Graph is created successfully  otherwise
//			Boolean "FALSE".
// 
// Notes:
//------------------------------------------------------------------
BOOL CDShowCam::BuildFilterGrp(CamConfig* ptCamcfg)
{
	HRESULT hr = S_OK;
	CPropertyBag  PropBag;
	//populate the Camera configeration info to a member stucture
	m_tCamcfg.bIsAudioEnb  = ptCamcfg->bIsAudioEnb;
	m_tCamcfg.bIsCapEnb    = ptCamcfg->bIsCapEnb;
	m_tCamcfg.bIsPrvEnb    = ptCamcfg->bIsPrvEnb;
	m_tCamcfg.bIsStillEnb  = ptCamcfg->bIsStillEnb;
	m_tCamcfg.hwndOwnerWnd = ptCamcfg->hwndOwnerWnd;
	m_tCamcfg.rc.bottom    = ptCamcfg->rc.bottom;
	m_tCamcfg.rc.left      = ptCamcfg->rc.left;
	m_tCamcfg.rc.right     = ptCamcfg->rc.right;
	m_tCamcfg.rc.top       = ptCamcfg->rc.top;
	m_tCamcfg.sCamID       = ptCamcfg->sCamID;


	IPersistPropertyBag* pVideoPropertyBag = NULL;
	IPersistPropertyBag* pAudioPropertyBag = NULL;

	CComVariant varCamName;
	VariantInit(&varCamName);
	
	
	if(ptCamcfg->bIsCapEnb)
	{
		hr = CoCreateInstance(CLSID_VideoCapture, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&m_pVideoCaptureFilter);
		if(FAILED(hr))
		{
			LOG(ERROR) + "CDShowCam::BuildFilterGrp:-1-VFW_S_NOPREVIEWPIN";
			return FALSE;
		}
		
		hr = m_pVideoCaptureFilter->QueryInterface(IID_IPersistPropertyBag, (void **)&pVideoPropertyBag);
		if(FAILED(hr))
		{
			LOG(ERROR) + "CDShowCam::BuildFilterGrp:-2-QueryInterface";
			return FALSE;
		}
		
		varCamName = ptCamcfg->sCamID.c_str();
		if(( varCamName.vt == VT_BSTR ) == NULL ) 
		{
			LOG(ERROR) + "CDShowCam::BuildFilterGrp:-3-Null varCamName";
			return FALSE;
		}
		
		PropBag.Write(L"VCapName", &varCamName);   
		pVideoPropertyBag->Load(&PropBag, NULL);
		pVideoPropertyBag->Release();
		
		// Add Video Capture filter to the graph.
		hr = m_pGraphBuilder->AddFilter(m_pVideoCaptureFilter, L"Video Capture Filter");
		if(FAILED(hr))
		{
			LOG(ERROR) + "CDShowCam::BuildFilterGrp:-4-AddFilter";
			return FALSE;
		}
		
		//--------------------------------Adding DMO Encoder Filter to the graph--------------------------------------
		hr = CoCreateInstance(CLSID_DMOWrapperFilter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&m_pVideoEncoder));
		if(FAILED(hr))
		{
			LOG(ERROR) + "CDShowCam::BuildFilterGrp:-5-CLSID_DMOWrapperFilter";
			return FALSE;
		}

		hr = m_pVideoEncoder->QueryInterface(IID_IDMOWrapperFilter, reinterpret_cast<void**>(&m_pVideoWrapperFilter));
		if(FAILED(hr))
		{
			LOG(ERROR) + "CDShowCam::BuildFilterGrp:-6-IID_IDMOWrapperFilter";
			return FALSE;
		}
		
		hr = m_pVideoWrapperFilter->Init(CLSID_CWMV9EncMediaObject, DMOCATEGORY_VIDEO_ENCODER);
		if(FAILED(hr))
		{
			LOG(ERROR) + "CDShowCam::BuildFilterGrp:-7-Init WrapperFilter";
			return FALSE;
		}
		m_pVideoWrapperFilter->Release();

		hr = m_pGraphBuilder->AddFilter(m_pVideoEncoder, L"WMV9 DMO Encoder");
		if(FAILED(hr))
		{
			LOG(ERROR) + "CDShowCam::BuildFilterGrp:-8-AddFilter";
			return FALSE;
		}
		
		//hr = m_pCaptureGraphBuilder->SetOutputFileName(&MEDIASUBTYPE_Asf, DFT_VDO_FN, &m_pMux, &m_pSink);
		hr = m_pCaptureGraphBuilder->SetOutputFileName(&MEDIASUBTYPE_Asf, DFT_VDO_FN, &m_pMux, &m_pSink);
		if((FAILED(hr)))
		{
			LOG(ERROR) + "CDShowCam::BuildFilterGrp:-9-SetOutputFileName";
			return FALSE;
		}

		hr = m_pCaptureGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_pVideoCaptureFilter, m_pVideoEncoder, m_pMux);
		if((FAILED(hr)))
		{
			switch (hr)
			{
			case VFW_S_NOPREVIEWPIN:
				LOG(ERROR) + "CDShowCam::BuildFilterGrp:-10-VFW_S_NOPREVIEWPIN";
				// SPS
				break;
			case E_FAIL:
				LOG(ERROR) + "CDShowCam::BuildFilterGrp:-11-E_FAIL";
				break;
			case E_INVALIDARG:
				LOG(ERROR) + "CDShowCam::BuildFilterGrp:-12-E_INVALIDARG";
				break;
			case E_POINTER:
				LOG(ERROR) + "CDShowCam::BuildFilterGrp:-13-E_POINTER";
				break;
			case VFW_E_NOT_IN_GRAPH:
				LOG(ERROR) + "CDShowCam::BuildFilterGrp:-14-VFW_E_NOT_IN_GRAPH";
				break;
			default:
				{
					WCHAR buf[80];
					wsprintf(buf, L"CDShowCam::BuildFilterGrp:-15-Unknown hr: 0x%X", hr);
					LOG(ERROR) + buf;
				}
				break;
			}
			return FALSE;
		}
		//-----------------------------------------------------------------------------------------------------------
	}

	if(ptCamcfg->bIsAudioEnb)
	{
		hr = CoCreateInstance(CLSID_AudioCapture, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&m_pAudioCaptureFilter);
		if(FAILED(hr))
		{
			LOG(ERROR) + "CDShowCam::BuildFilterGrp:-16-CLSID_AudioCapture";
			return FALSE;
		}
	
		hr = m_pAudioCaptureFilter->QueryInterface(IID_IPersistPropertyBag, (void **) &pAudioPropertyBag);
		if(SUCCEEDED(hr))
		{
			// use the default audio driver. If a particular driver was required
			// it is selected the same as the camera driver.
			hr = pAudioPropertyBag->Load(NULL, NULL);
			if(SUCCEEDED(hr))
			{
				// add the filter to the graph
				hr = m_pGraphBuilder->AddFilter(m_pAudioCaptureFilter, L"Audio Capture Filter");
				if(SUCCEEDED(hr))
				{
					hr = m_pCaptureGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio, m_pAudioCaptureFilter, NULL, m_pMux);
	    		}
				else
				{
					LOG(ERROR) + "CDShowCam::BuildFilterGrp:-17-RenderStream";
					return FALSE;
				}
					
			}
		}
	}

	//Setup still capture Filters
	if(ptCamcfg->bIsCapEnb && ptCamcfg->bIsStillEnb)
	{
		hr = SetupStill();
		if(FAILED(hr))
		{
			LOG(ERROR) + "CDShowCam::BuildFilterGrp:-18-##### Still Setup Error ######";
			return FALSE;
		}
	}

	if(ptCamcfg->bIsCapEnb && ptCamcfg->bIsPrvEnb)
	{
		hr = SetupPreview(ptCamcfg->hwndOwnerWnd, ptCamcfg->rc);
		if(FAILED(hr))
		{
			LOG(ERROR) + "CDShowCam::BuildFilterGrp:-19-##### Preview Error ######";
			return FALSE;
		}
	
		hr = SetupFlip();
		if(FAILED(hr))
		{
			LOG(ERROR) + "CDShowCam::BuildFilterGrp:-20-##### Flip Setup Error ######";
			return FALSE;
		}
	}
	return TRUE;
}// end BuildFilterGrp

//------------------------------------------------------------------
//
// Prototype:	BOOL ReBuildGrp()
//
// Description:	This is used to re connect required components of the Filter Graph.
//
// Parameters:	None
//
// Returns:	Boolean "TRUE" if Re build is successful otherwise
//			Boolean "FALSE".
// 
// Notes:
//------------------------------------------------------------------
BOOL CDShowCam::ReBuildGrp()
{
	HRESULT hr;
	//Remove Video capture Filter
	m_pGraphBuilder->RemoveFilter(m_pVideoCaptureFilter);
	m_pGraphBuilder->RemoveFilter(m_pVideoRenderer);	
	
	// Add Video Capture filter to the graph.
	hr = m_pGraphBuilder->AddFilter(m_pVideoCaptureFilter, L"Video Capture Filter");
	if(FAILED(hr))
	{
		LOG(ERROR) + "CDShowCam::ReBuildGrp:-1-AddFilter-Video Capture Filter";
		return FALSE;
	}
	
	// Add Video Renderer filter to the graph.
	hr = m_pGraphBuilder->AddFilter(m_pVideoRenderer, L"My Video Renderer");
	if(FAILED(hr))
	{
		LOG(ERROR) + "CDShowCam::ReBuildGrp:-2-AddFilter-My Video Renderer";
		return FALSE;
	}
	hr = m_pCaptureGraphBuilder->RenderStream( &PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pVideoCaptureFilter, NULL, m_pVideoRenderer);
	//Remove Filters from the graph
	if((FAILED(hr)))
	{
		LOG(ERROR) + "CDShowCam::ReBuildGrp:-3-RenderStream";
		return FALSE;
	}	
return TRUE;
}
//------------------------------------------------------------------
//
// Prototype:	BOOL RunGrp()
//
// Description:	This method switches the entire filter graph into a running state. 
//
// Parameters:	none
//
// Returns:	boolean "TRUE" if Filter Graph has started running successfully otherwise
//			boolean "FALSE".
// 
// Notes:
//------------------------------------------------------------------
BOOL CDShowCam::RunGrp()
{	
	BOOL result;
	LOG(INFO) + __FUNCTION__ + "CDShowCam::RunGrp-1 waiting for critical section";
	
	EnterCriticalSection(&m_DSCamCriticalSection);
	LOG(INFO) + __FUNCTION__ + "CDShowCam::RunGrp-2 entered critical section";
	
	HRESULT hr = m_pCaptureGraphBuilder->ControlStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_pVideoCaptureFilter, 0, 0, 0, 0);
	if (m_tCamcfg.bIsAudioEnb)
		hr = m_pCaptureGraphBuilder->ControlStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio, m_pAudioCaptureFilter, 0, 0, 0, 0);

	if(!(SUCCEEDED(hr)))
    {
		LOG(ERROR) + __FUNCTION__ + "CDShowCam::RunGrp-3 CaptureGraphBuilder->ControlStream failed";
			
		LeaveCriticalSection(&m_DSCamCriticalSection);
        return FALSE;
	}

	//Run the Filter Graph
	hr = m_pMediaControl->Run();
	// do we have to wait for fillters to change state ?
	if (hr==S_FALSE)
	{
		LOG(INFO) + __FUNCTION__ + "CDShowCam::RunGrp-4 waiting form_pMediaControl->Pause()";
		
		FILTER_STATE fs;
		int timeout= 10000;
		hr = m_pMediaControl->GetState(timeout, (OAFilterState*)&fs);
		if FAILED(hr)
		{
			LOG(WARNING) + __FUNCTION__ + "CDShowCam::RunGrp-5 CDShowCam::ResetGrp filters stop timeout";
		}
	}
	// if filters changed state immediately or before timeout.
	if(SUCCEEDED(hr))
		result = TRUE;
	else 
	{	
		LOG(ERROR) + __FUNCTION__ + "CDShowCam::RunGrp-6 m_pMediaControl->Run()failed";
		
		LeaveCriticalSection(&m_DSCamCriticalSection);
		return  FALSE;
	}

	LeaveCriticalSection(&m_DSCamCriticalSection);
	LOG(INFO) + __FUNCTION__ + "CDShowCam::RunGrp-7 left section";
	
	return result;
} //end RunGrp

//------------------------------------------------------------------
//
// Prototype:	BOOL PauseGrp()
//
// Description:	This method pauses all the filters in the filter graph. 
//
// Parameters:	none
//
// Returns:	boolean "TRUE" if Filter Graph has started running successfully otherwise
//			boolean "FALSE".
// 
// Notes:
//------------------------------------------------------------------
BOOL CDShowCam::PauseGrp()
{
	BOOL result ;
	LOG(INFO) + __FUNCTION__ + "CDShowCam::PauseGrp-1 waiting for critical section";
	
	EnterCriticalSection(&m_DSCamCriticalSection);
	LOG(INFO) + __FUNCTION__ + "CDShowCam::PauseGrp-2 entered for critical section";
	
	//Pause the Filter Graph
	HRESULT hr = m_pMediaControl->Pause();
	if (hr==S_FALSE)
		{
			LOG(INFO) + __FUNCTION__ + "CDShowCam::PauseGrp-3 waiting form_pMediaControl->Pause()";
			
			FILTER_STATE fs;
			int timeout= 10000;
			hr = m_pMediaControl->GetState(timeout, (OAFilterState*)&fs);
			if FAILED(hr)
			{
				LOG(WARNING) + __FUNCTION__ + "CDShowCam::PauseGrp-4 filters stop timeout";
			}
	}
	if(SUCCEEDED(hr))
	{		
		result= TRUE;
	}
	else  
		result =  FALSE;
	LeaveCriticalSection(&m_DSCamCriticalSection);
	LOG(INFO) + __FUNCTION__ + "CDShowCam::PauseGrp-5 left critical section";
	return result;
} //end PauseGrp

//------------------------------------------------------------------
//
// Prototype:	BOOL StopGrp()
//
// Description:	This method switches all filters in the filter graph to a stopped state. 
//
// Parameters:	none
//
// Returns:	boolean "TRUE" if Filter Graph has started running successfully otherwise
//			boolean "FALSE".
// 
// Notes:
//------------------------------------------------------------------
BOOL CDShowCam::StopGrp()
{
	HRESULT hr = -1;

	LOG(INFO) + __FUNCTION__ + "CDShowCam::StopGrp-1 waiting for critical section";
	
	EnterCriticalSection(&m_DSCamCriticalSection);
	LOG(INFO) + __FUNCTION__ + "CDShowCam::StopGrp-2 entered for critical section";	

	if (m_pMediaControl)
	{
		//Stop the Filter Graph
		hr = m_pMediaControl->Stop();
		if (hr==S_FALSE)
		{
			LOG(INFO) + __FUNCTION__ + "CDShowCam::StopGrp-3 waiting form_pMediaControl->Stop()";
			
			
			FILTER_STATE fs;
			int timeout= 10000;
			hr = m_pMediaControl->GetState(timeout, (OAFilterState*)&fs);
			if FAILED(hr)
			{
				LOG(WARNING) + __FUNCTION__ + "CDShowCam::StopGrp-4 filters stop timeout";
			}
		}
		else
		{
			LOG(INFO) + __FUNCTION__ + "CDShowCam::StopGrp-5 m_pMediaControl->Stop() returned";
		}
	}


	LeaveCriticalSection(&m_DSCamCriticalSection);
	
	if(SUCCEEDED(hr))
	{
		LOG(INFO) + __FUNCTION__ + "CDShowCam::StopGrp-6 exited critical section";
		return TRUE;
	}
	else
	{
		LOG(INFO) + __FUNCTION__ + "CDShowCam::StopGrp-6 exited critical section";
		return FALSE;
	}
	
} //end StopGrp

BOOL CDShowCam::ReleaseGrp()
{
	if (m_pImgCtrl)
	{
		m_pImgCtrl->Release();
		m_pImgCtrl = NULL;
	}

	if (m_pCamCtrl)
	{
		m_pCamCtrl->Release();
		m_pCamCtrl = NULL;
	}

	if (m_pStrConf)
	{
		m_pStrConf->Release();
		m_pStrConf = NULL;
	}

	if (m_pVideoCtrl)
	{
		m_pVideoCtrl->Release();
		m_pVideoCtrl = NULL;
	}

	if (m_pPrvPin)
	{
		m_pPrvPin->Release();
		m_pPrvPin = NULL;
	}

	if (m_pVideoRenderer)
	{
		m_pVideoRenderer->Release();
		m_pVideoRenderer = NULL;
	}

	if (m_pStillSink)
	{
		m_pStillSink->Release();
		m_pStillSink = NULL;
	}

	if (m_pSink)
	{
		m_pSink->Release();
		m_pSink = NULL;
	}

	if (m_pMux)
	{
		m_pMux->Release();
		m_pMux = NULL;
	}

	if (m_pVideoWrapperFilter)
	{
		m_pVideoWrapperFilter->Release();
		m_pVideoWrapperFilter = NULL;
	}

	if (m_pVideoEncoder)
	{
		m_pVideoEncoder->Release();
		m_pVideoEncoder = NULL;
	}

	if (m_pMediaSeeking)
	{
		m_pMediaSeeking->Release();
		m_pMediaSeeking = NULL;
	}

	if (m_pMediaEventEx)
	{
		m_pMediaEventEx->Release();
		m_pMediaEventEx = NULL;
	}

	if (m_pMediaControl)
	{
		m_pMediaControl->Release();
		m_pMediaControl = NULL;
	}

	if (m_pViewWindow)
	{
		m_pViewWindow->Release();
		m_pViewWindow = NULL;
	}

/*************************************************************/

	if (m_pVideoCaptureFilter)
	{
		m_pVideoCaptureFilter->Release();
		m_pVideoCaptureFilter = NULL;
	}

/*************************************************************/

	if (m_pGraphBuilder)
	{
		IEnumFilters* pFilters;
		if (FAILED(m_pGraphBuilder->EnumFilters(&pFilters))) 
		{
			LOG(ERROR) + "CDShowCam::ReleaseGrp-1 EnumFilters failed";
		}
			
		IBaseFilter *pFilter[10];
		ULONG n;
		LONG count=0;
		pFilters->Reset();
		pFilters->Next(10, &pFilter[0], &n) ;

		count = n;
		while(count >= 1)
		{
			if (FAILED(m_pGraphBuilder->RemoveFilter(pFilter[count-1])))
			{
				LOG(ERROR) + "CDShowCam::ReleaseGrp-2 RemoveFilter failed!";
			}
			HRESULT newRC = pFilter[count-1]->Release();
			while(newRC)
				newRC = pFilter[count-1]->Release();

			count--;
		}
		
		pFilters->Release();

		m_pGraphBuilder->Release();
		m_pGraphBuilder = NULL;
	}

/*************************************************************/

	if (m_pCaptureGraphBuilder)
	{
		m_pCaptureGraphBuilder->Release();
		m_pCaptureGraphBuilder = NULL;
	}

/*************************************************************/

	if (m_pAudioCaptureFilter)
	{
		m_pAudioCaptureFilter->Release();
		m_pAudioCaptureFilter = NULL;
	}
	return TRUE;
}

//------------------------------------------------------------------
//
// Prototype:	HRESULT Get_PropRng(PropType ePType, HANDLE hPropTbl)
//
// Description:	This is used to retrieve limit values of either Image, Camera or Custom property.
//
// Parameters:	ePType indicates the enumurated property type to query, 
//			    bPropTbl is a pointer to the corresponding property structure.
//
// Returns:	S_OK if Property values are retrieved successfully.
// 
// Notes:
//------------------------------------------------------------------

HRESULT CDShowCam::Get_PropRng(PropType ePType, HANDLE hPropTbl)
{
	HRESULT hr;
	m_pImgCtrl = NULL;
	m_pCamCtrl = NULL;		

	
	if(ePType==IMG)
	{
		ImgPropTbl* tbl = (ImgPropTbl* )hPropTbl;

		hr = m_pVideoCaptureFilter->QueryInterface(IID_IAMVideoProcAmp, (void **)&m_pImgCtrl);
		if(SUCCEEDED(hr))
		{
			hr = m_pImgCtrl->GetRange(tbl->vProp, &tbl->plMin, &tbl->plMax, &tbl->plDelta, &tbl->plDef, &tbl->plFlag);
			if(SUCCEEDED(hr))
			{
				hr = m_pImgCtrl->Get(tbl->vProp, &tbl->plVal, &tbl->plFlag);
				tbl->hr = hr;
				if(SUCCEEDED(hr))
				{
					LOG(INFO) + "----Property Value Get OK-----";
				}
				else
				{
					LOG(ERROR) + "----Property Value Get FAIL-----";
				}
			}
			
		}
		//free resources
		m_pImgCtrl->Release();
	}
	else if(ePType==CAM)
	{
		CamPropTbl* tbl = (CamPropTbl* )hPropTbl;

		hr = m_pVideoCaptureFilter->QueryInterface(IID_IAMCameraControl, (void **)&m_pCamCtrl);
		if(SUCCEEDED(hr))
		{
			hr = m_pCamCtrl->GetRange(tbl->p, &tbl->plMin, &tbl->plMax, &tbl->plDelta, &tbl->plDef, &tbl->plFlag);
			if(SUCCEEDED(hr))
			{
				hr = m_pCamCtrl->Get(tbl->p, &tbl->plVal, &tbl->plFlag);
				tbl->hr = hr;
			}
		}
		//free resources
		m_pCamCtrl->Release();
	}
	else
	{
		CusPropTbl* tbl = (CusPropTbl* )hPropTbl;
		
		hr = Cus_GetRange(tbl->p, &tbl->plMin, &tbl->plMax, &tbl->plDelta, &tbl->plDef);
		if(SUCCEEDED(hr))
		{
			hr = Cus_Get(tbl->p, &tbl->plVal);
			tbl->hr = hr;
		}
	}
	
	return hr;
} //end Get_PropRng

//------------------------------------------------------------------
//
// Prototype:	HRESULT Get_PropVal(PropType ePType, HANDLE hPropTbl)
//
// Description:	This is used to retrieve the value of either Image, Camera or Custom property.
//
// Parameters:	ePType indicates the enumurated property type to query, 
//			    bPropTbl is a pointer to the corresponding property structure.
//
// Returns:	S_OK if Property value is retrieved successfully.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::Get_PropVal(PropType ePType, HANDLE hPropTbl)
{
	HRESULT hr;
	m_pImgCtrl = NULL;
	m_pCamCtrl = NULL;
		
	
	if(ePType==IMG)
	{
		ImgPropTbl* tbl = (ImgPropTbl* )hPropTbl;

		hr = m_pVideoCaptureFilter->QueryInterface(IID_IAMVideoProcAmp, (void **)&m_pImgCtrl);
		if(SUCCEEDED(hr))
		{
			hr = m_pImgCtrl->Get(tbl->vProp, &tbl->plVal, &tbl->plFlag);	
			tbl->hr=hr;
		}
		//free resources
		m_pImgCtrl->Release();
	}
	else if(ePType==CAM)
	{
		CamPropTbl* tbl = (CamPropTbl* )hPropTbl;
		
		hr = m_pVideoCaptureFilter->QueryInterface(IID_IAMCameraControl, (void **)&m_pCamCtrl);
		if(SUCCEEDED(hr))
		{
			hr = m_pCamCtrl->Get(tbl->p, &tbl->plVal, &tbl->plFlag);	
			tbl->hr=hr;
		}
		//free resources
		m_pCamCtrl->Release();
	}
	else
	{
		CusPropTbl* tbl = (CusPropTbl* )hPropTbl;
		
		hr = Cus_Get(tbl->p, &tbl->plVal);
		tbl->hr=hr;
	}

	return hr;
} //end Get_PropVal

//------------------------------------------------------------------
//
// Prototype:	HRESULT Set_PropVal(PropType ePType, HANDLE hPropTbl)
//
// Description:	This is used to set the value of either Image, Camera or Custom property.
//
// Parameters:	ePType indicates the enumurated property type to query, 
//			    bPropTbl is a pointer to the corresponding property structure.
//
// Returns:	S_OK if Property value is successfully set.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::Set_PropVal(PropType ePType, HANDLE hPropTbl)
{
	HRESULT hr;

	m_pImgCtrl = NULL;
	m_pCamCtrl = NULL;
		
	
	if(ePType==IMG)
	{
		ImgPropTbl* tbl = (ImgPropTbl* )hPropTbl;

		hr = m_pVideoCaptureFilter->QueryInterface(IID_IAMVideoProcAmp, (void **)&m_pImgCtrl);
		if(SUCCEEDED(hr))
		{
			hr = m_pImgCtrl->Set(tbl->vProp, tbl->plVal, tbl->plFlag);	
			tbl->hr=hr;
		}
		//free resources
		m_pImgCtrl->Release();
	}
	else if(ePType==CAM)
	{
		CamPropTbl* tbl = (CamPropTbl* )hPropTbl;

		hr = m_pVideoCaptureFilter->QueryInterface(IID_IAMCameraControl, (void **)&m_pCamCtrl);
		if(SUCCEEDED(hr))
		{
			hr = m_pCamCtrl->Set(tbl->p, tbl->plVal, tbl->plFlag);	
			tbl->hr=hr;
		}
		//free resources
		m_pCamCtrl->Release();
	}
	else
	{
		CusPropTbl* tbl = (CusPropTbl* )hPropTbl;
		
		hr = Cus_Set(tbl->p, tbl->plVal);
		tbl->hr=hr;
	}


	return hr;
} //end Set_PropVal

//------------------------------------------------------------------
//
// Prototype:	HRESULT CaptureStill()
//
// Description:	This is used to trigger capture of a still image.
//				Here the still image name is auto generated and it will be
//				saved in "\My Documnets\My Pictures" folder
//
// Parameters:	none
//
// Returns:	S_OK if Still image is captured successfully.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::CaptureStill()
{
	WCHAR day[DAY_LENGTH], time[TIME_LENGTH], FName[TXT_LENGTH], DName[TXT_LENGTH];
	wstring StillFN;
	SYSTEMTIME st;

	IPin* pStillPin = NULL;
    IAMVideoControl* pVideoControl = NULL;
	IFileSinkFilter* pStillFileSink = NULL;
	HRESULT hr = S_OK;
	

    GetLocalTime(&st);
	
	wsprintf(day, L"%.2d%.2d%.2d", st.wMonth, st.wDay, st.wYear%100);
	wsprintf(time, L"%.2d%.2d%.2d", st.wHour, st.wMinute, st.wSecond);
	wsprintf(FName, L"\\My Documents\\My Pictures\\%s\\%s%s", day, time, L".jpg");
	wsprintf(DName, L"\\My Documents\\My Pictures\\%s", day);

	StillFN = FName;

	if(CreateDirectory(DName, NULL))
	{
#ifdef DEBUG_MODE
		DEBUGIT(LogFile,"\n!!!! Error Create Directory !!!!\n");
#endif
	}
		
	//Set File Name
	hr = m_pStillSink->QueryInterface(IID_IFileSinkFilter, (void **)&pStillFileSink);
	if(SUCCEEDED(hr))
	{
		hr = pStillFileSink->SetFileName((LPCOLESTR)StillFN.c_str(), NULL );
#ifdef DEBUG_MODE
		if(SUCCEEDED(hr))
		{
			OutputDebugString(StillFN.c_str());
		}
#endif
	}

	//Find the still image output pin, retrieve the IAMVideoControl interface, and
#ifdef DEBUG_MODE
	dTickCount=GetTickCount();
	wsprintf(waText, L"\n **** Time Stamp[%d]: Before Calling 'FindPin'for STILL Pin on VideoCaptureFilter\n", dTickCount);
	OutputDebugString(waText);
#endif
	hr = m_pCaptureGraphBuilder->FindPin((IUnknown*)m_pVideoCaptureFilter, PINDIR_OUTPUT, &PIN_CATEGORY_STILL, &MEDIATYPE_Video, FALSE, 0, &pStillPin);
#ifdef DEBUG_MODE
	dTickCount=GetTickCount();
	wsprintf(waText, L"\n **** Time Stamp[%d]: After Calling 'FindPin'for STILL Pin on VideoCaptureFilter\n", dTickCount);
	OutputDebugString(waText);
#endif
	if(SUCCEEDED(hr))
	{
		// Once you have the still image pin, you query it for the
		// IAMVideoControl interface which exposes the SetMode function for 
		// triggering the image capture.
#ifdef DEBUG_MODE
		dTickCount=GetTickCount();
		wsprintf(waText, L"\n **** Time Stamp[%d]: Before Querying IAMVideoControl Interface on VideoCaptureFilter\n", dTickCount);
		OutputDebugString(waText);
#endif
		if(SUCCEEDED(hr = m_pVideoCaptureFilter->QueryInterface( IID_IAMVideoControl, (void **)&pVideoControl )))
		{
#ifdef DEBUG_MODE
			dTickCount=GetTickCount();
			wsprintf(waText, L"\n **** Time Stamp[%d]: After Querying IAMVideoControl Interface on VideoCaptureFilter\n", dTickCount);
			OutputDebugString(waText);
#endif
        	// Now that trigger the still image capture.
#ifdef DEBUG_MODE
			dTickCount=GetTickCount();
			wsprintf(waText, L"\n **** Time Stamp[%d]: Before Calling SetMode on IAMVideoControl Interface\n", dTickCount);
			OutputDebugString(waText);
#endif
			hr = pVideoControl->SetMode( pStillPin, VideoControlFlag_Trigger );
#ifdef DEBUG_MODE
			dTickCount=GetTickCount();
			wsprintf(waText, L"\n **** Time Stamp[%d]: After Calling SetMode on IAMVideoControl Interface\n", dTickCount);
			OutputDebugString(waText);
#endif
		}
	}
	
	//If trigger succeeded, Wait for the EC_CAP_FILE_COMPLETED event.
	if(SUCCEEDED(hr))
	{
		LONG lEventCode = 0, lParam1 = 0, lParam2 = 0;
		do{
			// If no events are received within 60 seconds, then we�ll assume 
			// we have an error. Adjust this value as you see fit.
			hr = m_pMediaEventEx->GetEvent( &lEventCode, &lParam1,&lParam2, 80*1000 );
			if(SUCCEEDED(hr))
			{
				// If we recieve an event, free the event parameters.
				// we can do this immediately because we're not referencing 
				// lparam1 or lparam2.
				if(SUCCEEDED(hr = m_pMediaEventEx->FreeEventParams( lEventCode, lParam1, lParam2 )))
				{
					// If the event was received, then we successfully 
					// captured the still image and can quit looking.
					if(lEventCode == EC_CAP_FILE_COMPLETED)
					{
#ifdef DEBUG_MODE
						dTickCount=GetTickCount();
						wsprintf(waText, L"\n **** Time Stamp[%d]: receiving EC_CAP_FILE_COMPLETED event to notify STILL image capture is completed\n", dTickCount);
						OutputDebugString(waText);
#endif
						break;
					}
						
				}
			}
		// If the still image isn't captured within 60 seconds
		// of the trigger (or possibly longer if other events come in), 
		// then we�ll assume it�s an error. Continue processing events 
		// until we detect the error.
		}while(SUCCEEDED(hr));
	}
	
	//clean up
	if(pVideoControl)
	{
		pVideoControl->Release();
		pVideoControl = NULL;
	}

	if(pStillPin)
	{
		pStillPin->Release();
		pStillPin = NULL;
	}

    return hr;
} //end CaptureStill()

//------------------------------------------------------------------
//
// Prototype:	HRESULT CaptureStill(wstring wsSFName)
//
// Description:	This is used to trigger capture of a still image.
//				Here the still image name is passed as a parameter and it will be
//				saved in "\My Documnets\My Pictures" folder
//
// Parameters:	wsSFName is the name given for the still Image
//
// Returns:	S_OK if Still image is captured successfully.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::CaptureStill(wstring wsSFName)
{
	WCHAR DName[TXT_LENGTH];
	wstring StillFN = L"\\tempimg.jpg";
	//SYSTEMTIME st;

	IPin* pStillPin = NULL;
    IAMVideoControl* pVideoControl = NULL;
	IFileSinkFilter* pStillFileSink = NULL;
	HRESULT hr = S_OK;

//GetLocalTime(&st);

	//wsprintf(day, L"%.2d%.2d%.2d", st.wMonth, st.wDay, st.wYear%100);
	//wsprintf(FName, L"\\My Documents\\My Pictures\\%s\\%s%s", day, wsSFName.c_str(), L".jpg");
	//wsprintf(DName, L"\\My Documents\\My Pictures\\%s", day);
	int DNameLen = (wcsrchr(wsSFName.c_str(), L'\\')+1) - wsSFName.c_str();
	wcsncpy(DName, wsSFName.c_str(), DNameLen);
	DName[DNameLen] = L'\0';

	if(CreateDirectory(DName, NULL))
	{
#ifdef DEBUG_MODE
		DEBUGIT(LogFile,"\n!!!! Error Create Directory !!!!\n");
#endif
	}

	EnterCriticalSection(&m_DSCamCriticalSection);

	//Set File Name
	if(SUCCEEDED(hr = m_pStillSink->QueryInterface( IID_IFileSinkFilter, (void **)&pStillFileSink )))
	{
		hr = pStillFileSink->SetFileName((LPCOLESTR)StillFN.c_str(), NULL );
		if(SUCCEEDED(hr))
		{
			//DEBUGIT(LogFile,"setfile name succeeded");
			LOG(INFO) + "CDShowCam::CaptureStill-1 setfile name succeeded";
		}
		else
		{
			//DEBUGIT(LogFile,"set file name failed \n");
			LOG(ERROR) + "CDShowCam::CaptureStill-2 set file name failed";

		}
		
	}


	//Find the still image output pin, retrieve the IAMVideoControl interface, and
	if(SUCCEEDED(hr = m_pCaptureGraphBuilder->FindPin((IUnknown*)m_pVideoCaptureFilter, PINDIR_OUTPUT, &PIN_CATEGORY_STILL, &MEDIATYPE_Video, FALSE, 0, &pStillPin )))
	{
		// Once you have the still image pin, you query it for the
		// IAMVideoControl interface which exposes the SetMode function for 
		// triggering the image capture.

		//DEBUGIT(LogFile,"FindPin succeeded");
		LOG(INFO) + "CDShowCam::CaptureStill-3 FindPin succeeded";

		if(SUCCEEDED(hr = m_pVideoCaptureFilter->QueryInterface( IID_IAMVideoControl, (void **)&pVideoControl )))
		{
	    	// Now that trigger the still image capture.
			//DEBUGIT(LogFile,"query interface succeeded");
			LOG(INFO) + "CDShowCam::CaptureStill-4 query interface succeeded";
		
			hr = pVideoControl->SetMode( pStillPin, VideoControlFlag_Trigger); 
		}
		else
		{
			//DEBUGIT(LogFile,"query interface failed");
			LOG(ERROR) + "CDShowCam::CaptureStill-5 query interface failed";
		}
	}
	else
	{
		//DEBUGIT(LogFile,"FindPin failed");
		LOG(ERROR) + "CDShowCam::CaptureStill-6 FindPin failed";
	}

	if(SUCCEEDED(hr))
	{
		//DEBUGIT(LogFile,"SetMOde succeeded");
		LOG(INFO) + "CDShowCam::CaptureStill-7 SetMOde succeeded";
		
		LONG lEventCode = 0, lParam1 = 0, lParam2 = 0;
		do{
			// If no events are received within 5 seconds, then we�ll assume 
			// we have an error. Adjust this value as you see fit.
			hr = m_pMediaEventEx->GetEvent( &lEventCode, &lParam1,&lParam2, 5*1000 );

			//DEBUGIT(LogFile,"m_pMediaEventEx->GetEvent %ld \n", lEventCode);
			LOG(INFO) + "CDShowCam::CaptureStill-8 m_pMediaEventEx->GetEvent";

			if(SUCCEEDED(hr))
			{
				// If we recieve an event, free the event parameters.
				// we can do this immediately because we're not referencing 
				// lparam1 or lparam2.
				if(SUCCEEDED(hr = m_pMediaEventEx->FreeEventParams( lEventCode, lParam1, lParam2 )))
				{
					// If the event was received, then we successfully 
					// captured the still image and can quit looking.
					if(lEventCode == EC_CAP_FILE_COMPLETED)
					{
						//DEBUGIT(LogFile,"  EC_CAP_FILE_COMPLETED \n");
						LOG(INFO) + "CDShowCam::CaptureStill-9 EC_CAP_FILE_COMPLETED";
						DeleteFile(wsSFName.c_str());
						MoveFile(StillFN.c_str(), wsSFName.c_str());
						break;
					}
				}
				
			}
			else
			{
					//DEBUGIT(LogFile," timeout waiting for camera event \n");
					LOG(WARNING) + "CDShowCam::CaptureStill-10 timeout waiting for camera event";
					
			}
		// If the still image isn't captured within 60 seconds
		// of the trigger (or possibly longer if other events come in), 
		// then we�ll assume it�s an error. Continue processing events 
		// until we detect the error.
		}while(SUCCEEDED(hr));
	}
	else
	{
		//DEBUGIT(LogFile,"SetMode failed");
		LOG(ERROR) + "CDShowCam::CaptureStill-11 SetMode failed";
	}
	//clean up
	if(pVideoControl)
	{
		pVideoControl->Release();
		pVideoControl = NULL;
	}
	if(pStillPin)
	{
		pStillPin->Release();
		pStillPin = NULL;
	}

	LeaveCriticalSection(&m_DSCamCriticalSection);

    return hr;
} //end CaptureStill()

//------------------------------------------------------------------
//
// Prototype:	HRESULT Set_VdoFileName(wstring wsVFName)
//
// Description:	This is used to set a file name for the current video stream capturing.
//
// Parameters:	wsVFName Unicode string
//
// Returns:	S_OK if Video file name is set successfully.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::Set_VdoFileName(wstring wsVFName)
{
	LOG(INFO) + __FUNCTION__ + "wsVFName: " + wsVFName;

	m_wsVFName = wsVFName;

	LOG(INFO) + __FUNCTION__ + "m_wsVFName: " + m_wsVFName;
	HRESULT hr = m_pSink->SetFileName((LPCOLESTR)wsVFName.c_str(), 0);
		
#ifdef DEBUG_MODE
	if(FAILED(hr))
	{
		DEBUGIT(LogFile,"!!! Error Video File Name Set !!!\n");
		LOG(ERROR) + "!!! Error Video File Name Set !!!";
	}
#endif
	
	return hr;
} //end Set_VdoFileName

//------------------------------------------------------------------
//
// Prototype:	HRESULT StartCapture()
//
// Description:	This is used to trigger video stream capturing.
//
// Parameters:	none
//
// Returns:	S_OK if Stream capturing started successfully.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::StartCapture()
{
	HRESULT hr;
	REFERENCE_TIME rtStart = START_TIME, rtStop = STOP_TIME;
	WORD StartCookie = START_COOKIE, StopCookie = STOP_COOKIE;
	TCHAR waTemp[TXT_LENGTH];
	TCHAR waTempFile[TXT_LENGTH];

	OutputDebugString(m_wsVFName.c_str());
	wcscpy(waTempFile, m_wsVFName.c_str());
	if(DeleteFile(m_wsVFName.c_str()))
	{
		LOG(INFO) + "CDShowCam::StartCapture-1 File is Deleted !!!";
	}
	else
	{	
		DWORD dLError=GetLastError();
		if(dLError!=ERROR_FILE_NOT_FOUND)
		{
			wsprintf(waTemp, L"%s%d%s", EXT_VDO_FN, m_nFileAutoCnt, L".wmv");
			m_nFileAutoCnt++;
			m_wsVFName=waTemp;
			LOG(INFO) + __FUNCTION__ + "CDShowCam::StartCapture-2 Extra File Name:-" + m_wsVFName.c_str();
		}
	}
	if(m_pSink==NULL)
	{
		LOG(ERROR) + __FUNCTION__ + "CDShowCam::StartCapture-3 NULL Pointer !!";
	}
	hr = m_pSink->SetFileName((LPCOLESTR)m_wsVFName.c_str(), 0);
	m_wsVFName=waTempFile;			//To restore the original name
	if(FAILED(hr))
	{
		LOG(ERROR) + __FUNCTION__ + "CDShowCam::StartCapture-4 Error Setting File Name !!";
	}

	hr = m_pCaptureGraphBuilder->ControlStream(&PIN_CATEGORY_CAPTURE,
										  &MEDIATYPE_Video, m_pVideoCaptureFilter,
										  &rtStart, &rtStop, StartCookie, StopCookie);

	LOG(INFO) + __FUNCTION__ + "1: hr = m_pCaptureGraphBuilder = " + hr + "\n";
			
	hr = m_pCaptureGraphBuilder->ControlStream(&PIN_CATEGORY_CAPTURE,
										  &MEDIATYPE_Audio, m_pAudioCaptureFilter,
										  &rtStart, &rtStop, StartCookie, StopCookie);

	LOG(INFO) + __FUNCTION__ + "2: hr = m_pCaptureGraphBuilder = " + hr + "\n";

	if(SUCCEEDED(hr))
	{
	    LONG lEventCode = 0, lParam1 = 0, lParam2 = 0;

		// wait for the start events for the capture
    	do
		{
			// if no events are received within 30 seconds, then we have an error.
			// since we're starting a capture, there should be a minimum delay from the
			// start to the actual start.
			hr = m_pMediaEventEx->GetEvent( &lEventCode, &lParam1, &lParam2, 30*1000 );
			if(SUCCEEDED(hr))
			{
#ifdef DEBUG_MODE			
					if(lEventCode !=0)
					{
						TCHAR waTemp[TXT_LENGTH];
						// SPS
						wsprintf(waTemp, L"\nStart Event Code :- %ld ; Param2 :- %ld", lEventCode, lParam2);
						//OutputDebugString(waTemp);
						LOG(ERRROR) + waTemp;
						// SPS

					}
#endif
				
				LOG(INFO) + __FUNCTION__ + "GetEvent: lEventCode = " + lEventCode + " lParam1 = " + lParam1 + " lParam2 = " + lParam2;	
				// if the event code we recieved was EC_STREAM_CONTROL_STARTED, then
				// we successfully started the video capture
				if(lEventCode==EC_STREAM_CONTROL_STARTED && lParam2==StartCookie)
				{
					//DEBUGIT(LogFile,"\n---> Stream Capture Started --->");
					LOG(INFO) + "CDShowCam::StartCapture-5 ---> Stream Capture Started --->";
					break;
				}
		        // if we recieve an event, free the event parameters.
				// we can do this because we're not referencing lparam1 or lparam2.
				if(SUCCEEDED(hr = m_pMediaEventEx->FreeEventParams( lEventCode, lParam1, lParam2 )))
				{
#ifdef DEBUG_MODE
					//DEBUGIT(LogFile,"\n<--- Free Event Params --->");
					LOG(INFO) + "<--- Free Event Params --->";
#endif
					LOG(INFO) + __FUNCTION__ + "FreeEventParams: lEventCode = " + lEventCode + " lParam1 = " + lParam1 + " lParam2 = " + lParam2;
				}
			}
		// if we don't recieve the stream control started event within a reasonable amount of time,
		// then we have an error.
		}while(SUCCEEDED(hr));
			
	}

	return hr;
} //end StartCapture

//------------------------------------------------------------------
//
// Prototype:	HRESULT StopCapture()
//
// Description:	This is used to stop video stream capturing.
//
// Parameters:	none
//
// Returns:	S_OK if Stream capturing stopped successfully.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::StopCapture()
{
	HRESULT hr = S_OK;
	REFERENCE_TIME rtStart = START_TIME;
	REFERENCE_TIME rtStop;

	//REFERENCE_TIME rtStart = MAXLONGLONG;
	WORD VideoStartCookie = VSTART_COOKIE, VideoStopCookie = VSTOP_COOKIE;
	WORD AudioStartCookie = ASTART_COOKIE, AudioStopCookie = ASTOP_COOKIE;
	BOOL fPreviewPaused = FALSE;

	// Stop the video portion of the capture passing in the video
	// stop cookie we'll wait for.
	
	m_pMediaSeeking->GetCurrentPosition(&rtStop);
	hr = m_pCaptureGraphBuilder->ControlStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_pVideoCaptureFilter, &rtStart, &rtStop, VideoStartCookie, VideoStopCookie);
	if(SUCCEEDED(hr))
	{
#ifdef DEBUG_MODE
		DEBUGIT(LogFile,"\n---> Video Stream is stopped <---\n");
#endif
		// Stop the audio portion of the capture passing in the audio
		// stop cookie we'll wait for.
		if(m_tCamcfg.bIsAudioEnb)
		{
			hr = m_pCaptureGraphBuilder->ControlStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio, m_pAudioCaptureFilter, &rtStart, &rtStop, AudioStartCookie, AudioStopCookie);
			if(SUCCEEDED(hr))
			{
#ifdef DEBUG_MODE
				//DEBUGIT(LogFile,"\n---> Audio Stream is stopped <---\n");
				LOG(INFO) + "---> Audio Stream is stopped <---";
#endif
			}
			else
			{
#ifdef DEBUG_MODE

				//DEBUGIT(LogFile,"\n---> Audio Stream Failed to Stop --->\n");
				LOG(INFO) + "---> Audio Stream Failed to Stop --->";

#endif
			}

		}
	}
	else
	{
#ifdef DEBUG_MODE
		//DEBUGIT(LogFile,"\n---> Video Stream Falied to Stop ---->\n");
		LOG(ERROR) + "---> Video Stream Falied to Stop ---->";
#endif
	}
	
	// For performance purposes now that the video capture has been stopped, freeze the preview
	// window to free up CPU cycles for software video encoding.
	if(SUCCEEDED(hr))
	{
		if(SUCCEEDED(hr = m_pCaptureGraphBuilder->ControlStream( &PIN_CATEGORY_PREVIEW, 
													&MEDIATYPE_Video, m_pVideoCaptureFilter, NULL, 0, 0, 0 )))
		{
			fPreviewPaused = TRUE;
		}
		
	}
	// Wait for the ControlStream event. 
	OutputDebugString( L"\nWating for the control stream events\n" );	
	LONG lEventCode = 0, lParam1 = 0, lParam2 = 0;
	INT nNoOfEve=1;
	INT count=0;

	if(m_tCamcfg.bIsAudioEnb)
		nNoOfEve=2;

	do
	{
		hr=m_pMediaEventEx->GetEvent(&lEventCode, &lParam1, &lParam2, 20*1000);
		if(SUCCEEDED(hr))
		{
#ifdef DEBUG_MODE
			if(lEventCode!=0)
			{
				TCHAR waTemp[TXT_LENGTH];
				wsprintf(waTemp, L"\nStart Event Code :- %ld ; Param2 :- %ld", lEventCode, lParam2);
				OutputDebugString(waTemp);
			}
#endif
			if(lEventCode==EC_STREAM_CONTROL_STOPPED)
			{
				OutputDebugString( L"Received a control stream stop event\n" );
				count++;
			}
			m_pMediaEventEx->FreeEventParams(lEventCode, lParam1, lParam2);	
		}
	}while(count<nNoOfEve&&SUCCEEDED(hr));

	OutputDebugString( L"The file has been captured\n" );
	
	if(fPreviewPaused)
	{
		// an end time of MAX_TIME indicates for the stream to never stop 
		LONGLONG llEnd = MAX_TIME;
		m_pCaptureGraphBuilder->ControlStream( &PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pVideoCaptureFilter, NULL, &llEnd, 0, 0 );
	}	
	
	//Reset the Graph
	//StopGrp();
	//Sleep(1000);
	//RunGrp();
	
	return hr;
} //end StopCapture

//------------------------------------------------------------------
//
// Prototype:	HRESULT Set_Flip(BOOL bIsHori)
//
// Description:	This is used to set the flip, Horizontally or Vertically
//				depending on the value bIsHori.
//
// Parameters:	if bIsHori is "TRUE" then Horizontal Flip otherwise Vertical Flip
//
// Returns:	S_OK if Flip is set successfully.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::Set_Flip(BOOL bIsHori)
{
	HRESULT	hr;

	if(bIsHori)
	{
		hr = m_pVideoCtrl->SetMode(m_pPrvPin, VideoControlFlag_FlipHorizontal);
	}
	else
	{
		hr = m_pVideoCtrl->SetMode(m_pPrvPin, VideoControlFlag_FlipVertical);
	}
		
#ifdef DEBUG_MODE
	if(SUCCEEDED(hr))
	{
		DEBUGIT(LogFile,"\n------------Flip OK------------");
	}
	else
	{
		DEBUGIT(LogFile,"\n------------Flip FAIL------------");
	}
#endif

	return hr;
} //end Set_Flip (BOOL bIsHori)

//------------------------------------------------------------------
//
// Prototype:	HRESULT EnumFirstCap(PinType ePType, ImgFmt* ptIFmt, INT* pnNoOfCap)
//
// Description:	This is used to enumerate Video,Still and Preview resolution capabilities.
//
// Parameters:	
//
// Returns:	S_OK if all video resolutions are enumurated.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::EnumFirstCap(PinType ePType, ImgFmt* ptIFmt, INT* pnNoOfCap)
{	
	HRESULT hr;
	GUID pType;
	INT nSize = 0;
	m_pStrConf = NULL;
	INT nCounter=0;	
	INT nFlag;
	INT nIdx=0;

	switch(ePType)
	{
		case V:
			pType = PIN_CATEGORY_CAPTURE;
			nFlag = 0;
			break;
		case S:
			pType = PIN_CATEGORY_STILL;
			nFlag = 1;
			break;
		case P:
			pType = PIN_CATEGORY_PREVIEW;
			nFlag = 2;
			break;
	}

	//PIN type, Any media type, Pointer to the Capture filter.
	hr = m_pCaptureGraphBuilder->FindInterface(&pType, 0, m_pVideoCaptureFilter, \
										IID_IAMStreamConfig, (void**)&m_pStrConf);
	//hr = m_pCaptureGraphBuilder->FindInterface(&pType, 0, m_pVideoEncoder,
	//									IID_IAMStreamConfig, (void**)&m_pStrConf);
	if(SUCCEEDED(hr))
	{
		hr = m_pStrConf->GetNumberOfCapabilities(pnNoOfCap, &nSize);

		/*
		if(m_ptCamCap[nFlag]!=NULL)
		{
			delete [] m_ptCamCap[nFlag];
			DEBUGIT(LogFile,"\n---------- Old Object deleted!!!----------");
		}
	
		//Creating a CamCapability Structure to store enumurated camera capabilities for each PIN type
		m_ptCamCap[nFlag] = new CamCapability[nSize];
		*/
		if(SUCCEEDED(hr))
		{
			// Check the size to make sure we pass in the correct structure.
			if (nSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
			{
				// Use the video capabilities structure.
				VIDEO_STREAM_CONFIG_CAPS scc;
				AM_MEDIA_TYPE *pmtConfig;
				hr = m_pStrConf->GetStreamCaps(nIdx, &pmtConfig, (BYTE*)&scc);
				if (SUCCEEDED(hr))
				{
					ptIFmt->nMaxWidth = scc.MaxOutputSize.cx;
					ptIFmt->nMaxHeight = scc.MaxOutputSize.cy;
					ptIFmt->qwMaxFrameRate = scc.MaxFrameInterval;
																
					if(pmtConfig->majortype == MEDIATYPE_Video)
					{
						while((media_list[nCounter].pMSubType != NULL) &&
							(pmtConfig->subtype != *media_list[nCounter].pMSubType))
						{
									nCounter++;
						}
						ptIFmt->wsColorFmt = media_list[nCounter].wsClrName;

					}

				}
				// Delete the media type when you are done.
				DeleteMediaType(pmtConfig);
			
			}

		}

	}
	
	if(m_pStrConf != NULL)
		m_pStrConf->Release();
	
	return hr;

} //end EnumFirstCap(PinType ePType, ImgFmt* ptIFmt, INT* pnNoOfCap)

//------------------------------------------------------------------
//
// Prototype:	HRESULT EnumOtherCap(PinType ePType, ImgFmt* ptIFmt, INT nNoOfCap)
//
// Description:	This is used to enumerate Video,Still and Preview resolution capabilities.
//
// Parameters:	
//
// Returns:	S_OK if all video resolutions are enumurated.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::EnumOtherCap(PinType ePType, ImgFmt* ptIFmt, INT nNoOfCap)
{	
	HRESULT hr;
	GUID pType;
	m_pStrConf = NULL;
	INT nCounter=0;	
	INT nFlag;

	switch(ePType)
	{
		case V:
			pType = PIN_CATEGORY_CAPTURE;
			nFlag = 0;
			break;
		case S:
			pType = PIN_CATEGORY_STILL;
			nFlag = 1;
			break;
		case P:
			pType = PIN_CATEGORY_PREVIEW;
			nFlag = 2;
			break;

	}

	//PIN type, Any media type, Pointer to the capture filter.
	hr = m_pCaptureGraphBuilder->FindInterface(&pType, 0, m_pVideoCaptureFilter, \
										IID_IAMStreamConfig, (void**)&m_pStrConf);
	//hr = m_pCaptureGraphBuilder->FindInterface(&pType, 0, m_pVideoEncoder, \
	//									IID_IAMStreamConfig, (void**)&m_pStrConf);
	if(SUCCEEDED(hr))
	{
		
		// Use the video capabilities structure.
		for (int iFormat = 1; iFormat < nNoOfCap; iFormat++)
		{
			VIDEO_STREAM_CONFIG_CAPS scc;
			AM_MEDIA_TYPE *pmtConfig;
			hr = m_pStrConf->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);
			if (SUCCEEDED(hr))
			{
				ptIFmt->nMaxWidth = scc.MaxOutputSize.cx;
				ptIFmt->nMaxHeight = scc.MaxOutputSize.cy;
				ptIFmt->qwMaxFrameRate = scc.MaxFrameInterval;
						
				if(pmtConfig->majortype == MEDIATYPE_Video)
				{
					while((media_list[nCounter].pMSubType != NULL) &&
						(pmtConfig->subtype != *media_list[nCounter].pMSubType))						
					{
						nCounter++;
					}
					ptIFmt->wsColorFmt = media_list[nCounter].wsClrName;
					nCounter=0;
					
					/*
					//Update this structre for Class internal use
					m_ptCamCap[nFlag]++;
					m_ptCamCap[nFlag]->nIndex = iFormat;
					m_ptCamCap[nFlag]->tImgRes.nWidth = scc.MaxOutputSize.cx;
					m_ptCamCap[nFlag]->tImgRes.nHeight = scc.MaxOutputSize.cy;
					m_ptCamCap[nFlag]->wsColorFmt = media_list[nCounter].wsClrName;
					*/
				}
				ptIFmt++;
			}
			// Delete the media type when you are done.
			DeleteMediaType(pmtConfig);
		}
		/*
		//Insert last record to Identify the end Points
		m_ptCamCap[nFlag]++;
		m_ptCamCap[nFlag]->nIndex = 999;
		m_ptCamCap[nFlag]->tImgRes.nWidth = 999;
		m_ptCamCap[nFlag]->tImgRes.nHeight = 999;
		m_ptCamCap[nFlag]->wsColorFmt = L"";
		m_ptCamCap[nFlag]->qwMaxFrameRate = 999999;
		*/

	}

	if(m_pStrConf != NULL)
		m_pStrConf->Release();
	

	return hr;

} //end EnumOtherCap(PinType ePType, ImgFmt* ptIFmt, INT pnNoOfCap)

//------------------------------------------------------------------
//
// Prototype:	HRESULT Set_Resolution(ImageRes *ptRes, PinType ePType)
//
// Description:	This is used to set the resolution of either Preview, 
//				Still or Capture Pins.
//
// Parameters:	ptRes is a pointer to ImageRes struture, which holds the
//				resolution to set and ePType is Pin category enumuration;
//				V:-Capture Pin, S:-Still Pin, P:-Preview Pin
//				iDesiredWidth / iDesiredHeight: The desired size of the
//				output image.
//
// Returns:	S_OK if Image resolution is set successfully.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::Set_Resolution(ImageRes* ptRes, PinType ePType)
{	
	HRESULT hr;
	GUID pType;
	m_pStrConf = NULL;

	//capture device can support a range of output formats. For example, 
	//a device might support 16-bit RGB, 32-bit RGB, and YUYV. Within each of these formats, 
	//the device can support a range of frame sizes.
	//The IAMStreamConfig interface is used to report which formats the device supports, 
	//and to set the format. The IAMStreamConfig interface is exposed on the capture filter's capture pin,
	//preview pin, or both. Use the ICaptureGraphBuilder2::FindInterface method to get the interface
	//pointer:
		
	switch(ePType)
	{
		case V:
			pType = PIN_CATEGORY_CAPTURE;
			//DEBUGIT(LogFile,"\n----------CAPTURE PIN-----------\n");
			LOG(INFO) + "----------CAPTURE PIN-----------";
			break;
		case S:
			pType = PIN_CATEGORY_STILL;
			//DEBUGIT(LogFile,"\n----------STILL PIN-----------\n");
			LOG(INFO) + "----------STILL PIN-----------";
			break;
		case P:
			pType = PIN_CATEGORY_PREVIEW;
			//DEBUGIT(LogFile,"\n----------PREVIEW PIN-----------\n");
			LOG(INFO) + "----------PREVIEW PIN-----------";
			break;

	}
	
	//PIN type, Any media type, Pointer to the capture filter.
	hr = m_pCaptureGraphBuilder->FindInterface(&pType, 0, m_pVideoCaptureFilter,\
													IID_IAMStreamConfig, (void**)&m_pStrConf);
	//hr = m_pCaptureGraphBuilder->FindInterface(&pType, 0, m_pVideoEncoder,\
	//												IID_IAMStreamConfig, (void**)&m_pStrConf);
	
	if(SUCCEEDED(hr))
	{
		VIDEO_STREAM_CONFIG_CAPS scc;
		AM_MEDIA_TYPE *pmtConfig;
		INT iResIdx=0;

	    INT nCount = 0, nSize = 0;

		hr = m_pStrConf->GetNumberOfCapabilities(&nCount, &nSize);
	    if(FAILED(hr))
		{
            return hr;
		}
		
		bool bUserResolutionFound = false;
		for(int i=iResIdx; i<nCount;i++)
		{
			hr = m_pStrConf->GetStreamCaps(i, &pmtConfig, (BYTE*)&scc);
	
			if (SUCCEEDED(hr))
			{
				if((scc.MaxOutputSize.cx >= ptRes->nWidth) && 
					(scc.MaxOutputSize.cy >= ptRes->nHeight))
				{
					bUserResolutionFound = true;
					StopGrp();
					hr = m_pStrConf->SetFormat(pmtConfig);
					if((FAILED(hr)))
						return FALSE;
					//if(ePType==V)
					//	CDShowCam::ReBuildGrp();
					RunGrp();
					WCHAR szLog[512];
					// SPS
					wsprintf(szLog, L"Setting image output resolution to %i (width) x %i (height)", 
						scc.MaxOutputSize.cx, scc.MaxOutputSize.cy);
					LOG(INFO) + szLog;
					//m_pModule->Log(PB_LOG_INFO, szLog, _T(__FUNCTION__), __LINE__);
					// SPS
					break;
				}
			}
			// Delete the media type when you are done.
			DeleteMediaType(pmtConfig);
		}

		if (!bUserResolutionFound && nCount >= 1)
		{
			//  User specified resolution was not found, default to
			//  the last available resolution (always highest in my tests)
			hr = m_pStrConf->GetStreamCaps(nCount - 1, &pmtConfig, (BYTE*)&scc);
			if (SUCCEEDED(hr))
			{
				StopGrp();
				hr = m_pStrConf->SetFormat(pmtConfig);
				if((FAILED(hr)))
					return FALSE;
				RunGrp();
				// SPS
				WCHAR szLog[512];
				wsprintf(szLog, L"User specified image resolution is too high.  Setting image output resolution to %i (width) x %i (height)", 
					scc.MaxOutputSize.cx, scc.MaxOutputSize.cy);
				//m_pModule->Log(PB_LOG_WARNING, szLog, _T(__FUNCTION__), __LINE__);
				LOG(WARNING) + szLog;
				// SPS
			}
		}
	}
	
/*

	// To configure the device to use a format returned in the 
	// IAMStreamConfig::GetStreamCaps method, 
	// call IAMStreamConfig::SetFormat with the media type:
	AM_MEDIA_TYPE *pmtConfig;
	hr = pConfig->SetFormat(pmtConfig);
	CHK(hr);
	// If the pin is not connected, it will attempt to use this 
	// format when it does connect. If the pin is already connected, 
	// it attempts to reconnect using the new format. In either case, it is possible that the downstream filter will reject the format. 
	
	// For example, suppose that IAMStreamConfig::GetStreamCaps returns a 24-bit RGB
	// format, with a frame size of 320 x 240 pixels. 
	// You can get this information by examining the major type, subtype, 
	// and format block of the media type:

	if ((pmtConfig.majortype == MEDIATYPE_Video) &&
		(pmtConfig.subtype == MEDIASUBTYPE_RGB24) &&
		(pmtConfig.formattype == FORMAT_VideoInfo) &&
		(pmtConfig.cbFormat >= sizeof (VIDEOINFOHEADER)) &&
		(pmtConfig.pbFormat != NULL))
	{
		VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*)pmtConfig.pbFormat;
		// pVih contains the detailed format information.
		LONG lWidth = pVih->bmiHeader.biWidth;
		LONG lHeight = pVih->bmiHeader.biHeight;
	}
	// The VIDEO_STREAM_CONFIG_CAPS structure gives the minimum and maximum width 
	// and height that can be used for this media type. It also gives 
	// you the "step" size, which defines the increments by which you can adjust the 
	// width or height. For example, the device might return the following: 
	// MinOutputSize: 160 x 120 
	// MaxOutputSize: 320 x 240 <-- from microsoft doc
	// OutputGranularityX: 8 pixels (horizontal step size) 
	//OutputGranularityY: 8 pixels (vertical step size) 
	// Given these numbers, you could set the width to anything in
	// the range (160, 168, 176, ... 304, 312, 320) and the height to anything
	// in the range (120, 128, 136, ... 104, 112, 120).



	// To set a new frame size, directly modify the AM_MEDIA_TYPE structure returned in
	// IAMStreamConfig::GetStreamCaps:

	pVih->bmiHeader.biWidth = 160;
	pVih->bmiHeader.biHeight = 120;
	pVih->bmiHeader.biSizeImage = DIBSIZE(pVih->bmiHeader);

	// Then pass the media type to the IAMStreamConfig::SetFormat method, 
	// as described previously. 


Cleanup:
#ifdef DEBUG_MODE
	LoadString(hInst, STR_CLEANUP, text, MAX_LOADSTRING);
	OutputDebugString(text);
#endif

*/
	return SUCCEEDED(hr);
} //end Set_Resolution(ImageRes *ptRes, PinType ePType)

//------------------------------------------------------------------
//
// Prototype:	HRESULT Set_ColorFmt(ImageRes* ptRes, wstring* pwsClrFmt, PinType ePType)
//
// Description:	This is used to set the color format of either Preview, 
//				Still or Capture Pin.
//
// Parameters:	pwsClrFmt is a unicode string pointer, holding the color
//				format to be set and ePType is Pin category enumuration;
//				V:-Capture Pin, S:-Still Pin, P:-Preview Pin
//
// Returns:	S_OK if Color format is set successfully.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::Set_ColorFmt(ImageRes* ptRes, wstring* pwsClrFmt, PinType ePType)
{	
	HRESULT hr;
	GUID pType;
	m_pStrConf = NULL;
	INT nIdx=0;
	INT nCount = 0, nSize = 0;
	INT nCounter = 0;

	//DEBUGIT(LogFile,"\n---->");
	LOG(INFO) + "---->";
	OutputDebugString(pwsClrFmt->c_str());

	switch(ePType)
	{
		case V:
			pType = PIN_CATEGORY_CAPTURE;
			break;
		case S:
			pType = PIN_CATEGORY_STILL;
			break;
		case P:
			pType = PIN_CATEGORY_PREVIEW;
			break;

	}
	
	//PIN type, Any media type, Pointer to the capture filter.
	//hr = m_pCaptureGraphBuilder->FindInterface(&pType, 0, m_pVideoCaptureFilter,\
	//												IID_IAMStreamConfig, (void**)&m_pStrConf);
	hr = m_pCaptureGraphBuilder->FindInterface(&pType, 0, m_pVideoEncoder,\
													IID_IAMStreamConfig, (void**)&m_pStrConf);
	
	if(SUCCEEDED(hr))
	{
		VIDEO_STREAM_CONFIG_CAPS scc;
		AM_MEDIA_TYPE *pmtConfig;
		
		hr = m_pStrConf->GetNumberOfCapabilities(&nCount, &nSize);
	    if(FAILED(hr))
		{
			return hr;
		}

		if (nSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
		{
			while((*pwsClrFmt != media_list[nCounter].wsClrName) &&\
									(media_list[nCounter].wsClrName != L"INVALIDE"))
			{
				nCounter++;
			}
			
			if(media_list[nCounter].pMSubType !=NULL)
			{
				for(INT i=nIdx; i<nCount;i++)
				{
					hr = m_pStrConf->GetStreamCaps(i, &pmtConfig, (BYTE*)&scc);
					if (SUCCEEDED(hr))
					{
						if(pmtConfig->majortype == MEDIATYPE_Video)
						{						
							if(pmtConfig->subtype == *media_list[nCounter].pMSubType)
							{
								if((scc.MaxOutputSize.cx==ptRes->nWidth) && (scc.MaxOutputSize.cy==ptRes->nHeight))
								{
									//m_pMediaControl->Stop();
									StopGrp();
									hr = m_pStrConf->SetFormat(pmtConfig);
									//m_pMediaControl->Run();
									RunGrp();
									if(SUCCEEDED(hr))
									{
										return hr;
									}
									break;
								}
							}
						}
					}
				}
				// Delete the media type when you are done.
				DeleteMediaType(pmtConfig);
			}
			else
				//DEBUGIT(LogFile,"\n-------------NULL---------------");
				LOG(INFO) + "-------------NULL---------------";

		}

	}

	return hr;
} //end Set_ColorFmt(wstring* pwsClrFmt, PinType ePType)

//------------------------------------------------------------------
//
// Prototype:	HRESULT Set_FrameRate(LONGLONG qwFrmRate, PinType ePType)
//
// Description:	This is used to set the resolution of either Preview, 
//				Still or Capture Pins.
//
// Parameters:	pwsClrFmt is a unicode string pointer, holding the color format to set
//				and ePType is Pin category enumuration;
//				V:-Capture Pin, S:-Still Pin, P:-Preview Pin
//
// Returns:	S_OK if Color format is set successfully.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::Set_FrameRate(LONGLONG qwFrmRate, PinType ePType)
{	
	HRESULT hr;
	GUID pType;
	m_pStrConf = NULL;
	INT nIdx=0;
	INT nCount = 0, nSize = 0;

	switch(ePType)
	{
		case V:
			pType = PIN_CATEGORY_CAPTURE;
			break;
		case S:
			pType = PIN_CATEGORY_STILL;
			break;
		case P:
			pType = PIN_CATEGORY_PREVIEW;
			break;

	}
	
	//PIN type, Any media type, Pointer to the capture filter.
	hr = m_pCaptureGraphBuilder->FindInterface(&pType, 0, m_pVideoCaptureFilter,\
													IID_IAMStreamConfig, (void**)&m_pStrConf);
	
	if(SUCCEEDED(hr))
	{
		VIDEO_STREAM_CONFIG_CAPS scc;
		AM_MEDIA_TYPE *pmtConfig;
		
		hr = m_pStrConf->GetNumberOfCapabilities(&nCount, &nSize);
	    if(FAILED(hr))
		{
			return hr;
		}

		if (nSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
		{
					
			for(INT i=nIdx; i<nCount;i++)
			{
				hr = m_pStrConf->GetStreamCaps(i, &pmtConfig, (BYTE*)&scc);
				if (SUCCEEDED(hr))
				{
					if(pmtConfig->majortype == MEDIATYPE_Video)
					{						
						if(scc.MaxFrameInterval == qwFrmRate)
						{
							//m_pMediaControl->Stop();
							CDShowCam::StopGrp();
							hr = m_pStrConf->SetFormat(pmtConfig);
							//m_pMediaControl->Run();
							CDShowCam::RunGrp();
							if(SUCCEEDED(hr))
							{
								return hr;
							}
							break;
						}
					}
				}
				// Delete the media type when you are done.
				DeleteMediaType(pmtConfig);
			}
			//DEBUGIT(LogFile,"\n-----------Invalide Frame Rate--------------");
			LOG(ERROR) + "-----------Invalide Frame Rate--------------";
		}
	}

	return hr;
} //end Set_FrameRate(LONGLONG qwFrmRate, PinType ePType)

//------------------------------------------------------------------
//
// Prototype:	HRESULT SetupPreview(HWND OwnerWnd, RECT rc)
//
// Description:	This is a private method which sets up all componets 
//				necessary for a preview window.
//
// Parameters:	Window Handler(OwnerWnd) to which the preview window should display
//				and the location/dimenssion of the window is given as rc
//
// Returns:	S_OK if all preview setup is successful.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::SetupPreview(HWND OwnerWnd, RECT rc)
{

	HRESULT hr = S_OK;
//	IBaseFilter* pVideoRenderer = NULL;
    IVideoWindow* pVideoWindow = NULL;

hr = CoCreateInstance(CLSID_VideoRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&m_pVideoRenderer);
if(FAILED(hr))
{
	return FALSE;
}
hr = m_pGraphBuilder->AddFilter(m_pVideoRenderer, L"My Video Renderer");
if(FAILED(hr))
{
	return FALSE;
}

// Render the preview pin of the video capture filter. 
	//DEBUGIT(LogFile,"\n!!!! Render PREVIEW PIN !!!!\n");
	LOG(INFO) + "!!!! Render PREVIEW PIN !!!!";

if(SUCCEEDED(hr = m_pCaptureGraphBuilder->RenderStream( &PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pVideoCaptureFilter, NULL, m_pVideoRenderer )))
{
//if(SUCCEEDED(hr = m_pCaptureGraphBuilder->RenderStream( &PIN_CATEGORY_PREVIEW, &MEDIATYPE_Audio, m_pAudioCaptureFilter, NULL, NULL )))
{
	// If rendering the preview succeeded, then we need to find the video
	// renderer filter in order to set up the video window dimensions,
	// location, owner window, etc.
	//LoadString(hInst, STR_VRENDERER, text, MAX_LOADSTRING);
	//if(SUCCEEDED(hr = m_pGraphBuilder->FindFilterByName(L"My Video Renderer", &pVideoRenderer)))
	{
		if(SUCCEEDED(hr = m_pVideoRenderer->QueryInterface( IID_IVideoWindow, (void **)&pVideoWindow)))
		{
			// For now just set the owner window.
			hr = pVideoWindow->put_Owner((OAHWND) OwnerWnd);
			//---------------
			if(SUCCEEDED(hr))
			{
				hr = pVideoWindow->SetWindowForeground(OATRUE);
				if(!(SUCCEEDED(hr)))
				{
					return false;
				}
				
				hr = pVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
				if(!(SUCCEEDED(hr)))
				{
					return false;
				}
			}
			// Resize the video preview window to match owner window size
			if (pVideoWindow)
			{
				//LoadString(hInst, STR_VV_CAP, text, MAX_LOADSTRING);
				//pVideoWindow->SetWindowPosition(MARGIN_GAP,MARGIN_GAP,((rc.right-rc.left)-2*MARGIN_GAP),((rc.bottom-rc.top)*2/3-2*MARGIN_GAP));
				TCHAR waTemp[100];
				wsprintf(waTemp,L"IN:- %d -- %d -- %d -- %d",rc.bottom,rc.left,rc.right,rc.top);
				OutputDebugString(waTemp);

				pVideoWindow->SetWindowPosition(rc.left, rc.top, (rc.right-rc.left), (rc.bottom-rc.top));
				pVideoWindow->put_Caption(L"CAM1 preview");
			}

			hr = pVideoWindow->put_Visible(OATRUE);
			if(FAILED(hr))
			{
				return false;
			}
			//----------------
      	}
	}
}
}
	// cleanup
	if(pVideoWindow)
	{
		pVideoWindow->Release();
		pVideoWindow = NULL;
	}
	//if(pVideoRenderer)
	{
	//	pVideoRenderer->Release();
	//	pVideoRenderer = NULL;
	}

    return hr;
}// end SetupPreview

BOOL CDShowCam::ResizePreview(DWORD dwWidth, DWORD dwHeight)
{
	HRESULT hr = S_FALSE;
    IVideoWindow* pVideoWindow = NULL;

	if (m_pVideoRenderer)
	{
		if(SUCCEEDED(hr = m_pVideoRenderer->QueryInterface( IID_IVideoWindow, (void **)&pVideoWindow)))
		{
			if (SUCCEEDED(hr = pVideoWindow->SetWindowPosition(0, 0, dwWidth, dwHeight)))
			{
				pVideoWindow->Release();
				return TRUE;
			}
			pVideoWindow->Release();
		}
	}
	return FALSE;
}

//------------------------------------------------------------------
//
// Prototype:	HRESULT SetupStill()
//
// Description:	This is a private method which sets up all componets 
//				necessary for a still capture.
//
// Parameters:	none
//
// Returns:	S_OK if still setup is successful.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::SetupStill()
{
	HRESULT hr = S_OK;
	WCHAR DName[TXT_LENGTH];
	WCHAR waTemp[TXT_LENGTH];
	SYSTEMTIME st;
    GetLocalTime(&st);

	//LoadString(hInst, STR_STILL_D_F_FMT, text, MAX_LOADSTRING);
	wsprintf(waTemp, L"%.2d%.2d%.2d", st.wMonth, st.wDay, st.wYear%100);
	//LoadString(hInst, STR_STILL_DIR_PTH, text, MAX_LOADSTRING);
	wsprintf(DName, L"\\My Documents\\My Pictures\\%s", waTemp);
	CreateDirectory(DName, NULL);

	//CoCreate and add the still image sink
	//DEBUGIT(LogFile,"\n!!!! Create CLSID_IMGSinkFilter !!!!\n");
	LOG(INFO) + "!!!! Create CLSID_IMGSinkFilter !!!!";
	if(SUCCEEDED(hr = CoCreateInstance(CLSID_IMGSinkFilter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (LPVOID *)&m_pStillSink )))
	{
		//LoadString(hInst, STR_STILLSINK, text, MAX_LOADSTRING);
		if(SUCCEEDED(hr = m_pGraphBuilder->AddFilter(m_pStillSink, L"StillSink")))
		{
			//DEBUGIT(LogFile,"\n!!!! Add CLSID_IMGSinkFilter !!!!\n");
			LOG(INFO) + "!!!! Add CLSID_IMGSinkFilter !!!!";
			// Render the still image pin to the still image sink.
			hr = m_pCaptureGraphBuilder->RenderStream( &PIN_CATEGORY_STILL, &MEDIATYPE_Video, m_pVideoCaptureFilter, NULL, m_pStillSink);
			if(SUCCEEDED(hr))
			{
				//DEBUGIT(LogFile,"\n!!!! Render STILL PIN OK!!!!\n");
				LOG(INFO) + "!!!! Render STILL PIN OK!!!!";
			}
			else
			{
				//DEBUGIT(LogFile,"\n!!!! Render STILL PIN Fail!!!!\n");
				LOG(ERROR) + "!!!! Render STILL PIN Fail!!!!";
			}
		}
	}


	return hr;

}// end SetupStill

//------------------------------------------------------------------
//
// Prototype:	HRESULT SetupFlip()
//
// Description:	This is a private method which sets up all componets 
//				necessary for Window flipping.
//
// Parameters:	none
//
// Returns:	S_OK if Flip setup is successful.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::SetupFlip()
{
	HRESULT hr;

	m_pPrvPin = NULL;

	if(SUCCEEDED(hr = m_pCaptureGraphBuilder->FindPin((IUnknown*)m_pVideoCaptureFilter, PINDIR_OUTPUT, &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, FALSE, 0, &m_pPrvPin )))
	{
		hr = m_pVideoCaptureFilter->QueryInterface( IID_IAMVideoControl, (void **)&m_pVideoCtrl );
	}


#ifdef DEBUG_MODE
	if(FAILED(hr))
	{
	//	LoadString(hInst, ERR_FLIP_SETUP, text, MAX_LOADSTRING);
	//	OutputDebugString(text);
	}
#endif


	return hr;
}// end SetupFlip

//------------------------------------------------------------------
//
// Prototype:	HRESULT Cus_GetRange(CameraCustomProperty eProp, LONG* plMin,
//											LONG* plMax, LONG* plDelta, LONG* plDef)
//
// Description:	This is a private method which retrives the limits of a given custom property.
//
// Parameters:	eProp is the custom property to query, plMin is pointer to Minimum value of the property,
//				plMax is pointer to Maximum value of the property, plDelta is pointer to Step value of the property,
//				and finally plDef is pointer to Default value of the property)
//
// Returns:	S_OK if limits are retreived successfully.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::Cus_GetRange(CameraCustomProperty eProp, LONG* plMin, LONG* plMax, LONG* plDelta, LONG* plDef)
{
	HRESULT hr = S_OK;

	if(eProp == CUS_PROPERTY_AIMING)
	{
		*plMin=0;
		*plMax=2;
		*plDelta=1;
		*plDef=0;
	}
	else
	{
		*plMin=0;
		*plMax=0;
		*plDelta=0;
		*plDef=0;
	}


return hr;
}//end Cus_GetRange

//------------------------------------------------------------------
//
// Prototype:	HRESULT Cus_Get(CameraCustomProperty eProp, LONG* plVal)
//
// Description:	This is a private method which retrives the current value of the given custom property.
//
// Parameters:	eProp is the custom property to query, plVal returns the current value of the property.
//
// Returns:	S_OK if value is retreived successfully.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::Cus_Get(CameraCustomProperty eProp, LONG* plVal)
{
	HRESULT hr;

	if(eProp==CUS_PROPERTY_AIMING)
	{
		hr = GunGet((DWORD* )plVal);
	}

return hr;
}//end Cus_Get

//------------------------------------------------------------------
//
// Prototype:	HRESULT Cus_Set(CameraCustomProperty eProp, LONG plVal)
//
// Description:	This is a private method which sets value of the given custom property.
//
// Parameters:	eProp is the custom property to query, plVal is value to set.
//
// Returns:	S_OK if value is successfully set.
// 
// Notes:
//------------------------------------------------------------------
HRESULT CDShowCam::Cus_Set(CameraCustomProperty eProp, LONG plVal)
{
	HRESULT hr;

	if(eProp==CUS_PROPERTY_AIMING)
	{
		hr = GunSet((DWORD)plVal);
	}

return hr;
}//end Cus_Set


//------------------------------------------------------------------
//
// Prototype:	DWORD GunGet(DWORD *value)
//
// Description:	This is a private method which retreives current value of the custom property Aiming.
//
// Parameters:	value is double word pointer, holding current value of the property.
//
// Returns:	S_OK if successful.
// 
// Notes:
//------------------------------------------------------------------
DWORD CDShowCam::GunGet(DWORD* /* value -- unused*/)
{

	HRESULT hr=S_OK;
#ifdef CUSTOM_PROP
    IKsPropertySet* pKs;

	m_pCamCtrl = NULL;

	hr = m_pVideoCaptureFilter->QueryInterface(IID_IAMCameraControl, (void **)&m_pCamCtrl);
	if(SUCCEEDED(hr))
	{
		hr = m_pCamCtrl->QueryInterface(IID_IKsPropertySet,(void **)&pKs);

		if(SUCCEEDED(hr))
		{     
			DWORD myLen;
			CSPROPERTY_CAMERACONTROL_S psprop;
			psprop.Property.Set = PROPSETID_SYMBOLCUSTOM_CAMERACONTROL;
			psprop.Property.Id  = CSPROPERTY_SYMBOLCUSTOM_CAMERACONTROL_GUNSITE;
			psprop.Property.Flags = CSPROPERTY_TYPE_GET;

			psprop.Value = 0;
			psprop.Flags = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;

	        psprop.Capabilities = CSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL|CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;

			myLen = sizeof(CSPROPERTY_CAMERACONTROL_S);

			//hr = pKs->Set(PROPSETID_SYMBOLCUSTOM_CAMERACONTROL, CSPROPERTY_SYMBOLCUSTOM_CAMERACONTROL_GUNSITE,NULL,0, 
			//    &psprop, myLen);
			hr = pKs->Get(PROPSETID_SYMBOLCUSTOM_CAMERACONTROL, CSPROPERTY_SYMBOLCUSTOM_CAMERACONTROL_GUNSITE, NULL,0,
	                    &psprop, myLen, &myLen);          
	
			if(!SUCCEEDED(hr))
			{
#ifdef DEBUG_MODE
			// LoadString(hInst, ERR_GUN_GET, text, MAX_LOADSTRING);
			// OutputDebugString(text);
#endif
			}
			else
			{
			  if(value!=NULL)
			  {
                 *value = psprop.Value;
			  }
			}
			 hr = pKs->Release();          

		}
		//free resources
		m_pCamCtrl->Release();
	}
	else
	{
		DEBUGIT(LogFile,"\n!!!! Error Query IID_IAMCameraControl !!!! \n");
	}
#endif
return hr;
}//end GunGet

//------------------------------------------------------------------
//
// Prototype:	DWORD GunSet(DWORD value)
//
// Description:	This is a private method which sets the Aiming value.
//
// Parameters:	value is double word holding value to set.
//
// Returns:	S_OK if successful.
// 
// Notes:
//------------------------------------------------------------------
DWORD CDShowCam::GunSet(DWORD /* value --unused*/)
{

	HRESULT hr=S_OK;
#ifdef CUSTOM_PROP
    IKsPropertySet* pKs;

	m_pCamCtrl = NULL;
		
	hr = m_pVideoCaptureFilter->QueryInterface(IID_IAMCameraControl, (void **)&m_pCamCtrl);
	if(SUCCEEDED(hr))
	{
		hr = m_pCamCtrl->QueryInterface(IID_IKsPropertySet,(void **)&pKs);
		if(SUCCEEDED(hr))
		{	
				DWORD myLen;
				CSPROPERTY_CAMERACONTROL_S psprop;
				psprop.Property.Set = PROPSETID_SYMBOLCUSTOM_CAMERACONTROL;
				psprop.Property.Id  = CSPROPERTY_SYMBOLCUSTOM_CAMERACONTROL_GUNSITE;
				psprop.Property.Flags = CSPROPERTY_TYPE_SET;
				psprop.Value = value;
				psprop.Flags = CSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
				psprop.Capabilities = CSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL|CSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
				myLen = sizeof(CSPROPERTY_CAMERACONTROL_S);
		
  				hr = pKs->Set(PROPSETID_SYMBOLCUSTOM_CAMERACONTROL, CSPROPERTY_SYMBOLCUSTOM_CAMERACONTROL_GUNSITE,NULL,0, 
					&psprop, myLen);

				if(!SUCCEEDED(hr))
				{
#ifdef DEBUG_MODE
				//	LoadString(hInst, ERR_GUN_SET, text, MAX_LOADSTRING);
				//	OutputDebugString(text);
#endif
				}
				hr = pKs->Release();          
		}
		//free resources
		m_pCamCtrl->Release();
	}
	else
	{
		DEBUGIT(LogFile,"\n!!!! Error Query IID_IAMCameraControl !!!! \n");
	}
#endif
return hr;
}// end GunSet

HRESULT CDShowCam::Set_CaptureSound(LPWSTR szSoundFile)
{

	HRESULT result = S_FALSE;
	if ((szSoundFile) && (wcslen(szSoundFile) > 0))
	{
		HKEY hKey = 0;
		WCHAR buf[255] = {0};
		DWORD dwType = 0;
		DWORD dwBufSize = sizeof(buf);
		LPCWSTR subkey = L"Drivers\\BuiltIn\\DShowCam";

		LOG(INFO) + "CDShowCam::Set_CaptureSound - 0";

		if( RegOpenKey(HKEY_LOCAL_MACHINE,subkey,&hKey) == ERROR_SUCCESS)
		{
			LOG(INFO) + "CDShowCam::Set_CaptureSound - 1";
			dwType = REG_SZ;
			if( RegQueryValueEx(hKey,L"ShutterSound",0, &dwType, (BYTE*)buf, &dwBufSize) == ERROR_SUCCESS)
			{	
				LOG(INFO) + "CDShowCam::Set_CaptureSound - 2";
				if (wcscmp(buf, szSoundFile) == 0)
				{
					LOG(INFO) + "CDShowCam::Set_CaptureSound - 3";
					// write the shutter click wav to the registry
					result = RegSetValueEx(hKey, L"ShutterSound", 0, dwType, (BYTE*)szSoundFile,
						wcslen(szSoundFile)*sizeof(TCHAR));
					if (result == S_FALSE)
						LOG(ERROR) + "CDShowCam::Set_CaptureSound  - Shutter sound NOT set";
					else 
						LOG(INFO) + "CDShowCam::Set_CaptureSound  - Shutter sound set to [" + szSoundFile + "]";
				}
				RegCloseKey(hKey);
			}
		}
	}
	LOG(INFO) + "CDShowCam::Set_CaptureSound - 3";
	return result;
}

BOOL CDShowCam::HandlesCaptureSound()
{

	return m_bCaptureSoundRegKeyExists;
}

//#pragma warning (pop)
// end of file
