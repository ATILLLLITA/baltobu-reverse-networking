// FUN_181b23149
// 0x181b23149  size=32


/* WARNING: Control flow encountered bad instruction data */

void FUN_181b23149(void)

{
  char cVar1;
  undefined1 uVar2;
  undefined8 *puVar3;
  undefined8 *unaff_RBP;
  longlong unaff_RSI;
  byte *unaff_R15;
  undefined8 uStack_10;
  
  uVar2 = FUN_181b3b3da();
  puVar3 = (undefined8 *)&stack0xfffffffffffffff8;
  cVar1 = '\x1c';
  do {
    unaff_RBP = unaff_RBP + -1;
    puVar3 = puVar3 + -1;
    *puVar3 = *unaff_RBP;
    cVar1 = cVar1 + -1;
  } while ('\0' < cVar1);
  out(0xc,uVar2);
  *unaff_R15 = *unaff_R15 | (byte)&stack0xfffffffffffffff8;
  *(undefined1 *)(unaff_RSI + -0x2c460b96) = 0;
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


