#include <Foundation/FoundationInternal.h>
EZ_FOUNDATION_INTERNAL_HEADER

#  include <android_native_app_glue.h>
#  include <Foundation/Basics/Platform/Android/AndroidUtils.h>

void ezUuid::CreateNewUuid()
{
  // Use jni as uuid.h is not available
  android_app* app = ezAndroidUtils::GetAndroidApp();
  JNIEnv* env = ezAndroidUtils::GetJniEnv();

  static jclass uuidClass = env->FindClass("java/util/UUID");
  static jmethodID randomUUID = env->GetStaticMethodID(uuidClass, "randomUUID", "()Ljava/util/UUID;");
  static jmethodID getMostSignificantBits = env->GetMethodID(uuidClass, "getMostSignificantBits", "()J");
  static jmethodID getLeastSignificantBits = env->GetMethodID(uuidClass, "getLeastSignificantBits", "()J");

  jobject javaUuid = env->CallStaticObjectMethod(uuidClass, randomUUID);
  jlong mostSignificant = env->CallLongMethod(javaUuid, getMostSignificantBits);
  jlong leastSignificant = env->CallLongMethod(javaUuid, getLeastSignificantBits);

  m_uiHigh = mostSignificant;
  m_uiLow = leastSignificant;

  //#TODO maybe faster to read /proc/sys/kernel/random/uuid, but that can't be done via ezOSFile
  // see https://stackoverflow.com/questions/11888055/include-uuid-h-into-android-ndk-project
}

