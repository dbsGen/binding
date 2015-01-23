#include "native_class.h"

void NativeFactory::_bind_methods() {
    ObjectTypeDB::bind_method("find",&NativeFactory::find);
}

Variant NativeFactory::find(const String package_name) const {
    void *nc = native_class(package_name);
    if (nc) {
        Ref<NativeClass> _class = memnew(NativeClass);
        _class->_native_type_name = package_name;
        _class->native = nc;
        return Variant(_class);
    }
    return Variant();
}


String NativeClass::get_native_type_name() const {
    return _native_type_name;
}

void NativeClass::_bind_methods() {
    ObjectTypeDB::bind_native_method(METHOD_FLAGS_DEFAULT,"new",&NativeClass::_new, MethodInfo("new"));
    ObjectTypeDB::bind_method("get_native_type_name",&NativeClass::get_native_type_name);
}
Variant NativeClass::call(const StringName& p_method,const Variant** p_args,int p_argcount,Variant::CallError &r_error) {
    if (p_method == "new" || p_method == "get_native_type_name") {
        return Reference::call(p_method,p_args,p_argcount,r_error);
    }else {
        r_error.error = Variant::CallError::CALL_OK;
        Variant v = call_native_method(native, true, p_method, p_args, p_argcount, r_error);
        if (r_error.error != Variant::CallError::CALL_OK) {
            return Reference::call(p_method,p_args,p_argcount,r_error);
        }
        return v;
    }
}

Variant NativeClass::_new(const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
    Ref<NativeObject> _obj = memnew(NativeObject);
    void *native_target = new_native_object(_native_type_name, _obj, native, p_args, p_argcount);
    if (native_target) {
        _obj->native = native_target;
        return Variant(_obj);
    }
    return Variant();
}
void NativeClass::setvar(const Variant& p_key, const Variant& p_value,bool *r_valid) {
    *r_valid = set_native_value(native, true, p_key, p_value);
}

Variant NativeClass::getvar(const Variant& p_key, bool *r_valid) const {
    Variant ret;
    *r_valid = get_native_value(native, true, p_key, ret);
    return ret;
}

NativeObject **NativeObject::_cached_objects = NULL;
int NativeObject::_cached_count = 0;
int NativeObject::_cached_limit = 10;

NativeObject::NativeObject() {
    native = NULL;
    if (!_cached_objects) {
        _cached_objects = (NativeObject **)memalloc(_cached_limit*sizeof(NativeObject*));
        _cached_objects[0] = this;
        _cached_count = 1;
    }else {
        if (_cached_count < _cached_limit) {
            _cached_objects[_cached_count] = this;
        }else {
            _cached_limit += 10;
            _cached_objects = (NativeObject **)memrealloc(_cached_objects, _cached_limit* sizeof(NativeObject*));
            _cached_objects[_cached_count] = this;
        }
        _cached_count += 1;
    }
}

NativeObject::~NativeObject() {
    delete_native_object(native);
    for (int n = 0; n < _cached_count; n++) {
        if (_cached_objects[n] == this) {
            for (; n < _cached_count-1; n++) {
                _cached_objects[n] = _cached_objects[n+1];
            }
            _cached_count-=1;
            break;
        }
    }
}

void NativeObject::_bind_methods() {

}

void NativeObject::setvar(const Variant& p_key, const Variant& p_value,bool *r_valid) {
    *r_valid = set_native_value(native, false, p_key, p_value);
}

Variant NativeObject::getvar(const Variant& p_key, bool *r_valid) const {
    Variant ret;
    *r_valid = get_native_value(native, false, p_key, ret);
    return ret;
}

Variant NativeObject::call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
    r_error.error = Variant::CallError::CALL_OK;
    Variant v = call_native_method(native, false, p_method, p_args, p_argcount, r_error);
    if (r_error.error != Variant::CallError::CALL_OK) {
        return Reference::call(p_method,p_args,p_argcount,r_error);
    }
    return v;
}