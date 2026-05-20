// FUN_181637630
// 0x181637630  size=123


/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Removing unreachable block (ram,0x000181637700) */
/* WARNING: Removing unreachable block (ram,0x00018160d4d1) */
/* WARNING: Removing unreachable block (ram,0x00000032) */

void FUN_181637630(void)

{
  uint uVar1;
  int iVar2;
  int iStackX_8;
  
  uVar1 = (iStackX_8 * -4 | (uint)-iStackX_8 >> 0x1e) + 0xf1c2bcd5;
  uVar1 = (uVar1 >> 0x18 | (uVar1 & 0xff0000) >> 8 | (uVar1 & 0xff00) << 8 | uVar1 * 0x1000000) << 2
          | uVar1 * 0x1000000 >> 0x1e;
  iVar2 = (*(uint *)((ulonglong)uVar1 + 0xfffffffc) ^ uVar1) + 0x3f39f63a;
  uVar1 = (iVar2 * 2 | (uint)(iVar2 < 0)) + 0x91e89a64;
                    /* WARNING: Could not recover jumptable at 0x00018163788c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)(&LAB_1816377e2 + (int)-((uVar1 * 8 | uVar1 >> 0x1d) + 0x77cdfeeb)))(0xc,1);
  return;
}


