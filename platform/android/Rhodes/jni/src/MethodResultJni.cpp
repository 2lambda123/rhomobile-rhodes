#include "common/RhoStd.h"
#include "common/StringConverter.h"
#include "rhodes/JNIRhodes.h"
#include "rhodes/JNIRhoRuby.h"
#include "rhodes/jni/com_rhomobile_rhodes_api_MethodResult.h"
#include "api_generator/MethodResult.h"
#include "MethodResultJni.h"
#include "ruby/ext/rho/rhoruby.h"

#include "logging/RhoLog.h"

#undef DEFAULT_LOGCATEGORY
#define DEFAULT_LOGCATEGORY "MethodResultJNI"

namespace rho {
namespace apiGenerator {

const char * const MethodResultJni::METHOD_RESULT_CLASS = "com.rhomobile.rhodes.api.MethodResult";

jclass MethodResultJni::s_methodResultClass = 0;
jmethodID MethodResultJni::s_midMethodResult;
jmethodID MethodResultJni::s_midGetJson;
jmethodID MethodResultJni::s_midGetResultType;
jmethodID MethodResultJni::s_midReset;

jfieldID MethodResultJni::s_fidBoolean;
jfieldID MethodResultJni::s_fidInteger;
jfieldID MethodResultJni::s_fidDouble;
jfieldID MethodResultJni::s_fidString;
jfieldID MethodResultJni::s_fidList;
jfieldID MethodResultJni::s_fidMap;

jfieldID MethodResultJni::s_fidStrCallback;
jfieldID MethodResultJni::s_fidStrCallbackData;
jfieldID MethodResultJni::s_fidRubyProcCallback;
jfieldID MethodResultJni::s_fidResultParamName;
jfieldID MethodResultJni::s_fidObjectClassPath;
jfieldID MethodResultJni::s_fidRubyObjectClass;
//----------------------------------------------------------------------------------------------------------------------

JNIEnv* MethodResultJni::jniInit(JNIEnv *env)
{
    RAWTRACE3("%s - env: 0x%.8x, functions: 0x%.8x", __FUNCTION__, env, env->functions);
    if(env && !s_methodResultClass)
    {
        String exceptionMessage;
        s_methodResultClass = getJNIClass(RHODES_JAVA_CLASS_METHODRESULT);
        if(!s_methodResultClass)
        {
            RAWLOG_ERROR1("Failed to find java class: %s", METHOD_RESULT_CLASS);
            s_methodResultClass = 0;
            return NULL;
        }
        RAWTRACE("<init>");

        s_midMethodResult = env->GetMethodID(s_methodResultClass, "<init>", "(Z)V");
        if(env->ExceptionCheck() == JNI_TRUE)
        {
            RAWLOG_ERROR2("Failed to get %s constructor: $s", METHOD_RESULT_CLASS, clearException(env).c_str());
            s_methodResultClass = 0;
            return NULL;
        }
        RAWTRACE("getJson");
        s_midGetJson = env->GetMethodID(s_methodResultClass, "getJson", "()Ljava/lang/String;");
        if(env->ExceptionCheck() == JNI_TRUE)
        {
            RAWLOG_ERROR2("Failed to get method %s.getJson: %s", METHOD_RESULT_CLASS, clearException(env).c_str());
            s_methodResultClass = 0;
            return NULL;
        }
        RAWTRACE("getResultType");
        s_midGetResultType = env->GetMethodID(s_methodResultClass, "getResultType", "()I");
        if(env->ExceptionCheck() == JNI_TRUE)
        {
            RAWLOG_ERROR2("Failed to get method %s.getResultType: %s", METHOD_RESULT_CLASS, clearException(env).c_str());
            s_methodResultClass = 0;
            return NULL;
        }
        RAWTRACE("reset");
        s_midReset = env->GetMethodID(s_methodResultClass, "reset", "()V");
        if(env->ExceptionCheck() == JNI_TRUE)
        {
            RAWLOG_ERROR2("Failed to get method %s.reset: %s", METHOD_RESULT_CLASS, clearException(env).c_str());
            s_methodResultClass = 0;
            return NULL;
        }
        RAWTRACE("mBooleanResult");
        s_fidBoolean = env->GetFieldID(s_methodResultClass, "mBooleanResult", "Z");
        if(env->ExceptionCheck() == JNI_TRUE)
        {
            RAWLOG_ERROR2("Failed to get field %s.mBooleanResult: %s", METHOD_RESULT_CLASS, clearException(env).c_str());
            s_methodResultClass = 0;
            return NULL;
        }
        RAWTRACE("mIntegerResult");
        s_fidInteger = env->GetFieldID(s_methodResultClass, "mIntegerResult", "I");
        if(env->ExceptionCheck() == JNI_TRUE)
        {
            RAWLOG_ERROR2("Failed to get field %s.mIntegerResult: %s", METHOD_RESULT_CLASS, clearException(env).c_str());
            s_methodResultClass = 0;
            return NULL;
        }
        RAWTRACE("mDoubleResult");
        s_fidDouble = env->GetFieldID(s_methodResultClass, "mDoubleResult", "D");
        if(env->ExceptionCheck() == JNI_TRUE)
        {
            RAWLOG_ERROR2("Failed to get fiekd %s.mDoubleResult: %s", METHOD_RESULT_CLASS, clearException(env).c_str());
            s_methodResultClass = 0;
            return NULL;
        }
        RAWTRACE("mStrResult");
        s_fidString = env->GetFieldID(s_methodResultClass, "mStrResult", "Ljava/lang/String;");
        if(env->ExceptionCheck() == JNI_TRUE)
        {
            RAWLOG_ERROR2("Failed to get field %s.mStrResult: %s", METHOD_RESULT_CLASS, clearException(env).c_str());
            s_methodResultClass = 0;
            return NULL;
        }
        RAWTRACE("mListResult");
        s_fidList = env->GetFieldID(s_methodResultClass, "mListResult", "Ljava/util/List;");
        if(env->ExceptionCheck() == JNI_TRUE)
        {
            RAWLOG_ERROR2("Failed to get field %s.mListResult: %s", METHOD_RESULT_CLASS, clearException(env).c_str());
            s_methodResultClass = 0;
            return NULL;
        }
        RAWTRACE("mMapResult");
        s_fidMap = env->GetFieldID(s_methodResultClass, "mMapResult", "Ljava/util/Map;");
        if(env->ExceptionCheck() == JNI_TRUE)
        {
            RAWLOG_ERROR2("Failed to get field %s.mMapResult: %s", METHOD_RESULT_CLASS, clearException(env).c_str());
            s_methodResultClass = 0;
            return NULL;
        }
        RAWTRACE("mStrCallback");
        s_fidStrCallback = env->GetFieldID(s_methodResultClass, "mStrCallback", "Ljava/lang/String;");
        if(env->ExceptionCheck() == JNI_TRUE)
        {
            RAWLOG_ERROR2("Failed to get field %s.mStrCallback: %s", METHOD_RESULT_CLASS, clearException(env).c_str());
            s_methodResultClass = 0;
            return NULL;
        }
        RAWTRACE("mStrCallbackData");
        s_fidStrCallbackData = env->GetFieldID(s_methodResultClass, "mStrCallbackData", "Ljava/lang/String;");
        if(env->ExceptionCheck() == JNI_TRUE)
        {
            RAWLOG_ERROR2("Failed to get %s.mStrCallbackData field: %s", METHOD_RESULT_CLASS, clearException(env).c_str());
            s_methodResultClass = 0;
            return NULL;
        }
        RAWTRACE("mResultParamName");
        s_fidResultParamName = env->GetFieldID(s_methodResultClass, "mResultParamName", "Ljava/lang/String;");
        if(env->ExceptionCheck() == JNI_TRUE)
        {
            RAWLOG_ERROR2("Failed to get %s.mResultParamName field: %s", METHOD_RESULT_CLASS, clearException(env).c_str());
            s_methodResultClass = 0;
            return NULL;
        }
        RAWTRACE("mRubyProcCallback");
        s_fidRubyProcCallback = env->GetFieldID(s_methodResultClass, "mRubyProcCallback", "J");
        if(env->ExceptionCheck() == JNI_TRUE)
        {
            RAWLOG_ERROR2("Failed to get field  %s.mRubyProcCallback: %s", METHOD_RESULT_CLASS, clearException(env).c_str());
            s_methodResultClass = 0;
            return NULL;
        }
        RAWTRACE("mObjectClassPath");
        s_fidObjectClassPath = env->GetFieldID(s_methodResultClass, "mObjectClassPath", "Ljava/lang/String;");
        if(env->ExceptionCheck() == JNI_TRUE)
        {
            RAWLOG_ERROR2("Failed to get field %s.mObjectClassPath: %s", METHOD_RESULT_CLASS, clearException(env).c_str());
            s_methodResultClass = 0;
            return NULL;
        }
        RAWTRACE("mRubyObjectClass");
        s_fidRubyObjectClass = env->GetFieldID(s_methodResultClass, "mRubyObjectClass", "J");
        if(env->ExceptionCheck() == JNI_TRUE)
        {
            RAWLOG_ERROR2("Failed to get field %s.mRubyObjectClass: %s", METHOD_RESULT_CLASS, clearException(env).c_str());
            s_methodResultClass = 0;
            return NULL;
        }
        RAWTRACE(__FUNCTION__);

    }
    return env;
}
//----------------------------------------------------------------------------------------------------------------------

jboolean MethodResultJni::getBooleanResult(JNIEnv* env) const
{
    RAWTRACE(__FUNCTION__);
    return env->GetBooleanField(m_jhResult.get(), s_fidBoolean);
}
//----------------------------------------------------------------------------------------------------------------------

jint MethodResultJni::getIntegerResult(JNIEnv* env) const
{
    RAWTRACE(__FUNCTION__);
    return env->GetIntField(m_jhResult.get(), s_fidInteger);
}
//----------------------------------------------------------------------------------------------------------------------

jdouble MethodResultJni::getDoubleResult(JNIEnv* env) const
{
    RAWTRACE(__FUNCTION__);
    return env->GetDoubleField(m_jhResult.get(), s_fidDouble);
}
//----------------------------------------------------------------------------------------------------------------------

jstring MethodResultJni::getStringResult(JNIEnv* env) const
{
    RAWTRACE(__FUNCTION__);
    return static_cast<jstring>(env->GetObjectField(m_jhResult.get(), s_fidString));
}
//----------------------------------------------------------------------------------------------------------------------

jobject MethodResultJni::getListResult(JNIEnv* env) const
{
    RAWTRACE(__FUNCTION__);
    return env->GetObjectField(m_jhResult.get(), s_fidList);
}
//----------------------------------------------------------------------------------------------------------------------

jobject MethodResultJni::getMapResult(JNIEnv* env) const
{
    RAWTRACE(__FUNCTION__);
    return env->GetObjectField(m_jhResult.get(), s_fidMap);
}
//----------------------------------------------------------------------------------------------------------------------

jstring MethodResultJni::getJSONResult(JNIEnv* env) const
{
    RAWTRACE(__FUNCTION__);
    return static_cast<jstring>(env->CallObjectMethod(m_jhResult.get(), s_midGetJson));
}
//----------------------------------------------------------------------------------------------------------------------

int MethodResultJni::getResultType(JNIEnv* env) const
{
    RAWTRACE(__FUNCTION__);
    if(m_resType == typeNone && m_jhResult)
    {
        int res = env->CallIntMethod(m_jhResult.get(), s_midGetResultType);
        RAWTRACE1("Java result type: %d", res);
        return res;
    } else
    {
        RAWTRACE1("Native result type: %d", m_resType);
        return m_resType;
    }
}
//----------------------------------------------------------------------------------------------------------------------

rho::String MethodResultJni::getErrorMessage(JNIEnv* env) const
{
    RAWTRACE(__FUNCTION__);
    if(m_resType == typeNone && m_jhResult)
    {
        jhstring jhMessage = getStringResult(env);
        return rho_cast<std::string>(env, jhMessage.get());
    } else
    {
        return m_errMsg;
    }
}
//----------------------------------------------------------------------------------------------------------------------

jstring MethodResultJni::getStrCallback(JNIEnv* env) const
{
    RAWTRACE(__FUNCTION__);
    return static_cast<jstring>(env->GetObjectField(m_jhResult.get(), s_fidStrCallback));
}
//----------------------------------------------------------------------------------------------------------------------

jstring MethodResultJni::getStrCallbackData(JNIEnv* env) const
{
    RAWTRACE(__FUNCTION__);
    return static_cast<jstring>(env->GetObjectField(m_jhResult.get(), s_fidStrCallbackData));
}
//----------------------------------------------------------------------------------------------------------------------

jlong MethodResultJni::getRubyProcCallback(JNIEnv* env) const
{
    RAWTRACE(__FUNCTION__);
    return env->GetLongField(m_jhResult.get(), s_fidRubyProcCallback);
}
//----------------------------------------------------------------------------------------------------------------------

jlong MethodResultJni::getRubyObjectClass(JNIEnv* env) const
{
    RAWTRACE(__FUNCTION__);
    return env->GetLongField(m_jhResult.get(), s_fidRubyObjectClass);
}
//----------------------------------------------------------------------------------------------------------------------

jstring MethodResultJni::getObjectClassPath(JNIEnv* env) const
{
    RAWTRACE(__FUNCTION__);
    return static_cast<jstring>(env->GetObjectField(m_jhResult.get(), s_fidObjectClassPath));
}
//----------------------------------------------------------------------------------------------------------------------

jstring MethodResultJni::getResultParamName(JNIEnv* env) const
{
    RAWTRACE(__FUNCTION__);
    return static_cast<jstring>(env->GetObjectField(m_jhResult.get(), s_fidResultParamName));
}
//----------------------------------------------------------------------------------------------------------------------

void MethodResultJni::reset(JNIEnv* env)
{
    m_resType = typeNone;
    env->CallVoidMethod(m_jhResult.get(), s_midReset);
}
//----------------------------------------------------------------------------------------------------------------------

MethodResultJni::MethodResultJni(bool isRuby) : m_env(0), m_jhResult(0), m_bGlobalRef(false), m_hasCallback(false), m_resType(typeNone)
{
    m_env = jniInit();
    if (!m_env) {
        RAWLOG_ERROR("ctor - JNI initialization failed");
        return;
    }

    m_jhResult = m_env->NewObject(s_methodResultClass, s_midMethodResult, static_cast<jboolean>(isRuby));
}
//----------------------------------------------------------------------------------------------------------------------

MethodResultJni::MethodResultJni(JNIEnv* env, jobject jResult) : m_jhResult(jResult), m_bGlobalRef(false), m_hasCallback(false), m_resType(typeNone)
{
    m_env = jniInit(env);
    if (!m_env) {
        RAWLOG_ERROR("ctor - JNI initialization failed");
        return;
    }
}
//----------------------------------------------------------------------------------------------------------------------

MethodResultJni::MethodResultJni(const MethodResultJni& result) : m_env(0), m_jhResult(0), m_bGlobalRef(false), m_hasCallback(false), m_resType(typeNone)
{
    JNIEnv* env = result.m_env;
    m_jhResult = env->NewWeakGlobalRef(result.m_jhResult.get());
    m_bGlobalRef = true;
}
//----------------------------------------------------------------------------------------------------------------------

MethodResultJni::~MethodResultJni()
{
    if(m_bGlobalRef)
    {
        m_jhResult.release();
    }
}
//----------------------------------------------------------------------------------------------------------------------

void MethodResultJni::setCallback(JNIEnv* env, jstring jUrl, jstring jData)
{
    RAWTRACE(__FUNCTION__);

    env->SetObjectField(m_jhResult.get(), s_fidStrCallback, jUrl);
    if(jData)
    {
        env->SetObjectField(m_jhResult.get(), s_fidStrCallbackData, jData);
    }
    m_hasCallback = true;

    RAWTRACE("Callback has been set ^^^");
}
//----------------------------------------------------------------------------------------------------------------------

void MethodResultJni::setRubyProcCallback(JNIEnv* env, jlong jRubyProc)
{
    RAWTRACE(__FUNCTION__);

    env->SetLongField(m_jhResult.get(), s_fidRubyProcCallback, jRubyProc);

    RAWTRACE("Callback has been set ^^^");
}
//----------------------------------------------------------------------------------------------------------------------

void MethodResultJni::setRubyObjectClass(JNIEnv* env, jlong jClass)
{
    RAWTRACE(__FUNCTION__);

    env->SetLongField(m_jhResult.get(), s_fidRubyObjectClass, jClass);

    RAWTRACE("Ruby object class has been set ^^^");
}
//----------------------------------------------------------------------------------------------------------------------

void MethodResultJni::setObjectClassPath(JNIEnv* env, jstring jClassPath)
{
    RAWTRACE(__FUNCTION__);

    env->SetObjectField(m_jhResult.get(), s_fidObjectClassPath, jClassPath);

    RAWTRACE("Object class path has been set ^^^");
}
//----------------------------------------------------------------------------------------------------------------------

void MethodResultJni::setResultParamName(JNIEnv* env, jstring jName)
{
    RAWTRACE(__FUNCTION__);

    env->SetObjectField(m_jhResult.get(), s_fidResultParamName, jName);

    RAWTRACE("Result param name has been set ^^^");
}
//----------------------------------------------------------------------------------------------------------------------

void MethodResultJni::disconnect(JNIEnv* env)
{
    if(m_bGlobalRef)
    {
        RAWTRACE("Disconnect from global JNI reference");
        jobject jObj = m_jhResult.release();
        env->DeleteGlobalRef(jObj);
    } else
    {
        RAWTRACE("Disconnect from local JNI reference");
        m_jhResult.deleteRef();
    }

}
//----------------------------------------------------------------------------------------------------------------------

void MethodResultJni::callRubyBack(bool releaseCallback)
{
    RAWTRACE(__FUNCTION__);

    rho::String strResBody = RHODESAPP().addCallbackObject( new CRubyCallbackResult<MethodResultJni>(*this), (jRubyProc != 0 ? "body" : "__rho_inline") );

    jhstring jhStrCallback = getStrCallback(m_env);
    jhstring jhStrCallbackData = getStrCallbackData(m_env);
    jlong jRubyProc = getRubyProcCallback(m_env);

    if(!m_env->IsSameObject(jhStrCallback.get(), 0))
    {
        RHODESAPP().callCallbackWithData(rho_cast<rho::String>(m_env, jhStrCallback.get()), strResBody, rho_cast<rho::String>(m_env, jhStrCallbackData.get()), true);
    }else if (jRubyProc != 0)
    {
        VALUE oProc = static_cast<VALUE>(jRubyProc);

        RHODESAPP().callCallbackProcWithData( oProc, strResBody, rho_cast<rho::String>(m_env, jhStrCallbackData.get()), true);

        if(releaseCallback)
        {
            releaseRubyProcCallback(oProc);
        }
    }

}
//----------------------------------------------------------------------------------------------------------------------

void MethodResultJni::callJSBack()
{
    RAWTRACE(__FUNCTION__);

    jhstring jhStrCallbackID = getStrCallback(m_env);
    String strCallbackID = rho_cast<String>(m_env, jhStrCallbackID.get());

    jhstring jhJsVmID = getStrCallbackData(m_env);
    String strJsVmID = rho_cast<String>(m_env, jhJsVmID.get());
    int tabIndex = -1;
    //common::convertFromStringA(strJsVmID.c_str(), tabIndex);

    String strRes(CMethodResultConvertor().toJSON(*this));

    String strCallback("Rho.callbackHandler( \"");
    strCallback += strCallbackID;
    strCallback += "\", ";
    strCallback += strRes;
    strCallback += ")";

    jclass cls = getJNIClass(RHODES_JAVA_CLASS_WEB_VIEW);
    if (!cls) return;
    static jmethodID mid = getJNIClassStaticMethod(m_env, cls, "executeJs", "(Ljava/lang/String;I)V");
    if (!mid) return;

    jhstring jhCallback = rho_cast<jstring>(m_env, strCallback);
    m_env->CallStaticVoidMethod(cls, mid, jhCallback.get(), static_cast<jint>(tabIndex));
}
//----------------------------------------------------------------------------------------------------------------------

void MethodResultJni::releaseRubyProcCallback(unsigned long rubyProc)
{
    RAWTRACE(__FUNCTION__);
    rho_ruby_releaseValue(rubyProc);
}
//----------------------------------------------------------------------------------------------------------------------

RHO_GLOBAL void JNICALL Java_com_rhomobile_rhodes_api_MethodResult_nativeCallBack
  (JNIEnv * env, jobject jResult, jboolean jIsRuby, jboolean jReleaseCallback)
{
    RAWTRACE("nativeCallBack");

    MethodResultJni result(env, jResult);
    if (static_cast<bool>(jIsRuby))
    {
        result.callRubyBack(static_cast<bool>(jReleaseCallback));
    } else
    {
        result.callJSBack();
    }
}
//----------------------------------------------------------------------------------------------------------------------

RHO_GLOBAL void JNICALL Java_com_rhomobile_rhodes_api_MethodResult_nativeReleaseRubyProcCallback
  (JNIEnv * env, jclass , jlong jProcCallback)
{
    RAWTRACE("nativeReleaseRubyProcCallback");

    MethodResultJni::releaseRubyProcCallback(static_cast<unsigned long>(jProcCallback));
}
//----------------------------------------------------------------------------------------------------------------------



}}
