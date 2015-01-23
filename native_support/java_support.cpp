#include <jni.h>
//#include "/usr/lib/jvm/java-7-openjdk-amd64/include/jni.h"
#include <android/log.h>
#include "../../core/ustring.h"
#include "../../core/variant.h"
#include "../../core/reference.h"
#include "native_class.h"

#define TAG "gen_debug" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__) // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__) // 定义LOGF类型

JavaVM *gJavaVM;
const char *JavaSupportClass = "com/android/godot/JavaSupport";
const char *StringClass = "java/lang/String";
NativeObject *_now_init = NULL;
jclass SupportClass;

Variant value_from_jobject(String s_type, jobject obj, JNIEnv *env) {
    if (s_type == "Z") {
        jmethodID method = env->GetMethodID(env->GetObjectClass(obj), "booleanValue", "()Z");
        return Variant(env->CallBooleanMethod(obj, method));
    }else if (s_type == "I") {
        jmethodID method = env->GetMethodID(env->GetObjectClass(obj), "intValue", "()I");
        return Variant(env->CallIntMethod(obj, method));
    }else if (s_type == "B") {
        jmethodID method = env->GetMethodID(env->GetObjectClass(obj), "byteValue", "()B");
        return Variant(env->CallByteMethod(obj, method));
    }else if (s_type == "C") {
        jmethodID method = env->GetMethodID(env->GetObjectClass(obj), "charValue", "()C");
        return Variant(env->CallCharMethod(obj, method));
    }else if (s_type == "D") {
        jmethodID method = env->GetMethodID(env->GetObjectClass(obj), "doubleValue", "()D");
        return Variant(env->CallDoubleMethod(obj, method));
    }else if (s_type == "F") {
        jmethodID method = env->GetMethodID(env->GetObjectClass(obj), "floatValue", "()F");
        return Variant(env->CallFloatMethod(obj, method));
    }else if (s_type == "L") {
        jmethodID method = env->GetMethodID(env->GetObjectClass(obj), "longValue", "()L");
        return Variant(env->CallLongMethod(obj, method));
    }else if (s_type[0] == 'L') {
        if (s_type == "Ljava/lang/String;") {
            jstring j_str = (jstring)obj;

            const char *chs = env->GetStringUTFChars(j_str, NULL);
            String s_str(chs);
            env->ReleaseStringUTFChars(j_str, chs);

            return Variant(s_str);
        }else {
            Ref<NativeObject> res_obj = memnew(NativeObject);
            res_obj->native = obj;
            return Variant(res_obj);
        }
    }else {
        return Variant();
    }
}

extern "C" {
JNIEXPORT void JNICALL Java_com_android_godot_JavaSupport_cEmitSignal(JNIEnv *env, jclass cls, jobject sender, jstring signal, jobjectArray signatures, jobjectArray params) {
    int length = env->GetArrayLength(params);
    const char *chs = env->GetStringUTFChars(signal, NULL);
    String str_signal(chs);
    env->ReleaseStringUTFChars(signal, chs);
    bool found = false;
    for (int n = 0; n < NativeObject::_cached_count; n++) {
        NativeObject *object = NativeObject::_cached_objects[n];
        if (object->native && env->IsSameObject(sender, (jobject)object->native)) {
            found = true;
            Variant vars[5];
            for (int n = 0; n < 5 && n < length; n++) {
                jstring j_type = (jstring)env->GetObjectArrayElement(signatures, n);
                const char *c_type = env->GetStringUTFChars(j_type, NULL);
                String s_type(c_type);
                env->ReleaseStringUTFChars(j_type, c_type);

                vars[n] = value_from_jobject(s_type, env->GetObjectArrayElement(params, n), env);
            }
            object->emit_signal(StringName(str_signal), vars[0], vars[1], vars[2], vars[3], vars[4]);
        }
    }
    if (!found) {
        if (_now_init) {
            Variant vars[5];
            for (int n = 0; n < 5 && n < length; n++) {
                jstring j_type = (jstring)env->GetObjectArrayElement(signatures, n);
                const char *c_type = env->GetStringUTFChars(j_type, NULL);
                String s_type(c_type);
                env->ReleaseStringUTFChars(j_type, c_type);

                vars[n] = value_from_jobject(s_type, env->GetObjectArrayElement(params, n), env);
            }
            _now_init->emit_signal(StringName(str_signal), vars[0], vars[1], vars[2], vars[3], vars[4]);
        }
    }
}
JNIEXPORT void JNICALL Java_com_android_godot_JavaSupport_cAddUserSignal(JNIEnv *env, jclass cls, jobject sender, jstring signal, jobjectArray params) {
    int length = env->GetArrayLength(params);
    const char *chs = env->GetStringUTFChars(signal, NULL);
    String str_signal(chs);
    env->ReleaseStringUTFChars(signal, chs);
    bool found = false;
    for (int n = 0; n < NativeObject::_cached_count; n++) {
        NativeObject *object = NativeObject::_cached_objects[n];
        if (object->native && env->IsSameObject(sender, (jobject)object->native)) {
            found = true;
            MethodInfo methodInfo(str_signal);
            for (int n = 0; n < length; n++) {
                jstring j_str = (jstring)env->GetObjectArrayElement(params, n);
                const char *chs_ = env->GetStringUTFChars(j_str, NULL);
                String str_param(chs_);
                env->ReleaseStringUTFChars(j_str, chs_);
                PropertyInfo propertyInfo;
                propertyInfo.name = str_param;
                methodInfo.arguments.push_back(propertyInfo);
            }
            object->add_user_signal(methodInfo);
        }
    }
    if (!found) {
        if (_now_init) {
            MethodInfo methodInfo(str_signal);
            for (int n = 0; n < length; n++) {
                jstring j_str = (jstring)env->GetObjectArrayElement(params, n);
                const char *chs_ = env->GetStringUTFChars(j_str, NULL);
                String str_param(chs_);
                env->ReleaseStringUTFChars(j_str, chs_);
                PropertyInfo propertyInfo;
                propertyInfo.name = str_param;
                methodInfo.arguments.push_back(propertyInfo);
            }
            _now_init->add_user_signal(methodInfo);
        }
    }
}
}

JNIEnv *get_env(){
    if (gJavaVM) {
        int status;
        JNIEnv *env;

        status = gJavaVM->GetEnv((void **) &env, JNI_VERSION_1_4);
        if(status < 0) {
            status = gJavaVM->AttachCurrentThread(&env, NULL);
            if(status < 0) {
                return env;
            }
        }
        return env;
    }
    return NULL;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    gJavaVM = vm;
    JNIEnv* env = get_env();
    jclass clazz = env->FindClass(JavaSupportClass);

    SupportClass = (jclass)env->NewGlobalRef(clazz);
    return JNI_VERSION_1_4;
}

String get_package(jobject object) {
    JNIEnv *env = get_env();
    jmethodID objectSig = env->GetStaticMethodID(SupportClass, "objectSig", "(Ljava/lang/Object;)Ljava/lang/String;");
    jstring ret = (jstring)env->CallStaticObjectMethod(SupportClass, objectSig, object);
    const char* chs = (const char*)env->GetStringUTFChars(ret, NULL);
    String res = String(chs);
    env->ReleaseStringUTFChars(ret, chs);
    return res;
}

String constructor_sig(const String name, const Variant** p_args,int p_argcount) {

    JNIEnv *env = get_env();

    jclass cls = env->FindClass(JavaSupportClass);
    jmethodID constructorSig = env->GetStaticMethodID(cls, "constructorSig", "(Ljava/lang/String;[Ljava/lang/String;)Ljava/lang/String;");

    jstring j_name = env->NewStringUTF(name.utf8().get_data());
    jclass strClass = env->FindClass(StringClass);
    jobjectArray j_objArr = env->NewObjectArray(p_argcount, strClass, NULL);
    for (int n = 0; n < p_argcount; n++) {
        Variant::Type type = p_args[n]->get_type();
        String input;
        switch (type) {
            case Variant::BOOL:
                input = "Z";
                break;
            case Variant::INT:
                input = "I";
                break;
            case Variant::REAL:
                input = "F";
                break;
            case Variant::STRING:
                input = "java.lang.String";
                break;
            default: {
                const Object* obj = p_args[n]->operator Object*();
                if (obj->get_type() == "NativeObject") {
                    const NativeObject *native_object = obj->cast_to<NativeObject>();
                    jmethodID objectSig2 = env->GetStaticMethodID(SupportClass, "objectSig2", "(Ljava/lang/Object;)Ljava/lang/String;");
                    jstring sig = (jstring)env->CallStaticObjectMethod(SupportClass, objectSig2, (jobject)native_object->native);
                    const char *c_sig = (const char*)env->GetStringUTFChars(sig, NULL);
                    input = String(c_sig);
                    env->ReleaseStringUTFChars(sig, c_sig);
                }else {
                    continue;
                }
            }
        }
        env->SetObjectArrayElement(j_objArr, n, env->NewStringUTF(input.utf8().get_data()));
    }
    jstring j_ret = (jstring)env->CallStaticObjectMethod(cls, constructorSig, j_name, j_objArr);
    const char *c_ret = env->GetStringUTFChars(j_ret, NULL);
    String ret = String(c_ret);
    env->ReleaseStringUTFChars(j_ret, c_ret);
    return ret;
}

String method_sig(const jobject j_object, const bool is_static, const String method_name, const Variant** p_args,int p_argcount) {

    JNIEnv *env = get_env();
    jclass cls = env->FindClass(JavaSupportClass);
    jmethodID methodSig = env->GetStaticMethodID(cls, "methodSig", "(Ljava/lang/Object;ZLjava/lang/String;[Ljava/lang/String;)Ljava/lang/String;");

    jstring j_name = env->NewStringUTF(method_name.utf8().get_data());
    jclass strClass = env->FindClass(StringClass);
    jobjectArray j_objArr = env->NewObjectArray(p_argcount, strClass, NULL);
    for (int n = 0; n < p_argcount; n++) {
        Variant::Type type = p_args[n]->get_type();
        String input;
        switch (type) {
            case Variant::BOOL:
                input = "Z";
                break;
            case Variant::INT:
                input = "I";
                break;
            case Variant::REAL:
                input = "F";
                break;
            case Variant::STRING:
                input = "java.lang.String";
                break;
            default: {
                const Object* obj = p_args[n]->operator Object*();
                if (obj->get_type() == "NativeObject") {
                    const NativeObject *native_object = obj->cast_to<NativeObject>();
                    jmethodID objectSig2 = env->GetStaticMethodID(SupportClass, "objectSig2", "(Ljava/lang/Object;)Ljava/lang/String;");
                    jstring sig = (jstring)env->CallStaticObjectMethod(SupportClass, objectSig2, (jobject)native_object->native);
                    const char *c_sig = (const char*)env->GetStringUTFChars(sig, NULL);
                    input = String(c_sig);
                    env->ReleaseStringUTFChars(sig, c_sig);
                }else {
                    continue;
                }
            }
        }
        env->SetObjectArrayElement(j_objArr, n, env->NewStringUTF(input.utf8().get_data()));
    }
    jstring j_ret = (jstring)env->CallStaticObjectMethod(cls, methodSig, j_object, is_static, j_name, j_objArr);
    if (j_ret == NULL) {
        return "";
    }else {
        const char *c_ret = env->GetStringUTFChars(j_ret, NULL);
        String ret = String(c_ret);
        env->ReleaseStringUTFChars(j_ret, c_ret);
        return ret;
    }
}

void *native_class(const String name) {
    JNIEnv *env = get_env();
    if (env) {
        jclass cls = env->FindClass(name.replace(".", "/").utf8().get_data());
        return env->NewGlobalRef(cls);
    }
    return NULL;
}

void *new_native_object(const String name, Ref<NativeObject> willInit, const void *native_class, const Variant** p_args,int p_argcount) {
    JNIEnv *env = get_env();
    jclass cls = (jclass)native_class;
    if(!cls || name.length() == 0) {
        return NULL;
    }
    jvalue values[p_argcount];
    String str = constructor_sig(name, p_args, p_argcount);
    if (str.length() == 0)
        return NULL;
    jmethodID constructor = env->GetMethodID(cls, "<init>", str.utf8().get_data());//

    for (int n = 0; n < p_argcount; n++) {
        const Variant* var = p_args[n];
        Variant::Type type = var->get_type();
        switch (type){
            case Variant::BOOL:
                values[n].z = (jboolean)var->operator bool();
                break;
            case Variant::INT:
                values[n].i = (jint)var->operator int();
                break;
            case Variant::REAL:
                values[n].f = (jfloat)var->operator float();
                break;
            case Variant::STRING:
            {
                String str = var->operator String();
                values[n].l = env->NewStringUTF(str.utf8().get_data());
                break;
            };
            default: {
                const Object *obj = var->operator Object*();
                if (obj->get_type() == "NativeObject") {
                    const NativeObject *native_object = obj->cast_to<NativeObject>();
                    values[n].l = (jobject)native_object->native;
                }else {
                    continue;
                }
            }
        }

    }
    _now_init = *willInit;
    jobject obj = env->NewObjectA(cls, constructor, values);
    _now_init = NULL;

    return env->NewGlobalRef(obj);
}

void delete_native_object(const void *native) {
    JNIEnv *env = get_env();
    env->DeleteGlobalRef((jobject)native);
}

bool set_native_value(const void *native, bool is_static, const StringName& p_name,const Variant& p_value) {
    JNIEnv *env = get_env();
    jclass cls;
    if (is_static) {
        cls = (jclass)native;
    }else {
        cls = env->GetObjectClass((jobject)native);
    }
    if(!cls) {
        return false;
    }
    jmethodID memberSig = env->GetStaticMethodID(SupportClass, "memberSig", "(Ljava/lang/Object;ZLjava/lang/String;)Ljava/lang/String;");
    jstring j_member_sig = (jstring)env->CallStaticObjectMethod(SupportClass, memberSig, (jobject)native, is_static, env->NewStringUTF(String(p_name).utf8().get_data()));

    const char *c_sig = (const char*)env->GetStringUTFChars(j_member_sig, NULL);
    jfieldID fieldID;
    if (is_static) {
        fieldID = env->GetStaticFieldID(cls, String(p_name).utf8().get_data(), c_sig);
    }else {
        fieldID = env->GetFieldID(cls, String(p_name).utf8().get_data(), c_sig);
    }
    String s_sig(c_sig);
    bool ret = false;
    if (s_sig == "Z") {
        if (is_static) {
            env->SetStaticBooleanField(cls, fieldID, (bool)p_value);
        }else {
            env->SetBooleanField((jobject)native, fieldID, (bool)p_value);
        }
        ret = true;
    }else if (s_sig == "I") {
        if (is_static) {
            env->SetStaticIntField(cls, fieldID, (int)p_value);
        }else {
            env->SetIntField((jobject)native, fieldID, (int)p_value);
        }
        ret = true;
    }else if (s_sig == "B") {
        if (is_static) {
            env->SetStaticByteField(cls, fieldID, (jbyte)((int)p_value));
        }else {
            env->SetByteField((jobject)native, fieldID, (jbyte)((int)p_value));
        }
        ret = true;
    }else if (s_sig == "C") {
        if (is_static) {
            env->SetStaticCharField(cls, fieldID, (unsigned char)p_value);
        }else {
            env->SetCharField((jobject)native, fieldID, (unsigned char)p_value);
        }
        ret = true;
    }else if (s_sig == "D") {
        if (is_static) {
            env->SetStaticDoubleField(cls, fieldID, (double)p_value);
        }else {
            env->SetDoubleField((jobject)native, fieldID, (double)p_value);
        }
        ret = true;
    }else if (s_sig == "F") {
        if (is_static) {
            env->SetStaticFloatField(cls, fieldID, (float)p_value);
        }else {
            env->SetFloatField((jobject)native, fieldID, (float)p_value);
        }
        ret = true;
    }else if (s_sig == "L") {
        if (is_static) {
            env->SetStaticLongField(cls, fieldID, (jlong)p_value);
        }else {
            env->SetLongField((jobject)native, fieldID, (jlong)p_value);
        }
        ret = true;
    }else {
        if (s_sig[0] == 'L') {
            if (p_value.get_type() == Variant::STRING) {
                String p_str(p_value);
                if (is_static) {
                    env->SetStaticObjectField(cls, fieldID, (jobject)env->NewStringUTF(p_str.utf8().get_data()));
                } else {
                    env->SetObjectField((jobject)native, fieldID, (jobject)env->NewStringUTF(p_str.utf8().get_data()));
                }
                ret = true;
            }else {
                const Object *obj = (const Object*)p_value;
                if (obj->get_type() == "NativeObject") {
                    const NativeObject *native_object = obj->cast_to<NativeObject>();
                    if (is_static) {
                        env->SetStaticObjectField(cls, fieldID, (jobject)native_object->native);
                    }else {
                        env->SetObjectField((jobject)native, fieldID, (jobject)native_object->native);
                    }
                    ret = true;
                }else {
                    ret = false;
                }
            }
        }else if (s_sig.match("^[")){
            ret = false;
        }else {
            ret = false;
        }
    }
    env->ReleaseStringUTFChars(j_member_sig, c_sig);
    return ret;
}
bool get_native_value(const void *native, bool is_static, const StringName& p_name,Variant &r_ret) {
    JNIEnv *env = get_env();
    jclass cls;
    if (is_static) {
        cls = (jclass)native;
    }else {
        cls = env->GetObjectClass((jobject)native);
    }
    if(!cls) {
        return false;
    }
    jmethodID memberSig = env->GetStaticMethodID(SupportClass, "memberSig", "(Ljava/lang/Object;ZLjava/lang/String;)Ljava/lang/String;");
    jstring j_member_sig = (jstring)env->CallStaticObjectMethod(SupportClass, memberSig, (jobject)native, is_static, env->NewStringUTF(String(p_name).utf8().get_data()));

    const char *c_sig = (const char *)env->GetStringUTFChars(j_member_sig, NULL);
    jfieldID fieldID;
    if (is_static) {
        fieldID = env->GetStaticFieldID(cls, String(p_name).utf8().get_data(), c_sig);
    }else {
        fieldID = env->GetFieldID(cls, String(p_name).utf8().get_data(), c_sig);
    }
    String s_sig;
    s_sig.parse_utf8(c_sig,env->GetStringUTFLength(j_member_sig));
    bool ret = false;
    if (s_sig == "Z") {
        r_ret = Variant(is_static ? env->GetStaticBooleanField(cls, fieldID) : env->GetBooleanField((jobject)native, fieldID));
        ret = true;
    }else if (s_sig == "I") {
        r_ret = Variant(is_static ? env->GetStaticIntField(cls, fieldID): env->GetIntField((jobject)native, fieldID));
        ret = true;
    }else if (s_sig == "B") {
        r_ret = Variant(is_static?env->GetStaticByteField(cls, fieldID):env->GetByteField((jobject)native, fieldID));
        ret = true;
    }else if (s_sig == "C") {
        r_ret = Variant(is_static?env->GetStaticCharField(cls, fieldID):env->GetCharField((jobject)native, fieldID));
        ret = true;
    }else if (s_sig == "D") {
        r_ret = Variant(is_static?env->GetStaticDoubleField(cls, fieldID):env->GetDoubleField((jobject)native, fieldID));
        ret = true;
    }else if (s_sig == "F") {
        r_ret = Variant(is_static?env->GetStaticFloatField(cls, fieldID):env->GetFloatField((jobject)native, fieldID));
        ret = true;
    }else if (s_sig == "L") {
        r_ret = Variant(is_static?env->GetStaticLongField(cls, fieldID):env->GetLongField((jobject)native, fieldID));
        ret = true;
    }else if (s_sig[0] == 'L') {
        if (s_sig == "Ljava/lang/String;") {
            jstring j_str = (jstring)(is_static?env->GetStaticObjectField(cls, fieldID):env->GetObjectField((jobject)native, fieldID));
            const char* chs = (const char*)env->GetStringUTFChars(j_str, NULL);
            r_ret = Variant(String(chs));
            env->ReleaseStringUTFChars(j_str, chs);
            ret = true;
        }else {
            Ref<NativeObject> res_obj = memnew(NativeObject);
            res_obj->native = env->NewGlobalRef(is_static?env->GetStaticObjectField(cls, fieldID):env->GetObjectField((jobject)native, fieldID));
            r_ret = Variant(res_obj);
            ret = true;
        }
    }else if (s_sig.match("^[")) {
        ret = false;
    }else {
        ret = false;
    }
    env->ReleaseStringUTFChars(j_member_sig, c_sig);
    return ret;
}
Variant call_native_method(const void *native, bool is_static, const StringName& p_method,const Variant** p_args,int p_argcount,Variant::CallError &r_error) {
    JNIEnv *env = get_env();
    jobject obj = (jobject)native;
    if(!obj) {
        r_error.error = Variant::CallError::CALL_ERROR_INSTANCE_IS_NULL;
        return Variant();
    }
    String str = method_sig(obj, is_static, p_method, p_args, p_argcount);
    if (str.length() == 0) {
        r_error.error = Variant::CallError::CALL_ERROR_INVALID_METHOD;
        return Variant();
    }
    jmethodID method;
    if (is_static) {
        LOGD((String(p_method) + " : " + str).utf8().get_data());
        method = env->GetStaticMethodID((jclass)native, String(p_method).utf8().get_data(), str.utf8().get_data());
    }else {
        method = env->GetMethodID(env->GetObjectClass((jobject)native), String(p_method).utf8().get_data(), str.utf8().get_data());
    }

    jvalue values[p_argcount];
    for (int n = 0; n < p_argcount; n++) {
        const Variant* var = p_args[n];
        Variant::Type type = var->get_type();
        switch (type){
            case Variant::BOOL:
                values[n].z = (jboolean)var->operator bool();
                break;
            case Variant::INT:
                values[n].i = (jint)var->operator int();
                break;
            case Variant::REAL:
                values[n].f = (jfloat)var->operator float();
                break;
            case Variant::STRING:
            {
                String str = var->operator String();
                values[n].l = env->NewStringUTF(str.utf8().get_data());
                break;
            };
            default: {
                const Object *obj = var->operator Object*();
                if (obj->get_type() == "NativeObject") {
                    const NativeObject *native_object = obj->cast_to<NativeObject>();
                    values[n].l = (jobject)native_object->native;
                }else {
                    continue;
                }
            }
        }

    }

    String s_sig = str.substr(str.find_last(")") + 1, 999);

    if (s_sig == "Z") {
        return Variant(env->CallBooleanMethodA((jobject)native, method, values));
    }else if (s_sig == "I") {
        return Variant(env->CallIntMethodA((jobject)native, method, values));
    }else if (s_sig == "B") {
        return Variant(env->CallByteMethodA((jobject)native, method, values));
    }else if (s_sig == "C") {
        return Variant(env->CallCharMethodA((jobject)native, method, values));
    }else if (s_sig == "D") {
        return Variant(env->CallDoubleMethodA((jobject)native, method, values));
    }else if (s_sig == "F") {
        return Variant(env->CallFloatMethodA((jobject)native, method, values));
    }else if (s_sig == "L") {
        return Variant(env->CallLongMethodA((jobject)native, method, values));
    }else if (s_sig == "V") {
        env->CallVoidMethodA((jobject)native, method, values);
        return Variant();
    }else if (s_sig[0] == 'L') {
        if (s_sig == "Ljava/lang/String;") {
            jstring j_str = (jstring)env->CallObjectMethodA((jobject)native, method, values);
            const char* chs = (const char*)env->GetStringUTFChars(j_str, NULL);
            Variant ret = Variant(String(chs));
            env->ReleaseStringUTFChars(j_str, chs);
            return ret;
        }else {
            Ref<NativeObject> res_obj = memnew(NativeObject);
            res_obj->native = env->NewGlobalRef(env->CallObjectMethodA((jobject)native, method, values));
            res_obj->native = env->NewGlobalRef(env->CallObjectMethodA((jobject)native, method, values));
            return Variant(res_obj);
        }
    }else if (s_sig.match("^[")) {
    }else {
    }

    r_error.error = Variant::CallError::CALL_ERROR_INVALID_METHOD;
    return Variant();
}

