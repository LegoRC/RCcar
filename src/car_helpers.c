#include <stdbool.h>


// activate via interrupt request when car is in reverse
void activate_rear_lights(bool inReverse, const uint led1, const uint led2){
    if(inReverse){
        gpio_put(led1, 1);
        gpio_put(led2, 1);
    }
}
