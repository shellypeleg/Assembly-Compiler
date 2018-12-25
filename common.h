/*
 * =========================================================================================
 * name        : Assembler/common.c
 * author      : Lior Peleg (308541457), Shelly Peleg (205417116)
 * =========================================================================================
 */

#ifndef COMMON_H_
#define COMMON_H_

#define MAX_CHAR_TO_READ 1000
#define MAX_ROW_LENGTH 81 /*Max size for row in input file, including \n at the end */
#define MAX_LABEL_LENGTH 31 /*Max size for a label, including '\0' at the end */
#define CMD_MAX_NUM 16 /*Number of legal command names*/

/*Directives related strings*/
#define DOT_DATA ".data"
#define DOT_STRING ".string"
#define DOT_STRUCT ".struct"
#define DOT_ENTRY ".entry"
#define DOT_EXTERN ".extern"

/*For boolean type*/
typedef int bool;
#define true 1
#define false 0

/*For token delimeters*/
#define spaceDelimeter " "
#define commaDelimeter ","
#define tabDelimeter "\t"

#define whitespaceDelim " \t"

#define MAX_FILENAME_LENGTH 256
#define FILENAME_EXT_LENGTH 3 /*all files extension are at the most 3 characters long .ent .ext .ob .as*/

#define MAX_LEGAL_ADDRESS_TYPES 4

/*---------------------------------------------------------------*/

/*Supported command names in our assembly language*/
typedef enum {
    mov = 0, cmp, add, sub, not, clr, lea, inc, dec, jmp, bne, red, prn, jsr, rts, stop, instructionTypeError = -1
} instructionType;

/*---------------------------------------------------------------*/

/*Supported address types*/
typedef enum{
    Immediate = 0,
    Direct,
    Struct_op,
    Register_op,
    AddressTypeError = -1
}OperandType;

/*---------------------------------------------------------------*/

/*This struct will hold all the relevant information about a single command*/
typedef struct command{
    const char *cmd_str; /*command name - cannot be changed*/
    instructionType type;
    int operandsNum; /*the valid number of input operands for this command*/
}commandType;

/*---------------------------------------------------------------*/

/*Linking information
 *Absolute / Relocatable / External */
typedef enum {
    LINK_TYPE_A, LINK_TYPE_E, LINK_TYPE_R
} LINK_TYPE;

/*---------------------------------------------------------------*/

/*This struct will hold all the information about a single instruction*/
typedef struct{
    instructionType commandName;
    OperandType srcType;
    int srcReg; /*if the source operand is a register, the register number will be stored here*/
    OperandType dstType;
    int dstReg; /*if the destination operand is a register, the register number will be stored here*/
    int lType; /*linking type*/
}instructionFields;

/*---------------------------------------------------------------*/

/*This struct will hold information about a single symbol*/
typedef struct symbol_type {
    char label[MAX_LABEL_LENGTH];
    int symAddress; 
    LINK_TYPE symLinkType;
} symbol;

#endif