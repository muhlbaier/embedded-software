/** @page ccs Configuration in Code Composer Studio
 *
 * This guide assumes you have already created a project and have the library on your PC
 *
 * @section include Configuring Include Directories
 * In order for the compiler to find the libraries header files, you must set the include directories for the project.
 * 
 * To simplify things first configure a Linked Resource Path Variable:
 * - Right-click on the project in the Project Explorer, and click Properties.
 * - Go to Resource->Linked Resources then to the Path Variables tab
 * - Click "New..." and create a Path Variable named EMBEDDED_SOFTWARE and set the location to
 * the location of the library base.
 *
 * Next to add the actual includes:
 * - Right-click on the project in the Project Explorer, and click Properties.
 * - From the Properties window, go to Build->MSP430 Compiler->Include Options in the menu tree on the left. You should now see two list boxes.
 * - In the lower box, labeled 'Add dir to #include search path (--include_path, -l)' press the add button.
 * You need to add the following directories:
 *
 * - "${EMBEDDED_SOFTWARE}/include"
 * - "${EMBEDDED_SOFTWARE}/hal/ProcessorFamily/Processor" (example "${EMBEDDED_SOFTWARE}/hal/MSP430/MSP430F5529"")
 *
 * If you have created your system.h file in the project directory, you will also need to add the project directory to the include directories.
 * To do so:
 * - Press the add button but this time click 'Workspace' rather than 'File System'.
 * - This should open a window with each project in the workspace shown.
 * - Click the project you are configuring and press Ok.
 *
 * @section cFiles Adding the Source Files
 *
 * Once you have setup your include directories, you'll need to add the source files to your project.  First start by adding the library modules
 * to your project.
 * To do so:
 * - Right-click the project in the Project Explorer and click 'Add Files...'
 * - Locate the libraries source files at directory 'PathtoLibrary/src'.
 * - Select the .c files you would like to use for your project.  For example, if you are using the timing module and the task module,
 *   select timing.c and task.c.
 * - Click Ok, you should be prompted to either copy the files or link to them. So that the library can easily be updated across all
 *   projects at once, please select link and make the link relative to EMBEDDED_SOFTWARE
 *
 * Also, note that you will need to add all dependencies for a desired library.  For example, if you use the UART module, you will need to add the charReceiver module as well.  Refer to
 * module documentation to determine dependencies.
 *
 * Once you have added the library source files to your project, you will need to follow the same procedure to add the HAL layer's source files
 * for the desired processor.  These source files are located at 'PathtoLibrary/hal/ProcessorFamily/Processor
 * (example PathtoLibrary/hal/MSP430/MSP430F5529)'. 
 *
 * @warning DO NOT ADD THE hal_timing.c FILE!  This file is automatically included through
 * the timing.c file (a trick used to make the timing interrupt as efficient as possible).
 *
 * @section TIVAC/TM4C123GH6PMI
 * 
 *Below is a quick tutorial for setting up the embedded library in CCS for the TIVAC. The subsections provide a checklist. It is recommended to go through each, one at a time.
 *Open project properties by right clicking on the project and selecting properties. Once in properties you will need to make sure you have several different paths
 *
 *		@subsection Linked Resources
 *              Under Resources, click on Linked Resources. Ensure that you have the following path directories added.
 *              "PathToTivaWareLibrary", "PathToEmbeddedLibrary"
 *
 *		@subsection File Search Path
 *              Under CCSBuild and under ARM Linker, click on File Search Path. Ensure that you have the following include library files.
 *              "libc.a"  ,  "PathToTivaWareLibrary/driverlib/ccs/Debug/driverlib.lib"
 *
 *		@subsection Include Options
 *              Under CCSBuild and under ARM Compiler, click on Include Options. Ensure that you have the following path directories added.
 *              "PathToTivaWareLibrary" , "PathToTivaWareLibrary\examples\boards\ek-tm4c123gxl" , "PathToEmbeddedLibrary/hal/TIVAC/TM4C123" , "PathToEmbeddedLibrary/include"
 *
 */
