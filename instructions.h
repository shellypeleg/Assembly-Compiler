/*
 * =========================================================================================
 * name        : Assembler/instructions.h
 * author      : Lior Peleg (308541457), Shelly Peleg (205417116)
 * =========================================================================================
 */

#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#include "common.h"
#include "symbolTable.h"

#define ERR -1 /*invalid addressing type for operand*/
#define CODE_ARR_SIZE 500 /*Max size for instructions memory words*/


/*---------------------------------------------------------------*/

/*This method gets a row and checks if it contains only whitespaces
 *Input - row (as a string) to be checked
 *Output -  0 in case there's at least one non-whitespace character
 *          1 in case the row contains only whitespcaes 
 */
int IsRowEmpty(char *row);

/*---------------------------------------------------------------*/

/*This method gets a command name and returns all the information about it*/
commandType GetCommandByName(char *cmdName);

/*---------------------------------------------------------------*/

/*This method removes any whitespces in the given string and returns a trimmed string*/
char *TrimCommand(char *command, int rowNumber);

/*---------------------------------------------------------------*/
/*This method calculates the how many memory words each row in the assembly file takes
 *Input:  source - the first operand in the instruction
 *        destination - the second operand in the instruction
 *        rowNum - the curren row in the assembly file from which this command was taken
 *        cmdType - the command name
 *Output: the number of memory words the command takes
*/
int CalculateInstructionOffset(char *source, char *destination, int rowNum, instructionType cmdType);

/*---------------------------------------------------------------*/
/*This function gets a string and checks if it's a valid operand
 *Input  - A string
 *Output - Operand Type 
 */

OperandType GetOperandType(char *operand);

/*---------------------------------------------------------------*/

/*This method gets an operand type and returns how many memory words it takes*/
int OperandTypeOffset(OperandType op_type);

/*---------------------------------------------------------------*/
/*reset all the fields of the command info struct*/
void ResetCommandInfo(instructionFields *cmdInfo);

/*---------------------------------------------------------------*/

/*This method adds an instruction to the instruction words array
 *Input - link_type - the link type of this command (Absolute/ Relocatable/ External)
 *      - IC - instruction counter, the index of the command in the instruction list. 
 *      - commandCode - the command binary code without ARE 
 *      - inst_str - the arguments after the command name
 *Output - 0 - error found
 *       - 1 - no error found, instruction was added successfully
 */

int AddInstruction(LINK_TYPE link_type, int *IC, int commandCode, char *inst_str);

/*---------------------------------------------------------------*/

/*This function parses a command operands and stores its memory word in the instruction words array
* Input - operandName - the given operand to parse (name as a string)
*       - opType - current operand type (register/immadiet...)
*       - registerNum - if the current operand is a register, this will hold it's number
*       - IC - instruction counter
*       - rowNum - the current row number in the assembly file
*       - isDestination - flag to indicate if the given operand is source or destination 
* Output- 0 - Error found during parsing
*         1 - parsing and storing the memory word werr successfull
*/
int ParseOperand(char *operandName, OperandType *opType, int *registerNum, int *IC, int rowNum, int isDestination);

/*---------------------------------------------------------------*/
/*This function store the basic memory word into the instruction words array
 *Input - inst_field - all the instruction information
 *      - IC - the index of this command in the assembly file
 */
void StoreMemWord(instructionFields inst_field, int IC);

/*---------------------------------------------------------------*/

/*This function writes all the instructions memory words from the instructions array, to the object output file
 *Input - initAddress - the first address to begin with, usually 100
 *      - endAddress - the current IC count
 */

void WriteEncodingsToFile(int initAddress, int endAddress);



#endif