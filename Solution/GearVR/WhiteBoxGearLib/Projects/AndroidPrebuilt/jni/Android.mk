LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)

LOCAL_MODULE := WhiteBoxGearLib

LOCAL_SRC_FILES := ../../../ARM/Release/libWhiteBoxGearLib.a

LOCAL_EXPORT_C_INCLUDES := \
	$(LOCAL_PATH)/../../../../../../Inc/System/GearVR
 
 # $(LOCAL_PATH)/../../../WhiteBoxGearLib/Inc \

ifneq (,$(wildcard $(LOCAL_PATH)/$(LOCAL_SRC_FILES)))
endif

include $(PREBUILT_STATIC_LIBRARY)
