// FUN_181b4eaac
// 0x181b4eaac  size=27


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_181b4eaac(undefined8 param_1,undefined2 param_2)

{
  code *pcVar1;
  longlong unaff_RBP;
  undefined1 auStackX_8 [32];
  
  FUN_182ac02ff();
  out(param_2,0xff4dd057);
  *(char *)(unaff_RBP + -0x40faaac6) = *(char *)(unaff_RBP + -0x40faaac6) + '\x01';
  _DAT_bb5bdad8 = _DAT_bb5bdad8 + (int)auStackX_8;
  pcVar1 = (code *)swi(1);
  (*pcVar1)();
  return;
}


