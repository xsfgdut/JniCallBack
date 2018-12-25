//
// Created by shifan_xiao on 2018/12/24.
//

#include <jni.h>
#include <pthread.h>
#include <android/log.h>
#include <unistd.h>

#define LOG(...) __android_log_print(ANDROID_LOG_DEBUG,"Native",__VA_ARGS__)

JavaVM * gJavaVM;
jobject  gJavaObj;

static volatile int gIsThreadExit = 0;

static void* native_thread_exec(void *arg) {

    JNIEnv *env;

    //从全局的JavaVM中获取到环境变量
    (*gJavaVM)->AttachCurrentThread(gJavaVM,&env, NULL);

    //获取Java层对应的类
    jclass javaClass = (*env)->GetObjectClass(env,gJavaObj);
    if( javaClass == NULL ) {
        LOG("Fail to find javaClass");
        return 0;
    }

    //获取Java层被回调的函数
    jmethodID javaCallback = (*env)->GetMethodID(env,javaClass,"onNativeCallback","(I)V");
    if( javaCallback == NULL) {
        LOG("Fail to find method onNativeCallback");
        return 0;
    }

    LOG("native_thread_exec loop enter");

    int count = 0;

    //线程循环
    while(!gIsThreadExit) {

        //回调Java层的函数
        (*env)->CallVoidMethod(env,gJavaObj,javaCallback,count++);

        //休眠1秒
        sleep(1);
    }

    (*gJavaVM)->DetachCurrentThread(gJavaVM);

    LOG("native_thread_exec loop leave");
}

/*
 * Class:     com_ticktick_jnicallback_Native
 * Method:    nativeInitilize
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_ob_wifi_video_ui_activity_AircraftActivity_nativeInitilize(JNIEnv *env, jobject thiz) {

    //注意，直接通过定义全局的JNIEnv和jobject变量，在此保存env和thiz的值是不可以在线程中使用的

    //线程不允许共用env环境变量，但是JavaVM指针是整个jvm共用的，所以可以通过下面的方法保存JavaVM指针，在线程中使用
    (*env)->GetJavaVM(env,&gJavaVM);

    //同理，jobject变量也不允许在线程中共用，因此需要创建全局的jobject对象在线程中访问该对象
    gJavaObj = (*env)->NewGlobalRef(env,thiz);
}

/*
 * Class:     com_ticktick_jnicallback_Native
 * Method:    nativeThreadStart
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_ob_wifi_video_ui_activity_AircraftActivity_nativeThreadStart(JNIEnv *env, jobject thiz) {

    gIsThreadExit = 0;

    //通过pthread库创建线程
    pthread_t threadId;
    if( pthread_create(&threadId,NULL,native_thread_exec,NULL) != 0 ) {
        LOG("native_thread_start pthread_create fail !");
        return;
    }

    LOG("native_thread_start success");
}

/*
 * Class:     com_ticktick_jnicallback_Native
 * Method:    NativeThreadStop
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_ob_wifi_video_ui_activity_AircraftActivity_NativeThreadStop(JNIEnv *env, jobject thiz) {
    gIsThreadExit = 1;
    LOG("native_thread_stop success");
}


