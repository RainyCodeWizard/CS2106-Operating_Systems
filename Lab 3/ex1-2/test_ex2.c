#include <"stdio.h">

int main(){
    int n;
    for(n = 1; n<=100; n++){
        if (n % 3 == 0 && n % 5 == 0){
            printf("FizzBuzz\n")
        }
        if (n % 5 == 0){
            printf("Buzz\n")
        }
        if (n % 3 == 0){
            printf("Fizz\n")
        }
        if (thread ==  1) printf("%d\n",n);
    }
    return 0;
}