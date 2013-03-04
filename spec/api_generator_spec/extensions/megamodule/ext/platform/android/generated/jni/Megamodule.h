#pragma once

#include "common/RhoStd.h"
#include "logging/RhoLog.h"
#include "rhodes/JNIRhodes.h"
#include "MethodExecutorJni.h"

namespace rhoelements {
  class MethodResultJni;
}


namespace rho {
namespace examples {

using rhoelements::MethodResultJni;
using rhoelements::MethodExecutorJni;

class CMegamoduleBase : public MethodExecutorJni
{
protected:
    DEFINE_LOGCLASS;

    static const char* const FACTORY_SINGLETON_CLASS;
    static jclass s_clsFactorySingleton;
    static jmethodID s_midFactorySetInstance;
    static jmethodID s_midFactoryGetInstance;

    //IMegamoduleFactory staff
    static const char* const IFACTORY_CLASS;
    static jclass s_clsIFactory;
    static jmethodID s_midGetApiSingleton;
    static jmethodID s_midGetApiObject;

    //MegamoduleSingletonBase staff
    static const char* const SINGLETON_BASE_CLASS;
    static jclass s_clsSingletonBase;

    //MegamoduleBase staff
    static const char* const OBJECT_BASE_CLASS;
    static jclass s_clsObjectBase;

    //Method tasks

    static const char* const GETSTRINGPROPERTY_TASK_CLASS;
    static jclass s_clsgetStringPropertyTask;
    static jmethodID s_midgetStringPropertyTask;
    static const char* const GETINTEGERPROPERTY_TASK_CLASS;
    static jclass s_clsgetIntegerPropertyTask;
    static jmethodID s_midgetIntegerPropertyTask;
    static const char* const SETINTEGERPROPERTY_TASK_CLASS;
    static jclass s_clssetIntegerPropertyTask;
    static jmethodID s_midsetIntegerPropertyTask;
    static const char* const TYPESTEST_TASK_CLASS;
    static jclass s_clstypesTestTask;
    static jmethodID s_midtypesTestTask;
    static const char* const PRODUCEARRAY_TASK_CLASS;
    static jclass s_clsproduceArrayTask;
    static jmethodID s_midproduceArrayTask;
    static const char* const PRODUCEHASH_TASK_CLASS;
    static jclass s_clsproduceHashTask;
    static jmethodID s_midproduceHashTask;
    static const char* const PRODUCECOMPLICATEDRESULT_TASK_CLASS;
    static jclass s_clsproduceComplicatedResultTask;
    static jmethodID s_midproduceComplicatedResultTask;
    static const char* const GETOBJECTSCOUNT_TASK_CLASS;
    static jclass s_clsgetObjectsCountTask;
    static jmethodID s_midgetObjectsCountTask;
    static const char* const GETOBJECTBYINDEX_TASK_CLASS;
    static jclass s_clsgetObjectByIndexTask;
    static jmethodID s_midgetObjectByIndexTask;
    static const char* const SHOWALERTFROMUITHREAD_TASK_CLASS;
    static jclass s_clsshowAlertFromUIThreadTask;
    static jmethodID s_midshowAlertFromUIThreadTask;
    static const char* const SETPERIODICALLYCALLBACK_TASK_CLASS;
    static jclass s_clssetPeriodicallyCallbackTask;
    static jmethodID s_midsetPeriodicallyCallbackTask;
    static const char* const ISPERIODICALLYCALLBACKSETTED_TASK_CLASS;
    static jclass s_clsisPeriodicallyCallbackSettedTask;
    static jmethodID s_midisPeriodicallyCallbackSettedTask;
    static const char* const STOPPERIODICALLYCALLBACK_TASK_CLASS;
    static jclass s_clsstopPeriodicallyCallbackTask;
    static jmethodID s_midstopPeriodicallyCallbackTask;
    static const char* const COMPLICATEDTYPESTEST1_TASK_CLASS;
    static jclass s_clscomplicatedTypesTest1Task;
    static jmethodID s_midcomplicatedTypesTest1Task;
    static const char* const GETPROPERTY_TASK_CLASS;
    static jclass s_clsgetPropertyTask;
    static jmethodID s_midgetPropertyTask;
    static const char* const GETPROPERTIES_TASK_CLASS;
    static jclass s_clsgetPropertiesTask;
    static jmethodID s_midgetPropertiesTask;
    static const char* const GETALLPROPERTIES_TASK_CLASS;
    static jclass s_clsgetAllPropertiesTask;
    static jmethodID s_midgetAllPropertiesTask;
    static const char* const SETPROPERTY_TASK_CLASS;
    static jclass s_clssetPropertyTask;
    static jmethodID s_midsetPropertyTask;
    static const char* const SETPROPERTIES_TASK_CLASS;
    static jclass s_clssetPropertiesTask;
    static jmethodID s_midsetPropertiesTask;
    static const char* const CLEARALLPROPERTIES_TASK_CLASS;
    static jclass s_clsclearAllPropertiesTask;
    static jmethodID s_midclearAllPropertiesTask;
    static const char* const ENUMERATE_TASK_CLASS;
    static jclass s_clsenumerateTask;
    static jmethodID s_midenumerateTask;

    //IRhoApiDefaultId staff
    static const char* const IDEFAULTID_CLASS;
    static jclass s_clsIDefaultId;
    static jmethodID s_midGetDefaultID;
    static jmethodID s_midSetDefaultID;

    static JNIEnv* jniInit(JNIEnv* env);
    static JNIEnv* jniInit();

    static jobject getFactory(JNIEnv* env);
    static jobject getSingleton(JNIEnv* env);

    rho::String m_id;
    jobject getObject(JNIEnv* env);
public:
    static void setJavaFactory(JNIEnv* env, jobject jFactory);

    static rho::String getDefaultID();
    static void setDefaultID(const rho::String& id); 

    CMegamoduleBase(const rho::String& id)
        : MethodExecutorJni(), m_id(id)
        {}
    virtual ~CMegamoduleBase() {}
};

template <typename T>
class CMegamoduleProxy : public CMegamoduleBase
{
public:
    CMegamoduleProxy(const rho::String& id)
        : CMegamoduleBase(id)
        {}
    virtual ~CMegamoduleProxy() {}

    void getStringProperty(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "getStringProperty";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 2);

        jhobject jhObject = 
            getObject(env); 
        jhobject jhTask = env->NewObject(s_clsgetStringPropertyTask, s_midgetStringPropertyTask,
                    jhObject.get(), 
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }

    void getIntegerProperty(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "getIntegerProperty";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 2);

        jhobject jhObject = 
            getObject(env); 
        jhobject jhTask = env->NewObject(s_clsgetIntegerPropertyTask, s_midgetIntegerPropertyTask,
                    jhObject.get(), 
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }

    void setIntegerProperty(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "setIntegerProperty";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 3);

        jhobject jhObject = 
            getObject(env); 

        jholder< jint > jhvalue = (argsAdapter.size() <= 0) ?
            static_cast< jint >(0) :
                rho_cast< jint >(env, argsAdapter[0]);
        jhobject jhTask = env->NewObject(s_clssetIntegerPropertyTask, s_midsetIntegerPropertyTask,
                    jhObject.get(), 
                    jhvalue.get(),
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }

    void typesTest(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "typesTest";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 8);

        jhobject jhObject = 
            getObject(env); 

        jholder< jstring > jhparamStr = (argsAdapter.size() <= 0) ?
            static_cast< jstring >(0) :
                rho_cast< jstring >(env, argsAdapter[0]);

        jholder< jboolean > jhparamBool = (argsAdapter.size() <= 1) ?
            static_cast< jboolean >(0) :
                rho_cast< jboolean >(env, argsAdapter[1]);

        jholder< jint > jhparamInt = (argsAdapter.size() <= 2) ?
            static_cast< jint >(0) :
                rho_cast< jint >(env, argsAdapter[2]);

        jholder< jdouble > jhparamFloat = (argsAdapter.size() <= 3) ?
            static_cast< jdouble >(0) :
                rho_cast< jdouble >(env, argsAdapter[3]);

        jholder< jobject > jhparamArray = (argsAdapter.size() <= 4) ?
            static_cast< jobject >(0) :
                rho_cast< jobject >(env, argsAdapter[4]);

        jholder< jobject > jhparamHash = (argsAdapter.size() <= 5) ?
            static_cast< jobject >(0) :
                rho_cast< jobject >(env, argsAdapter[5]);
        jhobject jhTask = env->NewObject(s_clstypesTestTask, s_midtypesTestTask,
                    jhObject.get(), 
                    jhparamStr.get(),
                    jhparamBool.get(),
                    jhparamInt.get(),
                    jhparamFloat.get(),
                    jhparamArray.get(),
                    jhparamHash.get(),
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }

    void produceArray(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "produceArray";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 2);

        jhobject jhObject = 
            getObject(env); 
        jhobject jhTask = env->NewObject(s_clsproduceArrayTask, s_midproduceArrayTask,
                    jhObject.get(), 
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }

    void produceHash(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "produceHash";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 2);

        jhobject jhObject = 
            getObject(env); 
        jhobject jhTask = env->NewObject(s_clsproduceHashTask, s_midproduceHashTask,
                    jhObject.get(), 
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }

    void produceComplicatedResult(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "produceComplicatedResult";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 2);

        jhobject jhObject = 
            getObject(env); 
        jhobject jhTask = env->NewObject(s_clsproduceComplicatedResultTask, s_midproduceComplicatedResultTask,
                    jhObject.get(), 
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }

    static
    void getObjectsCount(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "getObjectsCount";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 2);

        jhobject jhObject = 
            getSingleton(env); 
        jhobject jhTask = env->NewObject(s_clsgetObjectsCountTask, s_midgetObjectsCountTask,
                    jhObject.get(), 
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }

    static
    void getObjectByIndex(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "getObjectByIndex";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 3);

        jhobject jhObject = 
            getSingleton(env); 

        jholder< jint > jhindex = (argsAdapter.size() <= 0) ?
            static_cast< jint >(0) :
                rho_cast< jint >(env, argsAdapter[0]);
        jhobject jhTask = env->NewObject(s_clsgetObjectByIndexTask, s_midgetObjectByIndexTask,
                    jhObject.get(), 
                    jhindex.get(),
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }

    void showAlertFromUIThread(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "showAlertFromUIThread";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 3);

        jhobject jhObject = 
            getObject(env); 

        jholder< jstring > jhmessage = (argsAdapter.size() <= 0) ?
            static_cast< jstring >(0) :
                rho_cast< jstring >(env, argsAdapter[0]);
        jhobject jhTask = env->NewObject(s_clsshowAlertFromUIThreadTask, s_midshowAlertFromUIThreadTask,
                    jhObject.get(), 
                    jhmessage.get(),
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, false);
    }

    void setPeriodicallyCallback(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "setPeriodicallyCallback";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 3);

        jhobject jhObject = 
            getObject(env); 

        jholder< jint > jhperiodInMilliseconds = (argsAdapter.size() <= 0) ?
            static_cast< jint >(0) :
                rho_cast< jint >(env, argsAdapter[0]);
        jhobject jhTask = env->NewObject(s_clssetPeriodicallyCallbackTask, s_midsetPeriodicallyCallbackTask,
                    jhObject.get(), 
                    jhperiodInMilliseconds.get(),
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }

    void isPeriodicallyCallbackSetted(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "isPeriodicallyCallbackSetted";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 2);

        jhobject jhObject = 
            getObject(env); 
        jhobject jhTask = env->NewObject(s_clsisPeriodicallyCallbackSettedTask, s_midisPeriodicallyCallbackSettedTask,
                    jhObject.get(), 
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }

    void stopPeriodicallyCallback(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "stopPeriodicallyCallback";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 2);

        jhobject jhObject = 
            getObject(env); 
        jhobject jhTask = env->NewObject(s_clsstopPeriodicallyCallbackTask, s_midstopPeriodicallyCallbackTask,
                    jhObject.get(), 
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }

    void complicatedTypesTest1(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "complicatedTypesTest1";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 3);

        jhobject jhObject = 
            getObject(env); 

        jholder< jobject > jhparamArray = (argsAdapter.size() <= 0) ?
            static_cast< jobject >(0) :
                rho_cast< jobject >(env, argsAdapter[0]);
        jhobject jhTask = env->NewObject(s_clscomplicatedTypesTest1Task, s_midcomplicatedTypesTest1Task,
                    jhObject.get(), 
                    jhparamArray.get(),
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }

    void getProperty(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "getProperty";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 3);

        jhobject jhObject = 
            getObject(env); 

        jholder< jstring > jhpropertyName = (argsAdapter.size() <= 0) ?
            static_cast< jstring >(0) :
                rho_cast< jstring >(env, argsAdapter[0]);
        jhobject jhTask = env->NewObject(s_clsgetPropertyTask, s_midgetPropertyTask,
                    jhObject.get(), 
                    jhpropertyName.get(),
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }

    void getProperties(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "getProperties";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 3);

        jhobject jhObject = 
            getObject(env); 

        jholder< jobject > jharrayofNames = (argsAdapter.size() <= 0) ?
            static_cast< jobject >(0) :
                rho_cast< jobject >(env, argsAdapter[0]);
        jhobject jhTask = env->NewObject(s_clsgetPropertiesTask, s_midgetPropertiesTask,
                    jhObject.get(), 
                    jharrayofNames.get(),
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }

    void getAllProperties(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "getAllProperties";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 2);

        jhobject jhObject = 
            getObject(env); 
        jhobject jhTask = env->NewObject(s_clsgetAllPropertiesTask, s_midgetAllPropertiesTask,
                    jhObject.get(), 
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }

    void setProperty(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "setProperty";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 4);

        jhobject jhObject = 
            getObject(env); 

        jholder< jstring > jhpropertyName = (argsAdapter.size() <= 0) ?
            static_cast< jstring >(0) :
                rho_cast< jstring >(env, argsAdapter[0]);

        jholder< jstring > jhpropertyValue = (argsAdapter.size() <= 1) ?
            static_cast< jstring >(0) :
                rho_cast< jstring >(env, argsAdapter[1]);
        jhobject jhTask = env->NewObject(s_clssetPropertyTask, s_midsetPropertyTask,
                    jhObject.get(), 
                    jhpropertyName.get(),
                    jhpropertyValue.get(),
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }

    void setProperties(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "setProperties";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 3);

        jhobject jhObject = 
            getObject(env); 

        jholder< jobject > jhpropertyMap = (argsAdapter.size() <= 0) ?
            static_cast< jobject >(0) :
                rho_cast< jobject >(env, argsAdapter[0]);
        jhobject jhTask = env->NewObject(s_clssetPropertiesTask, s_midsetPropertiesTask,
                    jhObject.get(), 
                    jhpropertyMap.get(),
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }

    void clearAllProperties(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "clearAllProperties";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 2);

        jhobject jhObject = 
            getObject(env); 
        jhobject jhTask = env->NewObject(s_clsclearAllPropertiesTask, s_midclearAllPropertiesTask,
                    jhObject.get(), 
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }

    static
    void enumerate(const T& argsAdapter, MethodResultJni& result)
    {
        LOG(TRACE) + "enumerate";

        JNIEnv *env = jniInit();
        if (!env) {
            LOG(FATAL) + "JNI initialization failed";
            return;
        }

        RHO_ASSERT(argsAdapter.size() <= 2);

        jhobject jhObject = 
            getSingleton(env); 
        jhobject jhTask = env->NewObject(s_clsenumerateTask, s_midenumerateTask,
                    jhObject.get(), 
                    static_cast<jobject>(result));

        run(env, jhTask.get(), result, true, true);
    }


};


}
}
