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

#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <linux/i2c.h>
#include "i2c-dev.h"

#include <linux/input.h>
#include <linux/uinput.h>
#include "hsuart.h"
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/select.h>

#include "ts_srv.h"

int touchscreen_power(int enable)
{
	/** TS power stuff */
	int vdd_fd, xres_fd, wake_fd, i2c_fd;

	struct i2c_rdwr_ioctl_data i2c_ioctl_data;
	struct i2c_msg i2c_msg;
	__u8 i2c_buf[16];
	int rc;

	/* TS file descriptors. Ignore errors. */
	vdd_fd = open(CY8CTMA395_VDD, O_WRONLY);
	if (vdd_fd < 0) {
		LOGE("Cannot open vdd - %d", errno);
		return -1;
	}

	xres_fd = open(CY8CTMA395_XRES, O_WRONLY);
	if (xres_fd < 0) {
		LOGE("Cannot open xres - %d", errno);
		return -1;
	}

	wake_fd = open(CY8CTMA395_WAKE, O_WRONLY);
	if (wake_fd < 0) {
		LOGE("Cannot open wake - %d", errno);
		return -1;
	}

	i2c_fd = open("/dev/i2c-5", O_RDWR);
	if (i2c_fd < 0) {
		LOGE("Cannot open i2c dev - %d", errno);
		return -1;
	}

	if (enable) {
		int retry_count = 0;

try_again:
		/* Set reset so the chip immediatelly sees it */
		lseek(xres_fd, 0, SEEK_SET);
		rc = write(xres_fd, "1", 1);
		if (rc != 1) LOGW("failed set xres");

		/* Then power on */
		lseek(vdd_fd, 0, SEEK_SET);
		rc = write(vdd_fd, "1", 1);
		if (rc != 1) LOGW("failed to enable vdd");

		/* Sleep some more for the voltage to stabilize */
		usleep(50000);

		lseek(wake_fd, 0, SEEK_SET);
		rc = write(wake_fd, "1", 1);
		if (rc != 1) LOGW("failed to assert wake");

		lseek(xres_fd, 0, SEEK_SET);
		rc = write(xres_fd, "0", 1);
		if (rc != 1) LOGW("failed to reset xres");

		usleep(50000);

		lseek(wake_fd, 0, SEEK_SET);
		rc = write(wake_fd, "0", 1);
		if (rc != 1) LOGW("failed to deassert wake");

		usleep(50000);

		i2c_ioctl_data.nmsgs = 1;
		i2c_ioctl_data.msgs = &i2c_msg;

		i2c_msg.addr = 0x67;
		i2c_msg.flags = 0;
		i2c_msg.buf = i2c_buf;

		i2c_msg.len = 2;
		i2c_buf[0] = 0x08; i2c_buf[1] = 0;
		rc = ioctl(i2c_fd, I2C_RDWR, &i2c_ioctl_data);
		if (rc != 1) LOGW("ioctl1 failed %d errno %d", rc, errno);

		/* Ok, so the TS failed to wake, we need to retry a few times
		 * before totally giving up */
		if ((rc != 1) && (retry_count++ < 3)) {
			lseek(vdd_fd, 0, SEEK_SET);
			rc = write(vdd_fd, "0", 1);
			usleep(10000);
			LOGW("TS wakeup retry #%d", retry_count);
			goto try_again;
		}

		i2c_msg.len = 6;
		i2c_buf[0] = 0x31; i2c_buf[1] = 0x01; i2c_buf[2] = 0x08;
		i2c_buf[3] = 0x0C; i2c_buf[4] = 0x0D; i2c_buf[5] = 0x0A;
		rc = ioctl(i2c_fd, I2C_RDWR, &i2c_ioctl_data);
		if (rc != 1) LOGW("ioctl2 failed %d errno %d", rc, errno);

		i2c_msg.len = 2;
		i2c_buf[0] = 0x30; i2c_buf[1] = 0x0F;
		rc = ioctl(i2c_fd, I2C_RDWR, &i2c_ioctl_data);
		if (rc != 1) LOGW("ioctl3 failed %d errno %d", rc, errno);

		i2c_buf[0] = 0x40; i2c_buf[1] = 0x02;
		rc = ioctl(i2c_fd, I2C_RDWR, &i2c_ioctl_data);
		if (rc != 1) LOGW("ioctl4 failed %d errno %d", rc, errno);

		i2c_buf[0] = 0x41; i2c_buf[1] = 0x10;
		rc = ioctl(i2c_fd, I2C_RDWR, &i2c_ioctl_data);
		if (rc != 1) LOGW("ioctl5 failed %d errno %d", rc, errno);

		i2c_buf[0] = 0x0A; i2c_buf[1] = 0x04;
		rc = ioctl(i2c_fd, I2C_RDWR, &i2c_ioctl_data);
		if (rc != 1) LOGW("ioctl6 failed %d errno %d", rc, errno);

		i2c_buf[0] = 0x08; i2c_buf[1] = 0x03;
		rc = ioctl(i2c_fd, I2C_RDWR, &i2c_ioctl_data);
		if (rc != 1) LOGW("ioctl7 failed %d errno %d", rc, errno);

		lseek(wake_fd, 0, SEEK_SET);
		rc = write(wake_fd, "1", 1);
		if (rc != 1) LOGW("failed to assert wake again");

	} else {
		// Disable
		lseek(vdd_fd, 0, SEEK_SET);
		rc = write(vdd_fd, "0", 1);
		if (rc != 1) LOGW("failed to disable vdd");

		/* XXX, should be correllated with LIFTOFF_TIMEOUT in ts driver */
		usleep(80000);
	}

	return 0;
}

int send_uevent(int fd, __u16 type, __u16 code, __s32 value)
{
	struct input_event event;

	memset(&event, 0, sizeof(event));
	event.type = type;
	event.code = code;
	event.value = value;
	gettimeofday(&event.time, NULL);

	if (write(fd, &event, sizeof(event)) != sizeof(event))
	{
		LOGE("Error on send_event %d", sizeof(event));
		return -1;
	}

	return 0;
}

int tpcmp(const void *v1, const void *v2)
{
	return ((*(struct candidate *)v2).pw - (*(struct candidate *)v1).pw);
}

int dist(int x1, int y1, int x2, int y2)  {
	return pow(x1 - x2, 2)+pow(y1 - y2, 2);
}

void calc_point()
{
	int i, j;
	int tweight = 0;
	float isum = 0, jsum = 0;
	float avgi, avgj;
	float xval, yval;
	float powered;

	int tpc = 0;
	struct touchpoint tpoint[10];

	int clc = 0;
	struct candidate clist[MAX_CLIST];

	// generate list of high values
	for (i = 0; i < 30; i++)
	{
		for (j = 0; j < 40; j++)
		{
#if DEBUG_RAW
			LOGD("%2.2X ", matrix[i][j]);
#endif
			if (matrix[i][j] > 32 && clc < MAX_CLIST)
			{
				int cvalid=1;
				clist[clc].pw = matrix[i][j];
				clist[clc].i = i;
				clist[clc].j = j;
				//check if local maxima
				if(i>0  && matrix[i-1][j] > matrix[i][j]) cvalid = 0;
				if(i<29 && matrix[i+1][j] > matrix[i][j]) cvalid = 0;
				if(j>0  && matrix[i][j-1] > matrix[i][j]) cvalid = 0;
				if(j<39 && matrix[i][j+1] > matrix[i][j]) cvalid = 0;
				if(cvalid) clc++;
			}
		}
	}

#if DEBUG
	LOGD("%d clc", clc);
#endif

	// sort candidate list by strength
	// qsort(clist, clc, sizeof(clist[0]), tpcmp);

#if DEBUG
	LOGD("%d %d %d", clist[0].pw, clist[1].pw, clist[2].pw);
#endif
	int k, l;
	for (k = 0; k < MIN(clc, 20); k++)
	{
		int newtp = 1;

		int rad = 3; // radius around candidate to use for calculation
		int mini = clist[k].i - rad+1;
		int maxi = clist[k].i + rad;
		int minj = clist[k].j - rad+1;
		int maxj = clist[k].j + rad;

		// discard points close to already detected touches
		for (l = 0; l < tpc; l++)
		{
			// if(tpoint[l].i >= mini
			//              && tpoint[l].i < maxi
			//              && tpoint[l].j >= minj
			//              && tpoint[l].j < maxj) newtp=0;
			if (tpoint[l].i >= mini+1 && tpoint[l].i < maxi-1 &&
					tpoint[l].j >= minj+1 && tpoint[l].j < maxj-1)
			{
				newtp = 0;
			}
		}

		// calculate new touch near the found candidate
		if (newtp && tpc < 10)
		{
			tweight = 0;
			isum = 0;
			jsum = 0;

			for (i = MAX(0, mini); i < MIN(30, maxi); i++)
			{
				for (j = MAX(0, minj); j < MIN(40, maxj); j++)
				{
					//powered = pow(matrix[i][j], 1.5);
#if DEBUG
					LOGD("d= %d", dist(i,j,clist[k].i,clist[k].j));
#endif
					int dd = dist(i, j, clist[k].i, clist[k].j);
					powered = matrix[i][j];
					//if(dd <= 1) powered = matrix[i][j];
					if (dd == 2 && 0.65f * matrix[clist[k].i][clist[k].j] < matrix[i][j])
						powered = 0.65f * matrix[clist[k].i][clist[k].j];

					if (dd == 4 && 0.15f * matrix[clist[k].i][clist[k].j] < matrix[i][j])
						powered = 0.15f * matrix[clist[k].i][clist[k].j];

					if (dd == 5 && 0.10f * matrix[clist[k].i][clist[k].j] < matrix[i][j])
						powered = 0.10f * matrix[clist[k].i][clist[k].j];

					if (dd == 8 && 0.05f * matrix[clist[k].i][clist[k].j] < matrix[i][j])
						powered = 0.05f * matrix[clist[k].i][clist[k].j];

					powered = pow(powered, 1.5);

					tweight += powered;
					isum += powered * i;
					jsum += powered * j;
				}
			}

			avgi = isum / (float)tweight;
			avgj = jsum / (float)tweight;
			tpoint[tpc].pw = tweight;
			tpoint[tpc].i = avgi;
			tpoint[tpc].j = avgj;
			tpc++;

			//yval = 1024 - avgj * 1024 / 39;
			//xval = avgi * 768 / 29;
			yval = 768 - avgi * 768 / 29;
			xval = 1024 - avgj * 1024 / 39;
			int xval_final = (768 - yval) * 1.33;
			int yval_final = xval * 0.752;

			send_uevent(uinput_fd, EV_KEY, BTN_TOUCH, 1);
			send_uevent(uinput_fd, EV_ABS, ABS_MT_PRESSURE, 1);
			send_uevent(uinput_fd, EV_ABS, ABS_MT_POSITION_X, xval_final);
			send_uevent(uinput_fd, EV_ABS, ABS_MT_POSITION_Y, yval_final);
			send_uevent(uinput_fd, EV_SYN, SYN_MT_REPORT, 0);

#if DEBUG
			LOGD("Coords %d %lf, %lf, %lf, %lf, %d",
					tpc, avgi, avgj, xval, yval, tweight);
#endif

		}
	}

	send_uevent(uinput_fd, EV_SYN, SYN_REPORT, 0);
}

void put_byte(unsigned char byte)
{
#if DEBUG
	LOGD("Putc %d %d", cidx, byte);
#endif
	if (cidx == 0 && byte != 0xFF) return;

	// Sometimes a send is aborted by the touch screen.
	// all we get is an out of place 0xFF
	if (byte == 0xFF && !cline_valid(1)) cidx = 0;
	cline[cidx++] = byte;
}

int cline_valid(int extras)
{
	if(cline[0] == (unsigned) 0xff && cline[1] == (unsigned) 0x43
	   && cidx == (unsigned) (44 - extras))
	{
#if DEBUG
		LOGD("cidx %d", cline[cidx-1]);
#endif
		return 1;
	}

	if (cline[0] == (unsigned) 0xff && cline[1] == (unsigned) 0x47
	    && cidx > (unsigned) 4 && cidx == (unsigned) (cline[2] + 4 - extras))
	{
#if DEBUG
		LOGD("cidx %d", cline[cidx-1]);
#endif
		return 1;
	}

	return 0;
}

void consume_line()
{
	int i, j;

	if(cline[1] == 0x47)
	{
		// Calculate the data points. All transfers complete.
		calc_point();
	}

	if(cline[1] == 0x43)
	{
		// This is a start event. Clear the matrix.
		if(cline[2] & 0x80)
		{
			for(i = 0; i < 30; i++)
				for(j = 0; j < 40; j++)
					matrix[i][j] = 0;
		}

		// Write the line into the matrix.
		for(i = 0; i < 40; i++)
			matrix[cline[2] & 0x1F][i] = cline[i+3];
	}

#if DEBUG
	LOGD("Received %d bytes", cidx-1);

	for(i = 0; i < cidx; i++)
		LOGD("%2.2X ",cline[i]);
#endif

	cidx = 0;
}

void snarf2(unsigned char* bytes, int size)
{
	int i = 0;

	while (i < size)
	{
		while (i < size)
		{
			put_byte(bytes[i]);
			i++;
			if (cline_valid(0))
			{
#if DEBUG
				LOGD("was valid");
#endif
				break;
			}
		}

		if(i >= size)
			break;

#if DEBUG
		LOGD("Cline went valid");
#endif
		consume_line();
	}

	if(cline_valid(0))
	{
		consume_line();
#if DEBUG
		LOGD("was valid2");
#endif
	}
}

void open_uinput()
{
	struct uinput_user_dev device;
	struct input_event myevent;
	int i,ret = 0;

	memset(&device, 0, sizeof device);

	uinput_fd = open(UINPUT_DEV, O_WRONLY);
	strcpy(device.name, "cy8c-touchscreen");

	device.id.bustype = BUS_VIRTUAL;
	device.id.vendor = 1;
	device.id.product = 1;
	device.id.version = 1;


	for (i = 0; i < ABS_MAX; i++)
	{
		device.absmax[i] = -1;
		device.absmin[i] = -1;
		device.absfuzz[i] = -1;
		device.absflat[i] = -1;
	}

	device.absmax[ABS_Y] = 1024;
	device.absmax[ABS_X] = 768;

	device.absmin[ABS_X] = 0;
	device.absfuzz[ABS_X] = 1;
	device.absflat[ABS_X] = 0;
	device.absmin[ABS_Y] = 0;
	device.absfuzz[ABS_Y] = 1;
	device.absflat[ABS_Y] = 0;
	device.absmin[ABS_PRESSURE] = 0;
	device.absmax[ABS_PRESSURE] = 1;
	device.absfuzz[ABS_PRESSURE] = 0;
	device.absflat[ABS_PRESSURE] = 0;

	device.absmax[ABS_MT_POSITION_X] = 1024;
	device.absmax[ABS_MT_POSITION_Y] = 768;

	device.absmin[ABS_MT_POSITION_X] = 0;
	device.absfuzz[ABS_MT_POSITION_X] = 1;
	device.absflat[ABS_MT_POSITION_X] = 0;
	device.absmin[ABS_MT_POSITION_Y] = 0;
	device.absfuzz[ABS_MT_POSITION_Y] = 1;
	device.absflat[ABS_MT_POSITION_Y] = 0;
	device.absmax[ABS_MT_TOUCH_MAJOR] = 1;
	device.absmax[ABS_MT_WIDTH_MAJOR] = 100;

	if (write(uinput_fd, &device, sizeof(device)) != sizeof(device))
	{
		LOGE("error setup");
	}

	if (ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY) < 0)
		LOGE("error evbit key");

	if (ioctl(uinput_fd, UI_SET_EVBIT, EV_SYN) < 0)
		LOGE("error evbit key");

	if (ioctl(uinput_fd, UI_SET_EVBIT, EV_ABS) < 0)
		LOGE("error evbit rel");

	if (ioctl(uinput_fd, UI_SET_ABSBIT, ABS_X) < 0)
		LOGE("error x rel");

	if (ioctl(uinput_fd, UI_SET_ABSBIT, ABS_Y) < 0)
		LOGE("error y rel");

	if (ioctl(uinput_fd, UI_SET_ABSBIT, ABS_PRESSURE) < 0)
		LOGE("error pressure rel");

	if (ioctl(uinput_fd, UI_SET_ABSBIT, ABS_TOOL_WIDTH) < 0)
		LOGE("error tool rel");

	if (ioctl(uinput_fd, UI_SET_ABSBIT, ABS_MT_TOUCH_MAJOR) < 0)
		LOGE("error tool rel");

	if (ioctl(uinput_fd, UI_SET_ABSBIT, ABS_MT_WIDTH_MAJOR) < 0)
		LOGE("error tool rel");

	if (ioctl(uinput_fd, UI_SET_ABSBIT, ABS_MT_POSITION_X) < 0)
		LOGE("error tool rel");

	if (ioctl(uinput_fd, UI_SET_ABSBIT, ABS_MT_POSITION_Y) < 0)
		LOGE("error tool rel");

	if (ioctl(uinput_fd, UI_SET_ABSBIT, ABS_MT_PRESSURE) < 0)
		LOGE("error tool rel");

	if (ioctl(uinput_fd, UI_SET_KEYBIT, BTN_TOUCH) < 0)
		LOGE("error keybit rel");

	if (ioctl(uinput_fd, UI_DEV_CREATE) < 0)
	{
		LOGE("error create");
	}
}

int main(int argc, char** argv)
{
	struct hsuart_mode uart_mode;
	int uart_fd, nbytes, i;
	char recv_buf[RECV_BUF_SIZE];
	fd_set fdset;
	struct timeval seltmout;

	if (touchscreen_power(1)) {
		LOGE("Unable to enable touchscreen power");
		return 1;
	}

	uart_fd = open("/dev/ctp_uart", O_RDONLY | O_NONBLOCK);
	if (uart_fd <= 0)
	{
		LOGE("Could not open uart");
		return 1;
	}

	open_uinput();

	ioctl(uart_fd, HSUART_IOCTL_GET_UARTMODE, &uart_mode);
	uart_mode.speed = 0x3D0900;
	ioctl(uart_fd, HSUART_IOCTL_SET_UARTMODE, &uart_mode);

	ioctl(uart_fd, HSUART_IOCTL_FLUSH, 0x9);

	LOGI("ts_srv has arrived");

	while (1)
	{
		// usleep(50000);
		FD_ZERO(&fdset);
		FD_SET(uart_fd, &fdset);
		seltmout.tv_sec = 0;
		/* 2x tmout */
		seltmout.tv_usec = LIFTOFF_TIMEOUT;

		if (0 == select(uart_fd+1, &fdset, NULL, NULL, &seltmout)) {
			/* Timeout means liftoff, send event */
#if DEBUG
			LOGD("timeout! sending liftoff");
#endif
			send_uevent(uinput_fd, EV_KEY, BTN_TOUCH, 0);
			send_uevent(uinput_fd, EV_ABS, ABS_MT_PRESSURE, 0);
			send_uevent(uinput_fd, EV_SYN, SYN_MT_REPORT, 0);

			send_uevent(uinput_fd, EV_SYN, SYN_REPORT, 0);

			FD_ZERO(&fdset);
			FD_SET(uart_fd, &fdset);

			/* Now enter indefinite sleep iuntil input appears */
			select(uart_fd+1, &fdset, NULL, NULL, NULL);
			/* In case we were wrongly woken up check the event
			 * count again */
			continue;
		}
		nbytes = read(uart_fd, recv_buf, RECV_BUF_SIZE);

		if (nbytes <= 0)
			continue;

#if DEBUG
		LOGD("Received %d bytes", nbytes);

		for(i=0; i < nbytes; i++)
			LOGD("%2.2X ", recv_buf[i]);
#endif
		send_uevent(uinput_fd, EV_ABS, ABS_MT_TRACKING_ID, 1);
		snarf2((unsigned char*) recv_buf, nbytes);
	}

	/* never reached? */
	return 0;
}

