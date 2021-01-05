# ECEN 5823 Assignment Skeleton

This project contains skeleton code used for coursework in University of Colorado [ECEN 5823 IoT Embedded Firmware](https://sites.google.com/colorado.edu/ecen5823-s2020/home).

Below is an overview of the sequence used to generate this repository.  For handling SDK upgrades, please see the instructions in [the wiki page](https://github.com/CU-ECEN-5823/ecen5823-assignments/wiki/Silicon-Labs-SDK-Upgrades).
* The project was generated starting with the new project Wizard built into [Simplicity Studio 4](https://www.silabs.com/products/development-tools/software/simplicity-studio).  
* The AppBuilder project was used with application type "Bluetooth SDK" and application type "SOC - Empty".
* Configurations were setup to target GNU Arm 4.9.3 and 7.2.1.  4.9.3 is set as the default project target due to existing project code file support requirements.
* Workspace paths were setup to pull in emlib functions needed for upcoming assignments.
* Relevant emlib project files were copied from SiliconLabs\SimplicityStudio\v4\developer\sdks\gecko_sdk_suite\v2.4\platform as needed and added into the respective directories at the root.
* The main.c file in the root folder was renamed [gecko_main.c](gecko_main.c).  Contents of the main while loop were moved into functions and the main() function was #ifdef'd out.
* The [src](src) subfolder was added to contain code specific to the ECEN 5823 course and source files were added to support ECEN 5823 and the simplicity studio exercise assignment.
* The [questions](questions) subfolder was added to contain questions to be answered for each assignment, along with the questions for the first assignment.
