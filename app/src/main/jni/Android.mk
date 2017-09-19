LOCAL_PATH := $(call my-dir) 
include $(CLEAR_VARS) 

IS_DEBUG := false

LOCAL_MODULE := syno_dev_ops
LOCAL_SRC_FILES := syno_dev_ops.cpp syno_dev.cpp


LOCAL_LDLIBS := -llog

ifeq ($(strip $(IS_DEBUG)), true)
    LOCAL_CFLAGS += -DDEBUG_DEV
endif


include $(BUILD_SHARED_LIBRARY)
