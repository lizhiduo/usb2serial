//
// Created by lizd on 2017/9/15.
//

#ifndef SYNO_QRCODE_SYNO_DEV_H
#define SYNO_QRCODE_SYNO_DEV_H

#include <string>

using namespace std;

#define READLENGTH  512

extern int syno_fd;

extern int open_dev(char *uartDevicePath);
extern int read_dev(unsigned char time, string &data, bool isWait);
extern int read_dev_block(string &data, bool isWait);

#endif //SYNO_QRCODE_SYNO_DEV_H
