#!/usr/bin/python

import wx
import os
import time
import re
import sys, os
import os.path
import subprocess
import datetime
import xml.etree.ElementTree as ET
import threading 
from threading import Thread
import wx.lib.newevent

VERSION="1.0"

ID_BUTTON=100


ID_LOAD=101
ID_RELOAD=102
ID_UNLOAD=103
ID_TOOLCHAIN=104
ID_EXIT=100

ID_BUILD=200
ID_REBUILD=201
ID_CANCELBUILD=202
ID_BUILDSCRIPT=203

ID_CONFIGURE=300
ID_TOOLCHAIN=301

ID_ABOUT=400

ID_SPLITTER=500


UpgradeLog, EVT_BUILD_UPGRADE_LOG = wx.lib.newevent.NewEvent()
EndBuild, EVT_END_BUILD_DONE = wx.lib.newevent.NewEvent()

tcp=""
tcn=""
sp=""

def drawALine():
    return "__________________________________________________________________________________________________________________"

def set_toolChainPath(val):
    global tcp
    tcp=val
    print "[settings]toolChainPath="+tcp
    
def toolChainPath():
    return tcp

def set_toolChainPrefix(val):
    global tcn
    tcn=val
    print "[settings]toolChainPrefix="+tcn
    
def toolChainPrefix():
    return tcn

def set_startpath(val):
    global sp
    sp=val
    print "[settings]startpath="+sp

def startpath():
    return sp    

def readPref():
    if os.path.isfile('bspLinuxCoCoox.pref') == False:
        print '[settings]no default path'
        set_toolChainPath("")
        set_toolChainPrefix("")
        set_startpath("")
        savePref()
        return
    f = open('bspLinuxCoCoox.pref', 'r')
    llist=f.readlines()
    for el in llist:
        if el.find("toolChainPath=")==0:
            set_toolChainPath(el[len("toolChainPath="):-1])
        elif el.find("toolChainPrefix=")==0:
            set_toolChainPrefix(el[len("toolChainPrefix="):-1])
        elif el.find("startpath=")==0:    
            set_startpath(el[len("startpath="):-1])
    f.close()

def savePref():
    f = open('bspLinuxCoCoox.pref', 'w')
    f.write("toolChainPath="+toolChainPath()+"\n")
    f.write("toolChainPrefix="+toolChainPrefix()+"\n")
    f.write("startpath="+startpath()+"\n")
    f.close()
    readPref()



#projectDef is a list of string with
#[ [name1,outdir1,[define options]1,isLibrary1,outputName1,compile option1,link option1, [before build]1,[after build]1],..., [nameN,outdirN,[define options]N,isLibrary1N,outputNameN,compile optionN,link optionN, [before build]N,[after build]N]]
class MyPjMgrCtrl(wx.ListCtrl):
    def __init__(self, parent, id):
        self.parent=parent
        wx.ListCtrl.__init__(self, parent, id, style=wx.LC_REPORT)
        self.InsertColumn(0, 'Project')
        self.SetColumnWidth(0, 600)
        self.projectDef=[]
        self.curProject=-1
        self.srcListDef=[]
        self.hdListDef=[]
        self.loadedFile=""

    def doload(self,afile):
        print "[MyListCtrl] doload project"
        #ok, now unload current
        del self.projectDef[:]
        del self.srcListDef[:]
        del self.hdListDef[:]
        IROMORIGIN=0x08000000
        IROMLENGTH=0x0001EF00
        IRAMORIGIN=0x20000000
        IRAMLENGTH=0x00004000
        self.projectDef=[]
        self.curProject=-1
        self.srcListDef=[]
        self.hdListDef=[]
        self.loadedFile=afile
        self.ClearAll()
        self.InsertColumn(0, 'Project')
        self.SetColumnWidth(0, 600)
        set_startpath(os.path.dirname(os.path.realpath(afile)))

        savePref()
        tree = ET.parse(afile)
        root = tree.getroot()
        allPath=[]
        for files in root.iter('Files'):
            for afile in files.iter('File'):
                rnp=afile.get('path')
                rns=rnp.split('/')
                if afile.get('type') == '1':
                    [fileName, fileExtension]=os.path.splitext(os.path.basename(rnp))
                    if fileExtension == '.h':
                        self.hdListDef.append(rnp)
                    else:
                        self.srcListDef.append(rnp)
        for hd in self.hdListDef:
            allPath.append(os.path.dirname(hd))
        for src in self.srcListDef:
            allPath.append(os.path.dirname(src))
        allPath.sort()
        allPath=list(set(allPath))
        for target in root.iter('Target'):
            aproject=[]
            BuildOption=target.find('BuildOption')
            Compile=BuildOption.find('Compile')
            compOpt="-mcpu=cortex-m0 -mthumb -Wall -ffunction-sections -g -I"+startpath()+" "
            linkOpt="-mcpu=cortex-m0 -mthumb -g -nostartfiles -Wl,-Map=Module.map"
            
            isCurrent=False
            if target.get('isCurrent')=="1":
                isCurrent=True
            
            for opt in Compile.iter('Option'):
                if opt.get('name')=='OptimizationLevel':
                    if opt.get('value')==4:
                        compOpt=compOpt+" -Os"+" "
                        linkOpt=linkOpt+" -Os"+" "
                    else:
                        compOpt=compOpt+" -O"+opt.get('value')+" "
                        linkOpt=linkOpt+" -O"+opt.get('value')+" "
                elif opt.get('name')=='UserEditCompiler':
                    otherOpt=opt.get('value').replace(";"," ")
                    compOpt=compOpt+otherOpt+" "
            Includepaths = Compile.find('Includepaths')
            for ip in Includepaths.iter('Includepath'):
                compOpt=compOpt+" -I"+os.path.join(startpath(),ip.get('path'))
            for pt in allPath:
                compOpt=compOpt+" -I"+os.path.join(startpath(),pt)
            DefinedSymbols = Compile.find('DefinedSymbols')
            defOpt=[]
            for ds in DefinedSymbols.iter('Define'):
                compOpt=compOpt+" -D" + ds.get('name')
                defOpt.append(ds.get('name'))

            
            Link=BuildOption.find('Link')
            for opt in Link.iter('Option'):
                if opt.get('name')=='UserEditLinker':
                    otherOpt=opt.get('value').replace(";"," ")
                    linkOpt=linkOpt+otherOpt+" "
            LocateLinkFile=Link.find('LocateLinkFile')
            LocateLinkFilePath=LocateLinkFile.get('path').replace('\\','/')
            linkOpt=linkOpt+" -Wl,--gc-sections --specs=nano.specs -Wl,-T"+os.path.join(startpath(),LocateLinkFilePath)
            LinkedLibraries=Link.find('LinkedLibraries')
            for ll in LinkedLibraries.iter('Libset'):
                alibdir=os.path.join(startpath(),ll.get('dir'))
                alibdir=alibdir.replace('\\','/')
                compOpt=compOpt+" -L" + alibdir+" "
                compOpt=compOpt+" -l" + ll.get('libs')+" "
                linkOpt=linkOpt+" -L" + alibdir+" "
                linkOpt=linkOpt+" -l" + ll.get('libs')+" "
            MemoryAreas=Link.find('MemoryAreas')
            for ma in MemoryAreas.iter('Memory'):
                if ma.get('name')=='IROM1':
                    IROMLENGTH=ma.get('size')
                    IROMORIGIN=ma.get('startValue')
                if ma.get('name')=='IRAM1':
                    IRAMLENGTH=ma.get('size')
                    IRAMORIGIN=ma.get('startValue')
            isLibrary=False
            outputName='Module'
            Output=BuildOption.find('Output')
            for opt in Output.iter('Option'):
                if opt.get('name') == 'OutputFileType':
                    if opt.get('value')=='1':
                        isLibrary=True
                elif opt.get('name') == 'Name':
                    outputName= opt.get('value')


            beforeBuild=[]
            backupLD="cp %s %s.backup" %(os.path.join(startpath(),LocateLinkFilePath),os.path.join(startpath(),LocateLinkFilePath))
            sedROM="sed -i 's/rom (rx).*/rom (rx)  : ORIGIN = %s, LENGTH = %s/' %s" %(IROMORIGIN,IROMLENGTH,os.path.join(startpath(),LocateLinkFilePath))
            sedRAM="sed -i 's/ram (rwx).*/ram (rwx) : ORIGIN = %s, LENGTH = %s/' %s" %(IRAMORIGIN,IRAMLENGTH,os.path.join(startpath(),LocateLinkFilePath))
            beforeBuild.append(backupLD)
            beforeBuild.append(sedROM)
            beforeBuild.append(sedRAM)
            restoreLD="mv %s.backup %s" %(os.path.join(startpath(),LocateLinkFilePath),os.path.join(startpath(),LocateLinkFilePath))
            afterGCC=[]
            afterGCC.append(restoreLD)
            afterBuild=[]            
            User=BuildOption.find('User')
            for UserRun in User.iter('UserRun'):
                if UserRun.get('type')=='Before' and UserRun.get('checked')=='1':
                    exe=UserRun.get('value')
                    exe=exe.replace('${project.path}',startpath())
                    exe=exe.replace('\\','/')
                    exe=exe.replace('.bat','.sh')
                    exe=exe.replace('.exe','')                    
                    exe=exe.replace('copy','cp -f') 
                    beforeBuild.append(exe)
                elif UserRun.get('type')=='After' and UserRun.get('checked')=='1':
                    exe=UserRun.get('value')
                    exe=exe.replace('${project.path}',startpath())
                    exe=exe.replace('\\','/')
                    exe=exe.replace('.bat','.sh')
                    exe=exe.replace('.exe','')                    
                    exe=exe.replace('copy','cp -f') 
                    afterBuild.append(exe)
            
            #[name1,outdir1,[define options]1,isLibrary1,outputName1,compile option1,link option1, [before build]1,[aftergcc]1,[after build]1,srclist]
            aproject.append(target.get('name'))
            aproject.append(os.path.join(startpath(),target.get('name')))
            aproject.append(defOpt)
            aproject.append(isLibrary)
            aproject.append(outputName)
            aproject.append(compOpt)
            aproject.append(linkOpt)
            aproject.append(beforeBuild)
            aproject.append(afterGCC)
            aproject.append(afterBuild)
            if isCurrent==True:
                self.projectDef.insert(0,aproject)
            else:
                self.projectDef.append(aproject)
            srclist=[]
            for srcfile in self.srcListDef:
                srclist.append(os.path.join(startpath(),srcfile))
            aproject.append(srclist)
        return True

    def load(self):
        print "[MyListCtrl] load project"

        openFileDialog = wx.FileDialog(self.parent, "Open", startpath(), "", "CoIDE project (*.coproj)|*.coproj", wx.FD_OPEN | wx.FD_FILE_MUST_EXIST)
        if openFileDialog.ShowModal() == wx.ID_CANCEL:
            openFileDialog.Destroy()
            return False     # the user changed idea...
        afile=openFileDialog.GetPath()
        openFileDialog.Destroy()
        if not afile:
            return False
        return self.doload(afile)

    def getload(self):
        print "[MyListCtrl] getload::",self.loadedFile
        return self.loadedFile
        
    def unload(self):
        print "[MyListCtrl] unlaod current"
        del self.projectDef[:]
        del self.srcListDef[:]
        del self.hdListDef[:]
        self.projectDef=[]
        self.curProject=-1
        self.srcListDef=[]
        self.hdListDef=[]
        self.loadedFile=""
        self.ClearAll()
        self.InsertColumn(0, 'Project')
        self.SetColumnWidth(0, 600)

    def getSubP(self):
        return self.projectDef

    def setSubProjectList(self,suprojectIndex):
        print "[MyListCtrl]update sub pj"
        self.ClearAll()
        self.InsertColumn(0, 'Project')
        self.SetColumnWidth(0, 600)
        self.curProject=suprojectIndex
        j=1
        self.InsertStringItem(j,drawALine())
        j=j+1 
        self.InsertStringItem(j, "   " +os.path.basename(self.loadedFile) + " Loaded")
        j=j+1 
        self.InsertStringItem(j,drawALine())
        j=j+1 
        self.InsertStringItem(j, "   OPTIONS")
        j=j+1 
        self.InsertStringItem(j,drawALine())
        j=j+1    
        for defopt in self.projectDef[self.curProject][2]:
            self.InsertStringItem(j, defopt)
            j=j+1 
        self.InsertStringItem(j, " ")
        j=j+1    
        self.InsertStringItem(j,drawALine())
        j=j+1         
        self.InsertStringItem(j, "   C FILES")
        j=j+1
        self.InsertStringItem(j,drawALine())
        j=j+1    
        for afile in self.srcListDef :
            self.InsertStringItem(j, "+ - - - - - - "+afile)
            j=j+1 
        self.InsertStringItem(j, " ")
        j=j+1 
        self.InsertStringItem(j,drawALine())
        j=j+1      
        self.InsertStringItem(j, "   HEADER FILES")
        j=j+1  
        self.InsertStringItem(j,drawALine())
        j=j+1          
        for afile in self.hdListDef:
            self.InsertStringItem(j, "+ - - - - - - "+afile)
            j=j+1 
        self.InsertStringItem(j, " ")
        j=j+1 

    #[name1,outdir1,[define options]1,isLibrary1,outputName1,compile option1,link option1, [before build]1,[after build]1]
    def generateBuildScript(self,afile,clean=False):
        print "[MyListCtrl]generate build script " +afile
        blib=self.projectDef[self.curProject][3]
        pname=self.projectDef[self.curProject][4]
        f=open(afile,"w")
        f.write("#!/bin/sh\n")
        f.write("STARTTIME=$(date +%s)\n")
        if len(self.projectDef[self.curProject][7])>0:
            for scr in self.projectDef[self.curProject][7]:
                f.write(scr+'\n')
        if  blib== True:
            f.write('echo "Compiling ' +self.projectDef[self.curProject][0] +' Library."\n')
        else:
            f.write('echo "Compiling ' +self.projectDef[self.curProject][0] +' Binary."\n')
        f.write('echo "creating output directories:"\n')
        f.write('echo "[mkdir] ' + self.projectDef[self.curProject][1]+'/Debug/bin"\n')
        f.write('echo "[mkdir] ' + self.projectDef[self.curProject][1]+'/Debug/obj"\n')
        f.write('mkdir -p ' + self.projectDef[self.curProject][1]+'/Debug/obj\n')
        f.write('mkdir -p ' + self.projectDef[self.curProject][1]+'/Debug/bin\n')
        f.write('mkdir -p ' + self.projectDef[self.curProject][1]+'/Debug/obj\n')
        f.write('rm -f ' + self.projectDef[self.curProject][1]+'/Debug/bin/*\n')
        f.write('rm -f ' + self.projectDef[self.curProject][1]+'/Debug/obj/*\n')        
        f.write('cd ' +self.projectDef[self.curProject][1]+'/Debug/obj\n')
        f.write('echo "%d total files to be compiled"\n' %len(self.srcListDef))        
        f.write('echo "'+toolChainPrefix()+'gcc '+self.projectDef[self.curProject][5]+' -c files..."\n')
        f.write(os.path.join(toolChainPath(),toolChainPrefix())+'gcc '+self.projectDef[self.curProject][5] +' -c')
        f.write(' \\\n')
        for srcfile in self.projectDef[self.curProject][10][0:-1]:
            f.write(' ' + srcfile + ' \\\n')
        f.write(' ' +self.projectDef[self.curProject][10][-1] + '\n')
        f.write('\n')
        f.write('cd ' +self.projectDef[self.curProject][1]+'/Debug/bin/\n')
        if  blib== True:
            f.write('echo "Generating Library."\n')
            f.write(os.path.join(toolChainPath(),toolChainPrefix())+'ar rvs lib'+pname+'.a ')
            for srcfile in self.srcListDef:
                [fileName, fileExtension]=os.path.splitext(os.path.basename(srcfile))
                f.write(' '+os.path.join('../obj',fileName+'.o'))
            f.write('\n')
            f.write('if [ $? -ne 0 ]\n')
            f.write('then\n')
            f.write('\tENDTIME=$(date +%s)\n')
            f.write('\t'+'echo ""\n')
            f.write('\t'+'echo ""\n')
            f.write('\t'+'echo ""\n')
            f.write('\techo "BUILD FAILED in $(($ENDTIME - $STARTTIME)) seconds..."\n')
            f.write('else\n')
        else:
            f.write('echo "Linking Binary."\n')
            f.write('echo "['+toolChainPrefix()+'gcc] -o '+pname+'.elf files... '+self.projectDef[self.curProject][6]+'"\n')
            f.write(os.path.join(toolChainPath(),toolChainPrefix())+'gcc -o '+pname+'.elf ')
            for srcfile in self.srcListDef:
                [fileName, fileExtension]=os.path.splitext(os.path.basename(srcfile))
                f.write(' '+os.path.join('../obj',fileName+'.o'))
            f.write(' '+self.projectDef[self.curProject][6]+'\n')
            f.write('if [ $? -ne 0 ]\n')
            f.write('then\n')
            f.write('\tENDTIME=$(date +%s)\n')
            f.write('\t'+'echo ""\n')
            f.write('\t'+'echo ""\n')
            f.write('\t'+'echo ""\n')
            f.write('\techo "BUILD FAILED !!!!![$(($ENDTIME - $STARTTIME)) seconds elapsed]"\n')
            f.write('\t'+'echo ""\n')
            f.write('\t'+'echo ""\n')
            f.write('\t'+'echo ""\n')
            f.write('else\n')
            f.write('\techo "Generating hex and bin output:"\n')
            f.write('\t'+os.path.join(toolChainPath(),toolChainPrefix())+'objcopy -O ihex "' + pname+'.elf" "'+pname+'.hex"\n')
            f.write('\t'+os.path.join(toolChainPath(),toolChainPrefix())+'objcopy -O binary "' + pname+'.elf" "'+pname+'.bin"\n')
            f.write('\t'+'echo "Program Size:"\n')
            f.write('\t'+os.path.join(toolChainPath(),toolChainPrefix())+'size Module.elf\n')
        f.write('\t'+'cd ' + startpath()+'\n')
        if len(self.projectDef[self.curProject][9])>0:
            for scr in self.projectDef[self.curProject][9]:
                f.write('\t'+scr+'\n')
        f.write('\t'+'ENDTIME=$(date +%s)\n')
        f.write('\t'+'echo ""\n')
        f.write('\t'+'echo ""\n')
        f.write('\t'+'echo ""\n')
        f.write('\t'+'echo "BUILD SUCCESSFULL [$(($ENDTIME - $STARTTIME)) seconds elapsed]"\n')
        f.write('\t'+'echo ""\n')
        f.write('\t'+'echo ""\n')
        f.write('\t'+'echo ""\n')
        f.write('fi\n')
        if len(self.projectDef[self.curProject][8])>0:
            for scr in self.projectDef[self.curProject][8]:
                f.write(scr+'\n')
        f.write('cd ' + startpath()+'\n')
        f.close()        
        os.chmod(afile, 0777)
        


class MyLogCtrl(wx.TextCtrl):
    def __init__(self, parent, id):
        wx.TextCtrl.__init__(self, parent, id, style=wx.TE_MULTILINE | wx.HSCROLL)
        self.Clear()


class MainWindow(wx.Frame):
    def __init__(self, parent, id, title):
        wx.Frame.__init__(self, parent, -1, title)
        
        self.splitter = wx.SplitterWindow(self, ID_SPLITTER, style=wx.SP_BORDER)
        self.splitter.SetMinimumPaneSize(50)
        
        self.showProject = MyPjMgrCtrl(self.splitter, -1)
        self.showLog = MyLogCtrl(self.splitter, -1)
        self.splitter.SplitVertically(self.showProject, self.showLog,-200)
       
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_SPLITTER_DCLICK, self.OnDoubleClick, id=ID_SPLITTER)

        self.filemenu= wx.Menu()
        fe=self.filemenu.Append(ID_EXIT,"E&xit"," Terminate the program")
        self.Bind(wx.EVT_MENU, self.OnExit, fe)
        self.helpmenu = wx.Menu()
        fab=self.helpmenu.Append(ID_ABOUT,"A&bout"," About this")
        self.Bind(wx.EVT_MENU, self.onAbout, fab)
        self.building=False
        self.cancel=0

        self.menuBar = wx.MenuBar()
        self.menuBar.Append(self.filemenu,"&File")
        self.menuBar.Append(self.helpmenu, "&Help")
        self.SetMenuBar(self.menuBar)
        self.Bind(wx.EVT_MENU, self.OnExit, id=ID_EXIT)

        self.sizer2 = wx.BoxSizer(wx.HORIZONTAL)

        self.comboBox=wx.ComboBox(self, -1)
        self.Bind(wx.EVT_COMBOBOX, self.OnSelectSubProject)
        self.buttonLOAD = wx.Button(self, ID_LOAD, "Load")
        self.Bind(wx.EVT_BUTTON, self.OnLoad, id=ID_LOAD)
        self.buttonUNLOAD = wx.Button(self, ID_UNLOAD, "Unload")
        self.Bind(wx.EVT_BUTTON, self.OnUnload, id=ID_UNLOAD)
        self.buttonRELOAD = wx.Button(self, ID_RELOAD, "Reload")
        self.Bind(wx.EVT_BUTTON, self.OnReload, id=ID_RELOAD)
        self.buttonTOOLCHAIN = wx.Button(self, ID_TOOLCHAIN, "Toolchain")
        self.Bind(wx.EVT_BUTTON, self.OnToolchain, id=ID_TOOLCHAIN)
        self.buttonCONFIGURE = wx.Button(self, ID_CONFIGURE, "Configure")
        self.Bind(wx.EVT_BUTTON, self.OnConfigure, id=ID_CONFIGURE)
        self.buttonBUILD = wx.Button(self, ID_BUILD, "Build")
        self.Bind(wx.EVT_BUTTON, self.OnBuild, id=ID_BUILD)
        self.buttonREBUILD = wx.Button(self, ID_REBUILD, "Rebuild")
        self.Bind(wx.EVT_BUTTON, self.OnRebuild, id=ID_REBUILD)
        self.buttonCANCEL = wx.Button(self, ID_CANCELBUILD, "Cancel")
        self.Bind(wx.EVT_BUTTON, self.OnCancel, id=ID_CANCELBUILD)
        self.buttonBUILDSCRIPT = wx.Button(self, ID_BUILDSCRIPT, "Generate build script")
        self.Bind(wx.EVT_BUTTON, self.OnGenerateBuildScript, id=ID_BUILDSCRIPT)
        
        self.buttonEXIT = wx.Button(self, ID_EXIT, "Quit")
        self.Bind(wx.EVT_BUTTON, self.OnExit, id=ID_EXIT)

        self.sizer2.Add(self.comboBox, 2, wx.EXPAND)
        self.sizer2.Add(self.buttonLOAD, 1, wx.EXPAND)
        self.sizer2.Add(self.buttonUNLOAD, 1, wx.EXPAND)
        self.sizer2.Add(self.buttonRELOAD, 1, wx.EXPAND)
        self.sizer2.Add(self.buttonTOOLCHAIN, 1, wx.EXPAND)
        self.sizer2.Add(self.buttonCONFIGURE, 1, wx.EXPAND)
        self.sizer2.Add(self.buttonBUILD, 1, wx.EXPAND)
        self.sizer2.Add(self.buttonREBUILD, 1, wx.EXPAND)
        self.sizer2.Add(self.buttonCANCEL, 1, wx.EXPAND)
        self.sizer2.Add(self.buttonBUILDSCRIPT, 1, wx.EXPAND)
        self.sizer2.Add(self.buttonEXIT, 1, wx.EXPAND)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.sizer2,0,wx.EXPAND)
        self.sizer.Add(self.splitter,1,wx.EXPAND)        
        self.SetSizer(self.sizer)

        size = wx.DisplaySize()
        self.SetSize(size)

        self.sb = self.CreateStatusBar()
        self.sb.SetStatusText(os.getcwd())
        self.Center()
        self.Show(True)
        self.nothingLoaded()
        
        self.Bind(EVT_BUILD_UPGRADE_LOG, self.upgradeLog)
        self.Bind(EVT_END_BUILD_DONE, self.buildDone)

    def onAbout(self, event):
        dlg = wx.MessageDialog(self, "PY Tool that understand CooCox IDE project.\n Let people compile their CoIDE project on Ubuntu.\nVersion " +VERSION,"About",wx.OK|wx.ICON_EXCLAMATION)
        dlg.ShowModal()
        dlg.Destroy()

    def greyOutAll(self,grey=True):
        if grey==True:
            self.cancel=0
            self.building=True
            self.buttonLOAD.Disable()
            self.buttonUNLOAD.Disable()
            self.buttonRELOAD.Disable()
            self.buttonTOOLCHAIN.Disable()
            self.buttonCONFIGURE.Disable()        
            self.buttonBUILD.Disable()
            self.buttonCANCEL.Enable()
            self.buttonREBUILD.Disable()
            self.buttonBUILDSCRIPT.Disable()
            self.buttonEXIT.Enable()
        else:
            self.building=False
            self.buttonLOAD.Enable()
            self.buttonUNLOAD.Enable()
            self.buttonRELOAD.Enable()
            self.buttonTOOLCHAIN.Enable()
            self.buttonCONFIGURE.Enable()        
            self.buttonBUILD.Enable()
            self.buttonREBUILD.Enable()
            self.buttonCANCEL.Disable()
            self.buttonBUILDSCRIPT.Enable()
            self.buttonEXIT.Enable()

    def nothingLoaded(self):
        self.comboBox.Disable()
        self.comboBox.Clear()
        self.buttonLOAD.Enable()
        self.buttonUNLOAD.Disable()
        self.buttonRELOAD.Disable()
        self.buttonTOOLCHAIN.Enable()
        self.buttonCONFIGURE.Disable()        
        self.buttonBUILD.Disable()
        self.buttonREBUILD.Disable()
        self.buttonCANCEL.Disable()
        self.buttonBUILDSCRIPT.Disable()
        self.buttonEXIT.Enable()
        self.projectloaded=False
        
    def loaded(self):
        self.buttonLOAD.Enable()
        self.buttonUNLOAD.Enable()
        self.buttonRELOAD.Enable()
        self.buttonTOOLCHAIN.Enable()
        self.buttonCONFIGURE.Enable()        
        self.buttonBUILD.Enable()
        self.buttonREBUILD.Enable()
        self.buttonBUILDSCRIPT.Enable()
        self.buttonEXIT.Enable()
        plist = self.showProject.getSubP()
        self.comboBox.Clear()
        for p in plist:
            self.comboBox.Append(p[0])
        self.comboBox.SetSelection(0)
        self.comboBox.Enable()
        self.showProject.setSubProjectList(0)
        self.projectloaded=True
    
    def OnDoubleClick(self, event):
        size =  self.GetSize()
        self.splitter.SetSashPosition(size.x / 2)

    def OnSize(self, event):
        size = self.GetSize()
        self.splitter.SetSashPosition(size.x / 2)
        self.sb.SetStatusText(os.getcwd())
        event.Skip()

    def OnLoad(self, event):
        if not toolChainPath():
            wx.MessageBox("Please select toolchain first")
            return
        if not toolChainPrefix():
            wx.MessageBox("Please select toolchain first")
            return
        self.showLog.AppendText("Loading project....")
        if self.showProject.load()==True:
            self.loaded()
            self.showLog.AppendText("done\n")
        else:
            self.showLog.AppendText("aborted\n")

    def OnUnload(self, event):
        self.showLog.AppendText("Project unloaded\n")
        self.showProject.unload()
        self.nothingLoaded()

    def OnReload(self,event):
        self.showLog.AppendText("Project reloaded\n")
        loaded=self.showProject.getload()
        self.showProject.unload()
        self.showProject.doload(loaded)  
        if self.showProject.doload(loaded)==True:
            self.loaded()
            self.showLog.AppendText("done\n")
        else:
            self.showLog.AppendText("aborted\n")      

    def OnSelectSubProject(self, event):
        self.showLog.AppendText("Changing sub project\n")
        self.showProject.setSubProjectList(self.comboBox.GetSelection())

    def OnToolchain(self, event):
        self.showLog.AppendText("Changing toolchain path...")
        openFileDialog = wx.FileDialog(self, "Select the gcc binary to use", toolChainPath(),"",  "gcc binary|*gcc*", wx.FD_OPEN)
        if openFileDialog.ShowModal() == wx.ID_CANCEL:
            openFileDialog.Destroy()
            self.showLog.AppendText("aborted\n")
            return False
        #check for 
        set_toolChainPath(os.path.dirname(os.path.realpath(openFileDialog.GetPath())))
        prefix=(os.path.basename(openFileDialog.GetPath())).split('gcc')
        set_toolChainPrefix(prefix[0])
        savePref()
        openFileDialog.Destroy()
        self.showLog.AppendText("done\n")

    def OnConfigure(self, event):
        if self.projectloaded==False:
            return
        self.showLog.AppendText("Configure project\n")
        dlg = wx.MessageDialog(self, "This feature is not yet implemented.\nBut you can still edit the xml " + os.path.basename(self.showProject.getload())+ " manually\n","Sorry...",wx.OK|wx.ICON_EXCLAMATION)
        dlg.ShowModal()
        dlg.Destroy()

    def generateBuildScript(self,afile,clean=False):
        self.showLog.AppendText("Generating build script...")
        self.showProject.generateBuildScript(afile,clean)
        self.showLog.AppendText("done\n")

    def upgradeLog(self,event):
        self.showLog.AppendText(event.attr1+"\n")

    def buildDone(self,event):
        self.showLog.AppendText(event.attr1+"\n")
        self.greyOutAll(False)

    def executeScript(self,usedScript,typeBuild):        
        j=1     
        if os.path.isfile(usedScript) == False:
            newEvent = EndBuild(attr1=usedScript+ " do not exist.\nCannot "+typeBuild+" !!!!!")
            wx.PostEvent(self, newEvent)
            return
        newEvent = UpgradeLog(attr1="Start " + typeBuild + "...")
        wx.PostEvent(self, newEvent)
        #GIVE EXECUTE RIGHT
        os.chmod(usedScript, 0777)
        sub_process = subprocess.Popen(usedScript,shell=True,
                     stdout=subprocess.PIPE, 
                     stderr=subprocess.STDOUT)
        for line in iter(sub_process.stdout.readline, ''):
            line = line.replace('\r', '').replace('\n', '')
            newEvent = UpgradeLog(attr1=line)
            wx.PostEvent(self, newEvent)
            if self.cancel != 0:
                break
        if os.path.isfile(usedScript) == True:
            os.remove(usedScript)
        newEvent = EndBuild(attr1=typeBuild+" process end")
        wx.PostEvent(self, newEvent)
        self.cancel=2

    def OnBuild(self, event):
        if self.projectloaded==False:
            return
        self.showLog.Clear()
        self.greyOutAll()
        self.building=True
        self.showLog.AppendText("Build process start.\n")
        self.generateBuildScript("/tmp/build.sh")
        t = Thread(target=self.executeScript, args=(["/tmp/build.sh","Build"]))
        t.start()   

    def OnCancel(self, event):
        if self.building==False:
            return
        if self.cancel==0:
            newEvent = UpgradeLog(attr1="\n\n\n\t\tCANCELLING....\n\n\n")
            wx.PostEvent(self, newEvent)
            self.cancel=1

    def OnRebuild(self, event):
        if self.projectloaded==False:
            return
        self.showLog.Clear()
        self.greyOutAll()
        self.showLog.AppendText("Rebuild process start.\n")
        self.generateBuildScript("/tmp/rebuild.sh",True)
        t = Thread(target=self.executeScript, args=(["/tmp/rebuild.sh","Rebuild"]))
        t.start()       

    def OnGenerateBuildScript(self, event):
        if self.projectloaded==False:
            return
        self.greyOutAll()
        self.showLog.AppendText("Generate build script...")
        openFileDialog = wx.FileDialog(self, "Save buildscript", startpath(), "", "shell (*.sh)|*.sh", wx.FD_SAVE|wx.FD_OVERWRITE_PROMPT)
        if openFileDialog.ShowModal() == wx.ID_CANCEL:
            openFileDialog.Destroy()
            self.greyOutAll(False)
            self.showLog.AppendText("aborted...\n")
            return False     # the user changed idea...
        afile=openFileDialog.GetPath()
        [fileName, fileExtension]=os.path.splitext(afile)
        if fileExtension != '.sh':
            afile=afile+'.sh'
        set_startpath(os.path.dirname(os.path.realpath(afile)))
        dlg = wx.MessageDialog(self, "Script generated will systematically\n clean before build", "Full rebuild ?", wx.YES_NO | wx.ICON_QUESTION)
        fullRebuild = dlg.ShowModal() == wx.ID_YES
        dlg.Destroy()
        openFileDialog.Destroy()
        self.generateBuildScript(afile,fullRebuild)
        self.greyOutAll(False)
        self.showLog.AppendText("Done\n")
        self.showLog.AppendText("Script available in " +afile +"\n")

    def OnExit(self,e):
        if self.building==True:
            if self.cancel==1:
                time.sleep(1)
                wx.PostEvent(self,e)
                return
            elif self.cancel==0:
                self.cancel=1
                self.showLog.AppendText("\n\n\nEXITING......\n\n\n")
                wx.PostEvent(self,e)
                return
        self.Close(True)

def checkPackage(pnameList):
    ins=[]
    for el in pnameList:
        ret=subprocess.Popen("dpkg -l | grep " + el, shell=True, stdout=subprocess.PIPE).stdout.read()
        if not ret:
            ins.append(el)
    if len(ins)>0:
        print "\n\n\nFollowing package(s) not installed:"
        for el in ins:
            print "-" +el
        print "Please use sudo apt-get install ... to install them\n\n\n"
        exit(1)

#check first if package are installed
checkPackage(["python-wxgtk","python-wxtool","libusb-1.0-0","libusb-1.0-0-dev"])
readPref()
app = wx.App(0)
MainWindow(None, -1, 'Bespoon CoIDE wrapper for ubuntu.')
app.MainLoop()
