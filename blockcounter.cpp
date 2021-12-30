
#include <cstdio>
#include <cassert>
#include <iostream>
#include <string>
#include <map>
#include <list>

#include "pin.H"

using namespace std;

// Representation of a Unique Instruction
struct UniqueInstr {
    ADDRINT addr;
    USIZE size;
    USIZE exec_count;

    UniqueInstr(ADDRINT addr = 0, USIZE size = 0) : addr(addr), size(size), exec_count(0) {}
};

// Representation of a Instruction Group
struct InstrGroup {
    USIZE exec_count;
    std::list<UniqueInstr*> instrs;

    InstrGroup(USIZE size) : exec_count(0) {}
};

// Instructions HashMap
std::map<ADDRINT, UniqueInstr*> instructions;

// Instruction Group as list
std::list<InstrGroup*> groups;


/* ===================================================================== */
/*  Form of getting the instruction or creating a new one                */
/* ===================================================================== */

UniqueInstr* get_instruction(ADDRINT addr, USIZE size = 0) {
    UniqueInstr* instr = instructions[addr];
    if (instr == 0) {
        instr = new UniqueInstr(addr, size);
        instructions[addr] = instr;
    } else {
        assert(instr->size == size);
    }

    return instr;
}

/* ===================================================================== */
/* Way Pin handles command line parameters                               */
/* ===================================================================== */

KNOB<string> KnobOutputFile(
    KNOB_MODE_WRITEONCE,
    "pintool",
    "o",
    "default",
    "specify output file name"
);

KNOB<string> KnobInputFile(
    KNOB_MODE_WRITEONCE,
    "pintool",
    "i",
    "default",
    "specify input file name"
);


/* ===================================================================== */
/* counter increases parameter for every group block                     */
/* ===================================================================== */

VOID counter(InstrGroup* group) {
    ++group->exec_count;
}

/* ===================================================================== */
/* Creates the Trace that builds the blocks                              */
/* ===================================================================== */

VOID Trace(TRACE trace, VOID* v) {
    // Visit every basic block in the trace.
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
        USIZE num_instrs = BBL_NumIns(bbl);
        InstrGroup* group = new InstrGroup(num_instrs);
        groups.push_back(group);

        // for each instruction in a bbl setup a memory address for it in the hash
        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins)) {
            ADDRINT addr = INS_Address(ins);
            USIZE size = INS_Size(ins);

            UniqueInstr* instr = get_instruction(addr, size);
            group->instrs.push_back(instr);
        }

        BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR) counter, IARG_PTR, group, IARG_END);
    }
}

/* ===================================================================== */
/* Counts group into the individual isntructions in the Hash.            */
/* ===================================================================== */

void flush_groups() {
    for (std::list<InstrGroup*>::iterator it = groups.begin(), ed = groups.end();
            it != ed; it++) {
        InstrGroup* group = *it;

        for (std::list<UniqueInstr*>::iterator it2 = group->instrs.begin(), ed2 = group->instrs.end();
                it2 != ed2; it2++) {
            UniqueInstr* instr = *it2;

            instr->exec_count += group->exec_count;
        }
    }
}

/* ===================================================================== */
/* Read previous output as input to add those counters on the hash       */
/* ===================================================================== */

void read_input_file(FILE* fp){
    if (fp == 0)
        return;

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    // Define constant data that will be worked as delimiter
    char delim[] = ":";

    // pointers for functions    
    char *str ="";
    char *ptr ="";
    
    // while file not ended, reading line
    while ((read = getline(&line, &len, fp)) != -1) {
        *str = *line;

        ptr = strtok_r(d, delim,&saveptr1);
        if (ptr != NULL){
            // this works only on perfect scenario        
            ADDRINT address = (ADDRINT)strtol(ptr, NULL, 16);
        }
        
        ptr = strtok_r(d, delim,&saveptr1);
        if (ptr != NULL){
            // this works only on perfect scenario        
            USIZE size = (USIZE) strtol(ptr, NULL, 16);
        }
        
        ptr = strtok_r(d, delim,&saveptr1);
        if (ptr != NULL){
            // this works only on perfect scenario        
            USIZE count = (USIZE) strtol(ptr, NULL, 16);
        }

        // create instr
        UniqueInstr* instr = get_instruction(address, size);
        // add count
        instr->exec_count += count
    }
    
    free(line);
    fclose(fp);
}
/* ===================================================================== */
/* Creates stream of instr for print on file or screen                   */
/* ===================================================================== */

void dump_instrs(FILE* fp) {
    FILE* output = fp != 0 ? fp : stdout;
    for (std::map<ADDRINT, UniqueInstr*>::iterator it = instructions.begin(), ed = instructions.end();
            it != ed; it++) {
        UniqueInstr* instr = it->second;
        fprintf(output ,"0x%lx:%lu:%lu\n", instr->addr, instr->size, instr->exec_count);
    }

    if (fp != 0)
        fclose(fp);
}

/* ===================================================================== */
/* Ends Application , Prints Data                                        */
/* ===================================================================== */

VOID Fini(INT32 code, VOID *v) {
    flush_groups();
    dump_instrs((FILE*) v);
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage() {
    PIN_ERROR("This Pintool prints the IPs of every instruction executed\n" 
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char * argv[]) {
    // Initialize pin
    if (PIN_Init(argc, argv))
      return Usage();
    
    FILE* fo = 0;
    //file input
    FILE* fi = 0;

    if (KnobOutputFile.Value() != "default") {
        fo = fopen(KnobOutputFile.Value().c_str(), "w"); //opening the output file.
        if (fo == NULL){
            printf("Error while opening the output file.\n" );
            exit(0);
        }
    }

    if (KnobInputFile.Value() != "default") {
        fi = fopen(KnobInputFile.Value().c_str(), "r"); //opening the input file.
        if (fi == NULL){
            printf("Error while opening the input file.\n");
            exit(0);
        }
        // Reads data from input file.
        read_input_file(fi);
    }

    // Register Instruction to be called to instrument instructions
    TRACE_AddInstrumentFunction(Trace, 0);

    // Register fini function.
    PIN_AddFiniFunction(Fini, fo);

    // Start the program, never returns.
    PIN_StartProgram();

    return 0;
}