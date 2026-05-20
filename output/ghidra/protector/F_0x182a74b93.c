// FUN_182a74b93
// 0x182a74b93  size=81


/* WARNING: Control flow encountered bad instruction data */

void FUN_182a74b93(undefined8 param_1,undefined2 param_2)

{
  int *piVar1;
  undefined4 uVar2;
  longlong lVar3;
  longlong lVar4;
  longlong unaff_RSI;
  undefined4 *unaff_RDI;
  
  lVar4 = FUN_182008d87();
  uVar2 = in(param_2);
  *unaff_RDI = uVar2;
  lVar3 = (longlong)*(int *)(unaff_RSI + -0x5f) * -0x60;
  piVar1 = (int *)(CONCAT71((uint7)(uint3)((uint3)((ulonglong)lVar4 >> 8) |
                                          (uint3)((uint)*(undefined4 *)(lVar4 + 0x7d) >> 8)),0xd5) +
                  -0x5ea88c97);
  *piVar1 = (*piVar1 - ((int)unaff_RDI + 4)) - (uint)((int)lVar3 != lVar3);
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


