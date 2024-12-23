#include <Wire.h>
#include <MPU6050.h>
#include <Servo.h>
#include <I2Cdev.h>

Servo xServo;
Servo yServo;
MPU6050 mpu;

int16_t ax, ay, az;
int16_t gx, gy, gz;

int xAngle = 90;
int yAngle = 90;

const float alpha = 0.9;

bool useKeyboard = false;
bool useButtons = false;
bool useGyroscope = false;

const int buttonXUp = 7;    //للأمام
const int buttonXDown = 8;  //للخلف
const int buttonYUp = 9;    //لليمين
const int buttonYDown = 10; //لليسار

void setup() {
    Serial.begin(9600);
    Wire.begin();
    mpu.initialize();
    xServo.attach(5);
    yServo.attach(6);

    pinMode(buttonXUp, INPUT_PULLUP);
    pinMode(buttonXDown, INPUT_PULLUP);
    pinMode(buttonYUp, INPUT_PULLUP);
    pinMode(buttonYDown, INPUT_PULLUP);

    calibrateGyroscope();

}

void loop() {
    /*if (useKeyboard) {
        controlWithKeyboard();
    } else if (useButtons) {
        controlWithButtons();
    } else {
        controlWithGyroscope();
    }

    delay(50);*/
    if (Serial.available() > 0) {
        char input = Serial.read();
        switch (input) {
            case 'g':
                useGyroscope = true;
                useKeyboard = false;
                useButtons = false;
                Serial.println("تم التبديل إلى الجيروسكوب");
                break;
            case 'k':
                useGyroscope = false;
                useKeyboard = true;
                useButtons = false;
                Serial.println("تم التبديل إلى الكيبورد");
                break;
            case 'b':
                useGyroscope = false;
                useKeyboard = false;
                useButtons = true;
                Serial.println("تم التبديل إلى الأزرار");
                break;
        }
    }

    if (useGyroscope) {
        controlWithGyroscope();
    } else if (useKeyboard) {
        controlWithKeyboard();
    } else if (useButtons) {
        controlWithButtons();
    }

    delay(50);

}

void controlWithGyroscope() {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    int newXAngle = map(gx, -17000, 17000, 0, 180);
    int newYAngle = map(gy, -17000, 17000, 180, 0);

    xAngle = alpha * xAngle + (1 - alpha) * newXAngle;
    yAngle = alpha * yAngle + (1 - alpha) * newYAngle;

    xServo.write(xAngle);
    yServo.write(yAngle);

    Serial.print("Gyroscope | X Angle: ");
    Serial.print(xAngle);
    Serial.print(" | Y Angle: ");
    Serial.println(yAngle);
}

void controlWithKeyboard() {
    if (Serial.available() > 0) {
        char input = Serial.read();
        switch (input) {
            case 'w':
                xAngle = constrain(xAngle + 5, 0, 180);
                break;
            case 's':
                xAngle = constrain(xAngle - 5, 0, 180);
                break;
            case 'a':
                yAngle = constrain(yAngle + 5, 0, 180);
                break;
            case 'd':
                yAngle = constrain(yAngle - 5, 0, 180);
                break;
        }

        xServo.write(xAngle);
        yServo.write(yAngle);

        Serial.print("Keyboard | X Angle: ");
        Serial.print(xAngle);
        Serial.print(" | Y Angle: ");
        Serial.println(yAngle);
    }
}

void controlWithButtons() {
    if (digitalRead(buttonXUp) == LOW) {
        xAngle = constrain(xAngle + 5, 0, 180);
        delay(10);
    }
    if (digitalRead(buttonXDown) == LOW) {
        xAngle = constrain(xAngle - 5, 0, 180);
        delay(10);
    }
    if (digitalRead(buttonYUp) == LOW) {
        yAngle = constrain(yAngle - 5, 0, 180);
        delay(10);
    }
    if (digitalRead(buttonYDown) == LOW) {
        yAngle = constrain(yAngle + 5, 0, 180);
        delay(10);
    }

    xServo.write(xAngle);
    yServo.write(yAngle);

    Serial.print("Buttons | X Angle: ");
    Serial.print(xAngle);
    Serial.print(" | Y Angle: ");
    Serial.println(yAngle);
}

void calibrateGyroscope() {
    Serial.println("ضبط المستشعر... الرجاء عدم تحريكه");
    long gxOffset = 0, gyOffset = 0, gzOffset = 0;

    for (int i = 0; i < 100; i++) {
        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        gxOffset += gx;
        gyOffset += gy;
        gzOffset += gz;
        delay(10);
    }

    gxOffset /= 100;
    gyOffset /= 100;
    gzOffset /= 100;

    mpu.setXGyroOffset(-gxOffset);
    mpu.setYGyroOffset(-gyOffset);
    mpu.setZGyroOffset(-gzOffset);

    Serial.println("تمت الضبط بنجاح");
}