
ex4.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <function_0>:
   0:	85 ff                	test   %edi,%edi
   2:	78 2c                	js     30 <function_0+0x30>
   4:	8b 05 00 00 00 00    	mov    0x0(%rip),%eax        # a <function_0+0xa>
   a:	74 04                	je     10 <function_0+0x10>
   c:	c3                   	retq   
   d:	0f 1f 00             	nopl   (%rax)
  10:	53                   	push   %rbx
  11:	31 c0                	xor    %eax,%eax
  13:	e8 00 00 00 00       	callq  18 <function_0+0x18>
  18:	8b 1d 00 00 00 00    	mov    0x0(%rip),%ebx        # 1e <function_0+0x1e>
  1e:	31 c0                	xor    %eax,%eax
  20:	e8 00 00 00 00       	callq  25 <function_0+0x25>
  25:	89 d8                	mov    %ebx,%eax
  27:	5b                   	pop    %rbx
  28:	c3                   	retq   
  29:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)
  30:	8b 05 00 00 00 00    	mov    0x0(%rip),%eax        # 36 <function_0+0x36>
  36:	c3                   	retq   
  37:	66 0f 1f 84 00 00 00 	nopw   0x0(%rax,%rax,1)
  3e:	00 00 

0000000000000040 <function_1>:
  40:	53                   	push   %rbx
  41:	48 83 ec 10          	sub    $0x10,%rsp
  45:	85 ff                	test   %edi,%edi
  47:	78 1f                	js     68 <function_1+0x28>
  49:	31 c0                	xor    %eax,%eax
  4b:	e8 00 00 00 00       	callq  50 <function_1+0x10>
  50:	8b 1d 00 00 00 00    	mov    0x0(%rip),%ebx        # 56 <function_1+0x16>
  56:	31 c0                	xor    %eax,%eax
  58:	e8 00 00 00 00       	callq  5d <function_1+0x1d>
  5d:	89 d8                	mov    %ebx,%eax
  5f:	48 83 c4 10          	add    $0x10,%rsp
  63:	5b                   	pop    %rbx
  64:	c3                   	retq   
  65:	0f 1f 00             	nopl   (%rax)
  68:	31 c0                	xor    %eax,%eax
  6a:	89 7c 24 0c          	mov    %edi,0xc(%rsp)
  6e:	e8 00 00 00 00       	callq  73 <function_1+0x33>
  73:	8b 7c 24 0c          	mov    0xc(%rsp),%edi
  77:	8b 05 00 00 00 00    	mov    0x0(%rip),%eax        # 7d <function_1+0x3d>
  7d:	83 ff ff             	cmp    $0xffffffff,%edi
  80:	74 dd                	je     5f <function_1+0x1f>
  82:	31 c0                	xor    %eax,%eax
  84:	e8 00 00 00 00       	callq  89 <function_1+0x49>
  89:	8b 05 00 00 00 00    	mov    0x0(%rip),%eax        # 8f <function_1+0x4f>
  8f:	eb ce                	jmp    5f <function_1+0x1f>
  91:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
  96:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
  9d:	00 00 00 

00000000000000a0 <function_2>:
  a0:	53                   	push   %rbx
  a1:	31 c0                	xor    %eax,%eax
  a3:	89 fb                	mov    %edi,%ebx
  a5:	e8 00 00 00 00       	callq  aa <function_2+0xa>
  aa:	31 c0                	xor    %eax,%eax
  ac:	e8 00 00 00 00       	callq  b1 <function_2+0x11>
  b1:	31 c0                	xor    %eax,%eax
  b3:	e8 00 00 00 00       	callq  b8 <function_2+0x18>
  b8:	85 db                	test   %ebx,%ebx
  ba:	78 14                	js     d0 <function_2+0x30>
  bc:	8b 1d 00 00 00 00    	mov    0x0(%rip),%ebx        # c2 <function_2+0x22>
  c2:	31 c0                	xor    %eax,%eax
  c4:	e8 00 00 00 00       	callq  c9 <function_2+0x29>
  c9:	89 d8                	mov    %ebx,%eax
  cb:	5b                   	pop    %rbx
  cc:	c3                   	retq   
  cd:	0f 1f 00             	nopl   (%rax)
  d0:	31 c0                	xor    %eax,%eax
  d2:	e8 00 00 00 00       	callq  d7 <function_2+0x37>
  d7:	8b 05 00 00 00 00    	mov    0x0(%rip),%eax        # dd <function_2+0x3d>
  dd:	5b                   	pop    %rbx
  de:	c3                   	retq   
  df:	90                   	nop

00000000000000e0 <function_3>:
  e0:	53                   	push   %rbx
  e1:	31 c0                	xor    %eax,%eax
  e3:	89 fb                	mov    %edi,%ebx
  e5:	e8 00 00 00 00       	callq  ea <function_3+0xa> // open
  ea:	31 c0                	xor    %eax,%eax
  ec:	e8 00 00 00 00       	callq  f1 <function_3+0x11> // close
  f1:	31 c0                	xor    %eax,%eax
  f3:	e8 00 00 00 00       	callq  f8 <function_3+0x18> // open
  f8:	85 db                	test   %ebx,%ebx
  fa:	78 14                	js     110 <function_3+0x30>
  fc:	8b 1d 00 00 00 00    	mov    0x0(%rip),%ebx        # 102 <function_3+0x22>
 102:	31 c0                	xor    %eax,%eax
 104:	e8 00 00 00 00       	callq  109 <function_3+0x29> // close in else
 109:	8d 43 01             	lea    0x1(%rbx),%eax
 10c:	5b                   	pop    %rbx
 10d:	c3                   	retq   
 10e:	66 90                	xchg   %ax,%ax
 110:	31 c0                	xor    %eax,%eax
 112:	e8 00 00 00 00       	callq  117 <function_3+0x37>
 117:	8b 1d 00 00 00 00    	mov    0x0(%rip),%ebx        # 11d <function_3+0x3d>
 11d:	8d 43 01             	lea    0x1(%rbx),%eax
 120:	5b                   	pop    %rbx
 121:	c3                   	retq   
