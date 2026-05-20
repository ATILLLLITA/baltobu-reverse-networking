// FUN_18163ec4b
// 0x18163ec4b  size=4211


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


