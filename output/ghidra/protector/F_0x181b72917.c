// entry
// 0x181b72917  size=88


/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Instruction at (ram,0x000181b7295a) overlaps instruction at (ram,0x000181b72959)
    */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void entry(longlong param_1,undefined2 param_2)

{
  byte *pbVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  bool bVar6;
  bool bVar7;
  char cVar8;
  
  cVar8 = '\0';
  FUN_181ff7615();
  FUN_181b811b5();
  uVar4 = in(param_2);
  param_1 = param_1 + -1;
  if (param_1 != 0 && cVar8 != '\0') {
    uVar4 = uVar4 | 0x142fe719;
  }
  uVar5 = (uint)param_1;
  bVar6 = _DAT_195be9858 < uVar5;
  _DAT_195be9858 = _DAT_195be9858 - uVar5;
  bVar7 = _DAT_1968e58a3 < uVar5;
  uVar2 = _DAT_1968e58a3 - uVar5;
  _DAT_1968e58a3 = uVar2 - bVar6;
  uVar2 = (uint)(bVar7 || uVar2 < bVar6);
  bVar6 = _DAT_196462dc9 < uVar5;
  uVar3 = _DAT_196462dc9 - uVar5;
  _DAT_196462dc9 = uVar3 - uVar2;
  uVar2 = (uint)(bVar6 || uVar3 < uVar2);
  bVar6 = _DAT_196869d6a < uVar5;
  uVar3 = _DAT_196869d6a - uVar5;
  _DAT_196869d6a = uVar3 - uVar2;
  uVar2 = (uint)(bVar6 || uVar3 < uVar2);
  bVar6 = _DAT_195e71078 < uVar5;
  uVar3 = _DAT_195e71078 - uVar5;
  _DAT_195e71078 = uVar3 - uVar2;
  uVar2 = (uint)(bVar6 || uVar3 < uVar2);
  bVar6 = _DAT_1967e459e < uVar5;
  uVar3 = _DAT_1967e459e - uVar5;
  _DAT_1967e459e = uVar3 - uVar2;
  _DAT_1a7459809 = (_DAT_1a7459809 - uVar5) - (uint)(bVar6 || uVar3 < uVar2);
  pbVar1 = (byte *)((ulonglong)(uVar4 | 0x147f677e) + 0x69);
  *pbVar1 = *pbVar1 ^ (byte)param_1;
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


