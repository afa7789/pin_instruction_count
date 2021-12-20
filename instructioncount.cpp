/*
 * Copyright 2002-2019 Intel Corporation.
 * 
 * This software is provided to you as Sample Source Code as defined in the accompanying
 * End User License Agreement for the Intel(R) Software Development Products ("Agreement")
 * section 1.L.
 * 
 * This software and the related documents are provided as is, with no express or implied
 * warranties, other than those that are expressly stated in the License.

 ../../../pin -t obj-intel64/instructioncount.so -- Arthur/a.out 
 cd ..
 
 ../../../pin -t obj-intel64/instructioncount.so -- /bin/ls
 cat inscount.out
 rm -rf inscount.out

 ../../../pin -t obj-intel64/instructioncount.so -- Arthur/a.out 
 cat inscount.out

 ../../../pin -t obj-intel64/instructioncount.so -- Arthur/a.out 
 ../../../pin -t obj-intel64/instructioncount_old.so -- Arthur/a.out 

 */

#include <cassert>
#include <iostream>
#include <stdio.h>
#include "pin.H"
#include "hashmap/hash.h"

using namespace std;

#define ARG_NUMBER_FILE 2

#define INDEX_WHERE_IS_FILE_NAME 1

KNOB<string> KnobOutputFile(
    KNOB_MODE_WRITEONCE,
    "pintool",
    "o",
    "default",
    "specify output file name"
);

// FILE * trace;

// This function is called before every instruction is executed
// and prints the IP
// VOID printip(VOID *ip) { fprintf(trace, "%p\n", ip); }

VOID countip(VOID *ip,INT size) {
    UniqueInstr* instr = get_instr( (Addr) ip , size);
    instr_inc(instr, 1);
}

// Pin calls this function every time a new instruction is encountered
VOID InstructionTrace(INS ins, VOID *v)
{   
    int size = INS_Size(ins);
    
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)countip, IARG_INST_PTR, IARG_UINT64 ,size , IARG_END);
}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
    dump_instrs();
    destroy_instrs_pool();
}

// This function is called when the application exits
VOID Fini_in_file(INT32 code, VOID *v)
{
        
    dump_instrs_in_file((FILE*)v);
    destroy_instrs_pool();
    fclose((FILE*)v);
}


/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    PIN_ERROR("This Pintool prints the IPs of every instruction executed\n" 
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char * argv[])
{

    FILE * fp;
    fp = NULL;
    
    init_instrs_pool();
    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    if( KnobOutputFile.Value() != "default" ){
        fp = fopen(KnobOutputFile.Value().c_str(), "w");//opening file.
    }

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(InstructionTrace, 0);

    if (fp == NULL) {
        // Register Fini to be called when the application exits
        PIN_AddFiniFunction(Fini, 0);
    }else{
        // Register Fini to be called when the application exits
        PIN_AddFiniFunction(Fini_in_file, fp);
    }


    // Start the program, never returns
    PIN_StartProgram();

    return 0;
}
