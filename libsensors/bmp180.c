/*
 * Copyright (C) 2019 Răileanu Cosmin <comico_work@outlook.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <linux/ioctl.h>
#include <linux/input.h>

#include <hardware/sensors.h>
#include <hardware/hardware.h>

#define LOG_TAG "Pressure_NoteII"
#include <utils/Log.h>

#include "noteII_sensors.h"
#include "ssp.h"

extern int mFlushed;

struct bmp180_data {
	char path_delay[PATH_MAX];
};

int bmp180_init(struct noteII_sensors_handlers *handlers,
	struct smdk4x12_sensors_device *device)
{
	struct bmp180_data *data = NULL;
	char path[PATH_MAX] = { 0 };
	int input_fd = -1;
	int rc;

	//ALOGD("%s(%p, %p)", __func__, handlers, device);

	if (handlers == NULL)
		return -EINVAL;

	data = (struct bmp180_data *) calloc(1, sizeof(struct bmp180_data));

	input_fd = input_open("pressure_sensor");
	if (input_fd < 0) {
		//ALOGD("%s: Unable to open input", __func__);
		goto error;
	}

	rc = sysfs_path_prefix("pressure_sensor", (char *) &path);
	if (rc < 0 || path[0] == '\0') {
		//ALOGD("%s: Unable to open sysfs", __func__);
		goto error;
	}

	int sf = snprintf(data->path_delay, PATH_MAX, "%s/poll_delay", path);
	if(sf <= 0)
	{
		//ALOGD("Pressure HAS FAILED !POLL_DELAY!");
		goto error;
	}

	handlers->poll_fd = input_fd;
	handlers->data = (void *) data;

	return 0;

error:
	if (data != NULL)
		free(data);

	if (input_fd >= 0)
		close(input_fd);

	handlers->poll_fd = -1;
	handlers->data = NULL;

	return -1;
}

int bmp180_deinit(struct noteII_sensors_handlers *handlers)
{
	//ALOGD("%s(%p)", __func__, handlers);

	if (handlers == NULL)
		return -EINVAL;

	if (handlers->poll_fd >= 0)
		close(handlers->poll_fd);
	handlers->poll_fd = -1;

	if (handlers->data != NULL)
		free(handlers->data);
	handlers->data = NULL;

	return 0;
}

int bmp180_activate(struct noteII_sensors_handlers *handlers)
{
	struct bmp180_data *data;
	int rc;

	//ALOGD("%s(%p)", __func__, handlers);

	if (handlers == NULL || handlers->data == NULL)
		return -EINVAL;

	data = (struct bmp180_data *) handlers->data;

	rc = ssp_sensor_enable(PRESSURE_SENSOR);
	if (rc < 0) {
		//ALOGD("%s: Unable to enable ssp sensor", __func__);
		return -1;
	}

	handlers->activated = 1;

	return 0;
}

int bmp180_deactivate(struct noteII_sensors_handlers *handlers)
{
	struct bmp180_data *data;
	int rc;

	//ALOGD("%s(%p)", __func__, handlers);

	if (handlers == NULL || handlers->data == NULL)
		return -EINVAL;

	data = (struct bmp180_data *) handlers->data;

	rc = ssp_sensor_disable(PRESSURE_SENSOR);
	if (rc < 0) {
		//ALOGD("%s: Unable to disable ssp sensor", __func__);
		return -1;
	}

	handlers->activated = 0;
	
	return 0;
}

int bmp180_set_delay(struct noteII_sensors_handlers *handlers, int64_t delay)
{
	struct bmp180_data *data;
	int rc;

	//ALOGD("%s(%p, %" PRId64 ")", __func__, handlers, delay);

	if (handlers == NULL || handlers->data == NULL)
		return -EINVAL;

	data = (struct bmp180_data *) handlers->data;

	rc = sysfs_value_write(data->path_delay, (int) delay);
	if (rc < 0) {
		//ALOGD("%s: Unable to write sysfs value", __func__);
		return -1;
	}

	return 0;
}

float bmp180_convert(int value)
{
	return value / 100.0f;
}

int bmp180_get_data(struct noteII_sensors_handlers *handlers,
	struct sensors_event_t *event)
{
	struct input_event input_event;
	int input_fd;
	int rc;
	int sensorId = SENSOR_TYPE_PRESSURE;

	if (handlers == NULL || event == NULL)
		return -EINVAL;

	if (mFlushed & (1 << sensorId)) { /* Send flush META_DATA_FLUSH_COMPLETE immediately */
		sensors_event_t sensor_event;
		memset(&sensor_event, 0, sizeof(sensor_event));
		sensor_event.version = META_DATA_VERSION;
		sensor_event.type = SENSOR_TYPE_META_DATA;
		sensor_event.meta_data.sensor = sensorId;
		sensor_event.meta_data.what = 0;
		*event++ = sensor_event;
		mFlushed &= ~(0x01 << sensorId);
		//ALOGD("AkmSensor: %s Flushed sensorId: %d", __func__, sensorId);
	}

	input_fd = handlers->poll_fd;
	if (input_fd < 0)
		return -EINVAL;

	memset(event, 0, sizeof(struct sensors_event_t));
	event->version = sizeof(struct sensors_event_t);
	event->sensor = handlers->handle;
	event->type = handlers->handle;

	do {
		rc = read(input_fd, &input_event, sizeof(input_event));
		if (rc < (int) sizeof(input_event))
			break;

		if (input_event.type == EV_REL) {
			switch (input_event.code) {
				case REL_HWHEEL:
					event->pressure = bmp180_convert(input_event.value);
					break;
				default:
					continue;
			}
		} else if (input_event.type == EV_SYN) {
			if (input_event.code == SYN_REPORT && event->pressure != 0) {
				event->timestamp = getTimestamp();
				break;
			} else {
				return -1;
			}
		}
	} while (1);

	return 0;
}

struct noteII_sensors_handlers bmp180 = {
	.name = "BMP180",
	.handle = SENSOR_TYPE_PRESSURE,
	.init = bmp180_init,
	.deinit = bmp180_deinit,
	.activate = bmp180_activate,
	.deactivate = bmp180_deactivate,
	.set_delay = bmp180_set_delay,
	.get_data = bmp180_get_data,
	.activated = 0,
	.needed = 0,
	.poll_fd = -1,
	.data = NULL,
};
