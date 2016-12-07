LOCAL_PATH:= $(call my-dir)
$(warning $(LOCAL_PATH))

include $(CLEAR_VARS)

include /opt/OpenCV-android-sdk/sdk/native/jni/OpenCV.mk

#LOCAL_MODULE     := imageproc
#LOCAL_SRC_FILES  := ImageProc.cpp

LOCAL_SRC_FILES  := native-util.cpp
LOCAL_MODULE     := native-util

include $(BUILD_SHARED_LIBRARY)
