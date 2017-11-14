# re_level2_ez_crackme

考察对简单解释器的逆向能力。

## 加密解密过程

```python
box=[]
for i in range(32):
	x=(x+51)%32
	box.append(x)
```

先用如上方式初始化一个box。

用这个box将输入的明文进行乱序。

```python
	head = (out[0]&0xe0)>>5
	for i in range(31):
		out[i] = ((out[i]&0x1f)<<3)+((out[i+1]&0xe0)>>5)
	out[31] = ((out[31]&0x1f)<<3) + head
```

然后用如上方式，将乱序后的结果进行整体循环左移3位。

```python
	key = 'deadbeef'.decode('hex')
	for i in range(32):
		out2.append(out[i]^((ord(key[i%4])+i)&0xff))
```

然后利用key和下标i对左移后的结果做异或即可。

完整python加密解密脚本：

```python
key = 'deadbeef'.decode('hex')

def encrypt(flag):
	out=[]
	out2=[]
	x=0#gen box
	box=[]
	for i in range(32):
		x=(x+51)%32
		box.append(x)
	for i in range(32):
		out.append(ord(flag[box[i]]))
	head = (out[0]&0xe0)>>5
	for i in range(31):
		out[i] = ((out[i]&0x1f)<<3)+((out[i+1]&0xe0)>>5)
	out[31] = ((out[31]&0x1f)<<3) + head
	for i in range(32):
		out2.append(out[i]^((ord(key[i%4])+i)&0xff))
	return  out2


def decrypt(enc_list):
	out=[]
	out2=[0]*32
	x=0#gen box
	box=[]
	for i in range(32):
		x=(x+51)%32
		box.append(x)
	for i in range(32):
		out.append(enc_list[i]^(ord(key[i%4])+i))

	tail = out[31]&0x7
	for i in reversed(range(1,32)):
		out[i] = ((out[i]&0xf8)>>3)+((out[i-1]&0x7)<<5)
	out[0] = ((out[0]&0xf8)>>3)+(tail<<5)
	for i in range(32):
		out2[box[i]] = out[i]
	return  ''.join(map(chr,out2))
```

## 解释器分析

```cpp
//register
#define _eax 0
#define _ebx 1
#define _ebx2 2
#define _ecx 3
#define _edx 4
#define _esp 5
#define _lf 6
#define _neq 7
#define _t_intp 8
#define _t_chp 9
#define _t_int 10
#define _flag 11
#define _enc 12
#define _key 13
//opcode
#define _mov (0<<1)
#define _mov32 (1<<1)
#define _lea_ch (2<<1)
#define _lea_int (3<<1)
#define _ldr_int (4<<1)
#define _ldr_ch (5<<1)
#define _add (6<<1)
#define _add_pint (7<<1)
#define _add_pch (8<<1)
#define _my_xor (9<<1)
#define _mod (10<<1)
#define _my_or (11<<1)
#define _my_and (12<<1)
#define _push (13<<1)
#define _pop (14<<1)
#define _shr (15<<1)
#define _shl (16<<1)
#define _ror (17<<1)
#define _cmpl (18<<1)
#define _cmpeq (19<<1)
#define loop (20<<1)
#define code_end (21<<1)
//type
#define rn 0	  
#define rr 1	  
```


定义了一些寄存器以及变量，解释器指令，以及指令后面的变量种类。一个完整的指令由高7位的类型和低1位的变量类型组成。

`rr`表示`op reg,reg`,`rn`表示`op reg,num`。


用宏写的解释代码

```cpp
char code[] = {
		_lea_ch | rr,_ebx, _flag,
		_my_xor | rr,_ecx, _ecx,
		_my_xor | rr,_eax,_eax,
		_my_xor | rr,_edx,_edx,

		loop,
		_add | rn,_eax, 51,
		_mod | rn,_eax, 32,
		_lea_ch | rr,_t_chp, _ebx,
		_add_pch | rr,_t_chp,_eax,
		_ldr_ch | rr,_t_int,_t_chp,
		_mov | rr,_edx,_t_int,
		_push | rr,_esp,_edx,
		_add | rn,_ecx, 1,
		_cmpl | rn, _ecx, 32,
		loop,

		_my_xor | rr,_eax,_eax,
		_lea_int | rr,_t_intp,_esp,
		_add_pint | rn,_t_intp, -32,
		_lea_int | rr,_ebx2,_t_intp,
		_ldr_int | rr,_t_int, _ebx2,
		_mov | rr,_eax,_t_int,
		_my_and | rn,_eax, 0xe0,
		_shr | rn,_eax, 5,
		_mov | rr,_edx,_eax,
		_my_xor | rr,_ecx, _ecx,
		loop,
		_ldr_int | rr,_t_int, _ebx2,
		_mov | rr,_eax,_t_int,
		_my_and | rn,_eax, 0x1f,
		_shl | rn,_eax, 3,
		_push | rr,_esp,_eax,
		_lea_int | rr,_t_intp,_esp,
		_add_pint | rn,_t_intp, -32,
		_lea_int | rr,_ebx2,_t_intp,
		_ldr_int | rr,_t_int, _ebx2,
		_mov | rr,_eax,_t_int,
		_my_and | rn,_eax, 0xe0,
		_shr | rn,_eax, 5,
		_pop | rr,_esp,_t_int,
		_add | rr,_t_int,_eax,
		_push | rr,_esp,_t_int,
		_add | rn,_ecx, 1,
		_cmpl | rn, _ecx, 31,
		loop,

		_ldr_int | rr,_t_int, _ebx2,
		_mov | rr,_eax,_t_int,
		_my_and | rn,_eax, 0x1f,
		_shl | rn,_eax, 3,
		_add | rr,_eax,_edx,
		_push | rr,_esp,_eax,

		_my_xor | rr,_ecx, _ecx,
		_mov32 | rr,_edx, _key,
		loop,
		_lea_int | rr,_t_intp,_esp,
		_add_pint | rn,_t_intp, -32,
		_lea_int | rr,_ebx2,_t_intp,
		_ldr_int | rr,_t_int, _ebx2,
		_mov | rr,_eax,_t_int,
		_push | rr,_esp,_eax,
		_mov | rr,_eax,_edx,
		_add | rr,_eax, _ecx,
		_pop | rr,_esp,_t_int,
		_my_xor | rr,_t_int,_eax,
		_push | rr,_esp,_t_int,
		_ror | rn,_edx, 8,
		_add | rn,_ecx, 1,
		_cmpl | rn, _ecx, 32,
		loop,

		_my_xor | rr,_ecx, _ecx,
		_my_xor | rr,_edx,_edx,
		_lea_ch | rr,_ebx,_enc,
		loop,
		_lea_ch | rr,_t_chp, _ebx,
		_add_pch | rr,_t_chp, _ecx,
		_ldr_ch | rr,_t_int,_t_chp,
		_mov | rr,_eax,_t_int,
		_push | rr,_esp,_eax,
		_lea_int | rr,_t_intp,_esp,
		_add_pint | rn,_t_intp, -33,
		_ldr_int | rr,_t_int,_t_intp,
		_pop | rr,_esp,_eax,
		_push | rr,_esp,_eax,
		_cmpeq | rr,_eax,_t_int,
		_my_or | rr,_edx, _neq,
		_add | rn,_ecx, 1,
		_cmpl | rn, _ecx, 32,
		loop,

		code_end
	};
```

其中loop的实现是用记录ip的方式来实现的。


完整的程序代码见附件。

