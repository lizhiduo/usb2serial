//
// Created by lizd on 2017/9/14.
//

#include "com_ccb_dev_interfaces_Qrcode.h"

#include <stdlib.h>
#include <unistd.h>


#include "syno_dev.h"

#include "myLog.h"

static char UART_DEV[20] = "/dev/ttyUSB0";

char g_coded_format[20] = "utf-8";

enum state{
    NO_DEV,
    CLOSE_STATE,
    OPEN_AND_READ,
    OPEN_NO_READ,
    IS_READ_WAIT
}dev_state = CLOSE_STATE;

/**
 * C的字符串转jstring的字符串
 */
static char* jstring2string(JNIEnv* env, jstring jstr)
{
	char* rtn = NULL;
	jclass clsstring = (env)->FindClass( "java/lang/String");
	jstring strencode = (env)->NewStringUTF( g_coded_format);
	jmethodID mid = (env)->GetMethodID( clsstring, "getBytes", "(Ljava/lang/String;)[B");
	jbyteArray barr= (jbyteArray)(env)->CallObjectMethod( jstr, mid, strencode);
	jsize alen = (env)->GetArrayLength( barr);
	jbyte* ba = (env)->GetByteArrayElements( barr, JNI_FALSE);

	if (alen > 0)
	{
		rtn = (char*)malloc(alen + 1);
		memcpy(rtn, ba, alen);
		rtn[alen] = 0;
	}

	(env)->ReleaseByteArrayElements(barr, ba, 0);
	return rtn;
}

static jstring str2jstring(JNIEnv* env, const char* pStr)
{
    int        strLen    = strlen(pStr);
    jclass     jstrObj   = (env)->FindClass( "java/lang/String");
    jmethodID  methodId  = (env)->GetMethodID( jstrObj, "<init>", "([BLjava/lang/String;)V");
    jbyteArray byteArray = (env)->NewByteArray( strLen);
    jstring    encode    = (env)->NewStringUTF( g_coded_format);  //utf-8 GB2312  GBK

    (env)->SetByteArrayRegion( byteArray, 0, strLen, (jbyte*)pStr);
    jstring retStr = (jstring)(env)->NewObject( jstrObj, methodId, byteArray, encode);
    (env)->DeleteLocalRef( byteArray );
    (env)->DeleteLocalRef( encode );
    return retStr;
}



JNIEXPORT jint JNICALL Java_com_ccb_dev_interfaces_Qrcode_open_1qrcode_1dev
  (JNIEnv *, jclass){

    #ifdef DEBUG_DEV
        LOGI("%s runing...", __func__);
    #endif
    int ret = 0;

    ret = open_dev(UART_DEV);
    if( ret < 0 ){
        LOGE("open_dev ERROR");
        dev_state = NO_DEV;
        return -1;
    }
    dev_state = OPEN_AND_READ;

    return 0;
  }


  JNIEXPORT jint JNICALL Java_com_ccb_dev_interfaces_Qrcode_close_1qrcode_1dev
    (JNIEnv *, jclass){

        #ifdef DEBUG_DEV
                LOGI("%s runing...", __func__);
        #endif

        close(syno_fd);

        dev_state = CLOSE_STATE;

        return 0;
    }

JNIEXPORT jstring JNICALL Java_com_ccb_dev_interfaces_Qrcode_read_1qrcode_1dev
  (JNIEnv *env, jclass, jint time){
        #ifdef DEBUG_DEV
            LOGI("%s runing...", __func__);
        #endif

        jstring ret;
        unsigned char waitTime = 0;
        int tmp = 0;
        bool isWait = false;

        string buf = "";

        if(time < 0){
            isWait = true;
        }else{
            isWait = false;
            tmp = time/100;
        }

        waitTime = (unsigned char) tmp;
        #ifdef DEBUG_DEV
            LOGI("waitTime : %d   time : %d", waitTime, time);
        #endif

        dev_state = IS_READ_WAIT;
        int ndata = 0;

        ndata = read_dev(waitTime, buf, isWait);

        dev_state = OPEN_AND_READ;
        if(ndata < 0 ){
            LOGE("READ ERROR");
            return NULL;
        }

         #ifdef DEBUG_DEV
            LOGI("[%s] buf : %s", __func__, buf.c_str());
         #endif

       #if 1
        LOGI("len: %d buf : %s", buf.length(), buf.c_str());
       #endif

        ret = str2jstring(env, buf.c_str());


        return ret;
  }



JNIEXPORT jint JNICALL Java_com_ccb_dev_interfaces_Qrcode_touch_1qrcode_1dev
  (JNIEnv *, jclass){

        return dev_state;
  }

  JNIEXPORT jint JNICALL Java_com_ccb_dev_interfaces_Qrcode_set_1port_1speed
    (JNIEnv *, jclass, jint speed){

        //LOGI("%s SPEED: %d", __func__, SPEED);

        if(speed>0 && speed <= 1200){
            SPEED = 1200;
        }else if(speed>1200 && speed <= 4800){
            SPEED = 4800;
        }else if(speed>4800 && speed <= 9600){
             SPEED = 9600;
        }else if(speed>9600 && speed <= 14400){
             SPEED = 14400;
        }else if(speed>14400 && speed <= 19200){
            SPEED = 19200;
        }else if(speed>19200 && speed <= 38400){
             SPEED = 38400;
         }else if(speed>38400 && speed <= 57600){
           SPEED = 57600;
       }else if(speed>57600 && speed <= 115200){
            SPEED = 115200;
        }
        //LOGI("%s SPEED: %d", __func__, SPEED);
        #ifdef DEBUG_DEV
            LOGI("%s SPEED: %d", __func__, SPEED);
         #endif

        return 0;
    }

    JNIEXPORT jint JNICALL Java_com_ccb_dev_interfaces_Qrcode_set_1coded_1format
      (JNIEnv *env, jclass, jstring encode){
            char *buf;

            if(encode != NULL){
                buf = jstring2string(env, encode);

                memset(g_coded_format, 0, 20);
                memcpy(g_coded_format, buf, strlen(buf));

                LOGI("format: %s  %s", buf, g_coded_format);

                free(buf);
            }

            return 0;

      }