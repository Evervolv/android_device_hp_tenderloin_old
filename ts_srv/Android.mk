# Makefile for ts_srv userspace input driver for HP Touchpad.
# Copyright (C) 2012 Wong Yong Jie
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := ts_srv
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES := \
	bionic/libc/kernel/common \
	bionic/libc/kernel/arch-arm

LOCAL_SRC_FILES := ts_srv.c

LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)
LOCAL_UNSTRIPPED_PATH := $(TARGET_OUT_EXECUTABLES_UNSTRIPPED)

include $(BUILD_EXECUTABLE)
