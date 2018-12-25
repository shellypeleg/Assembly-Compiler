/*
 * =========================================================================================
 * name        : Assembler/instructions.c
 * author      : Lior Peleg (308541457), Shelly Peleg (205417116)
 * =========================================================================================
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "instructions.h"
#include "outputFiles.h"


/*Max and min values for immadiet number - 8 bits*/
#define IMMADIET_MAX 127
#define IMMADIET_MIN -128

/*for register validation*/
#define REGISTER_PREFIX 'r'
#define REGISTER_MAX_NUM '7'

#define dotDelimeter "."

/*number of addressing types in the assembler*/
#define MAX_ADDRESS_TYPE 4

/*A complete instruction memory word*/
typedef struct{
    int instructionCode; /*memory word without ARE*/
    LINK_TYPE linkType; /*2 bits for ARE*/
}instructionWord;

instructionWord inst_arr[CODE_ARR_SIZE]; /*An array to hold all instruction words*/
/*---------------------------------------------------------------*/

/*This matrix hold the valid addressing type for each command
 *first 4 addressing types are for source operand, last 4 addresing types are for destination operand*/

OperandType legalAddressesType[CMD_MAX_NUM][8] = {
/*mov*/ {Immediate,Direct,Struct_op,Register_op,ERR,Direct,Struct_op,Register_op}, 
/*cmp*/ {Immediate,Direct,Struct_op,Register_op,Immediate,Direct,Struct_op,Register_op},
/*add*/ {Immediate,Direct,Struct_op,Register_op,ERR,Direct,Struct_op,Register_op},
/*sub*/ {Immediate,Direct,Struct_op,Register_op,ERR,Direct,Struct_op,Register_op},
/*not*/ {ERR,ERR,ERR,ERR,ERR,Direct,Struct_op,Register_op},
/*clr*/ {ERR,ERR,ERR,ERR,ERR,Direct,Struct_op,Register_op},
/*lea*/ {ERR,Direct,Struct_op,ERR,ERR,Direct,Struct_op,Register_op},
/*inc*/ {ERR,ERR,ERR,ERR,ERR,Direct,Struct_op,Register_op},
/*dec*/ {ERR,ERR,ERR,ERR,ERR,Direct,Struct_op,Register_op},
/*jmp*/ {ERR,ERR,ERR,ERR,ERR,Direct,Struct_op,Register_op},
/*bne*/ {ERR,ERR,ERR,ERR,ERR,Direct,Struct_op,Register_op},
/*red*/ {ERR,ERR,ERR,ERR,ERR,Direct,Struct_op,Register_op},
/*prn*/ {ERR,ERR,ERR,ERR,Immediate,Direct,Struct_op,Register_op},
/*jsr*/ {ERR,ERR,ERR,ERR,ERR,Direct,Struct_op,Register_op},
/*rts*/ {ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},
/*stop*/{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR}
};
/*---------------------------------------------------------------*/

/*This table will hold all the information about the commands in the assembly language:
 * command name,
 * command number,
 * valid input operands number,
 * valid addressing types for source operand,
 * valid addressing types for destination operand
 */
commandType cmdTab[CMD_MAX_NUM] = {
    {"rts", rts, 0},
    {"stop", stop, 0},
    {"not", not, 1},
    {"clr", clr, 1},
    {"inc", inc, 1},
    {"dec", dec, 1},
    {"jmp", jmp, 1},
    {"bne", bne, 1},
    {"red", red, 1},
    {"prn", prn, 1},
    {"jsr", jsr, 1},
    {"mov", mov, 2},
    {"cmp", cmp, 2},
    {"add", add, 2},
    {"sub", sub, 2},
    {"lea", lea, 2}
    };


/*---------------------------------------------------------------*/

/*This method gets a row and checks if it contains only whitespaces
 *Input - row (as a string) to be checked
 *Output -  0 in case there's at least one non-whitespace character
 *          1 in case the row contains only whitespcaes 
 */
int IsRowEmpty(char *row)
{
    int i = 0; /*will hold current string index*/
    int isEmpty = true; /*empty flag*/

    /*if the row has characters - check if only whitespaces*/
    /*strcmp resturns zero only if the 2 strings are the same*/
    if(strcmp(row,""))
    {
        /*check each character in the row. until this is the end of the string or the flag is false*/
        while (isEmpty && row && (row[i] != '\0') && (row[i] != '\n'))
        {
            if(isspace(row[i]))
                i++;
            else
                isEmpty = false;
        }
    }

    return isEmpty;
}

/*---------------------------------------------------------------*/

/*This method gets a command name and returns all the information about it*/
commandType GetCommandByName(char *cmdName)
{
    int i; /*current table index*/
    commandType resultCommand = {"invalid", instructionTypeError, ERR}; /*default command - error*/
    /*Go over the command table, and find the given command. If found - return the info about this command*/
    for(i=0; i<CMD_MAX_NUM; i++)
    {
        if((strcmp(cmdTab[i].cmd_str, cmdName)) == 0)
        {
            resultCommand = cmdTab[i];
            break;
        }
    }
    return resultCommand;
}

/*---------------------------------------------------------------*/

/*This method removes any whitespces in the given string and returns a trimmed string*/
char *TrimCommand(char *command, int rowNumber)
{
    int hasError = false; /*Error flag*/
    char *trimmedCommand; /*The string without the whitespaces*/
    int elementCount = 0; /*Number of elements found in the string, should be 3 at the most (first+comma+second)*/
    int commaAtEnd = false;  /*Comma is at the end flag*/
    int commaAtStart = false; /*Comma is at the start flag*/

    trimmedCommand = (char *)malloc(MAX_ROW_LENGTH);

    /*if memory allocation failed*/
    if(!trimmedCommand)
    {
        fprintf(stderr, "Error: Unable to allocate memory for a trimmed command in the first scan. Exiting the program.\n");
        exit(1);
    }

    /*reset the trimmed command to be all null character*/
    memset(trimmedCommand, '\0', sizeof(trimmedCommand));

    /*do until it's the end of the instruction or if element count is over 3*/
    while(command != NULL && elementCount <= 3)
    {
        /*check if the element contains a comma*/
        if(strchr(command, commaDelimeter[0]) != NULL)
        {
            unsigned int commaLocation; /*the location of the comma in the given command*/
            unsigned int elementLength;
            commaLocation = (strchr(command,commaDelimeter[0])-command); /*pointers subtraction will give the distance between them in numbers*/
            elementLength = strlen(command)-1; /*the length of the given element*/

            /*check if the comma is at the end*/
            if(commaLocation == elementLength)
                commaAtEnd = true;
            /*check if the comma is at the start*/
            if(commaLocation == 0)
                commaAtStart = true;
        }

        strcat(trimmedCommand, command);
        elementCount++;
        command = strtok(NULL,whitespaceDelim);
    }

    /*if the comma is not at the start or at the end of the element, the number of elements should be 1
     *if one of the elements starts or ends with a comma, the number of elements should be 2
     *there is no option for more than 3 elements
     */

    if((elementCount > 1 && !commaAtStart && !commaAtEnd) || ((commaAtEnd ^ commaAtStart) && elementCount !=2) || (elementCount >3))
    {
        hasError = true;
        fprintf(stderr, "Error: Invalid number of arguments in row %d.\n",rowNumber);
    }

    if(hasError)
        free(trimmedCommand);
    else
    {
        /*delete the row break from the end if exists*/
        int commandLength = strlen(trimmedCommand);
        if(trimmedCommand[commandLength-1] == '\n')
            trimmedCommand[commandLength-1] = '\0';
    }

    return trimmedCommand;
}

/*---------------------------------------------------------------*/
/*This method calculates the how many memory words each row in the assembly file takes
 *Input:  source - the first operand in the instruction
 *        destination - the second operand in the instruction
 *        rowNum - the curren row in the assembly file from which this command was taken
 *        cmdType - the command name
 *Output: the number of memory words the command takes
*/
int CalculateInstructionOffset(char *source, char *destination, int rowNum, instructionType cmdType)
{
    int sourceOffset = 0; /*how many memory words the source operand takes*/
    int destinationOffset = 0; /*how many memory words the destination operand takes*/
    OperandType srcType, dstType;

    /*Calculation for source operand only if it exists*/
    if(source != NULL && !IsRowEmpty(source))
    {       
        srcType = GetOperandType(source);
        /*if source type is not an error, check if it's addressing type is valid for the specific command*/
        if(srcType != AddressTypeError && cmdType != instructionTypeError)
        {   
            if(legalAddressesType[cmdType][srcType] == ERR)
            {
                    fprintf(stderr, "Error: Invalid addressing type for source operand. Can't calculate offset. Row Number: %d.\n",rowNum);
                    return 0;
            }

            else
            {
                /*get the source operand offset*/
                sourceOffset = OperandTypeOffset(srcType);

                /*if the offset is not valid, there's an error with the source operand*/
                if(sourceOffset == ERR)
                {
                    fprintf(stderr, "Error: Invalid source operand type for %s. Row number: %d.\n",source,rowNum);
                    return 0;
                }
            }
        }
    }
    
    /*Calculation for destination operand only if it exists*/
    if (destination != NULL)
    {
        
        dstType = GetOperandType(destination);
        
        /*if destination type is not an error, check if it's addressing type is valid for the specific command*/
        if(dstType != AddressTypeError && cmdType != instructionTypeError)
        { 
            if(legalAddressesType[cmdType][(MAX_ADDRESS_TYPE+dstType)] == ERR)
            {
                    fprintf(stderr, "Error: Invalid addressing type for destination operand. Can't calculate offset. Row Number: %d.\n",rowNum);
                    return 0;
            }

            else
            {
                /*Get the destination operand offset*/
                destinationOffset = OperandTypeOffset(dstType);

                /*if the offset is not valid, there's an error with the source operand*/
                if(destinationOffset == ERR)
                {
                    fprintf(stderr, "Error: Invalid destination operand type for %s. Row number: %d.\n",destination,rowNum);
                    return 0;
                }
            }
        }
    }

    /*if the 2 operands are registes it takes only one memory word for both*/
    if(srcType == Register_op && dstType == Register_op)
        destinationOffset = 0;

    /*return the sum of offset + one extra memory word for the basic memory word*/
    
    return sourceOffset + destinationOffset + 1;
}
/*---------------------------------------------------------------*/
/*This function gets a string and checks if it's a valid operand
 *Input  - A string
 *Output - Operand Type 
 */

OperandType GetOperandType(char *operand)
{
    OperandType resultOp = AddressTypeError; /*Will hold the operand if found*/
    int immadietNum; /*will hold the number of an immadiet number*/
    char symbol[MAX_LABEL_LENGTH]; /*will hold a label / struct name*/
    char *token; /*will hold the parameters of struct*/
    int checkedFlag = 0; /*if we checked any type flag*/

    /*allocate memory for the token*/
    token = (char *)malloc(MAX_ROW_LENGTH);
    if(!token)
    {
        fprintf(stderr,"Error: Cannot allocate memory for parsing struct operand. exiting program.\n");
        exit(1);
    }
    /*Check if the operand is an immadiet number - starts with '#' followed by a number*/
    if (*operand=='#')
    {
        char *tempImNum = (char*)malloc(sizeof(int));
        if(!tempImNum)
        {
            fprintf(stderr,"Error: Cannot allocate memory for temp immidaiet token. Exiting program\n");
            exit(1);
        }

        tempImNum = strtok(operand,"#");
        immadietNum = atoi(tempImNum);

        /*check if the number is in range of 8 bits - 128 to -127*/
        if(immadietNum<=IMMADIET_MAX && immadietNum >= IMMADIET_MIN)
        {
            resultOp = Immediate;
            checkedFlag = 1;
        }

        else
        {
            checkedFlag = 1;
        }
    }
    /*Check if the given operand is a register*/
    else if((strlen(operand) == 2) && (*operand == REGISTER_PREFIX) && (isdigit(operand[1])) && ((int)operand[1]-REGISTER_MAX_NUM <= 0))
    {
        resultOp = Register_op;
        checkedFlag = 1;
    }


    /*check if the given operand is a label name*/
    else if (sscanf(operand, "%30[a-zA-Z0-9]",symbol)==1)
    {
        if(strcmp(operand,symbol)==0)
        {
            
            resultOp = Direct;
            checkedFlag = 1;
        }
    }

    if(checkedFlag == 0)
    {
        char tempStruct[MAX_ROW_LENGTH];
        strcpy(tempStruct,operand);

        /*check if the given operand is a valid struct usage*/
        if((token = strtok(tempStruct,dotDelimeter))!=NULL)
        {
            if((strcmp(token,operand)) != 0)
            {
                /*the first parameter should be a valid label*/
                if (sscanf(token, "%30[a-zA-Z0-9]",symbol))
                {
                    if((strcmp(token,symbol))!= 0)
                    {
                        resultOp=AddressTypeError;
                    }
                
                }
                /*get the next parameter - should be struct field number 1/2 */
                token = strtok(NULL,dotDelimeter);
                
                if (strcmp(token,"1")!=0 && strcmp(token,"2")!=0)
                {
                    resultOp = AddressTypeError;
                }

                else
                    resultOp = Struct_op;
            }

            else
                resultOp = AddressTypeError;  
        }
    }

    return resultOp;
}


/*---------------------------------------------------------------*/

/*This method gets an operand type and returns how many memory words it takes*/
int OperandTypeOffset(OperandType op_type)
{
    int offset = 0; /*will hold the result*/

    /*operands that take 1 memory word*/
    if(op_type == Direct || op_type == Immediate || op_type == Register_op)
        offset = 1;

    /*operands that take 2 memory words*/
    else if(op_type == Struct_op)
        offset = 2;
    
    /*operand type error*/
    else if (op_type == AddressTypeError)
        return ERR;
    
    return offset;
}

/*---------------------------------------------------------------*/
/*reset all the fields of the command info struct*/
void ResetCommandInfo(instructionFields *cmdInfo)
{
    cmdInfo->dstReg = 0;
    cmdInfo->dstType = 0;
    cmdInfo->srcReg = 0;
    cmdInfo->srcType = 0;
    cmdInfo->lType = LINK_TYPE_A; /*Default linking type*/
}
/*---------------------------------------------------------------*/

/*This method adds an instruction to the instruction words array
 *Input - link_type - the link type of this command (Absolute/ Relocatable/ External)
 *      - IC - instruction counter, the index of the command in the instruction list. 
 *      - commandCode - the command binary code without ARE 
 *      - inst_str - the arguments after the command name
 *Output - 0 - error found
 *       - 1 - no error found, instruction was added successfully
 */

int AddInstruction(LINK_TYPE link_type, int *IC, int commandCode, char *inst_str)
{
    int hasError = false; /*Error flag*/

    /*nothing shows after after the command - error (commands without operands or label are not being handeld in this function*/
    if(!inst_str)
    {
        hasError = true;
        fprintf(stderr,"Error: Command can't be empty, must get operands or label.\n");

    }

    else
    {
        (*IC)++; /*save one place for the basic memory word*/
        inst_arr[*IC].instructionCode = (commandCode << 2) + link_type;
        inst_arr[*IC].linkType = link_type;

        /*if the link type is external, write the label apperance in the external output file*/
        if(link_type == LINK_TYPE_E)
        {
            ExternalWriteLine(inst_str, *IC);   
        }
    }

    return !hasError;
}
/*---------------------------------------------------------------*/

/*This function parses a command operands and stores its extra memory words in the instruction words array
* Input - operandName - the given operand to parse (name as a string)
*       - opType - current operand type (register/immadiet...)
*       - registerNum - if the current operand is a register, this will hold it's number
*       - IC - instruction counter
*       - rowNum - the current row number in the assembly file
*       - isDestination - flag to indicate if the given operand is source or destination 
* Output- 0 - Error found during parsing
*         1 - parsing and storing the memory word werr successfull
*/
int ParseOperand(char *operandName, OperandType *opType, int *registerNum, int *IC, int rowNum, int isDestination)
{
    int imNum; /*Will hold an immadiet number*/
    int hasError = false; /*Error flag*/
    symbol *sym; /*will hold information about a label from the symbol table*/
    char *labelName; /*will hold the struct name*/
    int structField; /*will hold the struct field number (1/2) */
    int typeFlag = 0;

    /*allocate memory for labelName*/
    labelName = (char *)malloc(MAX_ROW_LENGTH);

    if(!labelName)
    {
        fprintf(stderr,"Error: Cannot allocate memory for parsing struct usage. Exiting program.\n");
        exit(1);
    }

    /*Operand is a register*/
    /*Extra memory word is the register number*/
    if(strlen(operandName) == 2 && *operandName == REGISTER_PREFIX && isdigit(operandName[1]) && (int)operandName[1]-REGISTER_MAX_NUM <= 0)
    {
        *opType = Register_op;
        *registerNum = operandName[1]-'0'; /*will hold the register number - not as a character*/

        if(!isDestination)
        {
            *registerNum = *registerNum << 4; /*Store the register source number in the 6th-9th bits*/
        }

        if(!AddInstruction(LINK_TYPE_A, IC, *registerNum, operandName))
        {
            hasError = true;
            fprintf(stderr,"Error:Invalid register name- %s.Can't add its memory word. Row number: %d.\n",operandName,rowNum);
        }

        typeFlag = 1;
    }

    /*Operand is an immadiet number - # followed by a number*/
    /*Extra Memory word is the number itself*/
    else if(*operandName=='#')
    {
        char *tempImNum = (char*)malloc(sizeof(int));
        if(!tempImNum)
        {
            fprintf(stderr,"Error: Cannot allocate memory for temp immidaiet token. Exiting program\n");
            exit(1);
        }
        tempImNum = strtok(operandName,"#");
        imNum = atoi(tempImNum);

        /*check if the number is in range of 8 bits - 128 to -127*/
        if(imNum<=IMMADIET_MAX && imNum >= IMMADIET_MIN)
        {
            *opType = Immediate;

            if(!AddInstruction(LINK_TYPE_A, IC, imNum, operandName))
            {
                hasError = true;
                fprintf(stderr,"Error: Invalid immadiet number - %s. Can't add its memory word. Row number: %d.\n",operandName, rowNum);
            }
            typeFlag = 1;
        }

        else /*number not in range*/
        {
            hasError = true;
            fprintf(stderr,"Error: Immadiet number (%s) is not in range. can only be between %d - %d. Row number: %d.\n",operandName,IMMADIET_MIN, IMMADIET_MAX,rowNum);

        }

    }


    
    /*operand is a label name*/
    /*extra memory word is the label address*/
    else if(sscanf(operandName, "%30[a-zA-Z0-9]", labelName) == 1)
    {
        if((strcmp(labelName,operandName)) == 0)
        {
            *opType = Direct;
            sym = FindSymbol(labelName);

            if(sym)
            {
                hasError = !AddInstruction(sym->symLinkType, IC, sym->symAddress, labelName);
                typeFlag = 1;
            }

            else
            {
                hasError = true;
                fprintf(stderr,"Error: Invalid symbol - %s. can't add its memory word. Row number: %d.\n",operandName, rowNum);
            }
        }
    }
   
    if(typeFlag == 0)
    {
        char *tempOpName; /*Will hold the current operandName*/
        /*allocate memory*/
        tempOpName = (char *)malloc(MAX_ROW_LENGTH);
        /*memory allocation failed*/
        if(!tempOpName)
        {
            fprintf(stderr,"Error: cannot allocate memory for auxiliary token. Exiting program.\n");
            exit(1);
        }
        tempOpName = strcpy(tempOpName,operandName);

        /*operand is a struct usage*/
        /*2 extra memory words - one for symbol (struct name) address and the second for the field number*/
        labelName = strtok(tempOpName,dotDelimeter);

        if((strcmp(labelName,operandName)) != 0)
        { 
            /*get the next parameter - should be struct field number 1/2 */
            structField = atoi(strtok(NULL,dotDelimeter));

            *opType = Struct_op;
            sym = FindSymbol(labelName);

            if (!sym)
            {
                hasError = true;
                fprintf(stderr,"Error: Struct name wasn't found in the symbol table. Row number: %d.\n",rowNum);
            }

            else
            {
                /*First word to save is the struct name address*/
                if(AddInstruction(sym->symLinkType, IC, sym->symAddress, labelName))
                {
                    /*second word to save is the field number*/
                    if(structField == 1 || structField == 2)
                    {
                        if(!AddInstruction(LINK_TYPE_A,IC,structField,operandName))
                        {
                            hasError = true;
                            fprintf(stderr,"Error: Invalid struct field. can't add its memory word. Row number: %d.\n",rowNum);
                        }
                    }

                    else
                    {
                        hasError = true;
                        fprintf(stderr,"Error: invalid struct field number. must be 1 or 2. Can't add it's memory word. Row number: %d.\n",rowNum);
                    }
                }

                else
                {
                    hasError = true;
                    fprintf(stderr,"Error: Invalid struct usage - %s. can't add its memory word. Row number: %d.\n",operandName, rowNum);
                }
            }
        }
    }
    return !hasError;
}

/*---------------------------------------------------------------*/

/*This function store the basic memory word into the instruction words array
 *Input - inst_field - all the instruction information
 *      - IC - the index of this command in the assembly file
 */
void StoreMemWord(instructionFields inst_field, int IC)
{
    /*4 bits for OpCode*/
    inst_arr[IC].instructionCode = (int)inst_field.commandName;

    /*2 bits for source operand type*/
    inst_arr[IC].instructionCode = (inst_arr[IC].instructionCode << 2) + inst_field.srcType;

    /*2 bits for destination operand type*/
    inst_arr[IC].instructionCode = (inst_arr[IC].instructionCode << 2) + inst_field.dstType;

    /*2 bits for ARE = linking type*/
    inst_arr[IC].instructionCode = (inst_arr[IC].instructionCode << 2) + inst_field.lType;
}


/*---------------------------------------------------------------*/

/*This function writes all the instructions memory words from the instructions array, to the object output file
 *Input - initAddress - the first address to begin with, usually 100
 *      - endAddress - the current IC count
 */

void WriteEncodingsToFile(int initAddress, int endAddress)
{
    int i; /*instructions array index*/

    for(i = initAddress; i < endAddress; ++i)
    {
        ObjectWriteLine(i, inst_arr[i].instructionCode);
    }
}

/*---------------------------------------------------------------*/