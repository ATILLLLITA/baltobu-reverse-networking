// FUN_181b63831
// 0x181b63831  size=82


/* WARNING: Control flow encountered bad instruction data */

void FUN_181b63831(void)

{
  int iVar1;
  longlong unaff_RSI;
  
  FUN_18161b579();
  syscall();
  iVar1 = thunk_FUN_18161436d();
  *(int *)(unaff_RSI + 0xc729c3) =
       (*(int *)(unaff_RSI + 0xc729c3) - iVar1) - (uint)((byte)iVar1 < 0xd2);
  DAT_4045f134dabfac13 = (byte)iVar1;
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


