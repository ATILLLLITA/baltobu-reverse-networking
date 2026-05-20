// FUN_18160d3eb
// 0x18160d3eb  size=249


void FUN_18160d3eb(void)

{
  uint *puVar1;
  int iVar2;
  uint uVar3;
  uint in_stack_00000008;
  
  uVar3 = ~in_stack_00000008 - 1;
  puVar1 = (uint *)((ulonglong)
                    (((uVar3 >> 0x18 | (uVar3 & 0xff0000) >> 8 | (uVar3 & 0xff00) << 8 |
                      uVar3 * 0x1000000) >> 3 | (uVar3 >> 0x18) << 0x1d) ^ 0x2e19809a) + 0x100000000
                   );
  iVar2 = ~((*puVar1 ^ (uint)puVar1) + 1) - 1;
  uVar3 = iVar2 * 2 | (uint)(iVar2 < 0);
                    /* WARNING: Could not recover jumptable at 0x00018160d5dd. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)(&UNK_18160d579 + (int)uVar3))
            ((longlong)(int)uVar3,0,(ulonglong)puVar1 ^ (ulonglong)uVar3,0x3000000);
  return;
}


