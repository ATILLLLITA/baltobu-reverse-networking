// ========== FUN_181606000 @ 0x181606000  size=558 ==========

undefined4 FUN_181606000(void)

{
  LOCK();
  UNLOCK();
  return 0;
}



// ========== FUN_18160d3eb @ 0x18160d3eb  size=249 ==========

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



// ========== thunk_FUN_18161436d @ 0x181614363  size=5 ==========

/* WARNING: Removing unreachable block (ram,0x00018160e1a2) */

void thunk_FUN_18161436d(void)

{
  code *UNRECOVERED_JUMPTABLE;
  uint uVar1;
  undefined4 uStackX_8;
  
  uVar1 = 0xf2be4085 - uStackX_8;
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



// ========== thunk_FUN_18161436d @ 0x181614368  size=5 ==========

/* WARNING: Removing unreachable block (ram,0x00018160e1a2) */

void thunk_FUN_18161436d(void)

{
  code *UNRECOVERED_JUMPTABLE;
  uint uVar1;
  undefined4 uStackX_8;
  
  uVar1 = 0xf2be4085 - uStackX_8;
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



// ========== FUN_18161436d @ 0x18161436d  size=694 ==========

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



// ========== FUN_18161b579 @ 0x18161b579  size=35 ==========

void FUN_18161b579(void)

{
  return;
}



// ========== FUN_18162963c @ 0x18162963c  size=514 ==========

void FUN_18162963c(void)

{
  return;
}



// ========== FUN_1816306dd @ 0x1816306dd  size=523 ==========

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



// ========== FUN_181637630 @ 0x181637630  size=123 ==========

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



// ========== FUN_18163ec4b @ 0x18163ec4b  size=4211 ==========

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Removing unreachable block (ram,0x00018161d5e0) */
/* WARNING: Removing unreachable block (ram,0x00018161d631) */
/* WARNING: Removing unreachable block (ram,0x00018161d678) */
/* WARNING: Removing unreachable block (ram,0x00018163ecd3) */
/* WARNING: Removing unreachable block (ram,0x0001816290bb) */
/* WARNING: Removing unreachable block (ram,0x000181624480) */
/* WARNING: Removing unreachable block (ram,0x0001816245a9) */
/* WARNING: Removing unreachable block (ram,0x000181627a30) */
/* WARNING: Removing unreachable block (ram,0x00018162464b) */
/* WARNING: Removing unreachable block (ram,0x0001816246b5) */
/* WARNING: Removing unreachable block (ram,0x0001816246c2) */
/* WARNING: Removing unreachable block (ram,0x00018161d31b) */
/* WARNING: Removing unreachable block (ram,0x00018161d376) */
/* WARNING: Removing unreachable block (ram,0x00018161d37d) */
/* WARNING: Removing unreachable block (ram,0x00018161d489) */
/* WARNING: Removing unreachable block (ram,0x00018161d573) */
/* WARNING: Removing unreachable block (ram,0x00018161d576) */
/* WARNING: Removing unreachable block (ram,0x00018161f4ae) */
/* WARNING: Removing unreachable block (ram,0x00018161f52a) */
/* WARNING: Removing unreachable block (ram,0x00018161f60c) */
/* WARNING: Removing unreachable block (ram,0x00018161f610) */
/* WARNING: Removing unreachable block (ram,0x00018161f62a) */
/* WARNING: Removing unreachable block (ram,0x00018161d5c1) */
/* WARNING: Removing unreachable block (ram,0x0001816240cc) */
/* WARNING: Removing unreachable block (ram,0x000181624197) */
/* WARNING: Removing unreachable block (ram,0x0001816241d3) */
/* WARNING: Removing unreachable block (ram,0x000181624299) */
/* WARNING: Removing unreachable block (ram,0x00018162437c) */
/* WARNING: Removing unreachable block (ram,0x0001816229e0) */
/* WARNING: Removing unreachable block (ram,0x000181622a00) */
/* WARNING: Removing unreachable block (ram,0x000181622a8a) */
/* WARNING: Removing unreachable block (ram,0x000181622a8c) */
/* WARNING: Removing unreachable block (ram,0x000181622a95) */
/* WARNING: Removing unreachable block (ram,0x000181608305) */
/* WARNING: Removing unreachable block (ram,0x00018161cfbb) */
/* WARNING: Removing unreachable block (ram,0x00018161b7ba) */
/* WARNING: Removing unreachable block (ram,0x00018161b7e4) */
/* WARNING: Removing unreachable block (ram,0x00018161b87a) */
/* WARNING: Removing unreachable block (ram,0x00018161b87c) */
/* WARNING: Removing unreachable block (ram,0x00018161b8b9) */

longlong FUN_18163ec4b(void)

{
  uint uVar1;
  int in_stack_00000008;
  
  uVar1 = (in_stack_00000008 + 1U >> 2 | (in_stack_00000008 + 1U) * 0x40000000) + 0x6d2b3027;
  uVar1 = uVar1 >> 3 | uVar1 * 0x20000000;
  uVar1 = (*(uint *)((ulonglong)uVar1 + 0xfffffffc) ^ uVar1) + 0x2d4927c5;
  uVar1 = uVar1 >> 0x18 | (uVar1 & 0xff0000) >> 8 | (uVar1 & 0xff00) << 8 | uVar1 * 0x1000000;
  return (longlong)(int)~(uVar1 << 1 | (uint)((int)uVar1 < 0));
}



// ========== FUN_181645c69 @ 0x181645c69  size=587 ==========

/* WARNING: Removing unreachable block (ram,0x000181613381) */
/* WARNING: Heritage AFTER dead removal. Example location: s0xfffffffffffffd1b : 0x000181645e74 */
/* WARNING: Restarted to delay deadcode elimination for space: stack */

void FUN_181645c69(void)

{
  return;
}



// ========== FUN_181b20b13 @ 0x181b20b13  size=554 ==========

undefined8
FUN_181b20b13(undefined8 param_1,undefined8 *param_2,undefined8 param_3,longlong *param_4)

{
  longlong lVar1;
  undefined8 uVar2;
  uint *puVar3;
  uint uVar4;
  ulonglong uVar5;
  
  uVar4 = *(uint *)(param_4[1] + 0x1ea0c79);
  puVar3 = (uint *)(param_4[1] + 0x1ea0c7d);
  do {
    uVar5 = (ulonglong)uVar4;
    if (uVar4 == 0) {
LAB_181b20c06:
      lVar1 = param_4[5];
      uVar2 = *(undefined8 *)((longlong)param_2 + uVar5 + 0x120);
      *(undefined8 *)(lVar1 + 0xf8) = uVar2;
      param_2[4] = uVar2;
      uVar2 = *(undefined8 *)((longlong)param_2 + uVar5 + 0x128);
      *(undefined8 *)(lVar1 + 0x98) = uVar2;
      param_2[7] = uVar2;
      uVar2 = *(undefined8 *)((longlong)param_2 + uVar5 + 0x130);
      *(undefined8 *)(lVar1 + 0xa0) = uVar2;
      *param_2 = uVar2;
      uVar2 = *(undefined8 *)((longlong)param_2 + uVar5 + 0x138);
      *(undefined8 *)(lVar1 + 0xa8) = uVar2;
      param_2[1] = uVar2;
      uVar2 = *(undefined8 *)((longlong)param_2 + uVar5 + 0x140);
      *(undefined8 *)(lVar1 + 0xb0) = uVar2;
      param_2[2] = uVar2;
      uVar2 = *(undefined8 *)((longlong)param_2 + uVar5 + 0x148);
      *(undefined8 *)(lVar1 + 0x90) = uVar2;
      param_2[3] = uVar2;
      return 1;
    }
    if ((ulonglong)*puVar3 + param_4[1] == *param_4) {
      uVar5 = (ulonglong)(ushort)puVar3[1];
      goto LAB_181b20c06;
    }
    puVar3 = (uint *)((longlong)puVar3 + 6);
    uVar4 = uVar4 - 1;
  } while( true );
}



// ========== FUN_181b21164 @ 0x181b21164  size=5346 ==========

undefined8
FUN_181b21164(undefined4 *param_1,byte *param_2,longlong param_3,longlong *param_4,longlong param_5,
             ulonglong param_6,ulonglong *param_7)

{
  byte *pbVar1;
  byte bVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined2 *puVar5;
  bool bVar6;
  short sVar7;
  byte bVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  ulonglong uVar13;
  byte *pbVar14;
  uint uVar15;
  ulonglong uVar16;
  uint uVar18;
  longlong lVar19;
  ushort *puVar20;
  int iVar21;
  longlong lVar22;
  int iVar23;
  uint uVar24;
  undefined2 *puVar25;
  byte *pbVar26;
  ushort uVar27;
  int iVar28;
  ushort *puVar29;
  uint uVar30;
  int iVar31;
  ulonglong uVar32;
  ulonglong uVar33;
  uint local_res8;
  uint local_res18;
  uint local_68;
  uint local_64;
  ulonglong uVar17;
  
  puVar5 = *(undefined2 **)(param_1 + 4);
  local_68 = 0;
  uVar33 = 0;
  uVar32 = 1;
  uVar13 = 0;
  uVar3 = param_1[1];
  uVar18 = (1 << ((byte)param_1[2] & 0x1f)) - 1;
  uVar4 = *param_1;
  *param_4 = 0;
  *param_7 = 0;
  bVar8 = (byte)uVar4;
  local_res18 = 1;
  local_res8 = 1;
  uVar9 = (0x300 << (bVar8 + (char)param_1[1] & 0x1f)) + 0x736;
  local_64 = 1;
  if (uVar9 != 0) {
    puVar25 = puVar5;
    for (uVar16 = (ulonglong)uVar9; uVar16 != 0; uVar16 = uVar16 - 1) {
      *puVar25 = 0x400;
      puVar25 = puVar25 + 1;
    }
  }
  pbVar1 = param_2 + param_3;
  uVar16 = 0;
  uVar9 = 0xffffffff;
  uVar17 = uVar33;
  pbVar26 = param_2;
  do {
    if (pbVar26 == pbVar1) {
      return 1;
    }
    uVar15 = (int)uVar17 + 1;
    uVar17 = (ulonglong)uVar15;
    uVar16 = (ulonglong)((int)uVar16 << 8 | (uint)*pbVar26);
    pbVar26 = pbVar26 + 1;
  } while ((int)uVar15 < 5);
  uVar17 = uVar33;
  if (param_6 != 0) {
LAB_181b21365:
    uVar15 = local_64;
    sVar7 = 0xf;
    uVar27 = (ushort)uVar18;
    if (uVar27 != 0) {
      for (; uVar27 >> sVar7 == 0; sVar7 = sVar7 + -1) {
      }
    }
    uVar24 = (uint)uVar17 & uVar18;
    lVar22 = (longlong)(int)(local_68 << 4) + (longlong)(int)uVar24;
    if (uVar9 < 0x1000000) {
      if (pbVar26 == pbVar1) {
        return 1;
      }
      lVar19 = 0x3f;
      if (uVar16 != 0) {
        for (; uVar16 >> lVar19 == 0; lVar19 = lVar19 + -1) {
        }
      }
      uVar9 = uVar9 << 8;
      uVar16 = (ulonglong)((int)uVar16 << 8 | (uint)*pbVar26);
      pbVar26 = pbVar26 + 1;
    }
    uVar27 = puVar5[lVar22];
    uVar10 = (uVar9 >> 0xb) * (uint)uVar27;
    uVar30 = (uint)uVar32;
    if ((uint)uVar16 < uVar10) {
      uVar15 = 1;
      puVar5[lVar22] = (short)((int)(0x800 - (uint)uVar27) >> 5) + uVar27;
      lVar22 = (uVar13 >> (8 - bVar8 & 0x3f)) +
               (((1 << ((byte)uVar3 & 0x1f)) - 1 & uVar17) << (bVar8 & 0x3f));
      uVar9 = uVar10;
      if ((int)local_68 < 7) goto LAB_181b21606;
      uVar24 = (uint)*(byte *)((uVar17 - uVar32) + param_5);
      do {
        uVar24 = uVar24 * 2;
        uVar30 = uVar24 & 0x100;
        lVar19 = (longlong)(int)uVar30 + (longlong)(int)uVar15;
        if (uVar9 < 0x1000000) {
          if (pbVar26 == pbVar1) {
            return 1;
          }
          uVar9 = uVar9 << 8;
          uVar16 = (ulonglong)((int)uVar16 << 8 | (uint)*pbVar26);
          pbVar26 = pbVar26 + 1;
        }
        uVar27 = puVar5[lVar22 * 0x300 + lVar19 + 0x836];
        uVar10 = (uVar9 >> 0xb) * (uint)uVar27;
        if ((uint)uVar16 < uVar10) {
          uVar15 = uVar15 * 2;
          puVar5[lVar22 * 0x300 + lVar19 + 0x836] =
               (short)((int)(0x800 - (uint)uVar27) >> 5) + uVar27;
          uVar9 = uVar10;
          if (uVar30 != 0) goto joined_r0x000181b21600;
        }
        else {
          uVar9 = uVar9 - uVar10;
          uVar16 = (ulonglong)((uint)uVar16 - uVar10);
          uVar15 = uVar15 * 2 + 1;
          puVar5[lVar22 * 0x300 + lVar19 + 0x836] = uVar27 - (uVar27 >> 5);
          if (uVar30 == 0) goto joined_r0x000181b21600;
        }
      } while ((int)uVar15 < 0x100);
      goto LAB_181b21741;
    }
    uVar9 = uVar9 - uVar10;
    uVar10 = (uint)uVar16 - uVar10;
    puVar5[lVar22] = uVar27 - (uVar27 >> 5);
    if (uVar9 < 0x1000000) {
      if (pbVar26 == pbVar1) {
        return 1;
      }
      uVar9 = uVar9 * 0x100;
      uVar10 = uVar10 * 0x100 | (uint)*pbVar26;
      pbVar26 = pbVar26 + 1;
    }
    uVar27 = puVar5[(longlong)(int)local_68 + 0xc0];
    uVar11 = (uVar9 >> 0xb) * (uint)uVar27;
    if (uVar11 <= uVar10) {
      uVar9 = uVar9 - uVar11;
      uVar10 = uVar10 - uVar11;
      uVar13 = (ulonglong)(uVar27 >> 5);
      puVar5[(longlong)(int)local_68 + 0xc0] = uVar27 - (uVar27 >> 5);
      if (uVar9 < 0x1000000) {
        if (pbVar26 == pbVar1) {
          return 1;
        }
        uVar13 = (ulonglong)*pbVar26;
        uVar9 = uVar9 * 0x100;
        uVar10 = uVar10 * 0x100 | (uint)*pbVar26;
        pbVar26 = pbVar26 + 1;
      }
      uVar27 = puVar5[(longlong)(int)local_68 + 0xcc];
      lVar19 = 0;
      if (uVar13 != 0) {
        for (; (uVar13 >> lVar19 & 1) == 0; lVar19 = lVar19 + 1) {
        }
      }
      uVar11 = (uVar9 >> 0xb) * (uint)uVar27;
      if (uVar11 <= uVar10) {
        uVar9 = uVar9 - uVar11;
        uVar10 = uVar10 - uVar11;
        uVar27 = uVar27 - (uVar27 >> 5);
        puVar5[(longlong)(int)local_68 + 0xcc] = uVar27;
        if (uVar9 < 0x1000000) {
          if (pbVar26 == pbVar1) {
            return 1;
          }
          lVar22 = 0;
          if (uVar27 != 0) {
            for (; (uVar27 >> lVar22 & 1) == 0; lVar22 = lVar22 + 1) {
            }
          }
          uVar9 = uVar9 * 0x100;
          uVar10 = uVar10 * 0x100 | (uint)*pbVar26;
          pbVar26 = pbVar26 + 1;
        }
        uVar27 = puVar5[(longlong)(int)local_68 + 0xd8];
        uVar11 = (uVar9 >> 0xb) * (uint)uVar27;
        if (uVar10 < uVar11) {
          puVar5[(longlong)(int)local_68 + 0xd8] =
               (short)((int)(0x800 - (uint)uVar27) >> 5) + uVar27;
          uVar15 = local_res18;
        }
        else {
          uVar9 = uVar9 - uVar11;
          uVar10 = uVar10 - uVar11;
          puVar5[(longlong)(int)local_68 + 0xd8] = uVar27 - (uVar27 >> 5);
          if (uVar9 < 0x1000000) {
            if (pbVar26 == pbVar1) {
              return 1;
            }
            uVar9 = uVar9 * 0x100;
            uVar10 = uVar10 * 0x100 | (uint)*pbVar26;
            pbVar26 = pbVar26 + 1;
          }
          uVar27 = puVar5[(longlong)(int)local_68 + 0xe4];
          uVar12 = (uVar9 >> 0xb) * (uint)uVar27;
          if (uVar10 < uVar12) {
            puVar5[(longlong)(int)local_68 + 0xe4] =
                 (short)((int)(0x800 - (uint)uVar27) >> 5) + uVar27;
            uVar11 = uVar12;
            uVar15 = local_res8;
          }
          else {
            uVar11 = uVar9 - uVar12;
            uVar10 = uVar10 - uVar12;
            puVar5[(longlong)(int)local_68 + 0xe4] = uVar27 - (uVar27 >> 5);
            local_64 = local_res8;
          }
          local_res8 = local_res18;
        }
        uVar32 = (ulonglong)uVar15;
        local_res18 = uVar30;
LAB_181b21d61:
        bVar6 = (int)local_68 < 7;
        local_68 = 0xb;
        if (bVar6) {
          local_68 = 8;
        }
        puVar20 = puVar5 + 0x534;
        uVar30 = local_res18;
        goto LAB_181b21da1;
      }
      puVar5[(longlong)(int)local_68 + 0xcc] = (short)((int)(0x800 - (uint)uVar27) >> 5) + uVar27;
      if (uVar11 < 0x1000000) {
        if (pbVar26 == pbVar1) {
          return 1;
        }
        uVar11 = uVar11 * 0x100;
        uVar10 = uVar10 << 8 | (uint)*pbVar26;
        pbVar26 = pbVar26 + 1;
      }
      uVar16 = (ulonglong)uVar10;
      uVar27 = puVar5[lVar22 + 0xf0];
      uVar9 = (uVar11 >> 0xb) * (uint)uVar27;
      if (uVar9 <= uVar10) {
        uVar11 = uVar11 - uVar9;
        uVar10 = uVar10 - uVar9;
        puVar5[lVar22 + 0xf0] = uVar27 - (uVar27 >> 5);
        goto LAB_181b21d61;
      }
      puVar5[lVar22 + 0xf0] = (short)((int)(0x800 - (uint)uVar27) >> 5) + uVar27;
      if (uVar17 == 0) {
        return 1;
      }
      bVar6 = (int)local_68 < 7;
      local_68 = 0xb;
      if (bVar6) {
        local_68 = 9;
      }
      uVar33 = uVar17 + 1;
      bVar2 = *(byte *)((uVar17 - uVar32) + param_5);
      uVar13 = (ulonglong)bVar2;
      *(byte *)(uVar17 + param_5) = bVar2;
      goto LAB_181b2260e;
    }
    puVar5[(longlong)(int)local_68 + 0xc0] = (short)((int)(0x800 - (uint)uVar27) >> 5) + uVar27;
    local_64 = local_res8;
    puVar20 = puVar5 + 0x332;
    bVar6 = (int)local_68 < 7;
    local_68 = 3;
    local_res8 = local_res18;
    if (bVar6) {
      local_68 = 0;
    }
LAB_181b21da1:
    local_res18 = uVar30;
    iVar28 = 8;
    if (uVar11 < 0x1000000) {
      if (pbVar26 == pbVar1) {
        return 1;
      }
      uVar11 = uVar11 << 8;
      uVar10 = uVar10 << 8 | (uint)*pbVar26;
      pbVar26 = pbVar26 + 1;
    }
    uVar16 = (ulonglong)uVar10;
    uVar27 = *puVar20;
    uVar9 = (uVar11 >> 0xb) * (uint)uVar27;
    if (uVar10 < uVar9) {
      *puVar20 = (short)((int)(0x800 - (uint)uVar27) >> 5) + uVar27;
      puVar29 = puVar20 + (longlong)(int)(uVar24 * 8) + 2;
      iVar21 = 0;
      iVar28 = 3;
    }
    else {
      uVar11 = uVar11 - uVar9;
      uVar10 = uVar10 - uVar9;
      *puVar20 = uVar27 - (uVar27 >> 5);
      if (uVar11 < 0x1000000) {
        if (pbVar26 == pbVar1) {
          return 1;
        }
        uVar11 = uVar11 * 0x100;
        uVar10 = uVar10 * 0x100 | (uint)*pbVar26;
        pbVar26 = pbVar26 + 1;
      }
      uVar16 = (ulonglong)uVar10;
      uVar27 = puVar20[1];
      uVar15 = (uVar11 >> 0xb) * (uint)uVar27;
      if (uVar10 < uVar15) {
        puVar20[1] = (short)((int)(0x800 - (uint)uVar27) >> 5) + uVar27;
        puVar29 = puVar20 + (longlong)(int)(uVar24 * 8) + 0x82;
        uVar9 = uVar15;
        iVar21 = iVar28;
        iVar28 = 3;
      }
      else {
        uVar9 = uVar11 - uVar15;
        uVar16 = (ulonglong)(uVar10 - uVar15);
        puVar29 = puVar20 + 0x102;
        iVar21 = 0x10;
        puVar20[1] = uVar27 - (uVar27 >> 5);
      }
    }
    iVar23 = 1;
    iVar31 = iVar28;
    do {
      if (uVar9 < 0x1000000) {
        if (pbVar26 == pbVar1) {
          return 1;
        }
        uVar9 = uVar9 << 8;
        uVar16 = (ulonglong)((int)uVar16 << 8 | (uint)*pbVar26);
        pbVar26 = pbVar26 + 1;
      }
      uVar27 = puVar29[iVar23];
      uVar15 = (uVar9 >> 0xb) * (uint)uVar27;
      if ((uint)uVar16 < uVar15) {
        puVar29[iVar23] = (short)((int)(0x800 - (uint)uVar27) >> 5) + uVar27;
        uVar9 = uVar15;
        iVar23 = iVar23 * 2;
      }
      else {
        uVar9 = uVar9 - uVar15;
        uVar16 = (ulonglong)((uint)uVar16 - uVar15);
        puVar29[iVar23] = uVar27 - (uVar27 >> 5);
        iVar23 = iVar23 * 2 + 1;
      }
      iVar31 = iVar31 + -1;
    } while (iVar31 != 0);
    iVar23 = iVar23 + (iVar21 - (1 << (sbyte)iVar28));
    if (local_68 < 4) {
      local_68 = local_68 + 7;
      iVar31 = 6;
      iVar28 = 1;
      iVar21 = 3;
      if (iVar23 < 4) {
        iVar21 = iVar23;
      }
      lVar22 = (longlong)(iVar21 << 6);
      do {
        lVar19 = (longlong)iVar28;
        if (uVar9 < 0x1000000) {
          if (pbVar26 == pbVar1) {
            return 1;
          }
          uVar9 = uVar9 << 8;
          uVar16 = (ulonglong)((int)uVar16 << 8 | (uint)*pbVar26);
          pbVar26 = pbVar26 + 1;
        }
        uVar27 = puVar5[lVar22 + lVar19 + 0x1b0];
        uVar15 = (uVar9 >> 0xb) * (uint)uVar27;
        if ((uint)uVar16 < uVar15) {
          iVar28 = iVar28 * 2;
          puVar5[lVar22 + lVar19 + 0x1b0] = (short)((int)(0x800 - (uint)uVar27) >> 5) + uVar27;
          uVar9 = uVar15;
        }
        else {
          uVar9 = uVar9 - uVar15;
          uVar16 = (ulonglong)((uint)uVar16 - uVar15);
          iVar28 = iVar28 * 2 + 1;
          puVar5[lVar22 + lVar19 + 0x1b0] = uVar27 - (uVar27 >> 5);
        }
        iVar31 = iVar31 + -1;
      } while (iVar31 != 0);
      uVar15 = iVar28 - 0x40;
      if (3 < (int)uVar15) {
        iVar28 = ((int)uVar15 >> 1) + -1;
        uVar24 = uVar15 & 1 | 2;
        if ((int)uVar15 < 0xe) {
          lVar22 = (longlong)(int)uVar15;
          uVar15 = uVar24 << ((byte)iVar28 & 0x1f);
          puVar25 = puVar5 + ((ulonglong)uVar15 - lVar22) + 0x2af;
        }
        else {
          iVar28 = ((int)uVar15 >> 1) + -5;
          do {
            if (uVar9 < 0x1000000) {
              if (pbVar26 == pbVar1) {
                return 1;
              }
              uVar9 = uVar9 << 8;
              uVar16 = (ulonglong)((int)uVar16 << 8 | (uint)*pbVar26);
              pbVar26 = pbVar26 + 1;
            }
            uVar9 = uVar9 >> 1;
            uVar24 = uVar24 * 2;
            if (uVar9 <= (uint)uVar16) {
              uVar16 = (ulonglong)((uint)uVar16 - uVar9);
              uVar24 = uVar24 | 1;
            }
            iVar28 = iVar28 + -1;
          } while (iVar28 != 0);
          puVar25 = puVar5 + 0x322;
          uVar15 = uVar24 << 4;
          iVar28 = 4;
        }
        uVar24 = 1;
        iVar21 = 1;
        do {
          if (uVar9 < 0x1000000) {
            if (pbVar26 == pbVar1) {
              return 1;
            }
            uVar9 = uVar9 << 8;
            uVar16 = (ulonglong)((int)uVar16 << 8 | (uint)*pbVar26);
            pbVar26 = pbVar26 + 1;
          }
          uVar27 = puVar25[iVar21];
          uVar30 = (uVar9 >> 0xb) * (uint)uVar27;
          if ((uint)uVar16 < uVar30) {
            iVar31 = iVar21 * 2;
            puVar25[iVar21] = (short)((int)(0x800 - (uint)uVar27) >> 5) + uVar27;
            uVar9 = uVar30;
          }
          else {
            uVar9 = uVar9 - uVar30;
            uVar16 = (ulonglong)((uint)uVar16 - uVar30);
            iVar31 = iVar21 * 2 + 1;
            uVar15 = uVar15 | uVar24;
            puVar25[iVar21] = uVar27 - (uVar27 >> 5);
          }
          uVar24 = uVar24 * 2;
          iVar28 = iVar28 + -1;
          iVar21 = iVar31;
        } while (iVar28 != 0);
      }
      uVar32 = (ulonglong)(uVar15 + 1);
      uVar33 = uVar17;
      if (uVar15 + 1 == 0) goto LAB_181b2256c;
    }
    iVar23 = iVar23 + 2;
    if (uVar17 < uVar32) {
      return 1;
    }
    pbVar14 = (byte *)((uVar17 - (longlong)(int)uVar32) + param_5);
    while( true ) {
      bVar2 = *pbVar14;
      uVar13 = (ulonglong)bVar2;
      uVar33 = uVar17 + 1;
      pbVar14 = pbVar14 + 1;
      iVar23 = iVar23 + -1;
      *(byte *)(uVar17 + param_5) = bVar2;
      if (iVar23 == 0) break;
      uVar17 = uVar33;
      if (param_6 <= uVar33) goto LAB_181b2256c;
    }
    goto LAB_181b2260e;
  }
  goto LAB_181b22595;
joined_r0x000181b21600:
  while ((int)uVar15 < 0x100) {
LAB_181b21606:
    lVar19 = (longlong)(int)uVar15;
    if (uVar9 < 0x1000000) {
      if (pbVar26 == pbVar1) {
        return 1;
      }
      uVar9 = uVar9 << 8;
      uVar16 = (ulonglong)((int)uVar16 << 8 | (uint)*pbVar26);
      pbVar26 = pbVar26 + 1;
    }
    uVar27 = puVar5[lVar22 * 0x300 + lVar19 + 0x736];
    uVar24 = (uVar9 >> 0xb) * (uint)uVar27;
    if ((uint)uVar16 < uVar24) {
      uVar15 = uVar15 * 2;
      puVar5[lVar22 * 0x300 + lVar19 + 0x736] = (short)((int)(0x800 - (uint)uVar27) >> 5) + uVar27;
      uVar9 = uVar24;
    }
    else {
      uVar9 = uVar9 - uVar24;
      uVar16 = (ulonglong)((uint)uVar16 - uVar24);
      uVar15 = uVar15 * 2 + 1;
      puVar5[lVar22 * 0x300 + lVar19 + 0x736] = uVar27 - (uVar27 >> 5);
    }
  }
LAB_181b21741:
  uVar33 = uVar17 + 1;
  *(char *)(uVar17 + param_5) = (char)uVar15;
  uVar13 = (ulonglong)(uVar15 & 0xff);
  if ((int)local_68 < 4) {
    local_68 = 0;
  }
  else if ((int)local_68 < 10) {
    local_68 = local_68 - 3;
  }
  else {
    local_68 = local_68 - 6;
  }
LAB_181b2260e:
  uVar17 = uVar33;
  if (param_6 <= uVar33) goto LAB_181b2256c;
  goto LAB_181b21365;
LAB_181b2256c:
  if (uVar9 < 0x1000000) {
    if (pbVar26 == pbVar1) {
      return 1;
    }
    pbVar26 = pbVar26 + 1;
  }
LAB_181b22595:
  *param_4 = (longlong)pbVar26 - (longlong)param_2;
  *param_7 = uVar33;
  return 0;
}



// ========== FUN_181b22685 @ 0x181b22685  size=1272 ==========

undefined8 FUN_181b22685(longlong param_1,char *param_2)

{
  char *pcVar1;
  char *pcVar2;
  char *pcVar3;
  short sVar4;
  ushort unaff_DI;
  char *pcVar5;
  
  pcVar5 = (char *)0x0;
  if (param_2 != (char *)0x0) {
    do {
      pcVar2 = pcVar5 + param_1;
      sVar4 = 0xf;
      if (unaff_DI != 0) {
        for (; unaff_DI >> sVar4 == 0; sVar4 = sVar4 + -1) {
        }
      }
      sVar4 = 0;
      if ((ushort)pcVar2 != 0) {
        for (; ((ushort)pcVar2 >> sVar4 & 1) == 0; sVar4 = sVar4 + 1) {
        }
      }
      pcVar3 = pcVar2 + (6 - param_1);
      if ((((((((pcVar3 + -3 < param_2) && (*pcVar2 == 'Q')) && (pcVar5[param_1 + 1] == 'E')) &&
             ((pcVar5[param_1 + 2] == 'M' && (pcVar5[param_1 + 3] == 'U')))) ||
            ((((pcVar1 = pcVar3 + -1, pcVar1 < param_2 &&
               ((*pcVar2 == 'O' && (pcVar5[param_1 + 1] == 'r')))) && (pcVar5[param_1 + 2] == 'a'))
             && (((pcVar5[param_1 + 3] == 'c' && (pcVar5[param_1 + 4] == 'l')) &&
                 (pcVar5[param_1 + 5] == 'e')))))) ||
           ((((((pcVar3 < param_2 && (*pcVar2 == 'i')) &&
               ((pcVar5[param_1 + 1] == 'n' &&
                ((pcVar5[param_1 + 2] == 'n' && (pcVar5[param_1 + 3] == 'o')))))) &&
              (pcVar5[param_1 + 4] == 't')) &&
             ((pcVar5[param_1 + 5] == 'e' && (pcVar5[param_1 + 6] == 'k')))) ||
            ((((pcVar3 + 3 < param_2 &&
               (((((*pcVar2 == 'V' && (pcVar5[param_1 + 1] == 'i')) && (pcVar5[param_1 + 2] == 'r'))
                 && ((pcVar5[param_1 + 3] == 't' && (pcVar5[param_1 + 4] == 'u')))) &&
                (pcVar5[param_1 + 5] == 'a')))) &&
              (((pcVar5[param_1 + 6] == 'l' && (pcVar5[param_1 + 7] == 'B')) &&
               (pcVar5[param_1 + 8] == 'o')))) && (pcVar5[param_1 + 9] == 'x')))))) ||
          (((((((pcVar3 + 9 < param_2 && (*pcVar2 == 'V')) && (pcVar5[param_1 + 1] == 'i')) &&
              ((pcVar5[param_1 + 2] == 'r' && (pcVar5[param_1 + 3] == 't')))) &&
             (((pcVar5[param_1 + 4] == 'u' &&
               ((pcVar5[param_1 + 5] == 'a' && (pcVar5[param_1 + 6] == 'l')))) &&
              (((pcVar5[param_1 + 7] == ' ' &&
                (((pcVar5[param_1 + 8] == 'P' && (pcVar5[param_1 + 9] == 'l')) &&
                 (pcVar5[param_1 + 10] == 'a')))) &&
               (((pcVar5[param_1 + 0xb] == 't' && (pcVar5[param_1 + 0xc] == 'f')) &&
                ((pcVar5[param_1 + 0xd] == 'o' &&
                 ((pcVar5[param_1 + 0xe] == 'r' && (pcVar5[param_1 + 0xf] == 'm')))))))))))) ||
            (((pcVar1 < param_2 &&
              ((((*pcVar2 == 'V' && (pcVar5[param_1 + 1] == 'M')) && (pcVar5[param_1 + 2] == 'w'))
               && ((pcVar5[param_1 + 3] == 'a' && (pcVar5[param_1 + 4] == 'r')))))) &&
             (pcVar5[param_1 + 5] == 'e')))) ||
           (((((pcVar3 + 2 < param_2 && (*pcVar2 == 'P')) &&
              ((pcVar5[param_1 + 1] == 'a' &&
               (((pcVar5[param_1 + 2] == 'r' && (pcVar5[param_1 + 3] == 'a')) &&
                (pcVar5[param_1 + 4] == 'l')))))) &&
             ((pcVar5[param_1 + 5] == 'l' && (pcVar5[param_1 + 6] == 'e')))) &&
            ((pcVar5[param_1 + 7] == 'l' && (pcVar5[param_1 + 8] == 's')))))))) ||
         ((((pcVar1 < param_2 && (*pcVar2 == '7')) &&
           ((pcVar5[param_1 + 1] == '7' &&
            (((pcVar5[param_1 + 2] == '7' && (pcVar5[param_1 + 3] == '7')) &&
             (pcVar5[param_1 + 4] == '7')))))) && (pcVar5[param_1 + 5] == '7')))) {
        return 1;
      }
      pcVar5 = pcVar5 + 1;
    } while (pcVar5 < param_2);
  }
  return 0;
}



// ========== FUN_181b22b85 @ 0x181b22b85  size=375 ==========

void FUN_181b22b85(longlong param_1,uint param_2,uint param_3,longlong param_4,longlong param_5)

{
  uint uVar1;
  longlong *plVar2;
  byte bVar3;
  ulonglong uVar4;
  ushort *puVar5;
  ulonglong uVar6;
  
  uVar6 = 0;
  if (param_3 != 0) {
    param_4 = (ulonglong)param_2 + param_4;
    do {
      uVar1 = *(uint *)(param_4 + 4 + uVar6);
      if (uVar1 < 8) {
        return;
      }
      uVar4 = (ulonglong)uVar1 - 8 >> 1;
      if (uVar4 != 0) {
        puVar5 = (ushort *)(param_4 + 8 + uVar6);
        do {
          bVar3 = (byte)*puVar5 & 0xf;
          plVar2 = (longlong *)
                   ((ulonglong)*(uint *)(param_4 + uVar6) + (ulonglong)(*puVar5 >> 4) + param_5);
          if (bVar3 == 3) {
            *(int *)plVar2 = (int)*plVar2 + (int)param_1;
          }
          else if (bVar3 == 10) {
            *plVar2 = *plVar2 + param_1;
          }
          else if (bVar3 == 1) {
            *(short *)plVar2 = (short)*plVar2 + (short)((ulonglong)param_1 >> 0x10);
          }
          else if (bVar3 == 2) {
            *(short *)plVar2 = (short)*plVar2 + (short)param_1;
          }
          puVar5 = puVar5 + 1;
          uVar4 = uVar4 - 1;
        } while (uVar4 != 0);
      }
      uVar6 = uVar6 + *(uint *)(param_4 + 4 + uVar6);
    } while (uVar6 < param_3);
  }
  return;
}



// ========== FUN_181b22cfc @ 0x181b22cfc  size=303 ==========

int FUN_181b22cfc(byte *param_1,byte *param_2,char param_3)

{
  byte bVar1;
  longlong lVar2;
  byte bVar3;
  byte bVar4;
  
  lVar2 = 0x3f;
  if (param_1 != (byte *)0x0) {
    for (; (ulonglong)param_1 >> lVar2 == 0; lVar2 = lVar2 + -1) {
    }
  }
  bVar4 = 0;
  do {
    bVar3 = *param_1;
    bVar1 = *param_2;
    param_1 = param_1 + 1;
    param_2 = param_2 + 1;
    if (param_3 != '\0') {
      bVar3 = bVar3 ^ ((byte)(0x5d10ebad << (bVar4 & 0x1f)) |
                      (byte)(0x5d10ebad >> 0x20 - (bVar4 & 0x1f))) + bVar4;
      bVar4 = bVar4 + 1;
    }
  } while ((bVar3 != 0) && (bVar3 == bVar1));
  return (uint)bVar3 - (uint)bVar1;
}



// ========== FUN_181b22e2d @ 0x181b22e2d  size=7 ==========

/* WARNING: Control flow encountered bad instruction data */

void FUN_181b22e2d(longlong param_1)

{
  undefined4 in_EAX;
  uint7 uVar1;
  int *unaff_RSI;
  undefined8 unaff_R15;
  char cVar2;
  
  do {
    DAT_1562adbc1 = DAT_1562adbc1 | (byte)((ulonglong)param_1 >> 8);
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
  } while (param_1 != 0 && cVar2 != '\0');
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}



// ========== FUN_181b22e34 @ 0x181b22e34  size=23 ==========

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



// ========== thunk_EXT_FUN_1ad280eaa @ 0x181b22e4c  size=5 ==========

/* WARNING: Control flow encountered bad instruction data */

void thunk_EXT_FUN_1ad280eaa(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}



// ========== FUN_181b22e7d @ 0x181b22e7d  size=36 ==========

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



// ========== FUN_181b22ea1 @ 0x181b22ea1  size=3 ==========

void FUN_181b22ea1(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}



// ========== FUN_181b230ec @ 0x181b230ec  size=9 ==========

/* WARNING: Control flow encountered bad instruction data */

void FUN_181b230ec(void)

{
  FUN_181ba403c();
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}



// ========== FUN_181b230f6 @ 0x181b230f6  size=80 ==========

/* WARNING: Control flow encountered bad instruction data */

void FUN_181b230f6(undefined8 param_1,undefined8 param_2,undefined8 param_3)

{
  FUN_181b5afac(param_1,param_2,param_3);
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}



// ========== FUN_181b23149 @ 0x181b23149  size=32 ==========

/* WARNING: Control flow encountered bad instruction data */

void FUN_181b23149(void)

{
  char cVar1;
  undefined1 uVar2;
  undefined8 *puVar3;
  undefined8 *unaff_RBP;
  longlong unaff_RSI;
  byte *unaff_R15;
  undefined8 uStack_10;
  
  uVar2 = FUN_181b3b3da();
  puVar3 = (undefined8 *)&stack0xfffffffffffffff8;
  cVar1 = '\x1c';
  do {
    unaff_RBP = unaff_RBP + -1;
    puVar3 = puVar3 + -1;
    *puVar3 = *unaff_RBP;
    cVar1 = cVar1 + -1;
  } while ('\0' < cVar1);
  out(0xc,uVar2);
  *unaff_R15 = *unaff_R15 | (byte)&stack0xfffffffffffffff8;
  *(undefined1 *)(unaff_RSI + -0x2c460b96) = 0;
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}



// ========== FUN_181b3b3da @ 0x181b3b3da  size=139 ==========

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Removing unreachable block (ram,0x000181b3b3f8) */
/* WARNING: Removing unreachable block (ram,0x000181b3b3fb) */
/* WARNING: Removing unreachable block (ram,0x000181b3b40a) */
/* WARNING: Removing unreachable block (ram,0x000181b3b40e) */
/* WARNING: Removing unreachable block (ram,0x000181b3b469) */
/* WARNING: Removing unreachable block (ram,0x000181b3b474) */
/* WARNING: Removing unreachable block (ram,0x000181b3b47c) */
/* WARNING: Removing unreachable block (ram,0x000181b3b457) */
/* WARNING: Removing unreachable block (ram,0x000181b3b481) */

void FUN_181b3b3da(undefined8 param_1,longlong param_2)

{
  char *pcVar1;
  char unaff_BL;
  undefined1 unaff_BH;
  undefined6 unaff_0000001a;
  uint unaff_ESI;
  
  FUN_181bb832b();
  pcVar1 = (char *)FUN_181606000();
  *pcVar1 = *pcVar1 + (char)pcVar1;
  *pcVar1 = *pcVar1 + (char)pcVar1;
  *pcVar1 = *pcVar1 + unaff_BL;
  if ((int)unaff_ESI < 0x3aa68e0a) {
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  }
  out(0xa2,(int)((param_2 << 0x20 | (ulonglong)unaff_ESI) /
                (ulonglong)*(uint *)CONCAT62(unaff_0000001a,CONCAT11(unaff_BH,unaff_BL))));
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}



// ========== FUN_181b4eaac @ 0x181b4eaac  size=27 ==========

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_181b4eaac(undefined8 param_1,undefined2 param_2)

{
  code *pcVar1;
  longlong unaff_RBP;
  undefined1 auStackX_8 [32];
  
  FUN_182ac02ff();
  out(param_2,0xff4dd057);
  *(char *)(unaff_RBP + -0x40faaac6) = *(char *)(unaff_RBP + -0x40faaac6) + '\x01';
  _DAT_bb5bdad8 = _DAT_bb5bdad8 + (int)auStackX_8;
  pcVar1 = (code *)swi(1);
  (*pcVar1)();
  return;
}



// ========== FUN_181b50507 @ 0x181b50507  size=12 ==========

/* WARNING: Control flow encountered bad instruction data */

void FUN_181b50507(void)

{
  FUN_181b7b471();
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}



// ========== FUN_181b58d7a @ 0x181b58d7a  size=20 ==========

void FUN_181b58d7a(void)

{
  FUN_181645c69();
  return;
}



// ========== FUN_181b5afac @ 0x181b5afac  size=26 ==========

/* WARNING: Control flow encountered bad instruction data */

void FUN_181b5afac(void)

{
  FUN_18160d3eb();
  func_0x00012cbe72fb();
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}



// ========== FUN_181b63831 @ 0x181b63831  size=82 ==========

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



// ========== entry @ 0x181b72917  size=88 ==========

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



// ========== FUN_181b74f88 @ 0x181b74f88  size=15 ==========

void FUN_181b74f88(void)

{
                    /* WARNING: Subroutine does not return */
  FUN_181637630();
}



// ========== FUN_181b7b471 @ 0x181b7b471  size=34 ==========

/* WARNING: Control flow encountered bad instruction data */

undefined4 FUN_181b7b471(undefined8 param_1,undefined2 param_2)

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
  undefined8 local_res8;
  undefined8 uStack_10;
  
  uVar8 = (undefined7)((ulonglong)param_1 >> 8);
  bVar7 = (byte)param_1;
  local_res8 = 0xffffffff871cae1d;
  FUN_181606000();
  uVar6 = FUN_181b74f88();
  bVar5 = (byte)uVar6;
  if ((POPCOUNT(bVar5 & 0xbf) & 1U) != 0) {
    bVar3 = *(byte *)CONCAT71(uVar8,bVar7);
    *(byte *)CONCAT71(uVar8,bVar7) = *(char *)CONCAT71(uVar8,bVar7) + bVar7;
    pcVar1 = (char *)(CONCAT71(uVar8,bVar7) + 0x270f30b0);
    *pcVar1 = *pcVar1 + bVar5 + CARRY1(bVar3,bVar7);
    puVar9 = (undefined8 *)&stack0xfffffffffffffff8;
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



// ========== FUN_181b7b493 @ 0x181b7b493  size=44 ==========

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



// ========== FUN_181b811b5 @ 0x181b811b5  size=100 ==========

/* WARNING: Control flow encountered bad instruction data */

void FUN_181b811b5(void)

{
  byte in_SF;
  bool bVar1;
  
  bVar1 = (in_SF & 1) != 0;
  FUN_1816306dd();
  if (bVar1) {
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  }
  FUN_18163ec4b();
                    /* WARNING: Subroutine does not return */
  FUN_181637630();
}



// ========== FUN_181ba403c @ 0x181ba403c  size=8 ==========

void FUN_181ba403c(void)

{
  char *unaff_RSI;
  
  FUN_181b58d7a();
  *unaff_RSI = *unaff_RSI + '\x01';
  return;
}



// ========== FUN_181bb832b @ 0x181bb832b  size=42 ==========

/* WARNING: Control flow encountered bad instruction data */

void FUN_181bb832b(void)

{
  bool in_ZF;
  char in_SF;
  char in_OF;
  
  do {
  } while (in_ZF || in_OF != in_SF);
  FUN_18160d3eb();
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}



// ========== tls_callback_0 @ 0x181e36e5a  size=18 ==========

void tls_callback_0(undefined8 param_1,int param_2)

{
  if (param_2 == 1) {
    FUN_181b4eaac();
    return;
  }
  return;
}



// ========== FUN_181ff7615 @ 0x181ff7615  size=51 ==========

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_181ff7615(undefined8 param_1,undefined2 param_2)

{
  undefined1 uVar1;
  longlong unaff_RBP;
  uint unaff_EDI;
  
  _DAT_7db0223cb5c86fcd = FUN_18162963c();
  uVar1 = in(param_2);
  *(undefined1 *)(ulonglong)(unaff_EDI | *(uint *)(unaff_RBP + 0x32)) = uVar1;
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}



// ========== FUN_182008d87 @ 0x182008d87  size=15 ==========

void FUN_182008d87(void)

{
                    /* WARNING: Subroutine does not return */
  FUN_181637630();
}



// ========== FUN_182a74b93 @ 0x182a74b93  size=81 ==========

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



// ========== FUN_182ac02ff @ 0x182ac02ff  size=69 ==========

/* WARNING: Control flow encountered bad instruction data */

void FUN_182ac02ff(undefined8 param_1,undefined2 param_2)

{
  longlong lVar1;
  undefined4 *unaff_RSI;
  undefined2 in_FPUControlWord;
  undefined2 in_FPUStatusWord;
  undefined2 in_FPUTagWord;
  undefined2 in_FPULastInstructionOpcode;
  undefined8 in_FPUDataPointer;
  undefined8 in_FPUInstructionPointer;
  
  lVar1 = FUN_181b7b493();
  out(*unaff_RSI,param_2);
  out(param_2,(char)lVar1);
  *(undefined2 *)(lVar1 + 0x48) = in_FPUControlWord;
  *(undefined2 *)(lVar1 + 0x4c) = in_FPUStatusWord;
  *(undefined2 *)(lVar1 + 0x50) = in_FPUTagWord;
  *(undefined8 *)(lVar1 + 0x5c) = in_FPUDataPointer;
  *(undefined8 *)(lVar1 + 0x54) = in_FPUInstructionPointer;
  *(undefined2 *)(lVar1 + 0x5a) = in_FPULastInstructionOpcode;
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}



