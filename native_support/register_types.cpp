/* register_types.cpp */

#include "object_type_db.h"
#include "core/globals.h"
#include "native_class.h"
#if defined(IPHONE_ENABLED) || defined(OSX_ENABLED)
#include "objc/GOSupport.h"
#endif

void register_native_support_types() {
    Globals::get_singleton()->add_singleton(Globals::Singleton("NF", memnew(NativeFactory)));
    ObjectTypeDB::register_type<NativeFactory>();
    ObjectTypeDB::register_type<NativeClass>();
    ObjectTypeDB::register_type<NativeObject>();
#if defined(IPHONE_ENABLED) || defined(OSX_ENABLED)
    register_TestObject();
#endif
}
void unregister_native_support_types() {

}
