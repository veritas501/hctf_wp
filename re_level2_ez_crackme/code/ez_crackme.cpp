//gcc re_vm.cpp -m32 -o re_vm
#include <stdio.h>
#include <stdlib.h>

int inloop = 0;
char tmp_code[0x100] = { 0 };
int tmp_code_ip = 0;

void mov(int *a, char b) {
	*a = b;
}
void mov32(int *a, int b) {
	*a = b;
}
void lea_ch(char **a, char *b) {
	*a = b;
}
void lea_int(int **a, int *b) {
	*a = b;
}
void ldr_int(int *a, int * b) {
	*a = *b;
}
void ldr_ch(int *a, char * b) {
	*a = *b;
}

void add(int *a, int b) {
	*a += b;
}
void add_pint(int **a, int b) {
	*a += b;
}
void add_pch(char **a, int b) {
	*a += b;
}

void my_xor(int *a, int b) {
	*a = *a^b;
}
void mod(int *a, int b) {
	mov(a, *a%b);
}
void my_or (int *a, int b) {
	mov(a, *a | b);
}
int my_and(int *a, int b) {
	mov(a, *a&b);
	return *a;
}
void push(int **esp, int a) {
	mov(*esp, a);
	add_pint(esp, 1);
}
void pop(int **esp, int *a) {
	add_pint(esp, -1);
	mov(a, **esp);
}
void shr(int *a, int b) {
	*a >>= b;
	mov(a, my_and (a, 0xff));
}
void shl(int *a, int b) {
	*a <<= b;
	mov(a, my_and (a, 0xff));
}
void ror(int *a, int b) {
	int off = b % 32;
	*a = ((*a >> off)&(((long long)1 << (32 - off)) - 1)) + ((*a&(((long long)1 << off) - 1)) << (32 - off));
}
int cmpl(int a, int b) {
	if (a < b) {
		return 1;
	}
	return 0;
}
int cmpeq(char a, char b) {
	if (a == b) {
		return 0;
	}
	return 1;
}

char read_code(char * code, int &ip) {
	int tmp = ip;
	ip++;
	return code[tmp];
}


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

#define read2arg() do{arg1 = read_code(code, ip);arg2 = read_code(code, ip);}while(0)

void inter(char * code) {
	char cur_code = 0;
	char op = 0;
	char type = 0;
	int ip = 0;
	int loop_ip = 0;
	int eax = 0, ecx = 0, edx = 0;
	int * t_intp = 0;
	char * t_chp = 0;
	int t_int = 0;
	char * ebx = 0;
	int * ebx2 = 0;
	int stack[0x100] = { 0 };
	int lf = 0;
	int neq = 0;
	int *esp = stack;
	char arg1, arg2;
	
	printf("Please input the flag:");

	char *input = (char *)malloc(32);
	scanf("%32s", input);
	//char * input = "hctf{aaaaaaaaaaaaaaaaaaaaaaaaaa}";
	char **flag = &input;
	
	char ans[] = {213, 165, 203, 249, 9, 185, 207, 253, 237, 189, 195, 33, 225, 177, 199, 245, 221, 181, 219, 9, 249, 97, 223, 13, 253, 205, 195, 1, 241, 193, 215, 77};
	char * enc = ans;
	char ** p_enc = &enc;

	int key = 0xefbeadde;
	
	int * reg[] = {
		(int *)&eax,
		(int *)&ebx,
		(int *)&ebx2,
		(int *)&ecx,
		(int *)&edx,
		(int *)&esp,
		(int *)&lf,
		(int *)&neq,
		(int *)&t_intp,
		(int *)&t_chp,
		(int *)&t_int,
		(int *)flag,
		(int *)p_enc,
		(int *)&key,
	};

	while (1) {
		cur_code = read_code(code, ip);
		op = cur_code & 0xfe;
		type = cur_code & 1;

		switch (op) {
		case _mov:
			//puts("mov");
			read2arg();
			switch (type) {
			case rr:
				mov(reg[arg1], (char)*reg[arg2]);
				break;
			case rn:
				mov(reg[arg1],arg2);
				break;
			}
			break;
		case _mov32:
			//puts("mov32");
			read2arg();
			switch (type) {
			case rr:
				mov32(reg[arg1], (int)*reg[arg2]);
				break;
			case rn:
				mov32(reg[arg1], arg2);
				break;
			}
			break;
		case _lea_ch:
			//puts("lea_ch");
			switch (type) {
			case rr:
				read2arg();
				lea_ch((char **)reg[arg1], (char *)*reg[arg2]);
				break;
			case rn:
				break;
			}
			break;
		case _lea_int:
			//puts("_lea_int");
			read2arg();
			switch (type) {
			case rr:
				lea_int((int **)reg[arg1], (int *)*reg[arg2]);
				break;
			case rn:
				break;
			}
			break;
		case _ldr_int:
			//puts("_ldr_int");
			read2arg();
			switch (type) {
			case rr:
				ldr_int((int *)reg[arg1], (int *)*reg[arg2]);
				break;
			case rn:
				break;
			}
			break;
		case _ldr_ch:
			//puts("_ldr_ch");
			read2arg();
			switch (type) {
			case rr:
				ldr_ch((int *)reg[arg1], (char *)*reg[arg2]);
				break;
			case rn:
				break;
			}
			break;
		case _add:
			//puts("_add");
			read2arg();
			switch (type) {
			case rr:
				add((int *)reg[arg1], (int)*reg[arg2]);
				break;
			case rn:
				add((int *)reg[arg1], arg2);
				break;
			}
			break;
		case _add_pint:
			//puts("_add_pint");
			read2arg();
			switch (type) {
			case rr:
				add_pint((int **)reg[arg1], (int)*reg[arg2]);
				break;
			case rn:
				add_pint((int **)reg[arg1], arg2);
				break;
			}
			break;
		case _add_pch:
			//puts("_add_pch");
			read2arg();
			switch (type) {
			case rr:
				add_pch((char **)reg[arg1], (int)*reg[arg2]);
				break;
			case rn:
				add_pch((char **)reg[arg1], arg2);
				break;
			}
			break;
		case _my_xor:
			//puts("_my_xor");
			read2arg();
			switch (type) {
			case rr:
				my_xor(reg[arg1], (int)*reg[arg2]);
				break;
			case rn:
				my_xor(reg[arg1], arg2);
				break;
			}
			break;
		case _mod:
			//puts("_mod");
			read2arg();
			switch (type) {
			case rr:
				break;
			case rn:
				mod(reg[arg1], arg2);
				//printf("%d ", *reg[arg1]);
				break;
			}
			break;
		case _my_or:
			//puts("_or");
			read2arg();
			switch (type) {
			case rr:
				my_or(reg[arg1], (int)*reg[arg2]);
				break;
			case rn:
				my_or(reg[arg1], arg2);
				break;
			}
			break;
		case _my_and:
			//puts("_my_and");
			read2arg();
			switch (type) {
			case rr:
				my_and(reg[arg1], (int)*reg[arg2]);
				break;
			case rn:
				my_and(reg[arg1], arg2);
				break;
			}
			break;
		case _push:
			//puts("_push");
			read2arg();
			switch (type) {
			case rr:
				push((int **)reg[arg1], (int)*reg[arg2]);
				break;
			case rn:
				push((int **)reg[arg1], arg2);
				break;
			}
			break;
		case _pop:
			//puts("_pop");
			read2arg();
			switch (type) {
			case rr:
				pop((int **)reg[arg1], (int*)reg[arg2]);
				break;
			case rn:
				break;
			}
			break;
		case _shr:
			//puts("_shr");
			read2arg();
			switch (type) {
			case rr:
				shr((int *)reg[arg1], (int)*reg[arg2]);
				break;
			case rn:
				shr((int *)reg[arg1], arg2);
				break;
			}
			break;
		case _shl:
			//puts("_shl");
			read2arg();
			switch (type) {
			case rr:
				shl((int *)reg[arg1], (int)*reg[arg2]);
				break;
			case rn:
				shl((int *)reg[arg1], arg2);
				break;
			}
			break;
		case _ror:
			//puts("_ror");
			read2arg();
			switch (type) {
			case rr:
				ror((int *)reg[arg1], (int)*reg[arg2]);
				break;
			case rn:
				ror((int *)reg[arg1], arg2);
				break;
			}
			break;
		case _cmpl:
			//puts("_cmpl");
			read2arg();
			switch (type) {
			case rr:
				lf = cmpl((int)*reg[arg1], (int)*reg[arg2]);
				break;
			case rn:
				lf = cmpl((int)*reg[arg1], arg2);
				break;
			}
			break;
		case _cmpeq:
			//puts("_cmpeq");
			read2arg();
			switch (type) {
			case rr:
				neq = cmpeq((int)*reg[arg1], (int)*reg[arg2]);
				break;
			case rn:
				neq = cmpeq((int)*reg[arg1], arg2);
				break;
			}
			break;
		case loop:
			//puts("loop");
			if (!inloop) {
				inloop = 1;
				loop_ip = ip;
			}
			else {
				if (lf) {
					ip = loop_ip;
				}
				else {
					inloop = 0;
				}
			}
			break;
		case code_end:
			//puts("code_end");
			goto end;
		}
	}

end:
	if (!edx) {
		printf("success!!\n");
	}
	else {
		printf("fail...\n");
	}

	return;
}



int main(void) {
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
	inter(code);

	//system("pause");
	return 0;
}