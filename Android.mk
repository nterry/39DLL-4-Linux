LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := 39dll-4-android
LOCAL_SRC_FILES := android-src/main.cpp android-src/md5.cpp android-src/socket.cpp android-src/tools.cpp android-src/buffer.cpp android-src/list.cpp
#INCLUDE_HEADERS        := sys/socket.h sys/types.h netinet/in.h arpa/inet.h netinet/tcp.h stdio.h stdlib.h string.h iostream sys/ioctl.h

include $(BUILD_SHARED_LIBRARY)
