flag = r"hctf{aaaaaaaaaaaaaaaaaaaaaaaaaa}"
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


enc = encrypt(flag)

print enc
#print decrypt(encrypt(flag))
