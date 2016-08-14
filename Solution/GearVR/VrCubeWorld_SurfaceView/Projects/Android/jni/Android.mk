
LOCAL_PATH:= $(call my-dir)

#--------------------------------------------------------
# libvrcubeworld.so
#--------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE			:= vrcubeworld
LOCAL_CFLAGS			:= -std=c++11 -Werror -D __GEAR_VR
LOCAL_SRC_FILES			:=  $(addprefix ../,$(wildcard $(LOCAL_PATH)/../../../../../../../../Src/*.cpp)) \
	$(addprefix ../,$(wildcard $(LOCAL_PATH)/../../../../../../../../Src/Render/*.cpp)) \
	$(addprefix ../,$(wildcard $(LOCAL_PATH)/../../../../../../../../Src/System/*.cpp)) \
	$(addprefix ../,$(wildcard $(LOCAL_PATH)/../../../../../../../../Src/System/GearVR/*.cpp)) \
	$(addprefix ../,$(wildcard $(LOCAL_PATH)/../../../../../../../../Src/Types/*.cpp)) \
#../../../../../../../../Src/*.cp
LOCAL_LDLIBS			:= -llog -landroid -lGLESv3 -lEGL		# include default libraries

LOCAL_C_INCLUDES		:= $(LOCAL_PATH)/../../../../../../../../Inc/

LOCAL_STATIC_LIBRARIES	:= systemutils libovrkernel
LOCAL_SHARED_LIBRARIES	:= vrapi

include $(BUILD_SHARED_LIBRARY)

$(call import-module,VrApi/Projects/AndroidPrebuilt/jni)
$(call import-module,VrAppSupport/SystemUtils/Projects/AndroidPrebuilt/jni)