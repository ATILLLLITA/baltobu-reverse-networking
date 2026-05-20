// FUN_181b22e34
// 0x181b22e34  size=23


/* WARNING: Control flow encountered bad instruction data */

void FUN_181b22e34(longlong param_1)

{
  undefined4 in_EAX;
  uint7 uVar1;
  int *unaff_RSI;
  undefined8 unaff_R15;
  char cVar2;
  
  while( true ) {
    uVar1 = (uint7)((ulonglong)param_1 >> 8);
    register0x00000020 =
         (BADSPACEBASE *)(ulonglong)((uint)register0x00000020 | *(uint *)CONCAT71(uVar1,0xdb));
    param_1 = (ulonglong)uVar1 << 8;
    *unaff_RSI = *unaff_RSI + -0x48;
    cVar2 = *unaff_RSI == 0;
    *(undefined8 *)register0x00000020 = unaff_R15;
    *(undefined8 *)((longlong)register0x00000020 + -8) = 0x181b22e4a;
    in_EAX = FUN_182a74b93(param_1,in_EAX);
    param_1 = param_1 + -1;
    if (param_1 == 0 || cVar2 == '\0') break;
    DAT_1562adbc1 = DAT_1562adbc1 | (byte)((ulonglong)param_1 >> 8);
  }
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


