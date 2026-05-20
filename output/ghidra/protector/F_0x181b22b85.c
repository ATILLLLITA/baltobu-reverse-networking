// FUN_181b22b85
// 0x181b22b85  size=375


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


