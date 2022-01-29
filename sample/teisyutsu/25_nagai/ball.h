#ifndef CLASS_BALL_H
#define CLASS_BALL_H

typedef struct ball {
    //制御情報
    float position[2];
    float speed[2];
    int radius;
    float e;
} ball;


typedef struct field {
    float gravity;
    unsigned int width;
    unsigned int height;
} field;

void moveBall(ball*, field*, unsigned long);
int checkEnd(ball*, field*, unsigned long, int dx, int dy);
float kainokoshiki(float a, float b, float c);
#endif