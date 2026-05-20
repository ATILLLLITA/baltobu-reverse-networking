// FUN_18161436d
// 0x18161436d  size=694


/* WARNING: Removing unreachable block (ram,0x00018160e1a2) */

void FUN_18161436d(void)

{
  code *UNRECOVERED_JUMPTABLE;
  uint uVar1;
  undefined4 uStackX_10;
  
  uVar1 = 0xf2be4085 - uStackX_10;
  uVar1 = (uVar1 >> 0x18 | (uVar1 & 0xff0000) >> 8 | (uVar1 & 0xff00) << 8 | uVar1 * 0x1000000) ^
          0xc089cca5;
  UNRECOVERED_JUMPTABLE =
       (code *)(SUB168(SEXT816(0),8) +
               (longlong)
               (int)(0xffffffff - (*(uint *)((ulonglong)uVar1 + 0xfffffffc) ^ uVar1 ^ 0x38b69c12) ^
                    0x66b2b901) + 0x1816144e2);
                    /* WARNING: Could not recover jumptable at 0x00018161457b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*UNRECOVERED_JUMPTABLE)(0x34cfb1e88,UNRECOVERED_JUMPTABLE,0x36,0xcdc80967);
  return;
}


