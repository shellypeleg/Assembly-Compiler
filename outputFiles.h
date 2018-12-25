/*
 * =========================================================================================
 * name        : Assembler/outputFiles.h
 * author      : Lior Peleg (308541457), Shelly Peleg (205417116)
 * =========================================================================================
 */

#ifndef OUTPUTFILES_H_
#define OUTPUTFILES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"

#define asmExt ".as"


/*---------------------------------------------------------------*/
/*This method gets the current file name, and stores it in an array */
void SetFilesName(char *fileName);

/*---------------------------------------------------------------*/

/*This method opens the object file (.ob) which holds the main program encoding
 * Output - 1 if the opening was successfull
 *        - 0 opening failed
 */

int OpenObjectFile();

/*---------------------------------------------------------------*/

/*This method opens the entry file (.ent) which holds entry label encoding
 * Output - 1 if the opening was successfull
 *        - 0 opening failed
 */

int OpenEntryFile();

/*---------------------------------------------------------------*/

/*This method opens the external file (.ext) which holds external label apperance encoding
 * Output - 1 if the opening was successfull
 *        - 0 opening failed
 */

int OpenExternalFile();

/*---------------------------------------------------------------*/
/*This method writes the rows into the object file
 *Input - address - the instruction address
 *      - encoding - the instruction encoding
 */
void ObjectWriteLine(int address, int encoding);

/*---------------------------------------------------------------*/

/*This method writes the rows into the entry file
 *Input - label - the label name
 *      - address - the label address
 */

void EntryWriteLine(char *label, int address);

/*---------------------------------------------------------------*/

/*This method writes the rows into the external output file
 *Input - label - the external label name
 *      - address - the label apperance address
 */

void ExternalWriteLine(char *label, int address);

/*---------------------------------------------------------------*/
/*This method closes the object file (.ob) which holds the main program encoding
 * Output - 1 if the closing was successfull
 *        - 0 opening failed
 */

int CloseObjectFile();

/*---------------------------------------------------------------*/

/*This method closes the entry file (.ent) which holds entry label encoding
 * Output - 1 if the closing was successfull
 *        - 0 closing failed
 */

int CloseEntryFile();

/*---------------------------------------------------------------*/

/*This method opens the external file (.ext) which holds external label apperance encoding
 * Output - 1 if the opening was successfull
 *        - 0 opening failed
 */

int CloseExternalFile();

/*---------------------------------------------------------------*/

/*This method closes all open files (object, entry, external)
 If closing was successfull return true, otherwisw - false*/
void CloseOpenFiles();

/*---------------------------------------------------------------*/
/*this method deleted all the created files - in case error found while compiling the input files*/
void DeleteOutputFiles();

/*---------------------------------------------------------------*/

/*This method converts a binary number (represented as an integer) to a strange 32 base code (presented in symbols and letters)
 *Input  - an address in binary base (represented as a number)
 *Output - if converting was succssfull, this method returns an array of characters in 32 strange base*/

char *BinaryToStrange32BaseAddress(int address);

/*---------------------------------------------------------------*/

/*This method converts a binary number (represented as an integer) to a strange 32 base code (presented in symbols and letters)
 *Input  - a code in binary base (represented as a number)
 *Output - if converting was succssfull, this method returns an array of characters in 32 strange base*/

char *BinaryToStrange32BaseCode(int encoding);




#endif 