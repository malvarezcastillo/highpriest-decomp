"""Reader for GRF 0x102/0x103 archives (single-round DES variant on names and data).

   grf.py list GRF                 summary by extension and directory
   grf.py names GRF [REGEX]        entry names with sizes
   grf.py get GRF OUTDIR REGEX     extract matching entries
   grf.py verify GRF               decode every entry and check its size"""
import struct,sys,zlib,os,re,collections,mmap

IP=[58,50,42,34,26,18,10,2,60,52,44,36,28,20,12,4,62,54,46,38,30,22,14,6,64,56,48,40,32,24,16,8,
    57,49,41,33,25,17,9,1,59,51,43,35,27,19,11,3,61,53,45,37,29,21,13,5,63,55,47,39,31,23,15,7]
FP=[40,8,48,16,56,24,64,32,39,7,47,15,55,23,63,31,38,6,46,14,54,22,62,30,37,5,45,13,53,21,61,29,
    36,4,44,12,52,20,60,28,35,3,43,11,51,19,59,27,34,2,42,10,50,18,58,26,33,1,41,9,49,17,57,25]
P=[16,7,20,21,29,12,28,17,1,15,23,26,5,18,31,10,2,8,24,14,32,27,3,9,19,13,30,6,22,11,4,25]
S=[[14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13],
   [15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9],
   [10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12],
   [7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14],
   [2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3],
   [12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13],
   [4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12],
   [13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11]]

def _bytetab(perm,nin):
    """tab[i][b]: output bits contributed by input byte i holding b."""
    nb=nin//8; tab=[[0]*256 for _ in range(nb)]; nout=len(perm)
    for o,src in enumerate(perm):
        i,bit=(src-1)>>3,7-((src-1)&7)
        for b in range(256):
            if b>>bit&1: tab[i][b]|=1<<(nout-1-o)
    return tab
IPT=_bytetab(IP,64); FPT=_bytetab(FP,64)
SP=[]
for i in range(8):
    row=[]
    for v in range(64):
        s=S[i][(((v>>4)&2)|(v&1))*16+((v>>1)&15)]<<(28-4*i)
        out=0
        for o,src in enumerate(P):
            if s>>(32-src)&1: out|=1<<(31-o)
        row.append(out)
    SP.append(row)

def des(block):
    """One round with an all-zero key, as the client does it."""
    x=0
    for i in range(8): x|=IPT[i][block[i]]
    l,r=x>>32,x&0xffffffff
    f=SP[0][((r&1)<<5)|(r>>27)]^SP[7][((r&0x1f)<<1)|(r>>31)]
    for i in range(1,7): f^=SP[i][(r>>(27-4*i))&0x3f]
    x=((l^f)<<32)|r
    y=0
    for i in range(8): y|=FPT[i][(x>>(56-8*i))&0xff]
    return y.to_bytes(8,'big')

SWAP={0x00:0x2b,0x2b:0x00,0x01:0x68,0x68:0x01,0x48:0x77,0x77:0x48,0x60:0xff,0xff:0x60,0x6c:0x80,0x80:0x6c,0xb9:0xc0,0xc0:0xb9,0xeb:0xfe,0xfe:0xeb}
NIB=bytes(((b<<4)|(b>>4))&0xff for b in range(256))

def decode_name(raw):
    raw=raw.translate(NIB); out=bytearray()
    for i in range(0,len(raw)-7,8): out+=des(raw[i:i+8])
    return bytes(out).split(b'\0')[0]

def decode_data(buf,headeronly,cycle):
    buf=bytearray(buf); n=len(buf)//8
    if cycle<3: cycle=3
    elif cycle<5: cycle+=1
    elif cycle<7: cycle+=9
    else: cycle+=15
    cnt=0
    for i in range(n):
        o=i*8
        if i<20 or (not headeronly and i%cycle==0):
            buf[o:o+8]=des(buf[o:o+8])
        elif not headeronly:
            if cnt==7:
                t=buf[o:o+8]; cnt=0
                buf[o:o+8]=bytes((t[3],t[4],t[6],t[0],t[1],t[2],t[5],SWAP.get(t[7],t[7])))
            cnt+=1
    return bytes(buf)

class Grf:
    def __init__(self,path):
        self.f=open(path,'rb'); self.d=d=mmap.mmap(self.f.fileno(),0,access=mmap.ACCESS_READ)
        assert d[:15]==b'Master of Magic'
        toff,seed,cnt,self.ver=struct.unpack_from('<IIII',d,30)
        assert 0x101<=self.ver<0x104, hex(self.ver)
        t=d[46+toff:]; o=0; self.ents=[]
        for _ in range(cnt-seed-7):
            ln=struct.unpack_from('<I',t,o)[0]; o2=o+ln+4
            a,b,declen,typ,pos=struct.unpack_from('<IIIBI',t,o2)
            if typ&1:
                name=decode_name(t[o+6:o+ln])
                srclen=a-declen-715; ext=name[-4:].lower()
                if ext in (b'.gnd',b'.gat',b'.act',b'.str'): cyc=0
                else: cyc=len(str(srclen)) if srclen>0 else 1
                self.ents.append((name,srclen,b-37579,declen,cyc,pos+46))
            o=o2+17
    def find(self,name):
        """Entry for a name written with either slash, any case; None if absent."""
        if not hasattr(self,'_index'): self._index={e[0].decode('cp949','replace').lower():e for e in self.ents}
        return self._index.get(name.replace('/','\\').lower())
    def read(self,e):
        name,srclen,aligned,declen,cyc,pos=e
        raw=decode_data(self.d[pos:pos+aligned],cyc==0,cyc)
        data=zlib.decompress(raw[:srclen])
        assert len(data)==declen,name
        return data

if __name__=='__main__':
    g=Grf(sys.argv[2]); cmd=sys.argv[1]
    dec=lambda n:n.decode('cp949','replace')
    if cmd=='list':
        print('version %#x, %d files'%(g.ver,len(g.ents)))
        ext=collections.Counter(); size=collections.Counter(); dirs=collections.Counter()
        for e in g.ents:
            n=dec(e[0]); x=n.rsplit('.',1)[-1].lower() if '.' in n else ''
            ext[x]+=1; size[x]+=e[3]; dirs['\\'.join(n.split('\\')[:3])]+=1
        for x,c in ext.most_common(30): print('%6d %12d  .%s'%(c,size[x],x))
        print('--- top directories')
        for k,c in dirs.most_common(45): print('%6d  %s'%(c,k))
    elif cmd=='names':
        rx=re.compile(sys.argv[3],re.I) if len(sys.argv)>3 else None
        for e in g.ents:
            n=dec(e[0])
            if not rx or rx.search(n): print('%9d  %s'%(e[3],n))
    elif cmd=='get':
        out=sys.argv[3]; rx=re.compile(sys.argv[4],re.I); k=0
        for e in g.ents:
            n=dec(e[0])
            if not rx.search(n): continue
            rel=n.replace('\\','/')
            if '..' in rel.split('/') or rel.startswith('/') or ':' in rel: raise SystemExit('unsafe name '+n)
            dst=os.path.join(out,rel); os.makedirs(os.path.dirname(dst),exist_ok=True)
            open(dst,'wb').write(g.read(e)); k+=1
        print(k,'files extracted')
    elif cmd=='verify':
        bad=0
        for i,e in enumerate(g.ents):
            try: g.read(e)
            except Exception as x:
                bad+=1
                if bad<6: print('FAIL',dec(e[0]),repr(x)[:80])
        print(len(g.ents),'entries,',bad,'failed')
