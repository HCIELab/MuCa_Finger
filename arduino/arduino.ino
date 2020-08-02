#include <Muca.h>

Muca muca;

#define CALIBRATION_STEPS 20
short currentCalibrationStep = 0;
unsigned int calibrationGrid[NUM_ROWS * NUM_COLUMNS];

void setup() {
    Serial.begin(115200);

    muca.init();
    muca.useRawData(true); // If you use the raw data, the interrupt is not working
    muca.setGain(255);
}

void loop() {
    GetRaw();
}


void GetRaw() {
    if (muca.updated()) {
        if (currentCalibrationStep >= CALIBRATION_STEPS) {
            // Print the array value
            for (int i = 0; i < NUM_ROWS * NUM_COLUMNS; i++) {
                if (muca.grid[i] > 0) // Add offset to be sure it's positive:
                    Serial.print((muca.grid[i] - calibrationGrid[i] ) + 20 );
                if (i != NUM_ROWS * NUM_COLUMNS - 1)
                    Serial.print(",");
            }
            Serial.println();
        }
        else {  // Once the calibration is done
                //Save the grid value to the calibration array
            for (int i = 0; i < NUM_ROWS * NUM_COLUMNS; i++) {
                if (currentCalibrationStep == 0)        // Copy array:
                    calibrationGrid[i] = muca.grid[i];
                else                                    // Get average:
                    calibrationGrid[i] = (calibrationGrid[i] + muca.grid[i]) / 2;
            }
            currentCalibrationStep++;
            Serial.print("Calibration performed ");
            Serial.print(currentCalibrationStep);
            Serial.print("/");
            Serial.println(CALIBRATION_STEPS);
        }

    } // End Muca Updated

    delay(1);
}




char incomingMsg[20];

void serialEvent() {
    int charsRead;
    while (Serial.available() > 0) {
        charsRead = Serial.readBytesUntil('\n', incomingMsg, sizeof(incomingMsg) - 1);
        incomingMsg[charsRead] = '\0';  // Make it a string
        if (incomingMsg[0] == 'g') {
            Gain();
        }
        else if (incomingMsg[0] == 'c') {
            currentCalibrationStep = 0;
        }
    }
}



void Gain() {
    Serial.print("Received:"); Serial.println(incomingMsg);
    char *str;
    char *p = incomingMsg;
    byte i = 0;
    while ((str = strtok_r(p, ":", &p)) != NULL)  // Don't use \n here it fails
    {
        if (i == 1 )  {
            muca.setGain(atoi(str));
        }
        i++;
    }
    incomingMsg[0] = '\0'; // Clear array
}
