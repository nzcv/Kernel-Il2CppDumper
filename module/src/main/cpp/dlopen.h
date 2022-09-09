#ifndef HOOK_DLOPEN
#define HOOK_DLOPEN
#include <unistd.h>
#include <set>

void dlopen_posthook(const char* libname, void* handler);

#define HOOK_API(ret, func, p) \
    typedef ret (* TYPE_##func) p;\
    static TYPE_##func s_ptr_##func;\
    static ret (*old_##func) p; \
    static ret new_##func p

enum DLVersion { edl_unknown = 0, edl_v0, edl_v19, edl_v24, edl_v26 };

static DLVersion g_dl_version = edl_unknown;
static void* (*old_dlopen_for_all)(const char* name, int flags, const void* extinfo, void* caller_addr) = NULL;
static void* dlopen_for_all(const char* name, int flags, const void* extinfo, void* caller_addr) {
    void* handle = old_dlopen_for_all(name, flags, extinfo, caller_addr);
    dlopen_posthook(name, handle);
    return handle;
}

static bool hook_dlopen() {
    void* dlopen_addr = NULL;
    DLVersion v       = edl_unknown;
    // for 7t /apex/com.android.runtime/bin/linker64
    const char* linker_name = NULL;
    if (access("/apex/com.android.runtime/bin/linker64", F_OK) == 0) {
        linker_name = sizeof(void*) == 4 ? "/apex/com.android.runtime/bin/linker" : "/apex/com.android.runtime/bin/linker64";
    } else {
        linker_name = sizeof(void*) == 4 ? "/system/bin/linker" : "/system/bin/linker64";
    }

    do {
        dlopen_addr = DobbySymbolResolver(NULL, "__dl__Z9do_dlopenPKciPK17android_dlextinfoPKv");
        if (dlopen_addr) {
            v = edl_v26;
            break;
        }
        dlopen_addr = DobbySymbolResolver(NULL, "__dl__Z9do_dlopenPKciPK17android_dlextinfoPv");
        if (dlopen_addr) {
            v = edl_v24;
            break;
        }
        dlopen_addr = DobbySymbolResolver(NULL, "__dl__Z9do_dlopenPKciPK17android_dlextinfo");
        if (dlopen_addr) {
            v = edl_v19;
            break;
        }
        dlopen_addr = DobbySymbolResolver(NULL, "__dl_dlopen");
        if (dlopen_addr) {
            v = edl_v0;
            break;
        }
    } while (0);
    if (dlopen_addr) {
        LOGI("dlopen = %p, version = %d", dlopen_addr, v);
    } else {
        LOGE("[ERROR] dlopen = %p, version = %d", dlopen_addr, v);
    }

    if (v != edl_unknown) {
        *((void**)&old_dlopen_for_all) = dlopen_addr;
        HOOK(old_dlopen_for_all, dlopen_for_all, old_dlopen_for_all);
        return true;
    }
    return false;
}

/*
JNIEXPORT jint __attribute__((visibility("default"))) JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env = NULL;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    hook_dlopen();
    return JNI_VERSION_1_6;
}
*/
#endif /* HOOK_DLOPEN */
