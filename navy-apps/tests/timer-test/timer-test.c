#include <stdio.h>
#include <sys/time.h>
#include "../../libs/libndl/include/NDL.h"
void hello_world(){
    printf("helloworld\n");
}
int main(){
    NDL_Init(0);
    uint32_t time = 0;
    // printf("Debug:target1\n");
    // struct timeval tv;
    int cnt = 0;
    // printf("Debug:target2\n");
    while(1){
        time = NDL_GetTicks();
        // gettimeofday(&tv,NULL);
        // printf("Debug:target3\n");
        // printf("tv_usec=%d",tv.tv_usec);
        while(time< 500000*cnt){
            // gettimeofday(&tv,NULL);
            time = NDL_GetTicks();
        }
        
        hello_world();
        cnt++;
    }
}