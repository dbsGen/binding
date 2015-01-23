//
//  objc_support.cpp
//  Test
//
//  Created by Gen on 15-1-21.
//  Copyright (c) 2015年 Gen. All rights reserved.
//

#include "native_class.h"
#import <Foundation/Foundation.h>
#import <objc/runtime.h>
#import "objc/MyObject.h"

typedef id (*id_IMP)(id, SEL, ...);
typedef int (*int_IMP)(id, SEL, ...);
typedef float (*float_IMP)(id, SEL, ...);
typedef bool (*bool_IMP)(id, SEL, ...);
typedef void (*void_IMP)(id, SEL, ...);
typedef Class (*class_IMP)(id, SEL, ...);

class String;
typedef struct _info {
    char type;
    const Variant *var;
    size_t size;
} ArgInfomation;

void *native_class(const String name) {
    NSLog(@"Find object-c class %s", name.utf8().get_data());
    Class cls = objc_getClass(name.utf8().get_data());
    if (cls) {
        NSLog(@"Class found");
    } else {
        NSLog(@"Class is Null");
        NSLog(@"%@", objc_getClass("MyObject"));
    }
    [cls retain];
    return cls;
}
void *new_native_object(const String name, Ref<NativeObject> _will_init, const void *native_class, const Variant** p_args,int p_argcount) {
    NSLog(@"start new object-c Object %s", name.utf8().get_data());
    Class cls = objc_getClass(name.utf8().get_data());
    if (!cls) {
        return NULL;
    }
    unsigned int count = 0;
    Method *methods = class_copyMethodList(cls, &count);
    ArgInfomation arg_infos[p_argcount];
    
    for (int n = 0; n < count; n++) {
        Method m = methods[n];
        NSString *m_str = NSStringFromSelector(method_getName(m));
        
        if ([m_str hasPrefix:@"init"] && p_argcount == (method_getNumberOfArguments(m)-2)) {
            bool match = true;
            size_t mt = 0;
            
            for (int an = 0; an < p_argcount; an++) {
                char chs[10];
                const Variant *var = p_args[an];
                ArgInfomation &info = arg_infos[an];
                method_getArgumentType(m, an+2, chs, 10);
                
                switch (chs[0]) {
                    case 'i':
                    case 'I':
                    //                    case 'c':
                    //                    case 'q':
                    //                    case 's':
                    {
                        if (var->get_type() == Variant::REAL ||
                            var->get_type() == Variant::BOOL ||
                            var->get_type() == Variant::INT) {
                            info.type = chs[0];
                            info.var = var;
                            info.size = sizeof(int);
                        }else {
                            match = false;
                            break;
                        }
                    }
                        break;
                    case 'B':
                    {
                        if (var->get_type() == Variant::BOOL) {
                            info.type = chs[0];
                            info.var = var;
                            info.size = sizeof(BOOL);
                        }else {
                            match = false;
                            break;
                        }
                    }
                        break;
                    case 'f':
                    case 'F':
                    {
                        if (var->get_type() == Variant::REAL ||
                            var->get_type() == Variant::BOOL ||
                            var->get_type() == Variant::INT) {
                            info.type = chs[0];
                            info.var = var;
                            info.size = sizeof(float);
                        }else {
                            match = false;
                            break;
                        }
                    }
                        break;
                    case '@':
                    {
                        if (var->get_type() == Variant::STRING) {
                            info.type = chs[0];
                            info.var = var;
                            info.size = sizeof(id);
                        }else if (var->get_type() == Variant::OBJECT){
                            const Object *obj = var->operator Object*();
                            if (obj->get_type() == "NativeObject") {
                                info.type = chs[0];
                                info.var = var;
                                info.size = sizeof(id);
                            }else {
                                match = false;
                                break;
                            }
                        }
                    }
                    // Class
                    case '#':
                    {
                        if (var->get_type() == Variant::OBJECT) {
                            const Object *obj = var->operator Object*();
                            if (obj->get_type() == "NativeClass") {
                                info.type = chs[0];
                                info.var = var;
                                info.size = sizeof(Class);
                            }else {
                                match = false;
                                break;
                            }
                        }
                    }
                        
                    default:
                        break;
                }
                mt += info.size;
            }
            
            if (!match) {
                continue;
            }
            
            char *mem = (char*)malloc(mt);
            size_t off=0;
            NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
            for (int n = 0; n < p_argcount; n++) {
                ArgInfomation info = arg_infos[n];
                
                switch (info.type) {
                    case 'i':
                    case 'I':
                    {
                        int i = info.var->operator int();
                        memcpy(mem+off, &i, info.size);
                    }
                        break;
                    case 'B':
                    {
                        BOOL b = info.var->operator bool();
                        memcpy(mem+off, &b, info.size);
                    }
                        break;
                    case 'f':
                    case 'F':
                    {
                        float f = info.var->operator float();
                        memcpy(mem+off, &f, info.size);
                    }
                        break;
                    case '@':
                    {
                        if (info.var->get_type() == Variant::STRING) {
                            String str = info.var->operator String();
                            NSString *string = [NSString stringWithUTF8String:str.utf8().get_data()];
                            memcpy(mem+off, &string, info.size);
                        }else if (info.var->get_type() == Variant::OBJECT) {
                            const Object *obj = info.var->operator Object*();
                            if (obj->get_type() == "NativeObject") {
                                const NativeObject *native_object = obj->cast_to<NativeObject>();
                                memcpy(mem+off, native_object->native, info.size);
                            }
                        }
                    }
                        break;
                    case '#':
                    {
                        if (info.var->get_type() == Variant::OBJECT) {
                            const Object *obj = info.var->operator Object*();
                            if (obj->get_type() == "NativeClass") {
                                const NativeObject *native_object = obj->cast_to<NativeObject>();
                                memcpy(mem+off, native_object->native, info.size);
                            }
                        }
                    }
                        
                    default:
                        break;
                }
                off += info.size;
            }
            id result = [cls alloc];
            id_IMP imp = (id_IMP)method_getImplementation(m);
            imp(result, method_getName(m), *(void**)mem);
            [pool release];
            free(methods);
            free(mem);
            return result;
        }
    }

    free(methods);  
    return NULL;
}
void delete_native_object(const void *native) {
    [(id)native release];
}
bool set_native_value(const void *native, bool is_static, const StringName& p_name,const Variant& p_value) {
    if (is_static)
    {
        return NO;
    }else {
        Class cls = [(id)native class];
        CharString chr_string = String(p_name).utf8();
        const char *c_name = chr_string.get_data();
        
        objc_property_t pro = class_getProperty(cls, c_name);
        // check if it is a property.
        if (pro) {
            int len = chr_string.length();
            char setter_name[5+len];
            strcpy(setter_name, "set");
            if (c_name[0] >= 'a' && c_name[0] <= 'z')
            {  
                setter_name[3] = c_name[0] + ('A'-'a');
            } else {
                setter_name[3] = c_name[0];
            }
            for (int n = 1; n < len; n++) {
                setter_name[n+3] = c_name[n];
            }
            setter_name[len+3] = ':';
            setter_name[len+4] = 0;

            SEL sel = sel_registerName(setter_name);
            Method m = class_getInstanceMethod(cls, sel);
            if (!m)
                return false;
            NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
            unsigned int count = 0;
            objc_property_attribute_t *p_a = property_copyAttributeList(pro, &count);
            char t[30];
            for (int n = 0; n < count; n++) {
                if (strcmp(p_a[n].name, "T") == 0) {
                    memcpy(t, p_a[n].value, 30);
                    break;
                }
            }
            switch (t[0]) {
                case 'i':
                case 'I':
                {
                    typedef void (*i_s_IMP)(id, SEL, int);
                    i_s_IMP imp = (i_s_IMP)method_getImplementation(m);
                    imp((id)native, sel, (int)p_value);
                }
                    break;
                case 'B':
                {
                    typedef void (*b_s_IMP)(id, SEL, bool);
                    b_s_IMP imp = (b_s_IMP)method_getImplementation(m);
                    imp((id)native, sel, (bool)p_value);
                }
                    break;
                case 'f':
                case 'F':
                {
                    typedef void (*f_s_IMP)(id, SEL, float);
                    f_s_IMP imp = (f_s_IMP)method_getImplementation(m);
                    imp((id)native, sel, (float)p_value);
                }
                    break;
                case '@':
                {
                    void_IMP imp = (void_IMP)method_getImplementation(m);
                    if (strcmp(t, "@\"NSString\"") == 0) {
                        String p_str(p_value);
                        NSString *obj_str = [[NSString alloc] initWithUTF8String:p_str.utf8().get_data()];
                        imp((id)native, sel, obj_str);
                        [obj_str release];
                    }else {
                        const Object *obj = (const Object*)p_value;
                        if (obj->get_type() == "NativeObject") {
                            const NativeObject *native_object = obj->cast_to<NativeObject>();
                            imp((id)native, sel, (id)native_object->native);
                        }else {
                            [pool release];
                            return false;
                        }
                    }
                }
                case '#':
                {
                    const Object *obj = (const Object*)p_value; 
                    if (obj->get_type() == "NativeClass")
                    {
                        const NativeClass *native_class = obj->cast_to<NativeClass>();
                        void_IMP imp = (void_IMP)method_getImplementation(m);
                        imp((id)native, sel, (Class)native_class->native);
                    }else {
                        [pool release];
                        return false;
                    }
                }
                        break;
                    
                default:
                    [pool release];
                    return false;
            }
            [pool release];
        }else {
            Ivar var = class_getInstanceVariable(cls, c_name);
            if (var) {
                const char *c_type = ivar_getTypeEncoding(var);
                switch (c_type[0]) {
                    case 'i':
                    case 'I':
                    {
                        int i = (int)p_value;
                        object_setInstanceVariable((id)native, c_name, (void*)(*(void**)&i));
                    }
                        break;
                    case 'B':
                    {
                        BOOL b = (bool)p_value;
                        object_setInstanceVariable((id)native, c_name, (void*)(*(void**)&b));
                    }
                        break;
                    case 'f':
                    case 'F':
                    {
                        float f = (float)p_value;
                        object_setInstanceVariable((id)native, c_name, (void*)(*(void**)&f));
                    }
                        break;
                    case '@':
                    {
                        if (strcmp(c_type, "@\"NSString\"") == 0) {
                            String p_str(p_value);
                            NSString *obj_str = [[NSString alloc] initWithUTF8String:p_str.utf8().get_data()];
                            object_setInstanceVariable((id)native, c_name, (void*)obj_str);
                            [obj_str release];
                        }else {
                            const Object *obj = (const Object*)p_value;
                            if (obj->get_type() == "NativeObject") {
                                const NativeObject *native_object = obj->cast_to<NativeObject>();
                                object_setInstanceVariable((id)native, c_name, (void*)native_object->native);
                            }else {
                                return false;
                            }
                        }
                    }
                    case '#':
                    {
                        const Object *obj = (const Object*)p_value; 
                        if (obj->get_type() == "NativeClass")
                        {
                            const NativeClass *native_class = obj->cast_to<NativeClass>();
                            object_setInstanceVariable((id)native, c_name, (void*)native_class->native);
                        }else {
                            return false;
                        }
                    }
                    break;
                        
                    default:
                        return false;
                        break;
                }
            }else {
                return false;
            }
        }
    }
    return true;
}

bool get_native_value(const void *native, bool is_static, const StringName& p_name,Variant &r_ret) {
    if (is_static) {
        return false;
    }else {
        Class cls = [(id)native class];
        const char *c_name = String(p_name).utf8().get_data();
        objc_property_t pro = class_getProperty(cls, c_name);
        // check if it is a property.
        if (pro) {
            SEL sel = sel_registerName(c_name);
            Method m = class_getInstanceMethod(cls, sel);
            if (!m)
                return NO;
            NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
            unsigned int count = 0;
            objc_property_attribute_t *p_a = property_copyAttributeList(pro, &count);
            char t[30];
            for (int n = 0; n < count; n++) {
                if (strcmp(p_a[n].name, "T") == 0) {
                    memcpy(t, p_a[n].value, 30);
                    break;
                }
            }
            switch (t[0]) {
                case 'i':
                case 'I':
                {
                    int_IMP imp = (int_IMP)method_getImplementation(m);
                    r_ret = Variant(imp((id)native, sel));
                }
                    break;
                case 'B':
                {
                    bool_IMP imp = (bool_IMP)method_getImplementation(m);
                    r_ret = Variant(imp((id)native, sel));
                }
                    break;
                case 'f':
                case 'F':
                {
                    float_IMP imp = (float_IMP)method_getImplementation(m);
                    r_ret = Variant(imp((id)native, sel));
                }
                    break;
                case '@':
                {
                    id_IMP imp = (id_IMP)method_getImplementation(m);
                    if (strcmp(t, "@\"NSString\"") == 0) {
                        NSString *string = imp((id)native, sel);
                        r_ret = Variant(String([string UTF8String]));
                    }else {
                        id object = imp((id)native, sel);
                        Ref<NativeObject> res_obj = memnew(NativeObject);
                        res_obj->native = [object retain];
                        r_ret = Variant(res_obj);
                    }
                }
                case '#':
                {
                    class_IMP imp = (class_IMP)method_getImplementation(m);
                    Ref<NativeClass> _class = memnew(NativeClass);
                    Class r_cls = imp((id)native, sel);
                    _class->_native_type_name = String(class_getName(r_cls));
                    _class->native = [r_cls retain];
                    r_ret = Variant(_class);
                }
                        break;
                    
                default:
                    free(p_a);
                    [pool release];
                    return NO;
            }
            free(p_a);
            [pool release];
        }else {
            Ivar var = class_getInstanceVariable(cls, c_name);
            if (var) {
                const char *c_type = ivar_getTypeEncoding(var);
                switch (c_type[0]) {
                    case 'i':
                    case 'I':
                    {
                        int i;
                        object_getInstanceVariable((id)native, c_name, (void**)&i);
                        r_ret = Variant(i);
                    }
                        break;
                    case 'B':
                    {
                        BOOL b;
                        object_getInstanceVariable((id)native, c_name, (void**)&b);
                        r_ret = Variant(b);
                    }
                        break;
                    case 'f':
                    case 'F':
                    {
                        float f;
                        object_getInstanceVariable((id)native, c_name, (void**)&f);
                        r_ret = Variant(f);
                    }
                        break;
                    case '@':
                    {
                        if (strcmp(c_type, "@\"NSString\"") == 0) {
                            NSString *string = nil;
                            object_getInstanceVariable((id)native, c_name, (void**)&string);
                            r_ret = Variant(String([string UTF8String]));
                        }else {
                            id object = nil;
                            object_getInstanceVariable((id)native, c_name, (void**)&object);
                            Ref<NativeObject> res_obj = memnew(NativeObject);
                            res_obj->native = [object retain];
                            r_ret = Variant(res_obj);
                        }
                    }
                    case '#':
                    {
                        Class r_cls = nil;
                        object_getInstanceVariable((id)native, c_name, (void**)&r_cls);
                        Ref<NativeClass> _class = memnew(NativeClass);
                        _class->_native_type_name = String(class_getName(r_cls));
                        _class->native = [r_cls retain];
                        r_ret = Variant(_class);
                    }
                    break;
                        
                    default:
                        return NO;
                        break;
                }
            }else {
                return NO;
            }
        }
    }
    return true;
}

Variant call_native_method(const void *native, bool is_static, const StringName& p_method,const Variant** p_args,int p_argcount,Variant::CallError &r_error) {
    Class cls = object_getClass((id)native);
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSString *objc_method = [NSString stringWithUTF8String:String(p_method).utf8().get_data()];
    unsigned int count = 0;
    Method *methods = class_copyMethodList(cls, &count);
    Variant r_ret;
    bool ret = false;
    for (int i = 0; i < count; ++i)
    {
        if (p_argcount == 0)
        {
            Method m = methods[i];
            SEL selector = method_getName(m);
            if ([objc_method isEqualToString:NSStringFromSelector(selector)])
            {
                char ch_type[10];
                method_getReturnType(m, ch_type, 10);
                switch (ch_type[0]) {
                    case 'i':
                    case 'I':
                    {
                        int_IMP imp = (int_IMP)method_getImplementation(m);
                        r_ret = Variant(imp((id)native, selector));
                        ret = true;
                    }
                        break;
                    case 'B':
                    {
                        bool_IMP imp = (bool_IMP)method_getImplementation(m);
                        r_ret = Variant(imp((id)native, selector));
                        ret = true;
                    }
                        break;
                    case 'f':
                    case 'F':
                    {
                        float_IMP imp = (float_IMP)method_getImplementation(m);
                        float res = imp((id)native, selector);
                        r_ret = Variant(res);
                        ret = true;
                    }
                        break;
                    case '@':
                    {
                        id_IMP imp = (id_IMP)method_getImplementation(m);
                        id result = imp((id)native, selector);
                        if (!result)
                        {
                            r_ret = Variant();
                        }else if ([result isKindOfClass:[NSString class]]) {
                            r_ret = Variant(String([result UTF8String]));
                        }else {
                            Ref<NativeObject> res_obj = memnew(NativeObject);
                            res_obj->native = [result retain];
                            r_ret = Variant(res_obj);
                        }
                        ret = true;
                    }
                    case '#':
                    {
                        class_IMP imp = (class_IMP)method_getImplementation(m);
                        Class result = imp((id)native, selector);
                        if (!result)
                        {
                            r_ret = Variant();
                        }else {
                            Ref<NativeClass> _class = memnew(NativeClass);
                            _class->_native_type_name = String(class_getName(result));
                            _class->native = [result retain];
                            r_ret = Variant(_class);
                        }
                        ret = true;
                    }
                }
                if (ret)
                {
                    free(methods);
                    [pool release];
                    return r_ret;
                }
            }
        }else {
            Method m = methods[i];
            SEL selector = method_getName(m);
            if ([NSStringFromSelector(selector) hasPrefix:[objc_method stringByAppendingString:@":"]] &&
                method_getNumberOfArguments(m)-2 == p_argcount)
            {
                bool match = true;
                ArgInfomation arg_infos[p_argcount];
                size_t mt = 0;
                for (int i = 0; i < p_argcount; ++i)
                {
                    const Variant* var = p_args[i];
                    char chs[10];
                    ArgInfomation &info = arg_infos[i];
                    method_getArgumentType(m, i+2, chs, 10);
                    switch (chs[0]) {
                        case 'i':
                        case 'I':
                        {
                            if (var->get_type() == Variant::REAL ||
                                var->get_type() == Variant::BOOL ||
                                var->get_type() == Variant::INT) {
                                info.type = chs[0];
                                info.var = var;
                                info.size = sizeof(int);
                            }else {
                                match = false;
                                break;
                            }
                        }
                            break;
                        case 'B':
                        {
                            if (var->get_type() == Variant::BOOL) {
                                info.type = chs[0];
                                info.var = var;
                                info.size = sizeof(BOOL);
                            }else {
                                match = false;
                                break;
                            }
                        }
                            break;
                        case 'f':
                        case 'F':
                        {
                            if (var->get_type() == Variant::REAL ||
                                var->get_type() == Variant::BOOL ||
                                var->get_type() == Variant::INT) {
                                info.type = chs[0];
                                info.var = var;
                                info.size = sizeof(float);
                            }else {
                                match = false;
                                break;
                            }
                        }
                            break;
                        case '@':
                        {
                            if (var->get_type() == Variant::STRING) {
                                info.type = chs[0];
                                info.var = var;
                                info.size = sizeof(id);
                            }else if (var->get_type() == Variant::OBJECT){
                                const Object *obj = var->operator Object*();
                                if (obj->get_type() == "NativeObject") {
                                    info.type = chs[0];
                                    info.var = var;
                                    info.size = sizeof(id);
                                }else {
                                    match = false;
                                    break;
                                }
                            }
                        }
                        // Class
                        case '#':
                        {
                            if (var->get_type() == Variant::OBJECT) {
                                const Object *obj = var->operator Object*();
                                if (obj->get_type() == "NativeClass") {
                                    info.type = chs[0];
                                    info.var = var;
                                    info.size = sizeof(Class);
                                }else {
                                    match = false;
                                    break;
                                }
                            }
                        }
                            
                        default:
                            break;
                    }
                    mt += info.size;
                }
                if (!match)
                {
                    continue;
                }
                Byte *mem = (Byte*)malloc(mt);
                size_t off=0;
                for (int n = 0; n < p_argcount; n++) {
                    ArgInfomation info = arg_infos[n];
                    
                    switch (info.type) {
                        case 'i':
                        case 'I':
                        {
                            int i = info.var->operator int();
                            memcpy(mem+off, (int*)&i, info.size);
                        }
                            break;
                        case 'B':
                        {
                            BOOL b = info.var->operator bool();
                            memcpy(mem+off, (bool*)&b, info.size);
                        }
                            break;
                        case 'f':
                        case 'F':
                        {
                            float f = info.var->operator float();
                            memcpy(mem+off, &f, info.size);
                        }
                            break;
                        case '@':
                        {
                            if (info.var->get_type() == Variant::STRING) {
                                String str = info.var->operator String();
                                NSString *string = [NSString stringWithUTF8String:str.utf8().get_data()];
                                memcpy(mem+off, (id)&string, info.size);
                            }else if (info.var->get_type() == Variant::OBJECT) {
                                const Object *obj = info.var->operator Object*();
                                if (obj->get_type() == "NativeObject") {
                                    const NativeObject *native_object = obj->cast_to<NativeObject>();
                                    memcpy(mem+off, (id)native_object->native, info.size);
                                }
                            }
                        }
                            break;
                        case '#':
                        {
                            if (info.var->get_type() == Variant::OBJECT) {
                                const Object *obj = info.var->operator Object*();
                                if (obj->get_type() == "NativeClass") {
                                    const NativeObject *native_object = obj->cast_to<NativeObject>();
                                    memcpy(mem+off, (id)native_object->native, info.size);
                                }
                            }
                        }
                            
                        default:
                            break;
                    }
                    off += info.size;
                }

                char chs[10];
                method_getReturnType(m, chs, 10);
                Variant r_ret;
                switch (chs[0]) {
                    case 'i':
                    case 'I':
                    {
                        int_IMP imp = (int_IMP)method_getImplementation(m);
                        r_ret = Variant(imp((id)native, selector, *(void**)mem));
                    }
                        break;
                    case 'B':
                    {
                        bool_IMP imp = (bool_IMP)method_getImplementation(m);
                        r_ret = Variant(imp((id)native, selector, *(void**)mem));
                    }
                        break;
                    case 'f':
                    case 'F':
                    {
                        float_IMP imp = (float_IMP)method_getImplementation(m);
                        r_ret = Variant(imp((id)native, selector, *(void**)mem));
                    }
                        break;
                    case 'v':
                    {
                        void_IMP imp = (void_IMP)method_getImplementation(m);
                        imp((id)native, selector, *(void**)mem);
                        r_ret = Variant();
                    }
                        break;
                    case '@':
                    {
                        id_IMP imp = (id_IMP)method_getImplementation(m);
                        id result = imp((id)native, selector, *(void**)mem);
                        if (!result)
                        {
                            r_ret = Variant();
                        }else if ([result isKindOfClass:[NSString class]]) {
                            r_ret = Variant(String([result UTF8String]));
                        }else {
                            Ref<NativeObject> res_obj = memnew(NativeObject);
                            res_obj->native = [result retain];
                            r_ret = Variant(res_obj);
                        }
                    }
                        break;
                    case '#':
                    {
                        class_IMP imp = (class_IMP)method_getImplementation(m);
                        Class result = imp((id)native, selector, *(void**)mem);
                        if (!result)
                        {
                            r_ret = Variant();
                        }else {
                            Ref<NativeClass> _class = memnew(NativeClass);
                            _class->_native_type_name = String(class_getName(result));
                            _class->native = [result retain];
                            r_ret = Variant(_class);
                        }
                    }
                        break;
                }
                [pool release];
                free(methods);
                free(mem);
                return r_ret;
            }
        }
    }
    [pool release];
    free(methods);
    r_error.error = Variant::CallError::CALL_ERROR_INVALID_METHOD;
    return Variant();
}
