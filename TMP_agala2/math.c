//Using the functions provided by the TA
#include<math.h>

extern double log(double x);
extern double pow(double x, int y);
extern double expdev(double lambda);
extern double taylor_log(double input);

double pow(double x, int y){
    int i;
    double res = 1.0;
    for(i = 0; i < y; i++ ){
        res = res * x;
    }
    return res;
}

// Using taylor serices for natural logarithmic function
// Note: x > 0 && x <= 2
double taylor_log(double input){
    double x = input -1;
    double res = x;
    int i;
    for(i=2;i<=20;i++){
        if(i%2){
            res += pow(x, i) / (double)i;
        }else{
            res -= pow(x, i) / (double)i;
        }
    }
    return res;
}

double log(double x){
    double e = 2.71828; // constant e
    int partA = 0;
    double remainder = x;
    if(x < 2.0) {
        return taylor_log(x);
    } else {
        do{
            remainder /= e;
            partA ++;
        }while(remainder >= 2.0);
    }
    double partB = taylor_log(remainder);
    return partA + partB;
}


double expdev(double lamda) {
    // kprintf("\nGenerating Random number :)");
    double dummy;
    do
        dummy = (double) rand() / RAND_MAX;
    while (dummy == 0.0);

    return -log(dummy) / lamda;
}