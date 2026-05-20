// FUN_181b3b3da
// 0x181b3b3da  size=139


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


