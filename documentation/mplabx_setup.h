/** @page mplabx Configuration in MPLABX
 *
 * This guide assumes you have already created a project and have the library on your PC
 *
 * @section include Configuring Include Directories
 * In order for the compiler to find the libraries header files, you must set the include directories for the project.
 * To do so:
 * - Right-click on the project in the Projects window, and click Properties.
 * - From the Properties window, go to Conf->XC32 (Global Options)->xc32-gcc in the menu tree on the left.
 * - From the drop-down, select 'Preprocessing and messages'
 * - Click the ... button next to Include directories
 * - Click 'Browse' to add directories
 *
 * You need to add the following directories:
 *
 * - "PathtoLibrary\include"
 * - "PathtoLibrary\hal\hal_includes"
 * - "PathtoLibrary\hal\ProcessorFamily\Processor" (example "PathtoLibrary\hal\PIC32\PIC32MX250F128B")
 *
 * If you have created your system.h file in the project directory, you will also need to add the project directory to the include directories.
 * Unfortunately, MPLABX requires a workaround for you to configure this correctly.
 * From the Include Directories window:
 * - Double-click on the (Enter or 'Browse' string here)
 * - Type "..\<project dir>" replacing "<project dir>" with your projects name
 *
 * Press Ok when you are done making changes
 *
 * @section cFiles Adding the Source Files
 *
 * Once you have setup your include directories, you'll need to add the source files to your project.  First start by adding the library modules
 * to your project.
 * To do so:
 * - Right-click the project in the Project Explorer and click 'Add Existing Item'
 * - Locate the libraries source files at directory 'PathtoLibrary\src'.
 * - Select the .c files you would like to use for your project.  For example, if you are using the timing module and the task module,
 *   select timing.c and task.c.
 * - Ensure that the Copy checkbox on the right-side of the dialog is unchecked.  This will link the files rather than copy so that the
 *   library can easily be updated across all projects at once.
 *
 * Also, note that you will need to add all dependencies for a desired library.  For example, if you use the UART module, you will need to add the charReceiver module as well.  Refer to
 * module documentation to determine dependencies.
 *
 * Once you have added the library source files to your project, you will need to follow the same procedure to add the HAL layer's source files
 * for the desired processor.  These source files are located at 'PathtoLibrary/hal/ProcessorFamily/Processor
 * (example "PathtoLibrary\hal\MSP430\MSP430F5529")'. WARNING:  DO NOT ADD THE hal_timing.c FILE!  This file is automatically included through
 * the timing.c file (a trick used to make the timing interrupt as efficient as possible).
 *
 */