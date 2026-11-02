#include "kernel/types.h"
#include "user/user.h"

void delay_loop() {
    volatile int i;
    for(i = 0; i < 500000000; i++); 
}

int main() {
    printf("Before sbrk. Quickly open QEMU monitor now...\n");
    delay_loop();
    
    sbrk(1);
    
    printf("After sbrk. Quickly open QEMU monitor again...\n");
    delay_loop();
    
    exit(0);
}
