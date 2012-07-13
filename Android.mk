LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := 39dll-4-linux
LOCAL_SRC_FILES := main.cpp md5.cpp socket.cpp tools.cpp buffer.cpp list.cpp
#INCLUDE_HEADERS        := sys/socket.h sys/types.h netinet/in.h arpa/inet.h netinet/tcp.h stdio.h stdlib.h string.h iostream sys/ioctl.h

include $(BUILD_SHARED_LIBRARY)
