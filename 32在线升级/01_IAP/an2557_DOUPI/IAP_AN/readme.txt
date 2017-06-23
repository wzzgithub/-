/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : readme.txt
* Author             : MCD Application Team
* Version            : V2.0.2
* Date               : 09/22/2008
* Description        : Description of the AN2557 "STM32F10xxx in-application
*                      programming using the USART (IAP)".
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

Description
===================
This directory contains a set of sources files and pre-configured projects that 
describes how to build an application to be loaded into Flash memory using
In-Application Programming (IAP, through USART1).


Directory contents
==================
 + \binary     contains a set of sources files that build the application to be
               loaded with IAP 
 + \include    contains the IAP firmware header files
 + \project    contains pre-configured projects for RVMDK, EWARM5, RIDE and HiTOP
               toolchains
 + \source     contains the IAP firmware source files


Hardware environment
====================
This example runs on STMicroelectronics STM3210B-EVAL and STM3210E-EVAL evaluation
boards and can be easily tailored to any other hardware.
To select the STMicroelectronics evaluation board used to run the example, uncomment
the corresponding line in platform_config.h file.

 + STM3210B-EVAL 
    - Connect a null-modem female/female RS232 cable between the DB9 connector 
      CN6 (USART1) and PC serial port.
 
 + STM3210E-EVAL
    - Connect a null-modem female/female RS232 cable between the DB9 connector 
      CN12 (USART1) and PC serial port.

 + Hyperterminal configuration:
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - BaudRate = 115200 baud
    - flow control: None 
                    
   
How to use it
=============
In order to make the program work, you must do the following:
1. Generate a binary image for the pogram provided in the \binary directory
2. Program the internal Flash with the IAP (see below) 
3. Open a HyperTerminal window using the settings already defined in section
   "Hardware environment"
4. To run the IAP driver, keep the push-button (Key push-button on STM3210BEVAL/
   STM3210E-EVAL board) pressed at Reset. The IAP main menu is then displayed on
   the HyperTerminal window.
5. To download an application, press 1 and use the Ymodem protocol
      
      
In order to load the IAP code, you have do the following:
 + EWARMv5:
    - Open the STM32F10x_IAP.eww workspace
    - In the workspace toolbar select the project config:
        - STM3210B-EVAL: to configure the project for STM32 Medium-density devices
        - STM3210E-EVAL: to configure the project for STM32 High-density devices
    - Rebuild all files: Project->Rebuild all
    - Load project image: Project->Debug
    - Run program: Debug->Go(F5)

 + RIDE
    - Open the STM32F10x_IAP.rprj project
    - In the configuration toolbar(Project->properties) select the project config:
        - STM3210B-EVAL: to configure the project for STM32 Medium-density devices
        - STM3210E-EVAL: to configure the project for STM32 High-density devices
    - Rebuild all files: Project->build project
    - Load project image: Debug->start(ctrl+D)
    - Run program: Debug->Run(ctrl+F9)

 + RVMDK
    - Open the STM32F10x_IAP.Uv2 project
    - In the build toolbar select the project config:
        - STM3210B-EVAL: to configure the project for STM32 Medium-density devices
        - STM3210E-EVAL: to configure the project for STM32 High-density devices
    - Rebuild all files: Project->Rebuild all target files
    - Load project image: Debug->Start/Stop Debug Session
    - Run program: Debug->Run (F5)
    
 + HiTOP     
    - Open the HiTOP toolchain, a "using projects in HiTOP" window appears.
    - Select open an existing project.
    - Browse to open the STM32F10x_IAP.htp:
        - under STM32F10B_EVAL directory: to select the project for STM32 Medium-density devices.
        - under STM32F10E_EVAL directory: to select the project for STM32 High-density devices
    - Rebuild all files: Project->Rebuild all
    - Click on ok in the "download project" window.
    - Run program: Debug->Go(F5).
    Note: 
          - When using High-density devices, it is mandatory to reset the target
            before loading the project into  target.
          - It is recommended to run the reset script (click on TR button in the
            toolbar menu) after loading the project into target.

NOTE:
 - Medium-density devices are STM32F101xx and STM32F103xx microcontrollers where
   the Flash memory density ranges between 32 and 128 Kbytes.
 - High-density devices are STM32F101xx and STM32F103xx microcontrollers where
   the Flash memory density ranges between 256 and 512 Kbytes. 


******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE******
