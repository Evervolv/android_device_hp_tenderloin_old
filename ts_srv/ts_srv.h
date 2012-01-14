/*
 * This is a userspace touchscreen driver for cypress ctma395 as used
 * in HP Touchpad configured for Android.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 * The code was written from scratch:
 * the hard math and understanding the device output by jonpry @ gmail
 * uinput bits and the rest by Oleg Drokin green@linuxhacker.ru
 * Multitouch detection by Rafael Brune mail@rbrune.de
 * General fixes and cleanups by Wong Yong Jie yjwong92 @ gmail
 *
 * Copyright (c) 2011 CyanogenMod Touchpad Project.
 */

/* Use Android logging system */
#include <android/log.h>
#define LOG_TAG "ts_srv"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

/* Defines for debugging */
#define DEBUG 0
#define DEBUG_RAW 0

#define RECV_BUF_SIZE 1540
#define LIFTOFF_TIMEOUT 40000 /* 20 ms */

#define MAX_CLIST 75

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

/* Location of sysfs and device nodes */
#define UINPUT_DEV "/dev/uinput"
#define CY8CTMA395_VDD "/sys/devices/platform/cy8ctma395/vdd"
#define CY8CTMA395_XRES "/sys/devices/platform/cy8ctma395/xres"
#define CY8CTMA395_WAKE "/sys/user_hw/pins/ctp/wake/level"

unsigned char cline[64];
unsigned int cidx = 0;
unsigned char matrix[30][40];
int uinput_fd;

struct candidate {
        int pw;
        int i;
        int j;
};

struct touchpoint {
        int pw;
        float i;
        float j;
};

int touchscreen_power(int enable);
int send_uevent(int fd, __u16 type, __u16 code, __s32 value);
int tpcmp(const void *v1, const void *v2);
int dist(int x1, int y1, int x2, int y2);
void calc_point();
void put_byte(unsigned char byte);
int cline_valid(int extras);
void consume_line();
void snarf2(unsigned char* bytes, int size);
void open_uinput();

