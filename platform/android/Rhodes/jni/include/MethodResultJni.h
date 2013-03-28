#pragma once

#include "rhodes/JNIRhoRuby.h"
#include "rhodes/JNIRhoJS.h"
#include "ruby.h"
#include "json/JSONIterator.h"
#include "api_generator/MethodResultConvertor.h"

#include <string>

namespace rho {
namespace apiGenerator {

template <typename T> struct CallbackSelector;

class MethodResultJni;

template <>
struct CallbackSelector<VALUE> {
    CallbackSelector(MethodResultJni& result, JNIEnv* env, jstring jUrl, jstring jData);
};

template <>
struct CallbackSelector<rho::json::CJSONEntry> {
    CallbackSelector(MethodResultJni& result, JNIEnv* env, jstring jUrl, jstring jData);
};

class MethodResultJni
{
    static const char * const METHOD_RESULT_CLASS;

    static jclass s_methodResultClass;
    static jmethodID s_midMethodResult;

    static jmethodID s_midGetResultType;
    static jmethodID s_midGetJson;
    static jmethodID s_midReset;

    static jfieldID s_fidStrCallback;
    static jfieldID s_fidStrCallbackData;
    static jfieldID s_fidRubyProcCallback;
    static jfieldID s_fidResultParamName;
    static jfieldID s_fidObjectClassPath;
    static jfieldID s_fidRubyObjectClass;

    static jfieldID s_fidBoolean;
    static jfieldID s_fidInteger;
    static jfieldID s_fidDouble;
    static jfieldID s_fidString;
    static jfieldID s_fidList;
    static jfieldID s_fidMap;

    static JNIEnv* jniInit(JNIEnv*);
    JNIEnv* jniInit() const { return m_env = jniInit(m_env ? m_env : jnienv()); }

public:
    enum ResultType { typeNone = 0, typeBoolean, typeInteger, typeDouble, typeString, typeList, typeMap, typeError, typeArgError };

private:

    mutable JNIEnv* m_env;
    jhobject m_jhResult;
    bool m_bGlobalRef;

    bool m_hasCallback;

    ResultType m_resType;
    std::string m_errMsg;

    jboolean getBooleanResult(JNIEnv*) const;
    jint getIntegerResult(JNIEnv*) const;
    jdouble getDoubleResult(JNIEnv*) const;
    jstring getStringResult(JNIEnv*) const;
    jobject getListResult(JNIEnv*) const;
    jobject getMapResult(JNIEnv*) const;
    int getResultType(JNIEnv*) const;
    jstring getJSONResult(JNIEnv*) const;
    rho::String getErrorMessage(JNIEnv*) const;
    void reset(JNIEnv*);

    jstring getStrCallback(JNIEnv*) const;
    jstring getStrCallbackData(JNIEnv*) const;
    jlong getRubyProcCallback(JNIEnv*) const;

    jlong getRubyObjectClass(JNIEnv* env) const;
    jstring getObjectClassPath(JNIEnv* env) const;
    jstring getResultParamName(JNIEnv* env) const;

    void setRubyObjectClass(JNIEnv* env, jlong jClass);
    void setObjectClassPath(JNIEnv* env, jstring jClassPath);

    void setResultParamName(JNIEnv* env, jstring jName);

    void setCallback(JNIEnv* env, jstring jUrl, jstring jData);
    void setRubyProcCallback(JNIEnv* env, jlong jRubyProc);
public:
    MethodResultJni(bool isRuby);
    MethodResultJni(JNIEnv* env, jobject jResult);
    MethodResultJni(const MethodResultJni& result);
    ~MethodResultJni();

    jboolean getBooleanResult() const { return getBooleanResult(m_env); }
    jint getIntegerResult() const { return getIntegerResult(m_env); }
    jdouble getDoubleResult() const { return getDoubleResult(m_env); }
    jstring getStringResult() const { return getStringResult(m_env); }
    jobject getListResult() const { return getListResult(m_env); }
    jobject getMapResult() const { return getMapResult(m_env); }
    jstring getJSONResult() const { return getJSONResult(m_env); }
    int getResultType() const { return getResultType(m_env); }
    rho::String getErrorMessage() const { return getErrorMessage(m_env); }
    void reset() { reset(m_env); }
    void callRubyBack(bool releaseCallback);
    void callJSBack();
    static void releaseRubyProcCallback(unsigned long rubyProc);
    unsigned long getRubyObjectClass() const
    {
        JNIEnv* env = jniInit();
        if (!env) {
            RAWLOGC_ERROR("MethodResultJNI", "JNI initialization failed");
            return 0;
        }
        return static_cast<unsigned long>(getRubyObjectClass(env));
    }
    rho::String getObjectClassPath() const
    {
        JNIEnv* env = jniInit();
        if (!env) {
            RAWLOGC_ERROR("MethodResultJNI", "JNI initialization failed");
            return rho::String();
        }
        return rho_cast<rho::String>(env, getObjectClassPath(env));
    }
    rho::String getResultParamName() const
    {
        JNIEnv* env = jniInit();
        if (!env) {
            RAWLOGC_ERROR("MethodResultJNI", "JNI initialization failed");
            return rho::String();
        }
        return rho_cast<rho::String>(env, getResultParamName(env));
    }

    void setRubyObjectClass(unsigned long klass)
    {
        JNIEnv* env = jniInit();
        if (!env) {
            RAWLOGC_ERROR("MethodResultJNI", "JNI initialization failed");
            return;
        }
        setRubyObjectClass(env, static_cast<jlong>(klass));
    }

    void setObjectClassPath(const rho::String& klassPath)
    {
        JNIEnv* env = jniInit();
        if (!env) {
            RAWLOGC_ERROR("MethodResultJNI", "JNI initialization failed");
            return;
        }
        setObjectClassPath(env, rho_cast<jstring>(env, klassPath));
    }

    void setResultParamName(const rho::String& name)
    {
        JNIEnv* env = jniInit();
        if (!env) {
            RAWLOGC_ERROR("MethodResultJNI", "JNI initialization failed");
            return;
        }
        setResultParamName(env, rho_cast<jstring>(env, name));
    }

    bool hasCallback() const { return m_hasCallback; }
    void disconnect(JNIEnv* env);

    template <typename T>
    void setCallBack(T url)
    {
        JNIEnv* env = jniInit();
        if (!env) {
            setError("JNI initialization failed");
            RAWLOGC_ERROR("MethodResultJNI", "JNI initialization failed");
            return;
        }
        jhstring jhUrl = rho_cast<jstring>(env, url);
        if(!jhUrl) {
            setArgError("Callback URL must be a string");
            RAWLOGC_ERROR("MethodResultJNI", "Callback URL must be string");
            return;
        }

        //CallbackSelector<T>(*this, env, jhUrl.get(), 0);
        setCallback(env, jhUrl.get(), (jstring)0);
    }

    template <typename T>
    void setCallBack(T url, T data)
    {
        JNIEnv* env = jniInit();
        if (!env) {
            RAWLOGC_ERROR("MethodResultJNI", "JNI initialization failed");
            return;
        }
        jhstring jhUrl = rho_cast<jstring>(env, url);
        if(!jhUrl) {
            setArgError("Callback URL must be a string");
            RAWLOGC_ERROR("MethodResultJNI", "Callback URL must be string");
            return;
        }
        jhstring jhData = rho_cast<jstring>(env, data);
        if(!jhData) {
            setArgError("Callback URL data must be a string");
            RAWLOGC_ERROR("MethodResultJNI", "Callback URL must be string");
            return;
        }

        //CallbackSelector<T>(*this, env, jhUrl.get(), jhData.get());
        setCallback(env, jhUrl.get(), jhData.get());
    }
    void setRubyProcCallBack(unsigned long rubyProc)
    {
        JNIEnv* env = jniInit();
        if (!env) {
            RAWLOGC_ERROR("MethodResultJNI", "JNI initialization failed");
            return;
        }
        setRubyProcCallback(env, static_cast<jlong>(rubyProc));
    }

    operator jobject () { return m_jhResult.get(); }
    operator bool () const { return jobject(this) != 0; }

    void setError(const char* msg) {
        m_resType = typeError;
        m_errMsg = msg;
    }
    void setArgError(const char *msg) {
        m_resType = typeArgError;
        m_errMsg = msg;
    }
};

template<>
class CRubyResultConvertor<MethodResultJni> {
    MethodResultJni& m_oResult;
    bool m_bForCallback;

    bool hasObjectClass()
    {
        return m_oResult.getRubyObjectClass() != 0 || m_oResult.getObjectClassPath().length() > 0;
    }
    VALUE getObjectWithId(const rho::String& id)
    {
        VALUE res;
        static VALUE classValue = m_oResult.getRubyObjectClass();
        if(classValue ==0 || rho_ruby_is_NIL(classValue))
        {
            classValue = rho_ruby_get_class_byname(m_oResult.getObjectClassPath().c_str());
        }
        if(!rho_ruby_is_NIL(classValue))
        {
            res = rho_ruby_create_object_with_id(classValue, id.c_str());
        } else
        {
            res = rho_ruby_get_NIL();
        }
        return res;
    }

    VALUE getObjectOrString(const rho::String& str)
    {
        VALUE valObj = 0;
        if(hasObjectClass())
        {
            valObj = getObjectWithId(str.c_str());
            if(rho_ruby_is_NIL(valObj))
                valObj = rho_ruby_create_string(str.c_str());
        } else
            valObj = rho_ruby_create_string(str.c_str());

        return valObj;
    }
public:
    CRubyResultConvertor(MethodResultJni& result, bool forCallback) : m_oResult(result), m_bForCallback(forCallback) {}

    bool isBool() { return m_oResult.getResultType() == MethodResultJni::typeBoolean; }
    bool isInt() { return m_oResult.getResultType() == MethodResultJni::typeInteger; }
    bool isDouble() { return m_oResult.getResultType() == MethodResultJni::typeDouble; }
    bool isString() { return m_oResult.getResultType() == MethodResultJni::typeString; }
    bool isJSON() { return false; }
    bool isArray() { return m_oResult.getResultType() == MethodResultJni::typeList; }
    bool isHash() { return m_oResult.getResultType() == MethodResultJni::typeMap; }
    bool isError() { return m_oResult.getResultType() == MethodResultJni::typeError || m_oResult.getResultType() == MethodResultJni::typeArgError; }
    bool isNil() { return m_oResult.getResultType() == MethodResultJni::typeNone; }

    VALUE getBool() { return rho_cast<VALUE>(m_oResult.getBooleanResult()); }
    VALUE getInt() { return rho_cast<VALUE>(m_oResult.getIntegerResult()); }
    VALUE getDouble() { return rho_cast<VALUE>(m_oResult.getDoubleResult()); }
    VALUE getString()
    {
        VALUE res;
        RAWTRACEC("CRubyResultConvertor", "getString()");

        if(hasObjectClass())
        {
            rho::String str = rho_cast<rho::String>(m_oResult.getStringResult());
            RAWTRACEC1("CRubyResultConvertor", "getString(): %s - create object by id", str.c_str());
            res = getObjectWithId(str);
        } else
        {
            RAWTRACEC("CRubyResultConvertor", "getString(): create string");
            res = rho_cast<VALUE>(m_oResult.getStringResult());
        }

        return res;
    }

    VALUE getArray()
    {
        RAWTRACEC("CRubyResultConvertor", "getArray()");
        VALUE res;

        if (hasObjectClass())
        {
            CHoldRubyValue valArray(rho_ruby_create_array());
            HStringVector pIDs = rho_cast<HStringVector>(m_oResult.getListResult());

            for(HStringVector::element_type::size_type i = 0; i < pIDs->size(); ++i)
            {
                VALUE valObj = getObjectOrString((*pIDs)[i]);
                rho_ruby_add_to_array(valArray, valObj);
            }
            res = valArray;
        } else
        {
            res = rho_cast<VALUE>(m_oResult.getListResult());
        }
        return res;
    }

    VALUE getHash()
    {
        RAWTRACEC("CRubyResultConvertor", "getHash()");
        VALUE res;

        if (hasObjectClass())
        {
            CHoldRubyValue valHash(rho_ruby_createHash());
            HStringMap pHash = rho_cast<HStringMap>(m_oResult.getMapResult());

            for(HStringMap::element_type::const_iterator it = pHash->begin(); it != pHash->end(); ++it)
            {
                RAWTRACEC2("CRubyResultConvertor", "getHash(): %s->%s", it->first.c_str(), it->second.c_str());
                addHashToHash(valHash, it->first.c_str(), getObjectOrString(it->second));
            }
            res = valHash;
        } else
        {
            res = rho_cast<VALUE>(m_oResult.getMapResult());
        }

        return res;
    }

    VALUE getJSON()
    {
        return rho_cast<VALUE>(m_oResult.getJSONResult());
    }

    const char* getResultParamName() const
    {
        switch(m_oResult.getResultType())
        {
        case MethodResultJni::typeArgError:
            return "argError";
        case MethodResultJni::typeError:
            return "runtimeError";
        default:
            return m_oResult.getResultParamName().c_str();
        }
    }

    VALUE getError()
    {
        RAWTRACEC("CRubyResultConvertor", "getErrorMessage()");

        if(m_bForCallback)
        {
            return rho_ruby_create_string(m_oResult.getErrorMessage().c_str());
        } else
        {
            if(m_oResult.getResultType() == MethodResultJni::typeArgError)
                rho_ruby_raise_argerror(m_oResult.getErrorMessage().c_str());
            else if(m_oResult.getResultType() == MethodResultJni::typeError)
                rho_ruby_raise_runtime(m_oResult.getErrorMessage().c_str());
        }
        return rho_ruby_get_NIL();
    }
};

template<>
class CJSONResultConvertor<MethodResultJni>
{
    MethodResultJni& m_oResult;

    bool hasObjectClass()
    {
        return m_oResult.getObjectClassPath().length() > 0;
    }
    rho::String getObjectOrString(const rho::String& str)
    {
        rho::String res;
        if(hasObjectClass())
        {
            res = "{ \"__rhoID\": \"";
            res += str;
            res += "\", \"__rhoClass\": ";
            res += rho::json::CJSONEntry::quoteValue(m_oResult.getObjectClassPath());
            res += "}";
        } else
        {
            res += rho::json::CJSONEntry::quoteValue(str);
        }
        return res;
    }

public:
    CJSONResultConvertor(MethodResultJni& result) : m_oResult(result) {}

    bool isBool() { return m_oResult.getResultType() == MethodResultJni::typeBoolean; }
    bool isInt() { return m_oResult.getResultType() == MethodResultJni::typeInteger; }
    bool isDouble() { return m_oResult.getResultType() == MethodResultJni::typeDouble; }
    bool isJSON() { return false; }
    bool isString() { return m_oResult.getResultType() == MethodResultJni::typeString; }
    bool isArray() { return m_oResult.getResultType() == MethodResultJni::typeList; }
    bool isHash() { return m_oResult.getResultType() == MethodResultJni::typeMap; }
    bool isError() { return m_oResult.getResultType() == MethodResultJni::typeError || m_oResult.getResultType() == MethodResultJni::typeArgError; }
    bool isNil() { return m_oResult.getResultType() == MethodResultJni::typeNone; }

    rho::String getBool() { return m_oResult.getBooleanResult() ? "true" : "false"; }
    rho::String getInt()
    {
        return convertToStringA(m_oResult.getIntegerResult());
    }

    rho::String getDouble()
    {
        return convertToStringA(m_oResult.getDoubleResult());
    }
    rho::String getString()
    {
        return getObjectOrString(rho_cast<rho::String>(m_oResult.getStringResult()));
    }
    rho::String getArray()
    {
        return rho_cast<rho::String>(m_oResult.getJSONResult());
    }

    rho::String getHash()
    {
        return rho_cast<rho::String>(m_oResult.getJSONResult());
    }

    rho::String getJSON()
    {
        rho::String strRes = rho_cast<rho::String>(m_oResult.getJSONResult());
        if(jnienv()->ExceptionCheck())
        {
            m_oResult.setError(clearException(jnienv()).c_str());
            return "null";
        }
        return strRes;
    }

    const char* getResultParamName() const
    {
        switch(m_oResult.getResultType())
        {
        case MethodResultJni::typeArgError:
            return "argError";
        case MethodResultJni::typeError:
            return "runtimeError";
        default:
            return m_oResult.getResultParamName().c_str();
        }
    }

    rho::String getError()
    {
        rho::String resHash = "{\"code\":";
        if(m_oResult.getResultType() == MethodResultJni::typeArgError)
            resHash += "-32602,\"message\":";
        else if(m_oResult.getResultType() == MethodResultJni::typeError)
            resHash += "-32603,\"message\":";

        resHash += rho::json::CJSONEntry::quoteValue(m_oResult.getErrorMessage());
        resHash += "}";
        return resHash;
    }
};

}}


