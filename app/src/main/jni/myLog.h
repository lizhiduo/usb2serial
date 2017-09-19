//
// Created by lizd on 2017/9/15.
//

#ifndef SYNO_QRCODE_MYLOG_H
#define SYNO_QRCODE_MYLOG_H

#include <android/log.h>

#define  LOG_TAG    "syno_dev_ops"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,__VA_ARGS__)


#endif //SYNO_QRCODE_MYLOG_H
