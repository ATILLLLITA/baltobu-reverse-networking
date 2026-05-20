// FUN_181b811b5
// 0x181b811b5  size=100


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


