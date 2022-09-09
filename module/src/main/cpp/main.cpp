#include <dobby.h>
#include <jni.h>
#include <pthread.h>
#include "il2cpp_dump.h"
#include <cstring>

#include "hook.h"
auto HOOK = [](auto ptr, auto newMethod, auto &&oldBytes) {
    if (((void *)ptr) != nullptr) {
        DobbyHook((void *)ptr, (void *)newMethod, (void **)&oldBytes);
    }
};

#include "dlopen.h"

//static void* il2cpp_handle = nullptr;
HOOK_API(int, il2cpp_init, (const char* domain_name)) {
    LOGI("il2cpp_init(%s)", domain_name);
    int ret = old_il2cpp_init(domain_name);
    il2cpp_dump(il2cpp_handle, "/sdcard/");
    return ret;
}

static bool p_libil2cpp_inited = false;
void dlopen_posthook(const char *libname, void *handler) {
    if (strstr(libname, "libil2cpp.so") && p_libil2cpp_inited == false) {
        p_libil2cpp_inited = true;
        void *symbol       = nullptr;
        auto il2cpp_init = DobbySymbolResolver(NULL, "il2cpp_init");
        HOOK(il2cpp_init, new_il2cpp_init, old_il2cpp_init);
        il2cpp_handle = handler;
        //il2cpp::init_il2cpp_api(handler);
        LOGI("dlopen %s", libname);
    }
    static bool p_libunity_inited = false;
    if (strstr(libname, "libunity.so") && p_libunity_inited == false) {
        void *symbol      = nullptr;
        p_libunity_inited = true;
        // NEW_HOOK_DEF2("_ZL11UpdateTimerf", UpdateTimer);
    }
}

extern "C" void BNM_hookDlopen() __attribute__((constructor));
void BNM_hookDlopen() {
    LOGI("il2cpp-dumper");
    hook_dlopen();
}