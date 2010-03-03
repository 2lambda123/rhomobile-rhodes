#include "stdafx.h"

#if defined(_WIN32_WCE)

#include <nled.h>
#include "Vibrate.h"
#include <common/RhodesApp.h>

IMPLEMENT_LOGCLASS(CVibrate, "RingtoneManager");

CVibrate *CVibrate::m_pInstance = NULL;
CMutex CVibrate::m_mxLocker;

CVibrate::CVibrate (IRhoClassFactory* factory) : CRhoThread(factory), m_nDuration(2), m_bToggled(false)
{
}

CVibrate::~CVibrate ()
{
    untoggle();
}

void CVibrate::createCVibrate()
{
    static CVibrate instance(createClassFactory());
    m_pInstance = &instance;
}

CVibrate &CVibrate::getCVibrate()
{
    m_mxLocker.Lock();

    if (!m_pInstance)
        createCVibrate();
    
    m_mxLocker.Unlock();
    
    return *m_pInstance;
}

void CVibrate::toggle()
{
    toggle (m_nDuration);
}

void CVibrate::toggle(int seconds)
{
    m_nDuration = seconds;
    start(epNormal);
}

void CVibrate::untoggle()
{
    NLED_SETTINGS_INFO settings;
    settings.LedNum= 1; 
    settings.OffOnBlink= 0; 
    NLedSetDevice (NLED_SETTINGS_INFO_ID, &settings);
    m_bToggled = false;
}

void CVibrate::run() 
{
    NLED_SETTINGS_INFO settings;
    
    settings.LedNum= 1;
    NLedGetDeviceInfo(NLED_SETTINGS_INFO_ID,  &settings);
    
    if (!settings.OffOnBlink) {
        
        settings.LedNum= 1; 
        settings.OffOnBlink= 1;
        NLedSetDevice (NLED_SETTINGS_INFO_ID, &settings);
        m_bToggled = true;
        
        wait (m_nDuration);
    
        untoggle();
    }
}

#endif // _WIN32_WCE
