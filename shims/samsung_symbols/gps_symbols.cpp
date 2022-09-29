/*
 * Copyright (C) 2020 The LineageOS Project
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
#include <string.h>
#include <openssl/ssl.h>

// android::SensorManager::getSensorList(android::Sensor const* const**) const
extern "C" void _ZNK7android13SensorManager13getSensorListEPPKPKNS_6SensorE();
extern "C" void _ZNK7android13SensorManager13getSensorListEPPKPKNS_6SensorE(){}

extern "C" void _ZN7android16SensorEventQueue4readEP12ASensorEventj();
extern "C" void _ZN7android16SensorEventQueue4readEP12ASensorEventj(){}

extern "C" void _ZNK7android16SensorEventQueue5getFdEv();
extern "C" void _ZNK7android16SensorEventQueue5getFdEv(){}

extern "C" void _ZN7android13SensorManager16getDefaultSensorEi();
extern "C" void _ZN7android13SensorManager16getDefaultSensorEi(){}

extern "C" void _ZNK7android6Sensor7getNameEv();
extern "C" void _ZNK7android6Sensor7getNameEv(){}

extern "C" void _ZNK7android16SensorEventQueue12enableSensorEPKNS_6SensorE();
extern "C" void _ZNK7android16SensorEventQueue12enableSensorEPKNS_6SensorE(){}

extern "C" void _ZNK7android16SensorEventQueue12setEventRateEPKNS_6SensorEx();
extern "C" void _ZNK7android16SensorEventQueue12setEventRateEPKNS_6SensorEx(){}

extern "C" void _ZNK7android16SensorEventQueue13disableSensorEPKNS_6SensorE();
extern "C" void _ZNK7android16SensorEventQueue13disableSensorEPKNS_6SensorE(){}

extern "C" void _ZN7android8String16aSEOS0_();
extern "C" void _ZN7android8String16aSEOS0_(){}

extern "C" void _ZN7android8String16C1EOS0_();
extern "C" void _ZN7android8String16C1EOS0_(){}

extern "C" void _ZN7android9SingletonINS_13SensorManagerEE5sLockE();
extern "C" void _ZN7android9SingletonINS_13SensorManagerEE5sLockE(){}

extern "C" void _ZN7android9SingletonINS_13SensorManagerEE9sInstanceE();
extern "C" void _ZN7android9SingletonINS_13SensorManagerEE9sInstanceE(){}

extern "C" void _ZN7android13SensorManager16createEventQueueEv();
extern "C" void _ZN7android13SensorManager16createEventQueueEv(){}

extern "C" void _ZN7android13SensorManagerC1Ev();
extern "C" void _ZN7android13SensorManagerC1Ev(){}

extern "C" {

const SSL_METHOD *SSLv3_method(void) {
    const SSL_METHOD *kMethod = TLSv1_method();
    char ssl_version[] = {0, 0x03}; // 0x0300

    memcpy((void*)kMethod, ssl_version, 1);

    return kMethod;
}

const SSL_METHOD *SSLv3_client_method(void) {
  return SSLv3_method();
}

}
