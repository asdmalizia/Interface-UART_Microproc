#include <windows.h>
#include <stdio.h>

int main()
{
    int cont = 0;
    HANDLE hComm;                           // Handle to the Serial port
    char ComPortName[] = "COM3";            // Name of the Serial port(May Change) to be opened,
    BOOL  Status;                           // Status of the various operations 
    DWORD dwEventMask;                      // Event mask to trigger
    char  TempChar;                         // Temperory Character
    char  SerialBuffer[256];                // Buffer Containing Rxed Data
    DWORD NoBytesRead;                      // Bytes read by ReadFile()


    /---------------------------------- Opening the Serial Port -------------------------------------------/
    
    hComm = CreateFile(
        ComPortName,                    //port name
        GENERIC_READ | GENERIC_WRITE,   //Read/Write
        0,                              // No Sharing
        NULL,                           // No Security
        OPEN_EXISTING,                  // Open existing port only
        0,                              // Non Overlapped I/O
        NULL                            // Null for Comm Devices
    );

    if (hComm == INVALID_HANDLE_VALUE) {
        printf("\n    Error! - Port %s can't be opened\n", ComPortName);
        CloseHandle(hComm); 
        return 0;
    }
    else {
        printf("\n    Port %s Opened\n ", ComPortName);
    }

    /------------------------------- Setting the Parameters for the SerialPort ------------------------------/
			
    DCB dcbSerialParams = { 0 };                            // Initializing DCB structure
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    Status = GetCommState(hComm, &dcbSerialParams);         //retreives  the current settings

    if (Status == FALSE) {
        printf("\n    Error! in GetCommState()");
        CloseHandle(hComm); 
        return 0;
    }

    dcbSerialParams.BaudRate = CBR_9600;      // Setting BaudRate = 9600
    dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
    dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
    dcbSerialParams.Parity = NOPARITY;        // Setting Parity = None 

    Status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB 

    if (Status == FALSE) {
        printf("\n    Error! in Setting DCB Structure");
        CloseHandle(hComm); 
        return 0;
    }
    else {//If Successfull display the contents of the DCB Structure
        printf("\n\n    Setting DCB Structure Successfull\n");
        printf("\n       Baudrate = %d", dcbSerialParams.BaudRate);
        printf("\n       ByteSize = %d", dcbSerialParams.ByteSize);
        printf("\n       StopBits = %d", dcbSerialParams.StopBits);
        printf("\n       Parity   = %d", dcbSerialParams.Parity);
    }


    /------------------------------------ Setting Timeouts --------------------------------------------------/
			
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout         = 50;
    timeouts.ReadTotalTimeoutConstant    = 50;
    timeouts.ReadTotalTimeoutMultiplier  = 10;
    timeouts.WriteTotalTimeoutConstant   = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    
    if (SetCommTimeouts(hComm, &timeouts) == FALSE) {
        printf("\n\n    Error! in Setting Time Outs");
        CloseHandle(hComm); 
        return 0;
    }
    else
        printf("\n\n    Setting Serial Port Timeouts Successfull");


    /------------------------------------ Setting Receive Mask ----------------------------------------------/
    
    Status = SetCommMask(hComm, EV_RXCHAR); //Configure Windows to Monitor the serial device for Character Reception

    if (Status == FALSE) {
        printf("\n\n    Error! in Setting CommMask");
        CloseHandle(hComm); 
        return 0;
    }
    else
        printf("\n\n    Setting CommMask successfull");


    /------------------------------------ Setting WaitComm() Event   ----------------------------------------/
    
    printf("\n\n    Waiting for Data Reception");

    Status = WaitCommEvent(hComm, &dwEventMask, NULL); //Wait for the character to be received

    /-------------------------- Program will Wait here till a Character is received ------------------------/				

    if (Status == FALSE) {
        printf("\n    Error! in Setting WaitCommEvent()");
        CloseHandle(hComm); 
        return 0;
    }
    
    else {  //If  WaitCommEvent()==True Read the RXed data using ReadFile();
        printf("\n\n    Characters Received");
        while(1) {
            Status = WaitCommEvent(hComm, &dwEventMask, NULL);
            Status = ReadFile(hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);                
            if(TempChar == '1') {
                printf("comando: DECREMENTAR\t", TempChar);
                if(cont <= 0) {
                    cont = 0;
                }
                else {
                    cont--;
                }
                printf("contador: %d\n", cont);
            }
            else if(TempChar == '2') {
                printf("comando: INCREMENTAR\t", TempChar);
                if(cont >= 15) {
                    cont = 15;
                }
                else {
                    cont++;
                }
                printf("contador: %d\n", cont);
            }
            char p = (char)cont;
            Status = WriteFile(hComm, &p, sizeof(p), &NoBytesRead, NULL);
        }

        
        /------------Printing the RXed String to Console----------------------/

    }

    CloseHandle(hComm);                     //Closing the Serial Port
    return 0;
}
