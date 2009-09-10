#pragma once

#include "common/RhoStd.h"
#include "db/DBResult.h"
#include "logging/RhoLog.h"

namespace rho {

namespace db {
    class CDBAdapter;
}
namespace net {
    struct INetRequest;
}
namespace json {
    class CJSONEntry;
    class CJSONArrayIterator;
}

namespace sync {

class CSyncBlob
{
   String m_strBody;
   String m_strFilePath;

public:
    CSyncBlob(const String& body, const String& filePath ) :
      m_strBody(body), m_strFilePath(filePath){}

    const String& getBody()const{ return m_strBody; }
    const String& getFilePath()const{ return m_strFilePath; }
};

class CValue
{
public:
    String m_strValue;
	String m_strAttrType;
	uint64 m_nID;
	
    CValue(json::CJSONEntry& oJsonEntry);//throws JSONException
    CValue(json::CJSONEntry& oJsonEntry, int nVer);//throws JSONException
};

class CSyncEngine;
class CSyncSource
{
    DEFINE_LOGCLASS;

    CSyncEngine& m_syncEngine;

    int    m_nID;
    String m_strName;
    String m_strUrl;
    uint64 m_token;
    boolean m_bTokenFromDB;

    int m_nCurPageCount, m_nInserted, m_nDeleted, m_nTotalCount;
    boolean m_bGetAtLeastOnePage;
public:
    int m_nErrCode;
    String m_strError;
    String m_strParams;
    String m_strAction;
private:
    //String m_strPushBody;
    VectorPtr<CSyncBlob*> m_arSyncBlobs;
    String m_strAskParams;

public:
    CSyncSource(int id, const String& strUrl, const String& strName, uint64 token, CSyncEngine& syncEngine );
    virtual void sync();

    String getUrl()const { return m_strUrl; }
    int getID()const { return m_nID; }
    String getName() { return m_strName; }
    int getServerObjectsCount()const{ return m_nInserted+m_nDeleted; }

    uint64 getToken()const{ return m_token; }
    void setToken(uint64 token){ m_token = token; m_bTokenFromDB = false; }
    virtual boolean isEmptyToken()
    {
        return m_token == 0;
    }

//private:
    CSyncSource();
    CSyncSource(CSyncEngine& syncEngine );

    void syncClientChanges();
    void syncServerChanges();
    void makePushBody(String& strBody, const char* szUpdateType);
    //void makePushBody( rho::db::CDBResult& res );//throws DBException
    void getAndremoveAsk();
    void setAskParams(const String& ask){ m_strAskParams = ask;}
    String getAskParams()const{ return m_strAskParams;}
    void processToken(uint64 token);

    int getInsertedCount()const { return m_nInserted; }
    int getDeletedCount()const { return m_nDeleted; }
    void setCurPageCount(int nCurPageCount){m_nCurPageCount = nCurPageCount;}
    void setTotalCount(int nTotalCount){m_nTotalCount = nTotalCount;}
    int  getCurPageCount(){return m_nCurPageCount;}
    int  getTotalCount(){return m_nTotalCount;}

    void processServerData(const char* szData);
    boolean processSyncObject(json::CJSONEntry& oJsonEntry);
    boolean processSyncObject_ver1(json::CJSONEntry oJsonObject, int nSrcID);//throws Exception
    void processServerData_Ver0(json::CJSONArrayIterator& oJsonArr);
    void processServerData_Ver1(json::CJSONArrayIterator& oJsonArr);

    VectorPtr<CSyncBlob*>& getSyncBlobs(){ return m_arSyncBlobs; }
    void syncClientBlobs(const String& strBaseQuery);
    boolean sendClientChanges(String strUpdateType);//throws Exception

    String makeFileName(const CValue& value);//throws Exception
    boolean downloadBlob(CValue& value);//throws Exception
private:
    CSyncEngine& getSync(){ return m_syncEngine; }
    db::CDBAdapter& getDB();
    net::INetRequest& getNet();

};

}
}