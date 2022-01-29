#include <math.h>
#include <stdio.h>
#include "ball.h"

void moveBall(ball* thisBall, field* thisField, unsigned long deltaus) {
    float deltams = deltaus / 20000;
    float dx, dy;
    //X軸変位の反映
    dx = thisBall->speed[0] * deltams;
    //Y軸変位の反映
    dy = thisBall->speed[1] * deltams;
    dy += thisField->gravity * deltams * deltams / 2.0;

        thisBall->speed[1] += thisField->gravity * deltams;
    
    thisBall->position[0] += dx;
    thisBall->position[1] += dy;
}


float kainokoshiki(float a, float b, float c) {
  float kai1, kai2;
  kai1 = (-b + sqrtf(b*b+4*a*c))/2.0/a;
  kai2 = (-b - sqrtf(b*b+4*a*c))/2.0/a;
  printf("kai1 = %f", kai1);
  printf("kai2 = %f", kai2);
  if(kai1 < kai2) {
    return kai2;
  } else {
    return kai1;
  }
}