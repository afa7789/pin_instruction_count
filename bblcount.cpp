/*
 * Copyright 2002-2019 Intel Corporation.
 * 
 * This software is provided to you as Sample Source Code as defined in the accompanying
 * End User License Agreement for the Intel(R) Software Development Products ("Agreement")
 * section 1.L.
 * 
 * This software and the related documents are provided as is, with no express or implied
 * warranties, other than those that are expressly stated in the License.

 ../../../pin -t obj-intel64/inscount0.so -- Arthur/a.out 
 cd ..
 ../../../pin -t obj-intel64/inscount0.so -- /bin/ls
 cat inscount.out
 rm -rf inscount.out
 ../../../pin -t obj-intel64/inscount0.so -- Arthur/a.out 
 cat inscount.out
 ../../../pin -t obj-intel64/inscounteach0.so -- Arthur/a.out 
 ../../../pin -t obj-intel64/inscounteach0_old.so -- Arthur/a.out 
 ../../../pin -t obj-intel64/inscounteach0_old.so -o teste_saida.txt -- Arthur/a.out 

 */

#include <cassert>
#include <iostream>
#include <stdio.h>
#include "pin.H"
#include "hashmap/hash.h"

using namespace std;


KNOB<string> KnobOutputFile(
    KNOB_MODE_WRITEONCE,
    "pintool",
    "o",
    "default",
    "specify output file name"
);

// This function is called before every instruction is executed
VOID countip(UniqueInstr ** bbl ,USIZE quantity) {
    for( USIZE i = 0 ; i < quantity ; i++ ){
        instr_inc(bbl[i],1);
    }
}

// Pin calls this function every time a new basic block is encountered
// It inserts a call to docount
VOID Trace(TRACE trace, VOID* v)
{
    // Visit every basic block  in the trace
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        int counter = 0;
        UniqueInstr ** bbl_aux = (UniqueInstr**) malloc(sizeof(UniqueInstr *) * BBL_NumIns(bbl));
        // for each instruction in a bbl setup a memory address for it in the hash
        for( INS ins= BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins) ){
            USIZE size = INS_Size(ins);
            ADDRINT address = INS_Address(ins);
            bbl_aux[counter++] = get_instr( (Addr) address , size);
        }

        int bbl_ins_num = BBL_NumIns(bbl);
        // Insert a call to countip before every bbl, passing the number of instructions , and the pointer to the memory struct that stores the instruction count
        BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR)countip, IARG_PTR, bbl_aux, IARG_UINT64,bbl_ins_num , IARG_END);
    
    }
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
/* Print Help Message                                                    
/* ===================================================================== */

INT32 Usage()
{
    PIN_ERROR("This Pintool prints the IPs of every instruction executed\n" 
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/* ===================================================================== */
/* Main                                                                  
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
    // INS_AddInstrumentFunction(InstructionTrace, 0);

    // Register Instruction to be called to instrument instructions
    TRACE_AddInstrumentFunction(Trace, 0);


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
