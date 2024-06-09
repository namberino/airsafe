#include <Arduino.h>
#include <unity.h>

float calculatePPM(int sensorValue, float R0, float m, float c);

// test the calculatePPM function with known values
void test_calculatePPM_low_concentration()
{
    int sensorValue = 1000; // example value
    float R0 = 23.30;
    float m = -0.353;
    float c = 0.711;
    float expectedPPM = 1.2;
    float actualPPM = calculatePPM(sensorValue, R0, m, c);
    TEST_ASSERT_FLOAT_WITHIN(0.1, expectedPPM, actualPPM);
}

void test_calculatePPM_high_concentration()
{
    int sensorValue = 3000; // example value
    float R0 = 23.30;
    float m = -0.353;
    float c = 0.711;
    float expectedPPM = 120;
    float actualPPM = calculatePPM(sensorValue, R0, m, c);
    TEST_ASSERT_FLOAT_WITHIN(0.1, expectedPPM, actualPPM);
}

void setup()
{
    UNITY_BEGIN();
    RUN_TEST(test_calculatePPM_low_concentration);
    RUN_TEST(test_calculatePPM_high_concentration);
    UNITY_END();
}

void loop()
{

}

float calculatePPM(int sensorValue, float R0, float m, float c)
{
    float sensor_volt = sensorValue * (3.3 / 4096.0);
    float RS_gas = ((3.3 * 10.0) / sensor_volt) - 10.0;
    float ratio = RS_gas / R0;
    double ppm_log = (log10(ratio) - c) / m;
    double ppm = pow(10, ppm_log);
    return ppm;
}
