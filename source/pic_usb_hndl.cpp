// pic_usb_hndl.cpp

#include "pic_usb_hndl.h"

PICUSBHandler::PICUSBHandler(unsigned long vendor, unsigned long product) {
    _logger = new LogFile("log/rPicUsbHndl.log");
    _vendorID = vendor;
    _productID = product;
    int retval;
    char dname[32] = {0};
    usb_set_debug(255);
    usb_init();
    usb_find_busses();
    usb_find_devices();

    if(!(_lvr_hid = initializeUSB())) {
        std::cout << "\033[1;31m[ERROR]: \033[0mNo se pudo encontrar el dispositivo HID.\n";
        _inLog << "No se pudo encontrar el dispositivo HID.";
        _logger->escribirLog(2,&_inLog);
        _lvr_hid = nullptr;
    }

    retval = usb_get_driver_np(_lvr_hid, 0, dname, 31);
    if (!retval)
        usb_detach_kernel_driver_np(_lvr_hid, 0);


    retval = usb_set_configuration(_lvr_hid, 1);
    if ( retval < 0) {
        _inLog << "No se pudo establecer la configuración 1 : " << retval;
        _logger->escribirLog(2,&_inLog);
        _lvr_hid = nullptr;
    }

    retval = usb_claim_interface(_lvr_hid, INTERFACE);
    if ( retval < 0) {
        _inLog << "No se pudo reclamar la interfaz: " << retval;
        _logger->escribirLog(2,&_inLog);
        _lvr_hid = nullptr;
    }
}

PICUSBHandler::~PICUSBHandler() {
    usb_close(_lvr_hid);
    delete _logger;
}

usb_dev_handle* PICUSBHandler::initializeUSB() {
    struct usb_bus* bus;
    struct usb_device* dev;
    for (bus = usb_get_busses(); bus; bus = bus->next) {
        for (dev = bus->devices; dev; dev = dev->next) {
            if (dev->descriptor.idVendor == _vendorID &&
                dev->descriptor.idProduct == _productID ) {
                usb_dev_handle *handle;
                _inLog << "lvr_hid con Vendor Id: " << _vendorID << " y Product Id: " << _productID << " encontrado!";
                _logger->escribirLog(0,&_inLog);
                if (!(handle = usb_open(dev))) {
                    _inLog << "No se pudo abrir el dispositivo USB";
                    _logger->escribirLog(2,&_inLog);
                    return NULL;
                }

                return handle;
            }
        }
    }
    return NULL;
}

bool PICUSBHandler::haveDevice() {
    if (_lvr_hid == nullptr)
        return false;
    else
        return true;
}

void PICUSBHandler::getUSBData(byte command) {
    int r;
    char answer[reqIntLen];
    char question[reqIntLen];
    question[0] = command;

    _inLog << "Begining USB data transactions...";
    _logger->escribirLog(0,&_inLog);

    r = usb_interrupt_write(_lvr_hid, endpoint_Int_out, question, reqIntLen, timeout);

    if( r < 0 ) {
        std::cout << "\033[1;31m[ERROR]: \033[0mUSB interrupt write. USB write failed.\n";
        _inLog << "USB interrupt write. USB write failed.";
        _logger->escribirLog(2,&_inLog);
    } else {
        _inLog << "USB interrupt write. USB write success!";
        _logger->escribirLog(0,&_inLog);
    }

    r = usb_interrupt_read(_lvr_hid, endpoint_Int_in, answer, reqIntLen, timeout);

    if( r != reqIntLen ) {
        std::cout << "\033[1;31m[ERROR]: \033[0mUSB interrupt read. USB read failed.\n";
        _inLog << "USB interrupt read. USB read failed.";
        _logger->escribirLog(2,&_inLog);
    } else {
        _inLog << "USB interrupt read. USB read success!";
        _logger->escribirLog(0,&_inLog);

        float porcentajeADC;
        byte adcH;
        byte adcL;

        adcH = answer[0] & 0b00000011;
        adcL = answer[1];
        porcentajeADC = ( (float)( (adcH << 8) | adcL ) / 1023) * 100;

        std::cout << "Potenciometro: %" << porcentajeADC << std::endl;

        if (answer[2] == 0)
            std::cout << "Estado del Boton: " << "ENCENDIDO.\n";
        else
            std::cout << "Estado del Boton: " << "APAGADO.\n";
    }

    _inLog << "Transaction finished.";
    _logger->escribirLog(0,&_inLog);


//  usb_set_altinterface(_lvr_hid, 0);
    usb_release_interface(_lvr_hid, 0);
}
