// Import wiringPi/I2C library
#include <wiringPi.h>
#include <wiringPiI2C.h>

//Import oled library
#include "ssd1306_i2c.h"

//Import file control library
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
//Import read IP library
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
//Import read disk library
#include <sys/vfs.h>
#include <unistd.h>

#define HARDWARE_PATH "/proc/device-tree/model"
#define TEMP_PATH "/sys/class/thermal/thermal_zone0/temp"
#define MAX_SIZE 32

int main(void) {
  int readed_ip = 0;
  int count = 0;
  int fd_temp;
  int fd_hardware;            //Save the query to the Raspberry Pi hardware version information
  int raspi_version = 0;      //4: Raspberry Pi 4B, 3: Raspberry Pi 3B/3B+ 0: Unrecognized or other version
  double temp = 0, fan_state = 2;
  char buf[MAX_SIZE];

  // get system usage / info
  struct sysinfo sys_info;
  struct statfs disk_info;

  struct ifaddrs *ifAddrStruct = NULL;
  void *tmpAddrPtr = NULL;

  getifaddrs(&ifAddrStruct);

  ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);

  //Define I2C related parameters
  int fd_i2c;
  wiringPiSetup();
  fd_i2c = wiringPiI2CSetup(0x0d);
  while (fd_i2c < 0) {
    fd_i2c = wiringPiI2CSetup(0x0d);
    fprintf(stderr, "fail to init I2C\n");
    delay(500);
  }

  // disable rgb
  wiringPiI2CWriteReg8(fd_i2c, 0x07, 0x00);

  printf("init ok\n");

  while (1) {
    //Read the system information
    if (sysinfo(&sys_info) != 0) // sysinfo(&sys_info) != 0
    {
      printf("sysinfo-Error\n");
      ssd1306_clearDisplay();
      char *text = "sysinfo-Error";
      ssd1306_drawString(text);
      ssd1306_display();
      continue;
    } else {
      //clear display
      ssd1306_clearDisplay();

      //CPU usage
      char CPUInfo[MAX_SIZE] = {0};
      unsigned long avgCpuLoad = sys_info.loads[0] / 1000;
      sprintf(CPUInfo, "CPU:%ld%%", avgCpuLoad);

      //Running memory usage, remaining/total memory
      char RamInfo[MAX_SIZE];
      unsigned long totalRam = sys_info.totalram >> 20;
      unsigned long freeRam = sys_info.freeram >> 20;
      sprintf(RamInfo, "RAM:%ld/%ld MB", freeRam, totalRam);

      //Get IP address
      char IPInfo[MAX_SIZE];
      if (readed_ip == 0) {
        while (ifAddrStruct != NULL) {
          if (ifAddrStruct->ifa_addr->sa_family == AF_INET) { // check it is IP4 is a valid IP4 Address
            tmpAddrPtr = &((struct sockaddr_in *) ifAddrStruct->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

            if (strcmp(ifAddrStruct->ifa_name, "eth0") == 0) {
              sprintf(IPInfo, "eth0:%s", addressBuffer);
              readed_ip = 1;
              break;
            } else if (strcmp(ifAddrStruct->ifa_name, "wlan0") == 0) {
              sprintf(IPInfo, "wlan0:%s", addressBuffer);
              readed_ip = 1;
              break;
            } else {
              readed_ip = 0;
            }
          }
          ifAddrStruct = ifAddrStruct->ifa_next;
        }
        getifaddrs(&ifAddrStruct);
      }

      //Get version of Raspberry Pi
      if (raspi_version == 0) {
        char hardware[MAX_SIZE];
        fd_hardware = open(HARDWARE_PATH, O_RDONLY);
        if (fd_hardware < 0) {
          printf("failed to open /proc/device-tree/model\n");
        } else {
          if (read(fd_hardware, hardware, MAX_SIZE) < 0) {
            printf("fail to read hardware version\n");
          } else {
            //when use Pi 4，hardware=Raspberry Pi 4 Model B Rev 1.1
            char raspi[23] = "0";
            strncpy(raspi, hardware, 22);
            if (strcmp(raspi, "Raspberry Pi 4 Model B") == 0) {
              raspi_version = 4;
            } else if (strcmp(raspi, "Raspberry Pi 3 Model B") == 0) {
              raspi_version = 3;
            }
          }

        }
        close(fd_hardware);
      }

      //Read CPU temperature
      char CPUTemp[MAX_SIZE];
      fd_temp = open(TEMP_PATH, O_RDONLY);
      if (fd_temp < 0) {
        temp = 0;
        printf("failed to open thermal_zone0/temp\n");
      } else {
        //Read temperature and judge
        if (read(fd_temp, buf, MAX_SIZE) < 0) {
          temp = 0;
          printf("fail to read temp\n");
        } else {
          //Convert to floating point printing
          temp = atoi(buf) / 1000.0;
          sprintf(CPUTemp, "%.1fC", temp);
        }
      }
      close(fd_temp);

      //Read disk space, remaining/total space
      char DiskInfo[MAX_SIZE];
      statfs("/", &disk_info);
      unsigned long long totalBlocks = disk_info.f_bsize;
      unsigned long long totalSize = totalBlocks * disk_info.f_blocks;
      size_t mbTotalsize = totalSize >> 20;
      unsigned long long freeDisk = disk_info.f_bfree * totalBlocks;
      size_t mbFreedisk = freeDisk >> 20;
      sprintf(DiskInfo, "Disk:%ld/%ldMB", mbFreedisk, mbTotalsize);

      // display text on OLED
      ssd1306_drawText(78, 0, CPUTemp);
      char *bla = "Sander";
      ssd1306_drawText(0, 0, bla);
      ssd1306_drawText(0, 8, RamInfo);
      ssd1306_drawText(0, 16, CPUInfo);
      ssd1306_drawText(0, 24, IPInfo);

      // refresh display
      ssd1306_display();
      delay(10);
    }

    if (raspi_version == 4) {
      if (temp >= 55) {
        if (fan_state != 1) {
          wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x01);
          fan_state = 1;
        }
      } else if (temp <= 48) {
        if (fan_state != 0) {
          wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x00);
          fan_state = 0;
        }
      }
    } else if (raspi_version == 3) {
      if (temp >= 46) {
        if (fan_state != 1) {
          wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x01);
          fan_state = 1;
        }
      } else if (temp <= 40) {
        if (fan_state != 0) {
          wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x00);
          fan_state = 0;
        }
      }
    } else //if (raspi_version == 0)
    {
      if (temp >= 55) {
        if (fan_state != 1) {
          wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x01);
          fan_state = 1;
        }
      } else if (temp <= 48) {
        if (fan_state != 0) {
          wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x00);
          fan_state = 0;
        }
      }
    }

    delay(500);
    count++;
    delay(500);
  }

  return 0;
}
