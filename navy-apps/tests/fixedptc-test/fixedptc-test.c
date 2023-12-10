#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "../../libs/libfixedptc/include/fixedptc.h"

#define RAND_MAX 50
/* generrate [x,y] float*/
#define X 0
#define Y 46
float std_opt_add(float a,float b){
    return a + b;
}
float std_opt_sub(float a,float b){
    return a - b;
}
float std_opt_mul(float a,float b){
    return a * b;
}
float std_opt_div(float a,float b){
    return a / b;
}
float rand_gen(){
    
    float ret = X + 1.0 * (rand() % RAND_MAX)/RAND_MAX  * (Y - X);
    return ret;
}
int main(){

    srand(time(0));
    //((fixedpt)((3) * ((fixedpt)((fixedpt)1 << (32 - 24))) + ((3) >= 0 ? 0.5 : -0.5)))
    fixedpt a = fixedpt_rconst(3);
    fixedpt b = fixedpt_rconst(-46);
    fixedpt a1 = fixedpt_floor(a);
    fixedpt b1 = fixedpt_ceil(b);
    printf("a1=%f\nb1=%f\n",fixedpt_tofloat(a1),fixedpt_tofloat(b1));
    // int c = fixedpt_floor(a);
    
    // int i = 2;
    // int c = 0,d = 0;
    // c = fixedpt_toint(fixedpt_divi(a,i));
    // d = fixedpt_toint(fixedpt_divi(b,i));
    // fixedpt d = fixedpt_muli(a,c);
    // printf("c=%d\nd=%d\n",c,d);
    // printf("c=%d\n",c);
    printf("\n");
    /* test rand_gen */
    // for (int i = 0;i < 10;i++){
    //     float rand = rand_gen();
    //     printf("%.5f\n",rand);
    // }



}