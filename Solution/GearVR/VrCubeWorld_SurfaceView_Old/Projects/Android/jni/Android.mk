
LOCAL_PATH:= $(call my-dir)

#--------------------------------------------------------
# libvrcubeworld.so
#--------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE			:= vrcubeworld
LOCAL_CFLAGS			:= -std=c++11 -Werror
LOCAL_SRC_FILES			:= ../../../Src/VrCubeWorld_SurfaceView.cpp
LOCAL_LDLIBS			:= -llog -landroid -lGLESv3 -lEGL		# include default libraries

LOCAL_STATIC_LIBRARIES	:= systemutils libovrkernel WhiteBoxGearLib
LOCAL_SHARED_LIBRARIES	:= vrapi

include $(BUILD_SHARED_LIBRARY)

$(call import-module,VrApi/Projects/AndroidPrebuilt/jni)
$(call import-module,VrAppSupport/SystemUtils/Projects/AndroidPrebuilt/jni)
$(call import-module,WhiteBoxGearLib/Projects/AndroidPrebuilt/jni)