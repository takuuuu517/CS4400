
ex0.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <always_ok>:
#include "it.h"

int other_v0 = 0;
int other_v1 = 1;

int always_ok(int sel) {
   0:	55                   	push   %rbp
   1:	48 89 e5             	mov    %rsp,%rbp
   4:	48 83 ec 10          	sub    $0x10,%rsp
   8:	89 7d fc             	mov    %edi,-0x4(%rbp)
  open_it();
   b:	b8 00 00 00 00       	mov    $0x0,%eax
  10:	e8 00 00 00 00       	callq  15 <always_ok+0x15>
  close_it();
  15:	b8 00 00 00 00       	mov    $0x0,%eax
  1a:	e8 00 00 00 00       	callq  1f <always_ok+0x1f>
  return 1;
  1f:	b8 01 00 00 00       	mov    $0x1,%eax
}
  24:	c9                   	leaveq 
  25:	c3                   	retq   

0000000000000026 <also_always_ok>:

int also_always_ok(int sel) {
  26:	55                   	push   %rbp
  27:	48 89 e5             	mov    %rsp,%rbp
  2a:	48 83 ec 10          	sub    $0x10,%rsp
  2e:	89 7d fc             	mov    %edi,-0x4(%rbp)
  // sel += other_v0;
  open_it();
  31:	b8 00 00 00 00       	mov    $0x0,%eax
  36:	e8 00 00 00 00       	callq  3b <also_always_ok+0x15>
  close_it();
  3b:	b8 00 00 00 00       	mov    $0x0,%eax
  40:	e8 00 00 00 00       	callq  45 <also_always_ok+0x1f>
  open_it();
  45:	b8 00 00 00 00       	mov    $0x0,%eax
  4a:	e8 00 00 00 00       	callq  4f <also_always_ok+0x29>
  sel += other_v1;
  4f:	8b 05 00 00 00 00    	mov    0x0(%rip),%eax        # 55 <also_always_ok+0x2f>
  55:	01 45 fc             	add    %eax,-0x4(%rbp)
  close_it();
  58:	b8 00 00 00 00       	mov    $0x0,%eax
  5d:	e8 00 00 00 00       	callq  62 <also_always_ok+0x3c>
  open_it();
  62:	b8 00 00 00 00       	mov    $0x0,%eax
  67:	e8 00 00 00 00       	callq  6c <also_always_ok+0x46>
  close_it();
  6c:	b8 00 00 00 00       	mov    $0x0,%eax
  71:	e8 00 00 00 00       	callq  76 <also_always_ok+0x50>
  return sel;
  76:	8b 45 fc             	mov    -0x4(%rbp),%eax
}
  79:	c9                   	leaveq 
  7a:	c3                   	retq   

000000000000007b <never_ok>:

int never_ok(int sel) {
  7b:	55                   	push   %rbp
  7c:	48 89 e5             	mov    %rsp,%rbp
  7f:	48 83 ec 10          	sub    $0x10,%rsp
  83:	89 7d fc             	mov    %edi,-0x4(%rbp)
  open_it();
  86:	b8 00 00 00 00       	mov    $0x0,%eax
  8b:	e8 00 00 00 00       	callq  90 <never_ok+0x15>
  return 3; /* oops! - return without a balancing close */
  90:	b8 03 00 00 00       	mov    $0x3,%eax
}
  95:	c9                   	leaveq 
  96:	c3                   	retq   

0000000000000097 <also_never_ok>:

int also_never_ok(int sel) {
  97:	55                   	push   %rbp
  98:	48 89 e5             	mov    %rsp,%rbp
  9b:	48 83 ec 10          	sub    $0x10,%rsp
  9f:	89 7d fc             	mov    %edi,-0x4(%rbp)
  open_it();
  a2:	b8 00 00 00 00       	mov    $0x0,%eax
  a7:	e8 00 00 00 00       	callq  ac <also_never_ok+0x15>
  open_it(); /* oops! - already open */
  ac:	b8 00 00 00 00       	mov    $0x0,%eax
  b1:	e8 00 00 00 00       	callq  b6 <also_never_ok+0x1f>
  close_it(); /* doesn't matter whether this is here */
  b6:	b8 00 00 00 00       	mov    $0x0,%eax
  bb:	e8 00 00 00 00       	callq  c0 <also_never_ok+0x29>
  close_it(); /* doesn't matter whether this is here */
  c0:	b8 00 00 00 00       	mov    $0x0,%eax
  c5:	e8 00 00 00 00       	callq  ca <also_never_ok+0x33>
  return 4;
  ca:	b8 04 00 00 00       	mov    $0x4,%eax
}
  cf:	c9                   	leaveq 
  d0:	c3                   	retq   

00000000000000d1 <still_never_ok>:

int still_never_ok(int sel) {
  d1:	55                   	push   %rbp
  d2:	48 89 e5             	mov    %rsp,%rbp
  d5:	48 83 ec 10          	sub    $0x10,%rsp
  d9:	89 7d fc             	mov    %edi,-0x4(%rbp)
  open_it();
  dc:	b8 00 00 00 00       	mov    $0x0,%eax
  e1:	e8 00 00 00 00       	callq  e6 <still_never_ok+0x15>
  close_it();
  e6:	b8 00 00 00 00       	mov    $0x0,%eax
  eb:	e8 00 00 00 00       	callq  f0 <still_never_ok+0x1f>
  close_it(); /* oops! - not currently open */
  f0:	b8 00 00 00 00       	mov    $0x0,%eax
  f5:	e8 00 00 00 00       	callq  fa <still_never_ok+0x29>
  return 5;
  fa:	b8 05 00 00 00       	mov    $0x5,%eax
}
  ff:	c9                   	leaveq 
 100:	c3                   	retq   
