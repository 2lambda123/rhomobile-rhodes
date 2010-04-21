#include "URI.h"
#include "common/Tokenizer.h"

namespace rho {
namespace net {

URI::URI(const String& strUrl)
{
    m_strUrl = strUrl;
}

String URI::getPath()
{
    const char* url = m_strUrl.c_str();
    const char* pStartSrv, *pEndSrv;
    const char* pHttp = strstr(url,"://");
    if ( !pHttp )
        pHttp = strstr(url,":\\\\");

    if ( pHttp )
        pStartSrv = pHttp+3;
    else
        pStartSrv = url;

    pEndSrv = strchr( pStartSrv, '/');
    if ( !pEndSrv )
        pEndSrv = strchr( pStartSrv, '\\');

    const char* pStartPath = pEndSrv ? pEndSrv+1 : 0;
    if ( !pStartPath || !*pStartPath)
        return String();

    const char* pEndPath = pStartPath ? strrchr( pStartPath, '?') :0;
    if (!pEndPath)
        pEndPath = pStartPath + strlen(pStartPath);

    return String(pStartPath, pEndPath - pStartPath);
}

String URI::getQueryString()
{
    const char* szQuest = strrchr( m_strUrl.c_str(), '?');
    if ( !szQuest )
        return String();

    return String(szQuest+1);
}

String URI::getScheme()
{
    const char* url = m_strUrl.c_str();
    const char* pHttp = strstr(url,"://");
    if ( !pHttp )
        pHttp = strstr(url,":\\\\");

    if ( !pHttp )
        return String();

    String res(url, pHttp-url);
    return res;
}

String URI::getPathSpecificPart()
{
    const char* szQuest = strrchr( m_strUrl.c_str(), '?');
    if ( !szQuest )
        return m_strUrl;

    return String(m_strUrl.c_str(), szQuest-m_strUrl.c_str());
}

static void toHexString(int i, String& strRes, int radix)
{
    char buf[33];
	bool neg= false;
	int f, n;
	if(i<0) { neg= true; i= -i; };
	f= 32;
	buf[f--]= 0;
	do
	{
		n= i%radix;
		if(n<10) buf[f]= '0'+n;
		else buf[f]= 'a'+n-10;
		i= i/radix;
		f--;
	}
	while(i>0);

	if(neg) 
        buf[f--]= '-';

    strRes += (buf+f+1);
}

String URI::urlEncode(const String& fullPath)
{
    String res;
    urlEncode(fullPath, res);
    return res;
}

/*static*/ void URI::urlEncode(const String& fullPath, String& strRes)
{
    int len = fullPath.length();

    char c;
    boolean bFound = false;
    for  (int index=0; index < len ; index++){
      c = fullPath.at(index);
      if ( c == '^'  || c == '_'
		     || c == '\\' || c == '-'
		     || c == '.'
		     || (c >= 'A' && c <= 'Z')
		     || (c >= 'a' && c <= 'z')
		     || (c >= '0' && c <= '9') ) {
            if ( bFound ) 
                strRes += c;
	    } else {
            if ( !bFound )
            {
                strRes += fullPath.substr(0,index);
                bFound = true;
            }

		    strRes += '%';
		    if (c > 15) { // is it a non-control char, ie. >x0F so 2 chars
			    //sb.append(Integer.toHexString((int)c)); // just add % and the string
                toHexString(c, strRes, 16);
		    } else {
                strRes += '0';
                toHexString(c, strRes, 16);
			    //sb.append("0" + Integer.toHexString((int)c));
			    // otherwise need to add a leading 0
		    }
	    }
    	  
    }

    if ( !bFound )
        strRes += fullPath;
}

//"auth_token=; path=/; expires=Thu, 01 Jan 1970 00:00:00 GMT, auth_token=887b2ffd30a7b97be9a0986d7746a934421eec7d; path=/; expires=Sat, 24 Oct 2009 20:56:55 GMT, rhosync_session=BAh7BzoMdXNlcl9pZGkIIgpmbGFzaElDOidBY3Rpb25Db250cm9sbGVyOjpGbGFzaDo6Rmxhc2hIYXNoewAGOgpAdXNlZHsA--f9b67d99397fc534107fb3b7483ccdae23b4a761; path=/; expires=Sun, 10 Oct 2010 19:10:58 GMT; HttpOnly");
//"auth_token=; path=/; expires=Thu, 01 Jan 1970 00:00:00 GMT");
//"rhosync_session=BAh7CToNcGFzc3dvcmQiFTiMYru1W11zuoAlN%2FPtgjc6CmxvZ2luIhU4jGK7tVtdc7qAJTfz7YI3Ogx1c2VyX2lkaQYiCmZsYXNoSUM6J0FjdGlvbkNvbnRyb2xsZXI6OkZsYXNoOjpGbGFzaEhhc2h7AAY6CkB1c2VkewA%3D--a7829a70171203d72cd4e83d07b18e8fcf5e2f78; path=/; expires=Thu, 02 Sep 2010 23:51:31 GMT; HttpOnly");

/*static*/ void URI::parseCookie(const char* szCookie, String& strRes) 
{
    common::CTokenizer stringtokenizer(szCookie, ";");
	while (stringtokenizer.hasMoreTokens()) 
    {
		String tok = stringtokenizer.nextToken();
		tok = String_trim(tok);
		if (tok.length() == 0) {
			continue;
		}
		
		//expires=Thu, 01 Jan 1970 00:00:00 GMT, auth_token=
		int nExp = tok.find("expires=");
		if ( nExp >= 0 )
		{
			int nEnd = tok.find(',', nExp);
			if ( nEnd >= 0 )
			{
				int nEnd1 = tok.find(',', nEnd+1);
				if ( nEnd1 >= 0 )
					nEnd = nEnd1;
				else
					nEnd = tok.length()-1;
			}
			
			tok = tok.substr(0,nExp) + tok.substr(nEnd+1);
			tok = String_trim(tok);
		}
		
		int nEq = tok.find('=');
		if ( nEq < 0 )
			continue;
		
		strRes += tok + ";";  
/*
		int i = 0;
		if ( (i=tok.find("auth_token=")) >= 0 )
		{
			String val = trim(tok.substr(i+11));
			if ( val.length() > 0 )
			{
				cookie.strAuth = "auth_token=" + val;
				bAuth = true;
			}
		}else if ( (i=tok.find("path=")) >= 0 )
		{
			String val = trim(tok.substr(i+6));
			if ( val.length() > 0 )
			{
				if (bAuth)
					cookie.strAuth += ";path=" + val;
				else if (bSession)
					cookie.strSession += ";path=" + val;
			}
		}else if ( (i=tok.find("rhosync_session=")) >= 0 )
		{
			String val = trim(tok.substr(i+16));
			if ( val.length() > 0 )
			{
				cookie.strSession = "rhosync_session=" + val;
				bSession = true;
			}
		} */
	}
}
	
/**static*/ boolean URI::isLocalHost(const String& strUrl)
{
	return strUrl.find("http://localhost") == 0 ||
        strUrl.find("http://127.0.0.1") == 0;
}	

}
}
