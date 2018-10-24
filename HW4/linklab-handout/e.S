
ex2.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <always_ok>:
   0:	31 c0                	xor    %eax,%eax
   2:	85 ff                	test   %edi,%edi
   4:	75 0a                	jne    10 <always_ok+0x10>
   6:	c3                   	retq   
   7:	66 0f 1f 84 00 00 00 	nopw   0x0(%rax,%rax,1)
   e:	00 00 
  10:	48 83 ec 08          	sub    $0x8,%rsp
  14:	e8 00 00 00 00       	callq  19 <always_ok+0x19>
  19:	31 c0                	xor    %eax,%eax
  1b:	e8 00 00 00 00       	callq  20 <always_ok+0x20>
  20:	b8 01 00 00 00       	mov    $0x1,%eax
  25:	48 83 c4 08          	add    $0x8,%rsp
  29:	c3                   	retq   
  2a:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)

0000000000000030 <sometimes_ok>:
  30:	53                   	push   %rbx
  31:	31 c0                	xor    %eax,%eax
  33:	89 fb                	mov    %edi,%ebx
  35:	e8 00 00 00 00       	callq  3a <sometimes_ok+0xa>
  3a:	31 c0                	xor    %eax,%eax
  3c:	85 db                	test   %ebx,%ebx
  3e:	75 08                	jne    48 <sometimes_ok+0x18>
  40:	5b                   	pop    %rbx
  41:	c3                   	retq   
  42:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)
  48:	e8 00 00 00 00       	callq  4d <sometimes_ok+0x1d>
  4d:	b8 01 00 00 00       	mov    $0x1,%eax
  52:	5b                   	pop    %rbx
  53:	c3                   	retq   
  54:	66 90                	xchg   %ax,%ax
  56:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
  5d:	00 00 00 

0000000000000060 <never_ok>:
  60:	48 83 ec 08          	sub    $0x8,%rsp
  64:	31 c0                	xor    %eax,%eax
  66:	85 ff                	test   %edi,%edi
  68:	75 16                	jne    80 <never_ok+0x20>
  6a:	e8 00 00 00 00       	callq  6f <never_ok+0xf>
  6f:	31 c0                	xor    %eax,%eax
  71:	e8 00 00 00 00       	callq  76 <never_ok+0x16>
  76:	31 c0                	xor    %eax,%eax
  78:	48 83 c4 08          	add    $0x8,%rsp
  7c:	c3                   	retq   
  7d:	0f 1f 00             	nopl   (%rax)
  80:	e8 00 00 00 00       	callq  85 <never_ok+0x25>
  85:	31 c0                	xor    %eax,%eax
  87:	e8 00 00 00 00       	callq  8c <never_ok+0x2c>
  8c:	31 c0                	xor    %eax,%eax
  8e:	e8 00 00 00 00       	callq  93 <never_ok+0x33>
  93:	b8 01 00 00 00       	mov    $0x1,%eax
  98:	48 83 c4 08          	add    $0x8,%rsp
  9c:	c3                   	retq   
