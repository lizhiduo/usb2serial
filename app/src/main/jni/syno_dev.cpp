//
// Created by lizd on 2017/9/15.
//
#include <string.h>

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>



#include "termios.h"

#include "syno_dev.h"

#include "myLog.h"



int SPEED = 115200;

int syno_fd = -1;

char g_uart_dev[20]={0};
int MAX_TRY = 4;

int get_port_dev();

static speed_t getBaudrate(int baudrate)
{
	switch(baudrate) {
        case 0: return B0;
        case 50: return B50;
        case 75: return B75;
        case 110: return B110;
        case 134: return B134;
        case 150: return B150;
        case 200: return B200;
        case 300: return B300;
        case 600: return B600;
        case 1200: return B1200;
        case 1800: return B1800;
        case 2400: return B2400;
        case 4800: return B4800;
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
        case 460800: return B460800;
        case 500000: return B500000;
        case 576000: return B576000;
        case 921600: return B921600;
        case 1000000: return B1000000;
        case 1152000: return B1152000;
        case 1500000: return B1500000;
        case 2000000: return B2000000;
        case 2500000: return B2500000;
        case 3000000: return B3000000;
        case 3500000: return B3500000;
        case 4000000: return B4000000;
        default: return -1;
	}
}


int set_dev_mode(speed_t speed, int dataWidth, int nStop, unsigned char waitTime, bool isWait)
{
    struct termios new_cfg,old_cfg;
    int fd = 0;
    if(syno_fd < 0){
        LOGE("The dev is not open !");
        return -1;
    }

    fd = syno_fd;
    //preserve the old configuration
    if(tcgetattr(fd,&old_cfg)!=0){
        LOGE("tcgetattr() failed");
        return -1;
    }

    //set mode
    new_cfg.c_cflag |= CLOCAL | CREAD;

    new_cfg.c_lflag &=~ICANON; //原始模式

    //new_cfg.c_lflag |=ICANON;

    //set baudrate
    cfsetispeed(&new_cfg,speed);
    cfsetospeed(&new_cfg,speed);

    //set size of data
    new_cfg.c_cflag &=~CSIZE;
    switch(dataWidth){
        case 5:{
            new_cfg.c_cflag |= CS5;
        }break;
        case 6:{
            new_cfg.c_cflag |= CS6;
        }break;
        case 7:{
            new_cfg.c_cflag |= CS7;
        }break;
        case 8:{
            new_cfg.c_cflag |= CS8;
        }break;
        default :{
            new_cfg.c_cflag |= CS8;
        }break;
    }

    //set stopbit
    if(nStop==2)
        new_cfg.c_cflag |=(CSTOPB);
    else if(nStop==1)
        new_cfg.c_cflag &=~(CSTOPB);

    //set wait time
    if(isWait){
         new_cfg.c_cc[VTIME]= 5; //100 * 100ms
         new_cfg.c_cc[VMIN] = 0;
         //new_cfg.c_cc[VMIN] = 1;
    }else{
        new_cfg.c_cc[VTIME]= waitTime;  //n*100ms
        //new_cfg.c_cc[VTIME]= 0;
        new_cfg.c_cc[VMIN] = 0;
    }


    //start
    if(tcsetattr(fd, TCSANOW, &new_cfg) != 0){
        LOGE("tcsetattr() failed");
        return -1;
    }

    return 0;
}


int open_dev(char *uartDevicePath){
    int ret = 0;
     #ifdef DEBUG_DEV
        LOGI("uart path=%s", g_uart_dev);
     #endif

     ret = get_port_dev();
     if(ret<0){
        LOGE("GET PORT FAILED...");
        return -1;
     }
     LOGI("%s is open ...", g_uart_dev);
     //open device
    syno_fd = open(g_uart_dev, O_RDWR | O_NOCTTY ); //
    if(syno_fd == -1){
        LOGE("OPEN %s FAILED ...", g_uart_dev);
        return -1;
    }

    return syno_fd;
}

int read_dev(unsigned char time, string &data, bool isWait){
    char buf[READLENGTH+1] = {0};
    int nread = 0, bytes = 0;
    int ret = 0;
    string ret_buf = "";
    bool isRead = false;

    speed_t speed = getBaudrate(SPEED);
    //set mode
     ret = set_dev_mode( speed, 8, 1, 5, isWait);
     if(ret<0){
        LOGE("set_dev_mode fail...");
        return -1;
     }
    tcflush(syno_fd, TCIFLUSH); //清空读缓存
    int count = 0;
    while(1){

        memset(buf, 0, READLENGTH);
        nread = read(syno_fd, buf, READLENGTH);
        #ifdef DEBUG_DEV
        LOGI("nread : %d ", nread);
        #endif

        //阻塞和非阻塞的不同处理
        if(isWait){
            if(nread == 0 && isRead){
                #ifdef DEBUG_DEV
                LOGI("read ok .... \n break...");
                #endif
                break;
            }
            if(nread == 0){
                continue;
            }
            isRead = true;
        }else{

             if(nread <= 0  && count*5 >= time){
                LOGE("NO READ DATA...");
                return -1;
            }
            if(nread == 0  && isRead){
                #ifdef DEBUG_DEV
                     LOGI("count : %d ", count);
                     LOGI("break ....");
                 #endif
                 break;
            }
            if(nread == 0 && !isRead){
                count++;
                continue;
            }
            isRead = true;
        }


        string tmp = buf;
        #ifdef DEBUG_DEV
            LOGI("count : %d  len: %d  %s", count, tmp.length(), tmp.c_str());
        #endif
        ret_buf = ret_buf.append(tmp);
        #ifdef DEBUG_DEV
            LOGI("count : %d  len: %d  %s", count, ret_buf.length(), ret_buf.c_str());
        #endif

        count++;
    }
    //
    //LOGI("count : %d  len: %d  %s", count, ret_buf.length(), ret_buf.c_str());

    data = ret_buf;

    return nread;
}



int check_dev(char *uart_dev){
    char buff[READLENGTH] = {'\0'};
    int nread = 0, wr_num = 0;
    int ret = 0;

    char cmd[]={0x7e, 0x00, 0x07, 0x01, 0x00, 0xf0, 0x0f, 0xab, 0xcd};

     if((access(uart_dev, F_OK)) == -1){
        LOGE("%s is not exit...", uart_dev);
        return -1;
    }


    syno_fd = open(uart_dev, O_RDWR | O_NOCTTY ); // O_NONBLOCK
    if(syno_fd < 0){
        LOGE("OPEN %s FAILED ...", uart_dev);
        return -1;
    }
    speed_t speed = getBaudrate(SPEED);
    ret = set_dev_mode(speed, 8, 1, 1, false);
    if(ret<0){
        LOGE("set_dev_mode fail...");
        close(syno_fd);
     }

    wr_num = write(syno_fd, cmd, sizeof(cmd));
    if( wr_num != sizeof(cmd) ){
        LOGE("Write CMD failed ...");
        close(syno_fd);
        return -1;
    }


    if( (nread = read(syno_fd, buff, READLENGTH) ) >0 ){

        buff[nread+1] = '\0';
        #if 1
        LOGI("len: %d  flag: %x  str: %s\n", nread, buff[2], &buff[4]);
        #endif
        if( memcmp(&buff[4], "SD-MG1S02", 5) == 0){
            memcpy(g_uart_dev, uart_dev, strlen(uart_dev));
        }

    }else{
        LOGE("READ FAILED...");
        close(syno_fd);
        return -1;
    }

    close(syno_fd);

    return 0;
}

int get_port_dev(){
    int ret = 0;
    char PathName[20] = "/dev/ttyUSB";
    char uart_dev[20];

    for(int i=0; i<MAX_TRY; i++){
        memset(uart_dev, 0, 20);
        sprintf(uart_dev, "%s%d", PathName, i);

        //LOGI("name: %s", uart_dev);
        if(i == (MAX_TRY -1)){return -1;}
        //ret = check_dev(uart_dev);
        if((access(uart_dev, F_OK)) == -1){
                LOGE("%s is not exit...", uart_dev);
                continue;
        }else{
            memcpy(g_uart_dev, uart_dev, strlen(uart_dev));
            break;
        }

        //if(ret == 0){break;}
    }

    return 0;
}