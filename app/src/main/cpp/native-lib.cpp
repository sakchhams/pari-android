#include <jni.h>
#include <string>
#include <pari.h>
#include <android/log.h>

std::string pari_test();

extern "C"
JNIEXPORT jstring

JNICALL
Java_com_sak_paridemo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    __android_log_print(ANDROID_LOG_DEBUG, "Pari/Android", "running tests.");
    std::string hello = pari_test();
    return env->NewStringUTF(hello.c_str());
}

std::string pari_test()
{
    GEN M,N1,N2, F1,F2,D;
    int i;
    pari_init(4000000,500000);
    __android_log_print(ANDROID_LOG_DEBUG, "Pari/Android", "testing addition");
    N1 = addis(int2n(32), 1); /* 2^32 + 1 */
    __android_log_print(ANDROID_LOG_DEBUG, "Pari/Android", "testing subtraction");
    N2 = subis(int2n(21), 1); /* 2^21 - 1 */
    char buff[10*1024]; //overkill, I know, but just to be safe.
    snprintf(buff, sizeof(buff), "N1=%s\nN2=%s\n", GENtostr(N1), GENtostr(N2));
    std::string buffAsStdStr = buff;
    pari_close();
    return buffAsStdStr;
}


