// FUN_181b22685
// 0x181b22685  size=1272


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


