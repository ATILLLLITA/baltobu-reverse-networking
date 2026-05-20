// FUN_181b20b13
// 0x181b20b13  size=554


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


