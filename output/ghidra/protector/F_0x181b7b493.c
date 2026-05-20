// FUN_181b7b493
// 0x181b7b493  size=44


/* WARNING: Control flow encountered bad instruction data */

undefined4 FUN_181b7b493(undefined8 param_1,undefined2 param_2)

{
  char *pcVar1;
  byte *pbVar2;
  byte bVar3;
  char cVar4;
  byte bVar5;
  undefined4 uVar6;
  byte bVar7;
  undefined7 uVar8;
  undefined8 *puVar9;
  undefined8 *unaff_RBP;
  undefined8 uStack_18;
  
  uVar8 = (undefined7)((ulonglong)param_1 >> 8);
  bVar7 = (byte)param_1;
  uVar6 = FUN_181b74f88();
  bVar5 = (byte)uVar6;
  if ((POPCOUNT(bVar5 & 0xbf) & 1U) != 0) {
    bVar3 = *(byte *)CONCAT71(uVar8,bVar7);
    *(byte *)CONCAT71(uVar8,bVar7) = *(char *)CONCAT71(uVar8,bVar7) + bVar7;
    pcVar1 = (char *)(CONCAT71(uVar8,bVar7) + 0x270f30b0);
    *pcVar1 = *pcVar1 + bVar5 + CARRY1(bVar3,bVar7);
    puVar9 = (undefined8 *)&stack0xfffffffffffffff0;
    cVar4 = '\x13';
    do {
      unaff_RBP = unaff_RBP + -1;
      puVar9 = puVar9 + -1;
      *puVar9 = *unaff_RBP;
      cVar4 = cVar4 + -1;
    } while ('\0' < cVar4);
    LOCK();
    pbVar2 = (byte *)(CONCAT71(uVar8,bVar7) + -0x8b9be3a);
    bVar7 = *pbVar2;
    *pbVar2 = bVar5;
    uVar6 = CONCAT31((int3)((uint)uVar6 >> 8),bVar7);
    UNLOCK();
    out(param_2,uVar6);
    return uVar6;
  }
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


