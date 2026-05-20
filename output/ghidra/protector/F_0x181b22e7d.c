// FUN_181b22e7d
// 0x181b22e7d  size=36


void FUN_181b22e7d(longlong param_1,char param_2)

{
  code *pcVar1;
  short sVar2;
  undefined8 uVar3;
  undefined1 *puVar4;
  longlong unaff_RBX;
  short *unaff_RDI;
  
  in(0xd1);
  *(char *)(unaff_RBX + -0xe) = *(char *)(unaff_RBX + -0xe) - param_2;
  sVar2 = FUN_181b63831();
  do {
    unaff_RDI = unaff_RDI + 1;
    if (param_1 == 0) break;
    param_1 = param_1 + -1;
  } while (sVar2 != *unaff_RDI);
  uVar3 = FUN_181b50507();
  puVar4 = (undefined1 *)CONCAT62((int6)((ulonglong)uVar3 >> 0x10),CONCAT11(0xe8,(char)uVar3));
  *puVar4 = *puVar4;
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}


