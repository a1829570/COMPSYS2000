#include <string>
#include "VMTranslator.h"
#include <bitset>           //to convert numbers to binary string
#include <cstdint>          //uint16_t
#include <unordered_map>    //map
#include <sstream>          //string processing
#include <stdexcept>        //thowing exceptions

using namespace std;

//VMTranslator constructor 
VMTranslator::VMTranslator() {}

//VMTranslator destructor
VMTranslator::~VMTranslator() {}

/** Generate Hack Assembly code for a VM push operation */
string VMTranslator::vm_push(string segment, int offset){
    
    string seg_string = "default";

    if(segment == "local")      seg_string = "LCL";     //local
    if(segment == "argument")   seg_string = "ARG";     //argument
    if(segment == "this")       seg_string = "THIS";    //this
    if(segment == "that")       seg_string = "THAT";    //that
    
    if(segment == "static"){                            //static
        seg_string = "16";
        return "@" + seg_string + "\nD=A" + "\n@" + to_string(offset) + "\nA=D+A" + "\nD=M" + "\n@SP" + "\nM=M+1" + "\nA=M-1" + "\nM=D"; 
    } 

    if(segment == "pointer"){                           //pointer
        if(offset == 0) seg_string = "THIS";            //pointer 0 -> this
        else seg_string = "THAT";                       //pointer 1 -> that
        return "@" + seg_string + "\nD=M" + "\n@SP" + "\nM=M+1" + "\nA=M-1" + "\nM=D";
    }

    if(segment == "temp"){
        return "@" + to_string(offset) + "\nD=A" + "\n@5" + "\nA=D+A" + "\nD=M" + "\n@SP" + "\nM=M+1" + "\nA=M-1" + "\nM=D";        
    }   

    if(segment == "constant"){
        return "@" + to_string(offset) + "\nD=A" + "\n@SP" + "\nA=M" + "\nM=D" + "\n@SP" + "\nM=M+1";        
    }

    //if seg_string has been unchanged
    if(seg_string == "default") throw invalid_argument("VMTranslator()::vm_push(): invailid memory segment");

    return "@" + seg_string + "\nD=M" + "\n@" + to_string(offset) + "\nA=D+A" + "\nD=M" + "\n@SP" + "\nM=M+1" + "\nA=M-1" + "\nM=D"; 

}

/** Generate Hack Assembly code for a VM pop operation */
string VMTranslator::vm_pop(string segment, int offset){    

    string seg_string = "default";

    if(segment == "local")      seg_string = "LCL";     //local
    if(segment == "argument")   seg_string = "ARG";     //argument
    if(segment == "this")       seg_string = "THIS";    //this
    if(segment == "that")       seg_string = "THAT";    //that
    
    if(segment == "static"){   
        seg_string = "";
        return seg_string + "@16" + "\nD=A" + "\n@" + to_string(offset) + "\nD=D+A" + "\n@13" + "\nM=D" + "\n@SP" + "\nAM=M-1" + "\nD=M" + "\n@13" + "\nA=M" + "\nM=D";     
    } 

    if(segment == "pointer"){                           //pointer
        if(offset == 0) seg_string = "THIS";            //pointer 0 -> this
        else seg_string = "THAT";                       //pointer 1 -> that
        string blank = "";
        return blank + "@SP" + "\nM=M-1" + "\nA=M" + "\nD=M" + "\n@" + seg_string + "\nM=D";
    }

    if(segment == "temp"){
        return "@" + to_string(offset) + "\nD=A" + "\n@5" + "\nD=D+A" + "\n@13" + "\nM=D" + "\n@SP" + "\nM=M-1" + "\nA=M" + "\nD=M" + "\n@13" + "\nA=M" + "\nM=D";        
    }

    if(segment == "constant") throw invalid_argument("vm_pop: cannot pop to constant?");      

    //if seg_string has been unchanged
    if(seg_string == "default") throw invalid_argument("VMTranslator()::vm_pop(): invailid memory segment");

    return "@" + seg_string + "\nD=M" + "\n@" + to_string(offset) + "\nD=D+A" +  "\n@13" + "\nM=D" + "\n@SP" + "\nAM=M-1" + "\nD=M" + "\n@13" + "\nA=M" + "\nM=D";
}

/** Generate Hack Assembly code for a VM add operation */
string VMTranslator::vm_add(){
    string blank = "";
    return blank + "@SP" + "\nAM=M-1" + "\nD=M" + "\nA=A-1" + "\nM=M+D";
}

/** Generate Hack Assembly code for a VM sub operation */
string VMTranslator::vm_sub(){
    string blank = "";
    return blank + "@SP" + "\nAM=M-1" + "\nD=M" + "\nA=A-1" + "\nM=M-D";
}

/** Generate Hack Assembly code for a VM neg operation */
string VMTranslator::vm_neg(){
    string blank = "";
    return blank + "@SP" + "\nA=M-1" + "\nM=-M";
}

/** Generate Hack Assembly code for a VM eq operation */
string VMTranslator::vm_eq(){
    
    string res = "";
    res = res + "@SP"+"\nAM=M-1"+"\nD=M"+"\n@SP"+"\nAM=M-1"+"\nD=M-D"+"\n@" + "true_label"+"\nD;JEQ"+"\nD=0"+"\n@" + "false_label"+"\n0;JMP"+"\n(" + "true_label" + ")"+"\nD=-1"+"\n(" + "false_label" + ")"+"\n@SP"+"\nA=M"+"\nM=D"+"\n@SP"+"\nM=M+1";
    return res;

}


/** Generate Hack Assembly code for a VM gt operation */ //NOT COMPLETED
string VMTranslator::vm_gt(){
    return "";
}

/** Generate Hack Assembly code for a VM lt operation */ //NOT COMPLETED
string VMTranslator::vm_lt(){
    return "";
}

/** Generate Hack Assembly code for a VM and operation */
string VMTranslator::vm_and(){
    string blank = "";
    return blank + "@SP" + "\nAM=M-1" + "\nD=M" + "\nA=A-1" + "\nM=D&M";
}

/** Generate Hack Assembly code for a VM or operation */
string VMTranslator::vm_or(){
    string blank = "";
    return blank + "@SP" + "\nAM=M-1" + "\nD=M" + "\nA=A-1" + "\nM=D|M"; 
}

/** Generate Hack Assembly code for a VM not operation */
string VMTranslator::vm_not(){
    string blank = "";
    return blank + "@SP" + "\nA=M-1" + "\nM=!M";
}

/** Generate Hack Assembly code for a VM label operation */
string VMTranslator::vm_label(string label){
    
    string res = "";
    res = res + "(" + label + ")\n";
    return res;
}

/** Generate Hack Assembly code for a VM goto operation */
string VMTranslator::vm_goto(string label){

    string res = "";
    res = res + "@" + label + "\n0;JMP\n";
    return res;
}

/** Generate Hack Assembly code for a VM if-goto operation */
string VMTranslator::vm_if(string label){
    
    string res = "";
    res = res + "@SP" + "\nAM=M-1" + "\nD=M" + "\n@" + label + "\nD;JNE\n";
    return res;

}

/** Generate Hack Assembly code for a VM function operation */
string VMTranslator::vm_function(string function_name, int n_vars){
    return "";
}

/** Generate Hack Assembly code for a VM call operation */
string VMTranslator::vm_call(string function_name, int n_args){
    return "";
}

/** Generate Hack Assembly code for a VM return operation */
string VMTranslator::vm_return(){
    return "";
}