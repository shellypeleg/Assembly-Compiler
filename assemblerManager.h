/*
 * =========================================================================================
 * name        : Assembler/assemblerManager.h
 * author      : Lior Peleg (308541457), Shelly Peleg (205417116)
 * =========================================================================================
 */

#ifndef ASSEMBLERMANAGER_H_
#define ASSEMBLERMANAGER_H_


/*  This function gets a list of assembly files and compiles them
 *  Returns: 1 - The compiling was successfull
 *           0 - Compiling failed, an error was recieved during the compiling process  */
void AssemblyCompiler(char **filesList);

/*---------------------------------------------------------------*/

/*For getting the input file name, without the folder path before, if exists*/
void getOutputFilesName(char *file);

#endif