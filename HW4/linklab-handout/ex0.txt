
ex0.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <always_ok>:
   0:	48 83 ec 08          	sub    $0x8,%rsp
   4:	31 c0                	xor    %eax,%eax
   6:	e8 00 00 00 00       	callq  b <always_ok+0xb>
   b:	31 c0                	xor    %eax,%eax
   d:	e8 00 00 00 00       	callq  12 <always_ok+0x12>
  12:	b8 01 00 00 00       	mov    $0x1,%eax
  17:	48 83 c4 08          	add    $0x8,%rsp
  1b:	c3                   	retq   
  1c:	0f 1f 40 00          	nopl   0x0(%rax)

0000000000000020 <also_always_ok>:
  20:	53                   	push   %rbx
  21:	8b 1d 00 00 00 00    	mov    0x0(%rip),%ebx        # 27 <also_always_ok+0x7>
  27:	31 c0                	xor    %eax,%eax
  29:	01 fb                	add    %edi,%ebx
  2b:	e8 00 00 00 00       	callq  30 <also_always_ok+0x10>
  30:	31 c0                	xor    %eax,%eax
  32:	e8 00 00 00 00       	callq  37 <also_always_ok+0x17>
  37:	31 c0                	xor    %eax,%eax
  39:	e8 00 00 00 00       	callq  3e <also_always_ok+0x1e>
  3e:	03 1d 00 00 00 00    	add    0x0(%rip),%ebx        # 44 <also_always_ok+0x24>
  44:	31 c0                	xor    %eax,%eax
  46:	e8 00 00 00 00       	callq  4b <also_always_ok+0x2b>
  4b:	31 c0                	xor    %eax,%eax
  4d:	e8 00 00 00 00       	callq  52 <also_always_ok+0x32>
  52:	31 c0                	xor    %eax,%eax
  54:	e8 00 00 00 00       	callq  59 <also_always_ok+0x39>
  59:	89 d8                	mov    %ebx,%eax
  5b:	5b                   	pop    %rbx
  5c:	c3                   	retq   
  5d:	0f 1f 00             	nopl   (%rax)

0000000000000060 <never_ok>:
  60:	48 83 ec 08          	sub    $0x8,%rsp
  64:	31 c0                	xor    %eax,%eax
  66:	e8 00 00 00 00       	callq  6b <never_ok+0xb>
  6b:	31 c0                	xor    %eax,%eax
  6d:	e8 00 00 00 00       	callq  72 <never_ok+0x12>
  72:	b8 03 00 00 00       	mov    $0x3,%eax
  77:	48 83 c4 08          	add    $0x8,%rsp
  7b:	c3                   	retq   
  7c:	0f 1f 40 00          	nopl   0x0(%rax)

0000000000000080 <also_never_ok>:
  80:	48 83 ec 08          	sub    $0x8,%rsp
  84:	31 c0                	xor    %eax,%eax
  86:	e8 00 00 00 00       	callq  8b <also_never_ok+0xb>
  8b:	31 c0                	xor    %eax,%eax
  8d:	e8 00 00 00 00       	callq  92 <also_never_ok+0x12>
  92:	31 c0                	xor    %eax,%eax
  94:	e8 00 00 00 00       	callq  99 <also_never_ok+0x19>
  99:	31 c0                	xor    %eax,%eax
  9b:	e8 00 00 00 00       	callq  a0 <also_never_ok+0x20>
  a0:	b8 04 00 00 00       	mov    $0x4,%eax
  a5:	48 83 c4 08          	add    $0x8,%rsp
  a9:	c3                   	retq   
  aa:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)

00000000000000b0 <still_never_ok>:
  b0:	48 83 ec 08          	sub    $0x8,%rsp
  b4:	31 c0                	xor    %eax,%eax
  b6:	e8 00 00 00 00       	callq  bb <still_never_ok+0xb>
  bb:	31 c0                	xor    %eax,%eax
  bd:	e8 00 00 00 00       	callq  c2 <still_never_ok+0x12>
  c2:	31 c0                	xor    %eax,%eax
  c4:	e8 00 00 00 00       	callq  c9 <still_never_ok+0x19>
  c9:	b8 05 00 00 00       	mov    $0x5,%eax
  ce:	48 83 c4 08          	add    $0x8,%rsp
  d2:	c3                   	retq   
