// FUN_1816306dd
// 0x1816306dd  size=523


/* WARNING: Removing unreachable block (ram,0x0001816307f4) */

void FUN_1816306dd(void)

{
  uint uVar1;
  longlong lVar2;
  uint uVar3;
  int in_stack_00000008;
  
  uVar3 = -((in_stack_00000008 - 1U ^ 0x673edc18) + 1);
  uVar3 = uVar3 >> 1 | (uint)((uVar3 & 1) != 0) << 0x1f;
  uVar1 = (*(uint *)((ulonglong)uVar3 + 0xfffffffc) ^ uVar3) + 0x950bb713;
  uVar3 = uVar1 >> 1;
  lVar2 = (longlong)
          (int)-((uVar3 | (uint)((uVar1 & 1) != 0) << 0x1f) >> 0x18 | (uVar3 & 0xff0000) >> 8 |
                 (uVar3 & 0xff00) << 8 | uVar3 << 0x18);
                    /* WARNING: Could not recover jumptable at 0x0001816308e6. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)(lVar2 + 0x181630877))(lVar2,0xffffffff9ba53371,0x42088e7b);
  return;
}


