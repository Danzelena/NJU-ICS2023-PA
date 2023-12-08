#include<stdio.h>
#include<sys/time.h>
void hello_world(){
    printf("helloworld\n");
}
int main(){
    struct timeval *tv;
    int cnt = 1;
    while(1){
        gettimeofday(tv,NULL);
        while(tv->tv_usec < 500000*cnt){
            gettimeofday(tv,NULL);
        }
        hello_world();
        cnt++;
    }
}