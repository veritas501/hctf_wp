import sys, random, os, tempfile
from pwn import *
from templite import Templite

#flag = r"hctf{p1ay_Wi7h_p7r4Ce_1n__L1nuX}"
enc_key = "52Mi!R_u_MiFans?DO_u_like_me?Tha"


def rotl32(v,n):
  return (((((v)&0xffffffff) << (n%32))&0xffffffff) | (((v)&0xffffffff) >> (32 - (n%32))))

def encrypt16(flag):
  RC6_ROUNDS=20
  RC6_KR=(2*(RC6_ROUNDS+2))
  RC6_P=0xB7E15163
  RC6_Q=0x9E3779B9
  keylen=32
  L = [0]*8
  key = [0]*RC6_KR

  for i in range(8):
    L[i] = u32(enc_key[i*4:i*4+4])

  A = RC6_P

  for i in range(RC6_KR):
    key[i] = A
    A+=RC6_Q
    A=A&0xffffffff

  A=0
  B=0
  i=0
  j=0

  for m in range(3*RC6_KR):
    key[i] = rotl32(key[i]+A+B,3)
    A=key[i]
    L[j] = rotl32(L[j]+A+B, A+B);
    B = L[j]

    i+=1
    i%=RC6_KR

    j+=1
    j%=keylen/4

  p_in = [0]*4
  p_out= [0]*4
  t0=0
  t1=0

  for i in range(4):
    p_in[i] = u32(flag[i*4:i*4+4])

  k=0

  p_in[1]+=key[k]
  k+=1
  p_in[3]+=key[k]
  k+=1


  for i in range(RC6_ROUNDS):
    t0=rotl32(p_in[1]*(2*p_in[1]+1),5)
    t1=rotl32(p_in[3]*(2*p_in[3]+1),5)

    p_in[0]=rotl32(p_in[0]^t0,t1)+key[k]
    k+=1
    p_in[2]=rotl32(p_in[2]^t1,t0)+key[k]
    k+=1

    t0=p_in[0]
    p_in[0]=p_in[1]
    p_in[1]=p_in[2]
    p_in[2]=p_in[3]
    p_in[3]=t0

  p_in[0]+=key[k]
  k+=1
  p_in[2]+=key[k]
  k+=1

  for i in range(4):
    p_out[i] = p32(p_in[i]&0xffffffff)

  return ''.join(p_out)

def encrypt(flag):
  assert len(flag)==32,"the length of flag != 32"
  return encrypt16(flag[:16])+encrypt16(flag[16:])

def encrypt_function(filename):
  fp = open(filename,'rb')
  data = fp.read()
  fp.close()
  start=0x1541
  enc_len=0x3f5+0xca
  p = list(data[start:start+enc_len])
  key = [0x5a,0x98,0x2c,0x36,0x5f,0xea,0x90,0xc0,0xb1,0x51,0x71,0x1a,0x32,0xd5,0x86,0x4b,0x4f,0x33,0xb7,0xd9,0x15,0xf6,0x5b,0x99,0x10,0xbd,0x81,0x2e,0x73,0x33,0xde,0x07 ]
  j=0
  sbox=range(256)
  
  for i in range(256):
    j=(j+sbox[i]+key[i%len(key)])%256
    sbox[i],sbox[j] = sbox[j],sbox[i]
  
  k=0
  j=0
  i=0
  for k in range(len(p)):
    i=(i+1)%256
    j=(j+sbox[i])%256
    t=(sbox[i]+sbox[j])%256
    p[k] = chr(ord(p[k])^sbox[t])
  
  enc = data[:start]+''.join(p)+data[start+enc_len:]

  fp=open(filename,'wb')
  fp.write(enc)
  fp.close()

def generate(argv):
  if len(argv) != 3:
    print 'Usage: python generate_one.py [flag] [output_file]'
    sys.exit()

  flag = argv[1]
  output_file = argv[2]
  flag_enc_enc = str(map(ord,list(encrypt(flag)))).replace('[','{').replace(']','}')

  template = open('debugger.cpp.templite', 'r').read()
  c_code = Templite(template).render(flag_enc_enc=flag_enc_enc)

  fp = open('temp_debugger.cpp','w')
  fp.write(c_code)
  fp.close()

  os.system('mkdir build 2>/dev/null')
  os.system('gcc -z execstack -masm=intel -std=c++11 temp_debugger.cpp -o ' + output_file)
  os.system('strip '+output_file)
  os.system('rm temp_debugger.cpp')
  encrypt_function(output_file)
  os.system('upx '+output_file+' 1>/dev/null')
  os.system("mv ./"+output_file+" ./build/"+output_file)
if __name__ == '__main__':
  generate(sys.argv)