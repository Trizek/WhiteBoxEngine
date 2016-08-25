LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)

LOCAL_MODULE := libBullet

LOCAL_SRC_FILES := ../../../libBullet.a

#LOCAL_EXPORT_C_INCLUDES := \
#	$(LOCAL_PATH)/../../../../../../../Core/Libs/Bullet/src \
 
include $(PREBUILT_STATIC_LIBRARY)
