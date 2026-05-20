// FUN_181ff7615
// 0x181ff7615  size=51


/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_181ff7615(undefined8 param_1,undefined2 param_2)

{
  undefined1 uVar1;
  longlong unaff_RBP;
  uint unaff_EDI;
  
  _DAT_7db0223cb5c86fcd = FUN_18162963c();
  uVar1 = in(param_2);
  *(undefined1 *)(ulonglong)(unaff_EDI | *(uint *)(unaff_RBP + 0x32)) = uVar1;
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


