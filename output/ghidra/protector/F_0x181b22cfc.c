// FUN_181b22cfc
// 0x181b22cfc  size=303


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


