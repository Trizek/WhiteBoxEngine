
LOCAL_PATH:= $(call my-dir)

#--------------------------------------------------------
# libvrcubeworld.so
#--------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE			:= vrcubeworld
LOCAL_CFLAGS			:= -std=c++14  -D __GEAR_VR
LOCAL_SRC_FILES			:=  $(addprefix ../,$(wildcard $(LOCAL_PATH)/../../../../../../../../Src/*.cpp)) \
	$(addprefix ../,$(wildcard $(LOCAL_PATH)/../../../../../../../../Src/Render/*.cpp)) \
	$(addprefix ../,$(wildcard $(LOCAL_PATH)/../../../../../../../../Src/System/*.cpp)) \
	$(addprefix ../,$(wildcard $(LOCAL_PATH)/../../../../../../../../Src/System/GearVR/*.cpp)) \
	$(addprefix ../,$(wildcard $(LOCAL_PATH)/../../../../../../../../Src/LogSystem/*.cpp)) \
	$(addprefix ../,$(wildcard $(LOCAL_PATH)/../../../../../../../../Src/Types/*.cpp)) \
	$(addprefix ../,$(wildcard $(LOCAL_PATH)/../../../../../../../../Src/Png/*.cpp)) \
	$(addprefix ../,$(wildcard $(LOCAL_PATH)/../../../../../../../../Src/Animation/*.cpp)) \


#../../../../../../../../Src/*.cp
LOCAL_LDLIBS			:= -llog -landroid -lGLESv3 -lEGL -lz -lm	# include default libraries

LOCAL_C_INCLUDES		:= $(LOCAL_PATH)/../../../../../../../../Inc/ \
  $(LOCAL_PATH)/../../../../3rdParty/minizip/src \
  $(LOCAL_PATH)/../../../../VrAppSupport/SystemUtils/Include \
  $(LOCAL_PATH)/../../../Include \
  $(LOCAL_PATH)/../../../../../../../../Libs/Bullet/src \


LOCAL_STATIC_LIBRARIES	:= systemutils libovrkernel minizip libfreetype libBullet
LOCAL_SHARED_LIBRARIES	:= vrapi

include $(BUILD_SHARED_LIBRARY)

$(call import-module,VrApi/Projects/AndroidPrebuilt/jni)
$(call import-module,VrAppSupport/SystemUtils/Projects/AndroidPrebuilt/jni)
$(call import-module,3rdParty/minizip/build/androidprebuilt/jni)
$(call import-module,LibFreeType/Projects/AndroidPrebuilt/jni)
$(call import-module,LibBullet/Projects/AndroidPrebuilt/jni)