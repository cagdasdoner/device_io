#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/poll.h>
 
#define BUFFER_LENGTH 256
#define TIMEOUT 30
static char receive[BUFFER_LENGTH];

int main(){
   int ret;
   struct pollfd pofd;

   if(!(pofd.fd = open("/dev/chrdrv", O_RDONLY | O_NONBLOCK))) {
      printf("Failed to open up the devdrv.\n");
      return -1;
   }
   printf("Device is opened.\n");

   pofd.events = POLLIN;
   pofd.revents = 0;
   ret = poll(&pofd, 1, TIMEOUT * 1000);

   if (ret < 0) {
      printf("Poll failed with : %d\n", ret);
      return ret;
   }
   if (pofd.revents | POLLIN) {
      printf("POLLIN done, reading from the device.\n");
      ret = read(pofd.fd, receive, BUFFER_LENGTH);
      if (ret < 0){
         printf("Failed to read the message from the device.\n");
         return ret;
      }
      printf("The received message is: [%s]\n", receive);
   }

   close(pofd.fd);

   return 0;
}