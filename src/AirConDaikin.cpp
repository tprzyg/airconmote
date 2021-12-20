#include "AirConDaikin.h"

AirConDaikin::AirConDaikin() {
    initSerial();
    latestAskedState=0;
    inBufferIndex=0;
    latestmsg=0;
}

AirConDaikin::~AirConDaikin() {
    serial->end();
}

void AirConDaikin::setState(AirConState *newstate) {
    sendMode(newstate->onoff, newstate->mode, newstate->temperature, newstate->flowspeed);
}

AirConState *AirConDaikin::getState() {
    return NULL;
}

void AirConDaikin::initSerial() {
    serial = &Serial1;
    serial->begin(2400, SERIAL_8N2, 16, 17);
}

void AirConDaikin::loop() {
    loopAskState();
}

void AirConDaikin::loopAskState() {
    if (millis() - latestmsg > 5000) {
        if (++latestAskedState > 0x24) latestAskedState=0x01;
        byte msg[]={MSGCODE::READ_REGISTER, (byte)latestAskedState};
        sendMessage(msg, sizeof(msg));
    }
}

void AirConDaikin::readSerial() {
    while(serial->available()) {
        byte b;
        size_t readed = serial->readBytes(&b, 1);
        if (readed != 1) {
            // something bad happened !
            return;
        }
        switch (inBufferIndex) {
            case 0:
                if (b == 0x06) {
                    inBuffer[inBufferIndex++]=b;
                } else inBufferIndex = 0;
                break;

            case 1:
                if (b == 0x02) {
                    inBuffer[inBufferIndex++]=b;
                } else inBufferIndex = 0;
                break;

            default:
                if (inBufferIndex + 1 < sizeof(inBuffer)) {
                    inBuffer[inBufferIndex++]=b;
                    if (b == 0x03) {
                        decodeInputMessage();
                        inBufferIndex=0;
                    }
                } else {
                    // packet too big, reset !
                    inBufferIndex=0;
                }
                break;
        }
    }
}

void AirConDaikin::decodeInputMessage() {
    byte inMessage[sizeof(inBuffer-4)];
    for (int i=2; i<inBufferIndex-2; i++) {
        inMessage[i-2]=inBuffer[i] - 0x30;
    }

    int len = inBufferIndex-4;
    switch(inMessage[0]) {
        case MSGCODE::REGISTER_ANSWER: decodeRegisterAnswer(inMessage, len);
        case MSGCODE::REGISTER2_ANSWER: decodeRegister2Answer(inMessage, len);
    }
}

void AirConDaikin::decodeRegisterAnswer(byte *inMessage, int len) {
    switch(inMessage[0]) {
        case REGISTER::MODE:
            decodeRegisterAnswerMode(&inMessage[1], len-1);
            break;
        case REGISTER::FLOWAIR_DIRECTION:
            break;
        case REGISTER::MODE_POWER:
            break;
        case REGISTER::TEMP_SENSORS:
            break;
    }
}

void AirConDaikin::decodeRegister2Answer(byte *inMessage, int len) {

}

void AirConDaikin::decodeRegisterAnswerMode(byte *inMessage, int len) {
    realState.onoff = inMessage[0] == 0x01 ? true : false;
    switch(inMessage[1]) {
        case MODE::AUTO: realState.mode = "auto"; break;
        case MODE::DRY:  realState.mode = "dry";  break;
        case MODE::SNOW: realState.mode = "snow"; break;
        case MODE::HEAT: realState.mode = "heat"; break;
        case MODE::AIR:  realState.mode = "air";  break;
    }
    realState.temperature = inMessage[2] / 2.0 + 10.0;
    realState.flowspeed = inMessage[3];
}

void AirConDaikin::sendMessage(byte message[], int length) {
    outBuffer[0]=0x06;
    outBuffer[1]=0x02;
    byte checksum=0;
    for (int i=0; i<length; i++) {
        byte b=message[i] + 0x30;
        checksum+=b;
        outBuffer[2+i]=b;
    }
    outBuffer[2+length]=checksum;
    outBuffer[3+length]=0x03;

    serial->write(outBuffer, 4+length);
    latestmsg=millis();
}

void AirConDaikin::sendMode(bool onoff, String mode, float temp, int flowspeed) {
    byte message[6];
    message[0] = MSGCODE::WRITE_REGISTER;
    message[1] = REGISTER::MODE;

    // encode onoff :
    message[2] = onoff ? 0x01 : 0x00;

    // encode mode :
    if (mode.equals("auto")) {
        message[3]=MODE::AUTO; // auto
    } else if (mode.equals("dry")) {
        message[3]=MODE::DRY; // dry
    } else if (mode.equals("snow")) {
        message[3]=MODE::SNOW; // snow
    } else if (mode.equals("heat")) {
        message[3]=MODE::HEAT; // heat
    } else if (mode.equals("air")) {
        message[3]=MODE::AIR; // air
    } else {
        //console.error("setMode to unknown mode : ", mode);
        return;
    }

    // encode temp :
    message[4] = (temp - 10) * 2;

    // encode flowspeed :
    if (flowspeed >= 1 || flowspeed <= 7) {
        message[5] = flowspeed;
    } else {
        //console.error("setMode to unknown flowspeed : ", flowspeed);
        return;
    }

    sendMessage(message, sizeof(message));
}
