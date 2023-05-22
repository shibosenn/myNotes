//样可变参
#include <stdio.h>
#define LOGV(...)       printf(__VA_ARGS__);

int main()
{
      int a=10,b=11;
      LOGV("a=%d--b=%d\r\n",a,b);   //a=10--b=11
     return 0;
}