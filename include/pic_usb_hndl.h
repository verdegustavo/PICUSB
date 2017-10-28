// pic_usb_hndl.h

#pragma once

#include <iostream>
#include <usb.h>
#include <sstream>
#include "logFile.h"

typedef unsigned char byte;

class PICUSBHandler {
private:
//        const static  char* VERSION = "0.1.0";
        const static  byte INTERFACE = 0;
        const static  int reqIntLen = 3;
        const static  int endpoint_Int_in = 0x81; /* endpoint 0x81 address for IN */
        const static  int endpoint_Int_out = 0x01; /* endpoint 1 address for OUT */
        const static  int timeout = 5000; /* timeout in ms */

        unsigned long _vendorID;
        unsigned long _productID;
        usb_dev_handle* _lvr_hid;
        stringstream _inLog;
        LogFile* _logger;

public:
        PICUSBHandler(unsigned long vendor, unsigned long product);
        ~PICUSBHandler();
        usb_dev_handle* initializeUSB();
        bool haveDevice();
        void getUSBData(byte command);

};
