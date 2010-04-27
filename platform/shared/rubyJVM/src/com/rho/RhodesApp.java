package com.rho;

import com.rho.net.*;
import com.xruby.runtime.lang.RubyValue;
import java.util.Vector;

public class RhodesApp
{
	private static final RhoLogger LOG = RhoLogger.RHO_STRIP_LOG ? new RhoEmptyLogger() : 
		new RhoLogger("RhodesApp");
	RhoConf RHOCONF(){ return RhoConf.getInstance(); }
	
	static RhodesApp m_pInstance;
	
    private String m_strRhoRootPath, m_strBlobsDirPath, m_strDBDirPath;
	
    Vector/*<unsigned long>*/ m_arCallbackObjects = new Vector();
    private SplashScreen m_oSplashScreen = new SplashScreen();
    
    Mutex m_mxPushCallback = new Mutex();
    String m_strPushCallback = "", m_strPushCallbackParams = "";

	Mutex m_mxScreenRotationCallback = new Mutex();
    String m_strScreenRotationCallback = "", m_strScreenRotationCallbackParams = "";
    
    NetRequest getNet() { return RhoClassFactory.createNetRequest();}
    
    public static RhodesApp Create(String strRootPath)
    {
        if ( m_pInstance != null ) 
            return m_pInstance;

        m_pInstance = new RhodesApp(strRootPath);
        return m_pInstance;
    }
    
    public static void Destroy()
    {
    	
    }
    
    public static RhodesApp getInstance(){ return m_pInstance; }
    public SplashScreen getSplashScreen(){return m_oSplashScreen;}
	
    public String getBlobsDirPath(){return m_strBlobsDirPath; }
    public String getDBDirPath(){return m_strDBDirPath; }
    public String getRhoRootPath(){return m_strRhoRootPath;}
    
    RhodesApp(String strRootPath)
    {
        m_strRhoRootPath = strRootPath;
        
        getSplashScreen().init();
        
        initAppUrls();        
    }

    void initAppUrls() 
    {
        m_strBlobsDirPath = getRhoRootPath() + "db/db-files";
    	m_strDBDirPath = getRhoRootPath() + "db";
    }
    
    public String resolveDBFilesPath(String strFilePath)
    {
        if ( strFilePath.startsWith(getRhoRootPath()) )
            return strFilePath;

        return FilePath.join(getRhoRootPath(), strFilePath);
    }
    
    public String addCallbackObject(RubyValue valObject, String strName)
    {
        int nIndex = -1;
        for (int i = 0; i < (int)m_arCallbackObjects.size(); i++)
        {
            if ( m_arCallbackObjects.elementAt(i) == null )
                nIndex = i;
        }
        if ( nIndex  == -1 )
        {
            m_arCallbackObjects.addElement(valObject);
            nIndex = m_arCallbackObjects.size()-1;
        }else
            m_arCallbackObjects.setElementAt(valObject,nIndex);

        String strRes = "__rho_object[" + strName + "]=" + nIndex;

        return strRes;
    }

    public RubyValue getCallbackObject(int nIndex)
    {
        if ( nIndex < 0 || nIndex > m_arCallbackObjects.size() )
            return null;

        RubyValue res = (RubyValue)m_arCallbackObjects.elementAt(nIndex);
        m_arCallbackObjects.setElementAt(null,nIndex);        
        return res;
    }
    
    String canonicalizeRhoUrl(String strUrl)throws Exception 
    {
    	return getNet().resolveUrl(strUrl);    	
    }
    
    public void setPushNotification(String strUrl, String strParams )throws Exception
    {
        synchronized(m_mxPushCallback)
        {
            m_strPushCallback = canonicalizeRhoUrl(strUrl);
            m_strPushCallbackParams = strParams;
        }
    }

    public boolean callPushCallback(String strData)throws Exception
    {
        synchronized(m_mxPushCallback)
        {
            if ( m_strPushCallback.length() == 0 )
                return false;
        	
            String strBody = "status=ok&message=";
            strBody += URI.urlEncode(strData);
            strBody += "&rho_callback=1";
            if ( m_strPushCallbackParams.length() > 0 )
                strBody += "&" + m_strPushCallbackParams;

            NetResponse resp = getNet().pushData( m_strPushCallback, strBody, null );
            if (!resp.isOK())
                LOG.ERROR("Push notification failed. Code: " + resp.getRespCode() + "; Error body: " + resp.getCharData() );
            else
            {
                String szData = resp.getCharData();
                return !(szData != null && szData.equals("rho_push"));
            }
        }

        return true;
    }
 
    public void setScreenRotationNotification(String strUrl, String strParams)throws Exception
    {
        synchronized(m_mxScreenRotationCallback)
        {
            m_strScreenRotationCallback = canonicalizeRhoUrl(strUrl);
            m_strScreenRotationCallbackParams = strParams;
        }
    }

    public void callScreenRotationCallback(int width, int height, int degrees)throws Exception
    {
    	synchronized(m_mxScreenRotationCallback) 
    	{
    		if (m_strScreenRotationCallback.length() == 0)
    			return;
    		
    		String strBody = "rho_callback=1";
    		
            strBody += "&width=";   strBody += width;
    		strBody += "&heigth=";  strBody += height;
    		strBody += "&degrees="; strBody += degrees;
    		
            if ( m_strScreenRotationCallbackParams.length() > 0 )
                strBody += "&" + m_strPushCallbackParams;
    			
            NetResponse resp = getNet().pushData( m_strScreenRotationCallback, strBody, null );
            if (!resp.isOK()) {
                LOG.ERROR("Screen rotation notification failed. Code: " + resp.getRespCode() + "; Error body: " + resp.getCharData());
            }
        }
    }
    
    public void callPopupCallback(String strCallbackUrl, String id, String title)throws Exception
    {
        if ( strCallbackUrl == null || strCallbackUrl.length() == 0 )
            return;
    	
        strCallbackUrl = canonicalizeRhoUrl(strCallbackUrl);
        String strBody = "button_id=" + id + "&button_title=" + title;
        strBody += "&rho_callback=1";
        getNet().pushData( strCallbackUrl, strBody, null );
    }
    
}