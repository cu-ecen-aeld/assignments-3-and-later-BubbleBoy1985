#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>


int main(int argc, char *argv[])
{  
   //configure syslog
   openlog("writer", LOG_PID, LOG_USER);
   
   //check input arguments
   if (argc < 2)
   {
     syslog(LOG_ERR, "Need to provide WRITEFILE and WRITESTR parameters");
     return 1;
   }

   FILE *fd = NULL;
   if(!(fd = fopen(argv[1],"w")))
   {
     syslog(LOG_ERR, "Unable to open file %s", argv[1]);
     closelog();
     
     return 1;     
   }
   else //file was open successfully
   {
     if(fprintf(fd, "%s\n", argv[2]) < 0)
     { 
       syslog(LOG_WARNING, "Unable to write to file %s", argv[1]);
     }
     
     fclose(fd);
     closelog();
     return 0;
   }
}
