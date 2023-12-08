#include<stdio.h>
#include<sys/time.h>
void hello_world(){
    printf("helloworld\n");
}
int main(){
    printf("Debug:target1\n");
    struct timeval tv;
    int cnt = 1;
    printf("Debug:target2\n");
    while(1){
        gettimeofday(&tv,NULL);
        printf("Debug:target3\n");
        printf("tv_usec=%d",tv.tv_usec);
        while(tv.tv_usec < 500000*cnt){
            gettimeofday(&tv,NULL);
        }
        hello_world();
        cnt++;
    }
}