#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/user.h>
#include <asm/unistd.h>
#include <ctype.h>
#include "rc6.h"//modify from https://github.com/odzhan/rc6

///////////////////////////////////////////////PATCH
//patch to puts("fail");
#define brk_addr_to_puts_fail 0x0000000000401CD5
#define jmp_encrypt "\xE9\x47\xE6\xDF\xFF\xFF\xFF\xFF"
#define rc6_encrypt_start 0x0000000000401541
#define len_of_2encrypt_func 0x3f5+0xca
#define jmp_leijun_xor_cc 0x201324ccecd82a25
///////////////////////////////////////////////PATCH


#define keylen 32
#define s_long (sizeof(long))

long brk_addr=brk_addr_to_puts_fail;

unsigned char rc4_key[] = { 0x5a,0x98,0x2c,0x36,0x5f,0xea,0x90,0xc0,0xb1,0x51,0x71,0x1a,0x32,0xd5,0x86,0x4b,0x4f,0x33,0xb7,0xd9,0x15,0xf6,0x5b,0x99,0x10,0xbd,0x81,0x2e,0x73,0x33,0xde,0x07};
unsigned char s_box[256];

unsigned char plz_input_flag[] = {0x50,0x28,0x40,0xab,0xee,0xc4,0x5c,0x26,0xe3,0x46,0xbc,0x34,0xde,0xf0,0x91,0x74,0x89,0x1b,0x82,0x7e};//"Plz input the flag :"
unsigned char got_it[] = {0x79,0x2b,0x4f,0xab,0xe0,0xc5,0x58,0x73,0xfe,0x12,0xe8,0x66,0x92};//"you got it :)"
unsigned char again[] = {0x74,0x36,0x43,0xab,0xe6,0xcd,0x4d,0x3a,0xf9,0x46,0xf2,0x74};//"try again :("
unsigned char not_mifan[] = {0x43,0x31,0x40,0xab,0xf2,0x8a,0x4d,0x21,0xf2,0x46,0xa6,0x33,0xcf,0xf0,0x83,0x6a,0x9d,0x19,0x82,0x09,0xd8,0x8d,0x6e,0x5b,0xaa,0xc5,};//"Cuz u are not true Mifan!!"

//char flag[] = "hctf{H499y_p7rCe_3vEry_d4y_xxxx}";
char flag[0x40];
unsigned char flag_enc[] = ${ flag_enc_enc }$;

void rc4_init(void){
	int i = 0, j = 0;
	unsigned char k[256] = { 0 };
	char tmp;
	for (i = 0; i<256; i++) {
		s_box[i] = i;
	}
	for (i = 0; i<256; i++) {
		j = (j + s_box[i] + rc4_key[i%32]) % 256;
		tmp = s_box[i];
		s_box[i] = s_box[j];
		s_box[j] = tmp;
	}
}

void rc4_crypt(char*data, int len){
	int i = 0, j = 0, t = 0;
	int k = 0;
	for (k = 0; k<len; k++)
	{
		i = (i + 1) % 256;
		j = (j + s_box[i]) % 256;
		t = (s_box[i] + s_box[j]) % 256;
		data[k] ^= s_box[t];
	}
}

size_t hex2bin(void *bin, char hex[]) {
	size_t len, i;
	int x;
	uint8_t *p = (uint8_t*)bin;

	len = strlen(hex);

	if ((len & 1) != 0) {
		return 0;
	}

	for (i = 0; i < len; i++) {
		if (isxdigit((int)hex[i]) == 0) {
			return 0;
		}
	}

	for (i = 0; i < len / 2; i++) {
		sscanf(&hex[i * 2], "%2x", &x);
		p[i] = (uint8_t)x;
	}
	return len / 2;
}

union u {
	long val;
	char chars[s_long];
};

char leijun[]="52Mi!R_u_MiFans?DO_u_like_me?Thank_u_very_much!R_u_MiFans?DO_u_like_me?"
			"Thank_u_very_much!R_u_MiFans??DO_u_like_me?Thank_u_very_much!Hello,thank_you,"
			"thank_you_very_much~~~~~hehehehello~~Thank_you,thank_u_very_much!!How_R_U_Indian_Mi_fans?"
			"Do_u_like_Mi_4i?OK_Indian_Mi_fans,Do_u_like_Mi_band?Thank_U!\x00" jmp_encrypt;

void get_data(pid_t pid, long addr, char *dst, int len){
	int i;
	union u data;
	for (i = 0; i < len / s_long; i++)
	{
		data.val = ptrace(PTRACE_PEEKDATA, pid, addr + i * s_long, NULL);
		//printf("%p : %p\n",(void *)(addr + i * s_long),(void *)data.val);
		memcpy(dst + i * s_long, data.chars, s_long);
	}

	data.val = ptrace(PTRACE_PEEKDATA, pid, addr + i * s_long, NULL);
	memcpy(dst + i * s_long, data.chars, len % s_long);
	dst[len] = 0;
	//printf("[DEBUG] get_data : dst = %s\n",dst);
}

void send_data(pid_t pid, long addr, char *dst, int len){
	int i;
	union u data;
	for (i = 0; i < len / s_long; i++)
	{
		memcpy(data.chars, dst + i * s_long, s_long);
		//printf("%p : %p\n",(void *)(addr + i * s_long),(void *)data.val);
		ptrace(PTRACE_POKEDATA, pid, addr + i * s_long, data.val);
	}

	data.val = ptrace(PTRACE_PEEKDATA, pid, addr + i * s_long, NULL);
	memcpy(data.chars, dst + i * s_long, len % s_long);
	ptrace(PTRACE_POKEDATA, pid, addr + i * s_long, data.val);
}

void decrypt_data(pid_t pid, long addr, int len){
	char *tmp_str = (char *)malloc(len + 1);
	get_data(pid, addr, tmp_str, len);
	//printf("[dbg] crypted data: %s",tmp_str);
	rc4_crypt(tmp_str,len);
	//printf("[dbg] crypted data: <%s>\n",tmp_str);
	send_data(pid, addr, tmp_str, len);
}

void debugloop(pid_t pid)
{
	int status;
	struct user_regs_struct regs;
	int read_num=0;
	char write_flag=0;
	char set_pwn=0;
	while (1)
	{
		waitpid(pid,&status,0);
		
		if (WIFEXITED(status))
		{
			break;
		}
		ptrace(PTRACE_GETREGS, pid, NULL, &regs);
		//printf("%llu\n",regs.orig_rax);
		if (regs.orig_rax == __NR_write && regs.rdx!=1)
		{
			decrypt_data(pid, regs.rsi, regs.rdx);
		}
		if(regs.orig_rax == __NR_close && regs.rsi==1){//close
			if(set_pwn==1){
				long op;
				op=ptrace(PTRACE_PEEKDATA,pid,brk_addr,NULL);
				ptrace(PTRACE_POKEDATA,pid,brk_addr,0xcccccccccccccccc);
				
				decrypt_data(pid,rc6_encrypt_start,len_of_2encrypt_func);
				ptrace(PTRACE_CONT, pid, NULL, NULL);
				wait(&status);
				op=ptrace(PTRACE_PEEKDATA,pid,brk_addr,NULL);
				op^=jmp_leijun_xor_cc;
				ptrace(PTRACE_POKEDATA,pid,brk_addr,op);
				

				ptrace(PTRACE_GETREGS, pid, NULL, &regs);
				regs.rip--;
				regs.rdx=(unsigned long long)flag;
				ptrace(PTRACE_SETREGS,pid,NULL,&regs);
				//printf("close : %llx %llx %llu | RBP: %llx | RSP: %llx | RIP : %llx\n",regs.rdi,regs.rsi,regs.rdx,regs.rbp,regs.rsp,regs.rip);
				ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
			}
			
			set_pwn++;
		}
		if (regs.orig_rax == __NR_read && regs.rdx == 1)
		{	
			if(!read_num){

			}else if(read_num > 65){
				puts("Try again :(");
				puts("Cuz u r not true Mifan!!");
				kill(pid, SIGTERM);
				exit(0);
			}
			read_num++;
			//printf("read : %llx %llx %llu\n",regs.rdi,regs.rsi,regs.rdx);
		}

		ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
	}
}

#define is_traced(pid) do{\
	char buf1[512];\
	FILE* fin;\
	fin = fopen("/proc/self/status", "r");\
	int tpid;\
	while(fgets(buf1, 512, fin)) {\
		if(!strncmp(buf1, "TracerPid", 9)) {\
			sscanf(buf1, "TracerPid: %d", &tpid);\
			if(tpid != pid) {\
				__asm("pop rbp");\
				__asm("mov rsp,rbp");\
				__asm("ret");\
			}\
		}\
	 }\
	fclose(fin);\
}while(0)

void rc6_encrypt(void *K, void *input, void *output) {
	RC6_KEY key;
	uint32_t i, j, m, A, B, C, D, T0, T1, L[8], *kptr = (uint32_t*)K;

	// initialize L with key
	for (i = 0; i < keylen / 4; i++) {
		L[i] = kptr[i];
	}

	A = RC6_P;

	// initialize S with constants
	for (i = 0; i < RC6_KR; i++) {
		key.x[i] = A;
		A += RC6_Q;
	}

	A = B = i = j = m = 0;

	// mix with key
	for (; m < RC6_KR * 3; m++) {
		A = key.x[i] = ROTL32(key.x[i] + A + B, 3);
		B = L[j] = ROTL32(L[j] + A + B, A + B);

		i++;
		i %= RC6_KR;

		j++;
		j %= keylen / 4;
	}


	rc6_blk *in, *out;

	uint32_t *k = (uint32_t*)key.x;

	in = (rc6_blk*)input;
	out = (rc6_blk*)output;

	// load plaintext/ciphertext
	A = in->w[0];
	B = in->w[1];
	C = in->w[2];
	D = in->w[3];


	B += *k; k++;
	D += *k; k++;


	for (i = 0; i < RC6_ROUNDS; i++){
		T0 = ROTL32(B * (2 * B + 1), 5);
		T1 = ROTL32(D * (2 * D + 1), 5);

		A = ROTL32(A ^ T0, T1) + *k; k++;
		C = ROTL32(C ^ T1, T0) + *k; k++;
		// rotate 32-bits to the left
		T0 = A;
		A = B;
		B = C;
		C = D;
		D = T0;
	}
	A += *k; k++;
	C += *k; k++;

	// save plaintext/ciphertext
	out->w[0] = A;
	out->w[1] = B;
	out->w[2] = C;
	out->w[3] = D;
}
void encrypt(char * input_flag){//rc6 + fence_cipher
	int i = 0;
	int cmp = 0;
	uint8_t c[0x20];

	while (i < 2) {
		rc6_encrypt(leijun, input_flag + (i * 16), c + (i * 16));
		cmp |= memcmp(c + (i * 16), flag_enc + (i * 16), 16);
		i++;
	}
	if(!cmp){
		puts((const char * )got_it);
		exit(0);
	}
	puts((const char *)again);
	puts((const char *)not_mifan);
	exit(0);

}

/*
void rc6_decrypt(void *K, void *input, void *output) {
	RC6_KEY key;
	uint32_t i, j, m, A, B, C, D, T0, T1, L[8], *kptr = (uint32_t*)K;

	// initialize L with key
	for (i = 0; i < keylen / 4; i++) {
		L[i] = kptr[i];
	}

	A = RC6_P;

	// initialize S with constants
	for (i = 0; i < RC6_KR; i++) {
		key.x[i] = A;
		A += RC6_Q;
	}

	A = B = i = j = m = 0;

	// mix with key
	for (; m < RC6_KR * 3; m++)
	{
		A = key.x[i] = ROTL32(key.x[i] + A + B, 3);
		B = L[j] = ROTL32(L[j] + A + B, A + B);

		i++;
		i %= RC6_KR;

		j++;
		j %= keylen / 4;
	}

	rc6_blk *in, *out;
	uint32_t *k = (uint32_t*)key.x;

	in = (rc6_blk*)input;
	out = (rc6_blk*)output;

	// load plaintext/ciphertext
	A = in->w[0];
	B = in->w[1];
	C = in->w[2];
	D = in->w[3];


	k += 43;
	C -= *k; k--;
	A -= *k; k--;


	for (i = 0; i < RC6_ROUNDS; i++)
	{

		T0 = ROTL32(A * (2 * A + 1), 5);
		T1 = ROTL32(C * (2 * C + 1), 5);

		B = ROTR32(B - *k, T0) ^ T1; k--;
		D = ROTR32(D - *k, T1) ^ T0; k--;
		// rotate 32-bits to the right
		T0 = D;
		D = C;
		C = B;
		B = A;
		A = T0;

	}


	D -= *k; k--;
	B -= *k; k--;

	// save plaintext/ciphertext
	out->w[0] = A;
	out->w[1] = B;
	out->w[2] = C;
	out->w[3] = D;
}
*/

int main(int argc, char *argv[])
{	
	setbuf(stdin,0);
	setbuf(stdout,0);
	rc4_init();
	__asm(".intel_syntax noprefix");
	if(argc==1){
		//puts(leijun);
		pid_t parent_pid = getpid();
		//printf("[DEBUG] parent pid : %d\n",parent_pid);

		pid_t pid;
		char dbg_pid[0x20];
		sprintf(dbg_pid,"%d",parent_pid);
		char * new_argv[]= {argv[0],dbg_pid,0};
		pid = fork();
		if (pid < 0)
		{
			//puts("[DEBUG] fork error");
			return 0;
		}
		else if (!pid)
		{ //child
			ptrace(PTRACE_TRACEME, 0, NULL, NULL);
			if (execve(argv[0], new_argv, NULL) == -1)
			{
				//puts("[DEBUG] execl error!!");
				return 0;
			}
		}
		else
		{ //parent
			is_traced(0);
			debugloop(pid);
		}
		
	}else{
		if(argc == 2){//real child
			puts((const char *)plz_input_flag);
			scanf("%48s",flag);

			is_traced(atoi(argv[1]));

			puts((const char *)again);
			puts((const char *)not_mifan);
			exit(0);
		}
	}
	return 0;
}