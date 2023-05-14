#include <Windows.h>
#include <stdio.h>

#include "KeyPad/KeyPad.h"
#include "EvalExpr/EvalExpr.h"
#include "EvalExpr/EvalExpr.c"

char input[256];
uint8_t input_length = 0;

void main(void)
{

    printf("Hello!\n");

    InitSto();

    DWORD dwCommEvent;
    DWORD dwRead;
    DWORD dwWritten;
    DWORD lpEvtMask;
    char chRead;
    char ComPortName[] = "\\\\.\\COM4"; // Name of the Serial port(May Change) to be opened,
    int i = 0;

    HANDLE hComm;
    hComm = CreateFile(ComPortName,
                       GENERIC_READ | GENERIC_WRITE,
                       0,
                       0,
                       OPEN_EXISTING,
                       0,
                       0);
    if (hComm == INVALID_HANDLE_VALUE)
        printf("Error opening port.\n");

    //////////////////////////////////////////////////////
    DCB dcb;

    FillMemory(&dcb, sizeof(dcb), 0);
    if (!GetCommState(hComm, &dcb)) // get current DCB
        printf("Error GetCommState.\n");

    // Update DCB rate.
    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;          // Setting ByteSize = 8
    dcb.StopBits = ONESTOPBIT; // Setting StopBits = 1
    dcb.Parity = NOPARITY;     // Setting Parity = None
    dcb.DCBlength = sizeof(dcb);
    // Set new state.
    if (!SetCommState(hComm, &dcb))
        printf("Error SetCommState.\n");
    // Error in SetCommState. Possibly a problem with the communications
    // port handle or a problem with the DCB structure itself.

    /////////////////////////////////////////////////////////////////////
    COMMTIMEOUTS timeouts;

    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = 100;
    timeouts.ReadTotalTimeoutConstant = 100;
    timeouts.WriteTotalTimeoutMultiplier = 100;
    timeouts.WriteTotalTimeoutConstant = 100;

    if (!SetCommTimeouts(hComm, &timeouts))
        printf("Error timeouts.\n");

    if (!PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT))
        printf("Error PurgeComm.\n");
    ////////////////////////////////////////
    for (;;)
    {
        if (!SetCommMask(hComm, 0))
            printf("Error CommMask.\n");

        if (!SetCommMask(hComm, EV_RXCHAR))
            printf("Error CommMask.\n");

        printf("Waiting for characters.. \n\n");

        if (WaitCommEvent(hComm, &dwCommEvent, NULL))
        {
            input_length = 0;
            do
            {
                if (ReadFile(hComm, &chRead, 1, &dwRead, NULL))
                {
                    if (dwRead != 0)
                    {
                        // printf("Character Received: %d\n",chRead & 0xFF);
                        input[input_length] = chRead;
                        input_length++;
                    }
                }
                else
                {
                    printf("ErrorReadFile.\n");
                    break;
                }
            } while (dwRead);

            if(input_length > 0) {
                printf("Received: ");
                for(int i = 0; i < input_length; i++) {
                    printf("%d ", input[i] & 0xFF);
                }
                printf("\n");
                uint8_t errorCode = 0;
                double result = evaluate(input, input_length, &errorCode);
                printf("%f, %d\n", result, errorCode);

                WriteFile(hComm, &result, sizeof(double), &dwWritten, NULL);
                WriteFile(hComm, &errorCode, 1, &dwWritten, NULL);
            }
        }
        else
        {
            printf("Error WaitCommEvent.\n");
            break;
        }
        printf("=========================\n");
    }
}