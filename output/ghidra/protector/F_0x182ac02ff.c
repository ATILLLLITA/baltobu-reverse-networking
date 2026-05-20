// FUN_182ac02ff
// 0x182ac02ff  size=69


/* WARNING: Control flow encountered bad instruction data */

void FUN_182ac02ff(undefined8 param_1,undefined2 param_2)

{
  longlong lVar1;
  undefined4 *unaff_RSI;
  undefined2 in_FPUControlWord;
  undefined2 in_FPUStatusWord;
  undefined2 in_FPUTagWord;
  undefined2 in_FPULastInstructionOpcode;
  undefined8 in_FPUDataPointer;
  undefined8 in_FPUInstructionPointer;
  
  lVar1 = FUN_181b7b493();
  out(*unaff_RSI,param_2);
  out(param_2,(char)lVar1);
  *(undefined2 *)(lVar1 + 0x48) = in_FPUControlWord;
  *(undefined2 *)(lVar1 + 0x4c) = in_FPUStatusWord;
  *(undefined2 *)(lVar1 + 0x50) = in_FPUTagWord;
  *(undefined8 *)(lVar1 + 0x5c) = in_FPUDataPointer;
  *(undefined8 *)(lVar1 + 0x54) = in_FPUInstructionPointer;
  *(undefined2 *)(lVar1 + 0x5a) = in_FPULastInstructionOpcode;
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


