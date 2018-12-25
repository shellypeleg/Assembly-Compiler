/*
 * =========================================================================================
 * name        : Assembler/symbolTable.h
 * author      : Lior Peleg (308541457), Shelly Peleg (205417116)
 * =========================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"

#ifndef SYMBOLTABLE_H_
#define SYMBOLTABLE_H_


/*---------------------------------------------------------------*/
/* Initialize the Symbole Table, by creating a first node which will represent the first line of the table, and set pointers to it.
 * The first line won't hold a real symbole until the addSymbol function will be used for the first time */

void InitializeSymbolTable();

/*---------------------------------------------------------------*/

/*
* A method used to insert a new line to the symbol table
* Input:
    1. Symbol/label name.
    2. A counter to represent the address (IC/DC).
    3. An int to represent whether this is an external value.
    4. An int to represent whether this is next to an Action sentence.
 * Output:
   - If succeded - 1.
   - If failed - 0 + an error message. 
*/
int AddToSymbolTable (char * symbolName, int counter, int isExternal, int closeToActionLine);

/*---------------------------------------------------------------*/

/*
    This function looks for a specific label on the Symbol table.
    If the label can be found, the function creates a symbol type struct with all given details, and returns it.
    IF the label cannot be found, the function returns NULL.
*/
symbol* FindSymbol(char *label);

/*---------------------------------------------------------------*/

/*This method will free the allocated memory for the symbol table*/
void FreeSymbolTable();

/*---------------------------------------------------------------*/

/*This method will update the address field of a symbol to its current address + offset this method gets
 * Input: offsetAddress - the number to add to all relevant nodes */
void UpdateSymbolTable(int offsetAddress);


#endif