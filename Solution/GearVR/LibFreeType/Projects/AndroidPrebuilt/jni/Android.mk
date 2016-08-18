LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)

LOCAL_MODULE := libfreetype

LOCAL_SRC_FILES := ../../../libfreetype.a

LOCAL_EXPORT_C_INCLUDES := \
	$(LOCAL_PATH)/../../../../../../../Core/Libs/FreeType/include
 
 
include $(PREBUILT_STATIC_LIBRARY)
