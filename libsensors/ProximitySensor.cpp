/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>
#include <stdio.h>
#include <cstring>

#include <cutils/log.h>

#include "ProximitySensor.h"
#include "SensorBase.h"

#define LOG_TAG "ProximitySensor"
#define DEBUG 1

/*****************************************************************************/

ProximitySensor::ProximitySensor()
    : SensorBase(NULL, "proximity_sensor"),
      mEnabled(0),
      mInputReader(4),
      mHasPendingEvent(false)
{
    mPendingEvent.version = sizeof(sensors_event_t);
    mPendingEvent.sensor = ID_P;
    mPendingEvent.type = SENSOR_TYPE_PROXIMITY;
    memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));

    if (data_fd) {
        strcpy(input_sysfs_path, "/sys/class/input/");
        strcat(input_sysfs_path, input_name);
        strcat(input_sysfs_path, "/device/");
        input_sysfs_path_len = strlen(input_sysfs_path);
        enable(0, 1);
    }
}

ProximitySensor::~ProximitySensor() {
    if (mEnabled) {
        enable(0, 0);
    }
}

int ProximitySensor::setInitialState() {
    struct input_absinfo absinfo;

    if (mEnabled) {
    int ret = ioctl(data_fd, EVIOCGABS(EVENT_TYPE_PROXIMITY), &absinfo);

    if (ret) {
        ALOGE("%s: ioctl() failed with %d", __func__, ret);
        return 0;
    }

    // make sure to report an event immediately
    mHasPendingEvent = true;
    mPendingEvent.distance = indexToValue(absinfo.value);
    }

    return 0;
}

int ProximitySensor::setDelay(int32_t handle, int64_t ns)
{
    int fd;
    ALOGE("%s: mEnabled: %d, mHasPendingEvent: %d", __func__, mEnabled, mHasPendingEvent);

    strcpy(&input_sysfs_path[input_sysfs_path_len], "prox_poll_delay");
    fd = open(input_sysfs_path, O_RDWR);
    if (fd >= 0) {
        char buf[80];
        sprintf(buf, "%lld", ns);
        write(fd, buf, strlen(buf)+1);
        close(fd);
        return 0;
    }
    return -1;
}

static void* set_initial_state_fn(void *data) {
    ProximitySensor *sensor = (ProximitySensor*)data;

    ALOGE("%s: start", __func__);
    usleep(100000); // 100ms
    sensor->setDelay(0, 100000);
    ALOGE("%s: end", __func__);

    return NULL;
}

static void set_initial_state_fn(ProximitySensor *sensor) {
       pthread_attr_t thread_attr;
       pthread_t setdelay_thread;

       pthread_attr_init(&thread_attr);
       pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
       int rc = pthread_create(&setdelay_thread, &thread_attr, set_initial_state_fn, (void*)sensor);
       if (rc < 0) {
           ALOGE("%s: Unable to create thread", __func__);
       }
}

int ProximitySensor::enable(int32_t handle, int enabled) {

    int flags = enabled ? 1 : 0;
    int err;

    ALOGE("%s: Enable: %i, mEnabled: %d, mHasPendingEvent: %d", __func__, enabled, mEnabled, mHasPendingEvent);
    err = sspEnable(LOG_TAG, SSP_PROX, enabled);
    if (err < 0) {
        ALOGE("%s: sspEnable returned %d", __func__, err);
        return -1;
    }

    setInitialState();
    set_initial_state_fn(this);

    if (enabled) {
        mEnabled++;
        if (mEnabled > 32767) mEnabled = 32767;
    } else {
        mEnabled--;
        if (mEnabled < 0) mEnabled = 0;
    }

    return 0;
}

bool ProximitySensor::hasPendingEvents() const {
    return mHasPendingEvent;
}

int ProximitySensor::readEvents(sensors_event_t* data, int count)
{
    if (count < 1) {
        ALOGE("%s: count < 1", __func__);
        return -EINVAL;
    }

    if (mHasPendingEvent) {
        mHasPendingEvent = false;
        mPendingEvent.timestamp = getTimestamp();
        *data = mPendingEvent;
        return mEnabled ? 1 : 0;
    }

    ssize_t n = mInputReader.fill(data_fd);
    if (n < 0)
        return n;

    int numEventReceived = 0;
    input_event const* event;

    while (count && mInputReader.readEvent(&event)) {
        int type = event->type;
        if (type == EV_ABS) {
            ALOGE("%s: type=EV_ABS, code=EVENT_TYPE_PROXIMITY, value=%d", __func__, event->value);
            if (event->code == EVENT_TYPE_PROXIMITY) {
                if (event->value != -1) {
                    // FIXME: not sure why we're getting -1 sometimes
                    mPendingEvent.distance = indexToValue(event->value);
                }
            }
        } else if (type == EV_SYN) {
            ALOGE("%s: type=EV_SYN", __func__);
            mPendingEvent.timestamp = timevalToNano(event->time);
            if (mEnabled) {
                *data++ = mPendingEvent;
                count--;
                numEventReceived++;
            }
        } else {
            ALOGE("%s: unknown event (type=%d, code=%d)", __func__,
                    type, event->code);
        }
        mInputReader.next();
    }

    return numEventReceived;
}

float ProximitySensor::indexToValue(size_t index) const
{
    ALOGE_IF(DEBUG, "%s: Index = %zu", __func__, index);
    return index * PROXIMITY_THRESHOLD_CM;
}
