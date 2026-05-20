// FUN_181b21164
// 0x181b21164  size=5346


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


