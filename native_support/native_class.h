
#ifndef JAVA_CLASS_H
#define JAVA_CLASS_H

#include "../../core/ustring.h"
#include "../../core/variant.h"
#include "../../core/reference.h"

class NativeClass;
class NativeObject;

void *native_class(const String name);
void *new_native_object(const String name, Ref<NativeObject> _will_init, const void *native_class, const Variant** p_args,int p_argcount);
void delete_native_object(const void *native);
bool set_native_value(const void *native, const StringName& p_name,const Variant& p_value);
bool get_native_value(const void *native, const StringName& p_name,Variant &r_ret);
Variant call_native_method(const void *native, bool is_static, const StringName& p_method,const Variant** p_args,int p_argcount,Variant::CallError &r_error);

class NativeFactory : public Reference {
    OBJ_TYPE(NativeFactory, Reference);
protected:
    static void _bind_methods();
public:
    Variant java(const String package_name) const;

    NativeFactory(){}
    ~NativeFactory(){}
};

class NativeClass : public Reference {

    OBJ_TYPE(NativeClass, Reference );
private:
    String _native_type_name;
    friend class NativeFactory;

protected:

    Variant call(const StringName& p_method,const Variant** p_args,int p_argcount,Variant::CallError &r_error);
    static void _bind_methods();

public:
    void *native;

    String get_native_type_name() const;
    Variant _new(const Variant** p_args,int p_argcount,Variant::CallError& r_error);

    NativeClass(){}
    ~NativeClass(){
        if (native) delete_native_object(native);
    }
};

class NativeObject : public Reference {

    OBJ_TYPE(NativeObject, Reference );

protected:
    virtual void setvar(const Variant& p_key, const Variant& p_value,bool *r_valid=NULL);
    virtual Variant getvar(const Variant& p_key, bool *r_valid) const;

    virtual Variant call(const StringName& p_method,const Variant** p_args,int p_argcount,Variant::CallError &r_error);
    static void _bind_methods();

    static int _cached_limit;
public:
    void *native;
    static NativeObject **_cached_objects;
    static int _cached_count;

    NativeObject();
    ~NativeObject();
};

#endif