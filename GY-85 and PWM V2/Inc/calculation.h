//Либа для расчетов
#include "math.h"
#include "stdint.h"
#define area 0.00282743338823081391461637904495  //m2
#define resistance 1,838592     //Om
#define coil 112        //piece
#define maxVoltage 2.5 //V
#define stopVelocity 0.025  //rad/sec
#define kBdot 500000
#define kOmega 50000
#define kAlpha (0.04 * kOmega * kOmega)
#define PI 3.14159265358979323846
#define angleReq PI/4
void pwmValue(float k, float omega, float induction, uint8_t *result, int *direction);  //расчет ШИМ
void getVoltage(float omega, float B[3], float angle, float *voltageX, float *voltageY);
void scaleVoltage (float *voltageX, float *voltageY);
void getPWM (float voltage, int *dir, uint8_t *pwm);