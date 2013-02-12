import subprocess
import sys
import os
import re


def run_script(id):
    args=[ "../../bin/performance","text","pattern",str(id)]
    f = open('temp','w')
    process = subprocess.Popen(args,stdout=f)
    memuse = subprocess.Popen(["./mem.sh", str(process.pid)],stdout=open('temp2','w'))
    process.wait()
    while(True):
        retcode = memuse.poll() #returns None while subprocess is running
        if(retcode is not None):
            break
    time = None
    memory = None
    matches = None
    for l in open('temp').readlines():
 #       print l
        m = re.match(r"^(.+) seconds$",l)
        if(m):
            time = float(m.group(1))
        mm = re.match(r"^(.+) matches$",l)
        if(mm):
            matches = int(mm.group(1))
    for l in open('temp2').readlines():
        m=re.match(r"^\s*(\d+)$",l)
        if(m):
            memory = int(m.group(1))
    os.system("rm temp")
    os.system("rm temp2")
    if time == None:
        raise mo
    return (time,memory,matches)

def random_test(ids, pattern_length,text_length,pattern_count,alphabet_size=30):
    os.system("bin/random_patterns %d %d %d > pattern" % (pattern_length,pattern_count,alphabet_size))
    os.system("bin/random_text %d %d > text" % (text_length,alphabet_size))
    for id in ids:
        r =  run_script(id)
        print str(id),
        print r
    os.system("rm pattern")
    os.system("rm text")
    return r

def book_test(ids, textbook, patternbook,text_length, pattern_length, pattern_count):
    print "%d patterncount, %d patternlength, %d textlength" % (pattern_count,pattern_length,text_length)
    os.system("bin/book_patterns %s %d %d > pattern" % (patternbook, pattern_length,pattern_count))
    os.system("cat %s  | cut -c %d- > text" % (textbook,text_length))
    for id in ids:
        r =  run_script(id)
        print str(id),
        print r
    os.system("rm pattern")
    os.system("rm text")
    return r
ids = [0,1,2]
def run_book():
    for pc in range(5000,20000,3000):
        for pl in range(4,20,3):
            book_test(ids,"../musketeers.txt","../robinson.txt",50000,pl,pc)

def run_random():
    for pc in range(5000,20000,3000):
        for pl in range(3,20,3):
            print "patternlength: %d, patterncount %d" % (pl,pc)
            random_test(ids,pl,1000000,pc,15)
#run_random()
#run_book()
book_test(ids,"../musketeers.txt","../robinson.txt",150000,4,14000)
