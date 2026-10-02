"""Owned Mach-O fixtures; no input instructions are executed."""
from pathlib import Path
import os
import random
import struct
import subprocess
import tempfile
from kernelcabinet_equivalence import review_extract_image

ROOT = Path(__file__).resolve().parents[1]

def main():
    tested = 0
    with tempfile.TemporaryDirectory(prefix='kernelcabinet-safety-') as folder:
        work = Path(folder)
        binary = work/'kernelcabinet'
        subprocess.run(['clang','-std=c99','-Wall','-Wextra','-Werror','-O1',
                        '-fsanitize=address,undefined','-fno-sanitize-recover=all',
                        *map(str,sorted((ROOT/'Sources').glob('*.c'))),'-o',str(binary)],check=True)
        valid = review_extract_image(1,random.Random(20261002))
        cases = [valid[:length] for length in (0,1,31,32,40,71,104,335,4096)]
        # Malformed commands, tables, embedded headers, addresses and records.
        for offset,value,fmt in [(16,0xffffffff,'I'),(20,0xffffffff,'I'),(36,0,'I'),
                                 (36,9,'I'),(36,0xffffffff,'I'),(96,0xffffffff,'I'),
                                 (184,0xffffffffffffffff,'Q'),(192,0xffffffff,'I'),
                                 (264,7,'Q'),(0x1000,0xfffffffffffffff8,'Q'),
                                 (0x1000,0x4001,'Q'),(0x1200,0x10000,'Q'),
                                 (0x4000+36,0,'I'),(0x4000+72,0xffffffffffffffff,'Q')]:
            damaged=bytearray(valid)
            struct.pack_into('<'+fmt,damaged,offset,value)
            cases.append(bytes(damaged))
        damaged=bytearray(valid)
        damaged[0x1800+16:0x1800+80]=b'X'*64
        cases.append(bytes(damaged))
        def run(data,expected,selection=b'0\n', setup=None):
            nonlocal tested
            case=work/str(tested)
            case.mkdir()
            image=case/'owned-image'
            image.write_bytes(data)
            before=image.read_bytes()
            if setup: setup(case)
            done=subprocess.run([str(binary),str(image)],input=selection,cwd=case,capture_output=True,timeout=5)
            assert done.returncode==expected,(tested,done.returncode,done.stderr)
            assert b'Sanitizer' not in done.stderr and b'runtime error' not in done.stderr,done.stderr
            assert image.read_bytes()==before
            tested+=1
            return case,done
        for data in cases:
            case,_=run(data,2)
            assert not (case/'kernelcabinet-index-0.bin').exists()
        for selection in [b'-1\n',b'1\n',b'0tail\n',b'9'*90+b'\n',b'\n',b'9999999999999999999999999999999\n']:
            run(valid,2,selection)
        case,_=run(valid,0)
        output=case/'kernelcabinet-index-0.bin'
        assert output.read_bytes()==valid[0x4000:0x4300]
        assert output.stat().st_mode & 0o777 == 0o600
        case,_=run(valid,1,setup=lambda c:(c/'kernelcabinet-index-0.bin').write_bytes(b'keep'))
        assert (case/'kernelcabinet-index-0.bin').read_bytes()==b'keep'
        def symlink(case):
            (case/'keep').write_bytes(b'keep')
            (case/'kernelcabinet-index-0.bin').symlink_to(case/'keep')
        case,_=run(valid,1,setup=symlink)
        assert (case/'keep').read_bytes()==b'keep'
        damaged=bytearray(valid)
        damaged[0x1800+16:0x1800+80]=b'../outside\x1b[31m\0'.ljust(64,b'\0')
        case,done=run(bytes(damaged),0)
        assert b'\x1b' not in done.stdout and b'\\x1b' in done.stdout
        assert (case/'kernelcabinet-index-0.bin').exists() and not (work/'outside').exists()
        # Reproducible structural mutations exercise the bounds guards under sanitizers.
        rng=random.Random(886)
        for _ in range(100):
            damaged=bytearray(valid)
            offset=rng.choice([16,20,36,96,184,192,264,272,0x4000+16,0x4000+20,0x4000+36])
            struct.pack_into('<I',damaged,offset,rng.randrange(0x100000000))
            run(bytes(damaged),2)
        case=work/'special';case.mkdir()
        target=case/'valid';target.write_bytes(valid)
        link=case/'link';link.symlink_to(target)
        fifo=case/'fifo';os.mkfifo(fifo)
        for path in (link,fifo,case,case/'missing'):
            done=subprocess.run([str(binary),str(path)],capture_output=True,timeout=5)
            assert done.returncode in (1,2)
            tested+=1
    print(f'PASS: {tested} process cases with ASan/UBSan; input preserved and outputs exclusive.')

if __name__=='__main__':main()
