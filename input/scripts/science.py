import subprocess
import sys
import os
import re


def run_script(id):
    args = ["ls","text","pattern",str(id)]
    args=[ "../../bin/performance","text","pattern","1"]
    f = open('temp','w')
    process = subprocess.Popen(args,stdout=f)
    memuse = subprocess.Popen(["./memuse.sh", str(process.pid)],stdout=open('temp2','w'))
    process.wait()
    while(True):
        retcode = memuse.poll() #returns None while subprocess is running
        if(retcode is not None):
            break
    time = None
    memory = None
    for l in open('temp').readlines():
        m = re.match(r"^(.+) seconds$",l)
        if(m):
            time = float(m.group(1))
    for l in open('temp2').readlines():
        m=re.match(r"^\s*(\d+)$",l)
        if(m):
            memory = int(m.group(1))
    os.system("rm temp")
    os.system("rm temp2")
    return (time,memory)

def random_test(id, pattern_length,text_length,pattern_count,alphabet_size=30):
    os.system("bin/random_patterns %d %d %d > pattern" % (pattern_length,pattern_count,alphabet_size))
    os.system("bin/random_text %d %d > text" % (text_length,alphabet_size))
    r =  run_script(id)
    os.system("rm pattern")
    os.system("rm text")
    return r

def book_test(id, textbook, patternbook, pattern_length, pattern_count):
    os.system("bin/book_patterns %s %d %d > pattern" % (patternbook, pattern_length,pattern_count))
    os.system("cp %s text" % textbook)
    r =  run_script(id)
    os.system("rm pattern")
    os.system("rm text")
    return r

print random_test(1,30,5000000,120000,19)
#print book_test(1,"../musketeers.txt","../robinson.txt",10,500)
