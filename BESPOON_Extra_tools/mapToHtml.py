#!/usr/bin/python

from __future__ import with_statement
import re
import sys, os
import datetime

def fwrite(f,txt):
    f.write(txt+'\n')

class Summary:
    def __init__(self, name, Origin, size, Attributes):
        self.name = name
        self.Origin = Origin
        self.size = size
        self.Attributes = Attributes
    def __str__(self):
        return [self.name,self.Origin,self.size,self.Attributes]

class Symbol:
    def __init__(self, name, address, size,symFile):
        self.address = address
        self.iaddress = int(address,16)
        self.size = size
        self.isize = int(size,16)
        self.symFile=symFile
        self.name = name
    def __str__(self):
        return self.name
        

class Section:
    def __init__(self, sectName, start,size, loadAddr=None):
        self.sectName = sectName
        self.start = start
        self.size = size
        self.loadAddr = loadAddr
        self.symbols=[]
        self.sortedSymbols=[]
        self.location="unknown"
    def __str__(self):
        return self.sectName+""
    def append(self,symbol):
        if symbol == None:
            return
        if symbol.iaddress == 0 or symbol.isize  == 0:
            self.symbols.append(None)
        else:
            self.symbols.append(symbol)
            placed=False
            i=0
            for s in self.sortedSymbols:                
                if s.isize < symbol.isize:
                    placed=True
                    self.sortedSymbols.insert( i, symbol)
                    break
                i=i+1
            if placed==False:
                self.sortedSymbols.append( symbol)

def cvrtString2DecInt(value):
    if value[0:2] == '0x':
        return int(value,16)
    elif value[0:3] == '-0x':
        return -int(value[1:],16)
    return int(value)
    
def cvrtInt2HexString(value):
    return '\r    0x%.8x   \r' %value   

def cvrtInt2IntString(value):
    return '\r     %d      \r' %value   



def analyzeMap(inFile):
    #===============================
    # Load the Sections and Symbols
    #
    found_memMap=False
    startAnalyze=False
    newSect=None
    found_symName=None
    found_symAddr=None
    found_symSize=None
    summary = []
    sections = []
    print "--Analyzing--"
    with open(inFile) as f:
        in_sections = False
        in_symbol = False
        for l in f:
            tab=l.split()
            line=' '.join(tab)
            if line.find("Memory Configuration")>=0:
                found_memMap=True
            if found_memMap==True:
                sumar=l.split()
                if l[0] == '.' or line.find("Linker script and memory map")>=0:
                    found_memMap=False
                    startAnalyze=True
                elif len(sumar)==4:
                    if sumar[0].lower() != 'name' and sumar[1].lower() != 'origin':
                        summary.append(Summary(sumar[0],sumar[1],sumar[2],sumar[3]))
            if startAnalyze==True:
                if l.find("START GROUP")>=0:
                    startAnalyze=False
                    if found_symName!=None:
                        if found_symAddr!=None:
                            newSect.append(Symbol(found_symName,found_symAddr,found_symSize,found_symFunc))
                            found_symName=None
                            found_symAddr=None
                            found_symSize=None
                            found_symFunc=None
                    if newSect != None:
                        sections.append(newSect)
                elif l.find("LOAD")>=0:
                    if found_symName!=None:
                        if found_symAddr!=None:
                            newSect.append(Symbol(found_symName,found_symAddr,found_symSize,found_symFunc))
                            found_symName=None
                            found_symAddr=None
                            found_symSize=None
                            found_symFunc=None
                    if newSect != None:
                        sections.append(newSect)
                else:                    
                    if l[0] == '.' and len(tab)>=3:
                        in_sections=True
                        if(int(tab[2], 16)==0):
                            in_sections=False
                        else:
                            if newSect != None:
                                sections.append(newSect)
                            if len(tab)==6 and tab[3].lower() == 'load':
                                newSect=Section(tab[0],tab[1],tab[2],tab[5])
                            else:
                                newSect=Section(tab[0],tab[1],tab[2])
                            in_symbol=False
                    if in_sections == True:
                        if found_symName!=None:
                            if found_symAddr==None:
                                if tab[0][0:2]=='0x':
                                    found_symAddr=tab[0]
                                    if(len(tab)>1):                                        
                                        if tab[1][0:2]=='0x':
                                            found_symSize=tab[1]
                                            if len(tab)>2:
                                                found_symFunc=''
                                                for i in range(2,len(tab)):
                                                    found_symFunc=found_symFunc+tab[i]
                                                newSect.append(Symbol(found_symName,found_symAddr,found_symSize,found_symFunc))
                                                found_symName=None
                                                found_symAddr=None
                                                found_symSize=None
                                                found_symFunc=None
                            else:
                                newSect.append(Symbol(found_symName,found_symAddr,found_symSize,found_symFunc))
                                found_symName=None
                                found_symAddr=None
                                found_symSize=None
                                found_symFunc=None
                        if l[0:2] == ' .' or l[0:7] == ' COMMON':
                            found_symName=None
                            found_symAddr=None
                            found_symSize=None
                            found_symFunc=None
                            found_symName=tab[0]
                            if(len(tab)>1):
                                if tab[1][0:2]=='0x':
                                    if(len(tab)>2):
                                        found_symAddr=tab[1]
                                        if tab[2][0:2]=='0x':
                                            found_symSize=tab[2]
                                            if len(tab)>3:
                                                found_symFunc=''
                                                for i in range(3,len(tab)):
                                                    found_symFunc=found_symFunc+tab[i]
                                                newSect.append(Symbol(found_symName,found_symAddr,found_symSize,found_symFunc))
                                                found_symName=None
                                                found_symAddr=None
                                                found_symSize=None
                                                found_symFunc=None
    print "--AnalyzingDONE--"
    return [summary,sections]

def generateHTML(outFile,skippedList,sortedOnly,summary,sections):
    #===============================
    # Gererate the HTML File
    #
    print "--Generating--"
    firmware_size=0
    outdirName=os.path.dirname(outFile)

    outfileNameExt=os.path.basename(outFile)
    outfileName, outfileExtension = os.path.splitext(outfileNameExt)
    f = open(outFile, "w")
    

    fwrite(f,'<html><head>')
    fwrite(f,'<body>')
    fwrite(f,'<h1>')
    fwrite(f,'<p>\n<p></p>')
    fwrite(f,'<hr />')
    fwrite(f,'<p>\n<p></p>')
    now = datetime.datetime.now()
    fwrite(f,'SOFTWARE "%s"' %(outfileName))
    fwrite(f,'<hr />')
    fwrite(f,'GENERATED AT ' + now.strftime("%Y-%m-%d %H:%M"))
    fwrite(f,'<p>\n<p></p>')
    fwrite(f,'<hr />')
    fwrite(f,'<p>\n<p></p>')
    fwrite(f,'</h1>')
    fwrite(f,'<h2>')
    fwrite(f,'<ol>')
    fwrite(f,'    <li><A HREF="#MEMSUMMARY"> CHIPSET MEMORY SUMMARY </A></li>')
    fwrite(f,'    <li><A HREF="#SECSUMMARY"> SECTIONS SUMMARY </A></li>')
    fwrite(f,'    <li><A HREF="#SECDETSORTED"> SECTIONS DETAILS SORTED BY SIZE </A></li>')
    fwrite(f,'    <ol style="list-style-type:lower-alpha;">')
    for s in sections:
        skipIt=False
        if len(s.sortedSymbols) <1:
            skipIt=True
        for filtName in skippedList:
            if(filtName.lower() == s.sectName.lower()):
                skipIt=True
                break
        if skipIt == False:
            fwrite(f,'      <li><A HREF="#%sSORTED">%s</A></li>' %(s.sectName,s.sectName))
    fwrite(f,'    </ol>')
    fwrite(f,'    <li><A HREF="#SECDETUNSORTED"> SECTIONS DETAILS UNSORTED </A></li>')
    fwrite(f,'    <ol style="list-style-type:lower-alpha;">')
    for s in sections:
        skipIt=False
        if len(s.sortedSymbols) <1:
            skipIt=True
        for filtName in skippedList:
            if(filtName.lower() == s.sectName.lower()):
                skipIt=True
                break
        if skipIt == False:
            fwrite(f,'      <li><A HREF="#%sUNSORTED">%s</A></li>' %(s.sectName,s.sectName))
    fwrite(f,'    </ol>')
    fwrite(f,'</ol>')
    fwrite(f,'</h2>')
    fwrite(f,'<p>\n<p></p>')
    fwrite(f,'<hr />')
    fwrite(f,'<p>\n<p></p>')
    fwrite(f,'<A NAME="MEMSUMMARY"></A>')
    fwrite(f,'<h2>CHIPSET MEMORY SUMMARY</h2>')
    fwrite(f,'<TABLE border=4 cellspacing=4 cellpadding=4 width=70%>')
    fwrite(f,'    <tr>')
    fwrite(f,'      <td>  NAME  </td>')
    fwrite(f,'      <td align=center>  START </td>')
    fwrite(f,'      <td align=center>  END </td>')
    fwrite(f,'      <td align=center>  ATTRIBUTES </td>')
    fwrite(f,'      <td align=center>  SIZE (HEX)</td>')
    fwrite(f,'      <td align=center>  SIZE (DEC)</td>')    
    fwrite(f,'    </tr>')
    for s in summary:
        org = cvrtString2DecInt(s.Origin)
        siz = cvrtString2DecInt(s.size)
        end = org+siz-1        
        fwrite(f,'    <tr>')
        fwrite(f,'      <td>%s</td>' %s.name)        
        fwrite(f,'      <td align=center>%s</td>' %cvrtInt2HexString(org))
        fwrite(f,'      <td align=center>%s</td>' %cvrtInt2HexString(end))
        fwrite(f,'      <td align=center>%s</td>' %s.Attributes)
        fwrite(f,'      <td align=center>%s</td>' %cvrtInt2HexString(siz))
        fwrite(f,'      <td align=center>%s</td>' %cvrtInt2IntString(siz))        
        fwrite(f,'    </tr>')
    fwrite(f,'</table>')
    fwrite(f,'<p>\n<p></p>')
    for s in sections:
        org = cvrtString2DecInt(s.start)
        siz = cvrtString2DecInt(s.size)
        end = org+siz-1
        for where in summary:
            orgWhere = cvrtString2DecInt(where.Origin)
            sizWhere = cvrtString2DecInt(where.size)
            endWhere = orgWhere+sizWhere-1 
            if(org>=orgWhere) and (end<=endWhere):
                s.location=where.name
                if s.location=='rom':
                    firmware_size+=siz
                break
        fwrite(f,'    </tr>')
    if firmware_size>0:
        fwrite(f,'<h2>FIRMWARE SIZE : %dbytes / 0x%x</h2>' %(firmware_size,firmware_size))
    else:
        fwrite(f,'<h2>FIRMWARE SIZE : -%dbytes / -0x%x</h2>' %(-firmware_size,-firmware_size))
    fwrite(f,'<p>\n<p></p>')
    for s in sections:
        if s.sectName=="heap":
            org = cvrtString2DecInt(s.start)
            siz = cvrtString2DecInt(s.size)
            end = org+siz-1
            if siz <0:
                fwrite(f,'<h2>NECESSARY RAM FOR BESPOON SW( DYNAMIC ALLOCATION): -%dbytes / -0x%x</h2>' %(-siz,-siz))    
            else:
                fwrite(f,'<h2>NECESSARY RAM FOR BESPOON SW( DYNAMIC ALLOCATION): %dbytes / 0x%x</h2>' %(siz,siz))    
    for s in sections:
        if s.sectName=="heap and stack" or s.sectName=="stack":
            org = cvrtString2DecInt(s.start)
            siz = cvrtString2DecInt(s.size)
            end = org+siz-1
            if siz <0:
                fwrite(f,'<h2>AVAILABLE RAM FOR STACK AND OTHER DYNAMIC ALLOCATION: -%dbytes / -0x%x</h2>' %(-siz,-siz))
            else:
                fwrite(f,'<h2>AVAILABLE RAM FOR STACK AND OTHER DYNAMIC ALLOCATION: %dbytes / 0x%x</h2>' %(siz,siz))
    
    fwrite(f,'<hr />')
    fwrite(f,'<p>\n<p></p>')
    fwrite(f,'<A NAME="SECSUMMARY"></A>')
    fwrite(f,'<h2>SECTION SUMMARY</h2>')
    fwrite(f,'<TABLE border=4 cellspacing=4 cellpadding=4 width=70%>')
    fwrite(f,'    <tr align="center">')
    fwrite(f,'      <td>  SECTION NAME  </td>')
    fwrite(f,'      <td align=center>  START </td>')
    fwrite(f,'      <td align=center>  END </td>')
    fwrite(f,'      <td align=center>  SIZE (HEX)</td>')
    fwrite(f,'      <td align=center>  SIZE (DEC)</td>')    
    fwrite(f,'      <td align=center>  LOCATION </td>')    
    fwrite(f,'    </tr>')
    for s in sections:
        org = cvrtString2DecInt(s.start)
        siz = cvrtString2DecInt(s.size)
        end = org+siz-1
        fwrite(f,'    <tr>')
        fwrite(f,'      <td>%s</td>' %s.sectName)
        fwrite(f,'      <td align=center>%s</td>' %cvrtInt2HexString(org))
        fwrite(f,'      <td align=center>%s</td>' %cvrtInt2HexString(end))
        fwrite(f,'      <td align=center>%s</td>' %cvrtInt2HexString(siz))
        fwrite(f,'      <td align=center>%s</td>' %cvrtInt2IntString(siz)) 
        fwrite(f,'      <td align=center>%s</td>' %s.location)
        fwrite(f,'    </tr>')
    fwrite(f,'</table>')
    fwrite(f,'<p>\n<p></p>')
    fwrite(f,'<hr />')    
    fwrite(f,'<p>\n<p></p>')
    fwrite(f,'<A NAME="SECDETSORTED"></A>')
    fwrite(f,'<h2>SECTION DETAILS SORTED BY SIZE</h2>')
    for s in sections:
        skipIt=False
        if len(s.sortedSymbols) <1:
            skipIt=True
        for filtName in skippedList:
            if(filtName.lower() == s.sectName.lower()):
                skipIt=True
                break
        if skipIt == False:
            fwrite(f,'<A NAME="%sSORTED"></A>' %s.sectName)
            fwrite(f,'<h2>%s elements sorted by size. Total size %d. Located in %s</h2>' %(s.sectName,cvrtString2DecInt(s.size),s.location))
            if s.loadAddr != None:
                fwrite(f,'Load address : %s</h2>' %s.loadAddr)
            fwrite(f,'<TABLE border=4 cellspacing=4 cellpadding=4 width=70%>')
            fwrite(f,'    <tr>')
            fwrite(f,'      <td>Name</td>')
            fwrite(f,'      <td align=center>Start Address</td>')
            fwrite(f,'      <td align=center>End Address</td>')
            fwrite(f,'      <td align=center>Size (HEX) </td>' )
            fwrite(f,'      <td align=center>Size (DEC) </td>' )
            fwrite(f,'      <td align=right>File or function</td>' )
            fwrite(f,'    </tr>')
            for sym in s.sortedSymbols:            
                if sym != None:
                    org = cvrtString2DecInt(sym.address)
                    siz = cvrtString2DecInt(sym.size)
                    end = org+siz-1      
                    fwrite(f,'    <tr>')
                    fwrite(f,'      <td>%s</td>' %sym.name)
                    fwrite(f,'      <td align=center>%s</td>' %cvrtInt2HexString(org))
                    fwrite(f,'      <td align=center>%s</td>' %cvrtInt2HexString(end))
                    fwrite(f,'      <td align=center>%s</td>' %cvrtInt2HexString(siz))
                    fwrite(f,'      <td align=center>%s</td>' %cvrtInt2IntString(siz))
                    fwrite(f,'      <td align=right>%s</td>' %sym.symFile)
                    fwrite(f,'    </tr>')
            fwrite(f,'</table>')    
            fwrite(f,'<p>\n<p></p>')
    
    if sortedOnly == False:
        fwrite(f,'<p>\n<p></p>')
        fwrite(f,'<hr />')
        fwrite(f,'<p>\n<p></p>')
        fwrite(f,'<A NAME="SECDETUNSORTED"></A>')
        fwrite(f,'<h2>SECTION DETAILS UNSORTED</h2>')
        for s in sections:
            skipIt=False
            if len(s.symbols) <1:
                skipIt=True
            for filtName in skippedList:
                if(filtName.lower() == s.sectName.lower()):
                    skipIt=True
                    break
            if skipIt == False:
                fwrite(f,'<A NAME="%sUNSORTED"></A>' %s.sectName)
                fwrite(f,'<h2>%s elements in memory order. Total size %d. Located in %s</h2>' %(s.sectName,cvrtString2DecInt(s.size),s.location))
                if s.loadAddr != None:
                    fwrite(f,'Load address : %s</h2>' %s.loadAddr)
                fwrite(f,'<TABLE border=4 cellspacing=4 cellpadding=4 width=70%>')
                fwrite(f,'    <tr>')
                fwrite(f,'      <td>Name</td>')
                fwrite(f,'      <td align=center>Start Address</td>')
                fwrite(f,'      <td align=center>End Address</td>')
                fwrite(f,'      <td align=center>Size (HEX) </td>' )
                fwrite(f,'      <td align=center>Size (DEC) </td>' )
                fwrite(f,'      <td align=right>File or function</td>' )
                fwrite(f,'    </tr>')
                for sym in s.symbols:            
                    if sym != None:
                        org = cvrtString2DecInt(sym.address)
                        siz = cvrtString2DecInt(sym.size)
                        end = org+siz-1      
                        fwrite(f,'    <tr>')
                        fwrite(f,'      <td>%s</td>' %sym.name)
                        fwrite(f,'      <td align=center>%s</td>' %cvrtInt2HexString(org))
                        fwrite(f,'      <td align=center>%s</td>' %cvrtInt2HexString(end))
                        fwrite(f,'      <td align=center>%s</td>' %cvrtInt2HexString(siz))
                        fwrite(f,'      <td align=center>%s</td>' %cvrtInt2IntString(siz))
                        fwrite(f,'      <td align=right>%s</td>' %sym.symFile)
                        fwrite(f,'    </tr>')
                fwrite(f,'</table>')    
                fwrite(f,'<p>\n<p></p>')
        fwrite(f,'<p>\n<p></p>')
    fwrite(f,'</body></html>')
    f.close()
    print "--GeneratingDone--"

def showHelp():
    print('use mapToHtml.py inputMap <OPTIONS>')
    print('OPTIONS are option:')
    print('<skip=[set1,set2...]> =>programm will not show  show sect1, sect2, etc...')
    print('<sortedonly> =>programm will not show details section in memory order, but only sorted one')
    print('<HeapSize=valueinhex> =>Indicate the heap size value if known, in hex or dec.')
    print('<Overhead=valueinhex> =>Indicate the overhead before heap/stack ( about 96 bytes ), in hex or dec.')
    print('<out>=aComplete/relativePath> will output the result into the given file ( extension will be replaced by html). If not given, original map file will be used, with extension replaced')
    print('<help>=show help and exit, ignore all other arguments')

def Main_Entry():
    #===============================
    # Check pgrm input args
    #
    if len(sys.argv) < 2:
        showHelp()
        return
    inFile=None
    outFile=None
    skippedList=[]
    skippedList.append('Name')
    sortedOnly=False
    ramstart=0
    ramend=0
    ramsize=0
    heapsize=0
    overhead=96
    for i in range(1,len(sys.argv)):
        argStr=sys.argv[i]
        testOpt=argStr.lower()
        if(testOpt.find("help")>=0):
            showHelp()
            return
        elif(testOpt.find("out=")>=0):
            outFile=argStr.replace("out=","")
        elif(testOpt.find("sortedonly")==0):
            sortedOnly=True
        elif(testOpt.find("skip=[")>=0):
            slist=testOpt.replace("skip=[","").replace("]","")
            splitSlist = slist.split(',')
            for elmt in splitSlist:
                skippedList.append(elmt)
            sortedOnly=True
        elif(testOpt.find("heapsize=")>=0):
            val= testOpt.replace("heapsize=","")
            heapsize = cvrtString2DecInt(val)
        elif(testOpt.find("overhead=")>=0):
            val= testOpt.replace("overhead=","")
            overhead = cvrtString2DecInt(val)
        else:
            inFile=argStr
    if(inFile == None):
        showHelp()
        return
    if(outFile==None):
        dirName=os.path.dirname(inFile)
        fileNameExt=os.path.basename(inFile)
    else:
        dirName=os.path.dirname(outFile)
        fileNameExt=os.path.basename(outFile)
    fileName, fileExtension = os.path.splitext(fileNameExt)

    generatedOut=os.path.join(dirName,fileName+'.html')
    
    #analyze
    [summary,sections2] = analyzeMap(inFile)
    for sumar in summary:
        if sumar.name.lower() == 'ram':
            ramstart = int(sumar.Origin, 16)
            ramsize = int(sumar.size, 16)
            ramend = ramstart +  ramsize - 1

    sections=[]
    #sometimes, there is twice the definition.Remove dual
    for sect in sections2:
        if sect.sectName != '.heap' and sect.sectName != '.co_stack' :
            found=False
            for othersect in sections:
                if othersect.sectName == sect.sectName:
                    found=True
            if found == False:
                sections.append(sect)

    #generate
    if overhead!=0:
        overheadAddr=0
        for sect in sections:
            if sect.sectName != 'Name' and sect.sectName != '.heap' and sect.sectName != '.co_stack' :
                if int(sect.start,16)>=ramstart:
                    if int(sect.start,16)>= overheadAddr:
                        overheadAddr=int(sect.start,16)+int(sect.size,16)
        sections.append(Section("Overhead Before heap",hex(overheadAddr),hex(overhead)))
    if ramstart!=0 and ramend!=0 and ramend>ramstart:
        if heapsize == 0:
            leftforHeapAndStack=ramsize
            heapAddr = 0
            for sect in sections:
                if sect.sectName != 'Name':
                    if int(sect.start,16)>=ramstart:
                        leftforHeapAndStack = leftforHeapAndStack-int(sect.size,16)
                        if sect.sectName != '.heap' and sect.sectName != '.co_stack' :
                            if int(sect.start,16)>= heapAddr:
                                heapAddr=int(sect.start,16)+int(sect.size,16)
            sections.append(Section("heap and stack",hex(heapAddr),hex(leftforHeapAndStack)))
        elif (ramsize>heapsize):
            leftforStack=ramsize-heapsize
            heapAddr = 0
            for sect in sections:
                if sect.sectName != 'Name':
                    if int(sect.start,16)>=ramstart:
                        leftforStack = leftforStack-int(sect.size,16)
                        if sect.sectName != '.heap' and sect.sectName != '.co_stack' :
                            if int(sect.start,16)>= heapAddr:
                                heapAddr=int(sect.start,16)+int(sect.size,16)
            sections.append(Section("heap",hex(heapAddr),hex(heapsize)))
            stackAddr=heapAddr+heapsize
            sections.append(Section("stack",hex(stackAddr),hex(leftforStack)))
        else:
            print 'bad value for heapsize !!!'
        
    generateHTML(generatedOut,skippedList,sortedOnly,summary,sections)

Main_Entry()





