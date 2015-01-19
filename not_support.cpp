#include "native_class.h"

void *native_class(const String name) {
    return NULL;
}

void *new_native_object(const String name, Ref<NativeObject> _will_init, const void *native_class, const Variant** p_args,int p_argcount) {
    return NULL;
}
void delete_native_object(const void *native) {}
bool set_native_value(const void *native, const StringName& p_name,const Variant& p_value) {
    return false;
}
bool get_native_value(const void *native, const StringName& p_name,Variant &r_ret) {
    return false;
}
Variant call_native_method(const void *native, bool is_static, const StringName& p_method,const Variant** p_args,int p_argcount,Variant::CallError &r_error) {
    r_error.error = Variant::CallError::CALL_ERROR_INVALID_METHOD;
    return Variant();
}
