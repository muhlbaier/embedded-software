/** @page mspgcc Configuration of MSPGCC in Code Composer Studio
 *
 * This guide assumes you have already created a project and have the library on your PC
 *
 * @section installing Installing the MSPGCC compiler
 *
 * Before attempting to install the MSPGCC compiler, verify that you do not already have it.  You may have
 * installed it when installing CCS.
 *
 * To check if you have the MSPGCC compiler installed:
 * - Right-click on the project in the Project Explorer, and click Properties.
 * - Navigate to General
 * - In the compiler version drop down, check if you have the GNU compiler listed
 *
 * If you have the GCC compiler installed, you may skip to the next section.
 *
 * If you do not have the GCC compiler installed:
 * - Navigate to View -> CCS App Center
 * - Search or find on the front page the MSP430 GCC CCS Add-On
 * - Install the Add-On
 *
 * @section configuring Configuring your project to use the GCC compiler
 *
 * These instructions will guide you through setting up the GCC compiler for an existing project
 *
 * Before configuring anything, you must create a new build configuration that uses the GCC compiler.
 * To do so:
 * - Right-click on the project in the Project Explorer, and click Properties.
 * - Navigate to General
 * - In the compiler version drop down, select the GNU compiler
 * - Press Ok on the Properties window and CCS will notify you that it will automatically create a new build configuration
 *
 * Now that you have a build configuration that uses the GCC compiler, you must setup the project much like you do for the TI compiler
 *
 * In order for the compiler to find the libraries header files, you must set the include directories for the project.
 * To simplify things first configure a Linked Resource Path Variable:
 * - Right-click on the project in the Project Explorer, and click Properties.
 * - Go to Resource->Linked Resources then to the Path Variables tab
 * - Click "New..." and create a Path Variable named LIB_ROOT and set the location to
 * the location of the library base.
 *
 * Next to add the actual includes:
 * - Right-click on the project in the Project Explorer, and click Properties.
 * - From the Properties window, go to Build->GNU Compiler->Directories in the menu tree on the left.
 * - In the box, labeled 'Include paths (-I)'' press the add button.
 * - Click 'File System' and browse to locate the directories.
 *
 * You need to add the following directories:
 *
 * - "${LIB_ROOT}/include"
 * - "${LIB_ROOT}/hal/ProcessorFamily/Processor" (example "${LIB_ROOT}/hal/MSP430/MSP430F5529"")
 *
 * If you have created your system.h file in the project directory, you will also need to add the project directory to the include directories.
 * To do so:
 * - Press the add button but this time click 'Workspace' rather than 'File System'.
 * - This should open a window with each project in the workspace shown.
 * - Click the project you are configuring and press Ok.
 *
 * Press Ok when you are done making changes
 *
 * The last thing that needs to be configured to build your project is to add two Symbols to the configuration.  The first will allow the
 * MSP430 support files to work correctly.  The second allows the library to change how it handles certain preprocessor directives such as
 * interrupt declarations (see below)
 *
 * To add the symbols:
 * - Right-click on the project in the Project Explorer, and click Properties.
 * - Navigate to Build->GNU Compiler->Symbols
 * - In the top list box, add two symbols
 *    - \_\_MSP430F5529\_\_
 *    - MSPGCC
 *
 *  @section interrupts Interrupt Declarations
 *
 * There are very few things that change from a software perspective when switching between the TI compiler and the GCC compiler.  One
 * quirk that is not uniform across compilers is that declaration of interrupts.  However, using the code template below and the symbol
 * you previously defined, the declarations can be written so that both compilers are satisfied.
 *
 * @code
 *
 * #ifdef MSPGCC
 * __attribute__((interrupt(<INTERRUPT_VECTOR>)))
 * void <ISR_NAME>(void) {
 * #else
 * #pragma vector=<INTERRUPT_VECTOR>
 * __interrupt void <ISR_NAME>(void){
 * #endif
 * // Insert your ISR functionality here
 * }
 * @endcode
 *
 */
