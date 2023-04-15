#include <Muca.h>

Muca muca;

#define CALIBRATION_STEPS 50
short currentCalibrationStep = 0;
unsigned int calibrationGrid[NUM_ROWS * NUM_COLUMNS];

void setup() {
    Serial.begin(115200);

    // We only use 10 x 6 electrodes for this demo:
    muca.skipLine(TX_lines, (const short[]) {/*1,2,3,4,5,6,7,8,9,10,11,*/12,13,14,15,16,17,18,19,20,21}, 10);
    muca.skipLine(RX_lines, (const short[]) {1,2,3,4,5,6/*,7,8,9,10,11,12*/}, 6);

    muca.init();
    muca.useRawData(true); // If you use the raw data, the interrupt is not working
    muca.setGain(31);
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
                // Save the grid value to the calibration array
            const float SR = 0.2; // smooth ratio
            for (int i = 0; i < NUM_ROWS * NUM_COLUMNS; i++) {
                if (currentCalibrationStep == 0)        // Copy array:
                    calibrationGrid[i] = muca.grid[i];
                else                                    // Get average:
                    calibrationGrid[i] = SR * calibrationGrid[i] + (1 - SR) * muca.grid[i];
            }
            currentCalibrationStep++;
            Serial.print("Calibration step: ");
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
