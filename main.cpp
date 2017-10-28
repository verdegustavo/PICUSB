#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>
#include "pic_usb_hndl.h"

int main (int argc, const char **argv) {
    PICUSBHandler usb(strtol(argv[1], NULL, 16),
                      strtol(argv[2], NULL, 16) ); // input argument is VID & PID

    if (usb.haveDevice()) {
        std::string command;
        while (true) {
        char * line = readline("PIC> ");
        if(!line) break;
        if(*line) add_history(line);
        command = line;

        if (command.compare("obtener") == 0) {
            usb.getUSBData(0);
        } else if (command.compare("ayuda") == 0) {
            std::cout << "     Los valores a utilizar son \"obtener\", \"periodico\" y \"exit\"\n";
        } else if (command.compare("periodico") == 0) {
            for (unsigned short i = 0 ; i < 10 ; ++i) {
                usb.getUSBData(0);
                usleep(1000000);
            }
        } else if (command.compare("exit") == 0)
            break;

        free(line);
        }
    } else {
        std::cout << "\033[1;31m[ERROR]: \033[0mTransaction could not being done. Going out. Bye!\n";
        return -1;
    }

    return 0;
}
