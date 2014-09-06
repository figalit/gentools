from optparse import OptionParser
from subprocess import Popen
import subprocess
import tempfile
import time
import os
import sys

def striplin(l) : return l.replace('\n','').strip()

class CmdList:
    
    def __init__(self, fn=None):
        self.lCmds = []
        self.lCmdsValid = []
        self.lGroups = []
        self.mIdcsToGroup = []
        self.mGroupToIdcs = {}
        if fn: self.load(fn, append = False)

    def parseGroupDirective(self, l):
        if len(l)>4 and l[:4]=='####':
            (anName, anValue) = (l[4:].split('#')[0], l[4:].split('#')[1])
            if anName=='grpname': return (True, anValue)
        return False       

    def load(self, fn, append=False):
        self.lastFnLoaded = fn
        if not append:
            self.lCmds = [] 
            self.lCmdsValid = []   
            self.mIdcsToGroup = []
            self.mGroupToIdcs = {}
            self.lGroups = []
        
        curGrp = None
        self.mGroupToIdcs[ None ] = []
        for l in open(fn,'r'):
            l=striplin(l)
            parsegn = self.parseGroupDirective(l)
            if parsegn:
                grpName = parsegn[1]
                if grpName in self.lGroups:
                    grpName='%s_%d_'%(grpName,0)
                    grpNameUniq=1
                    while grpName in self.lGroups:
                        grpName='%s_%d_'%('_'.join(grpName.split('_')[:-2]),grpNameUniq)
                        grpNameUniq+=1
                self.lGroups += [grpName]
                curGrp = grpName
                self.mGroupToIdcs[ curGrp ] = []
                
                continue
                        
            elif l[0]=='#': 
                self.lCmds += [l]
                self.lCmdsValid += [False]
                self.mIdcsToGroup += [None]
                self.mGroupToIdcs[ curGrp ].append( len(self.lCmds)-1 )
            else:
                self.lCmds += [l]
                self.lCmdsValid += [True]
                self.mIdcsToGroup += [curGrp]
                self.mGroupToIdcs[ curGrp ].append( len(self.lCmds)-1 )
              
                                           
    def regroupNper(self, nper,baseName='group'):
        self.lGroups = []
        self.mGroupToIdcs = {}
        
        curgi = 0
        curgsize = 0
        curgname = '%s_%d_'%(baseName,curgi)
        self.lGroups += [curgname]
        self.mGroupToIdcs[curgname] = []
        for ic in xrange(len(self.lCmds)):
            if self.lCmdsValid[ic]:
                if curgsize==nper:
                    curgi += 1
                    curgname = '%s_%d_'%(baseName,curgi)
                    self.lGroups += [curgname]
                    self.mGroupToIdcs[curgname] = []
                    curgsize = 0
                
                self.mIdcsToGroup[ic] = curgname
                self.mGroupToIdcs[ curgname ] += [ic]
                curgsize+=1
                                
    # ungrouped commands are saved at top.
    def save(self, fn=None):
        fil = fn and open(fn, 'w') or open(self.lastFnLoaded,'w')
        lastGroup = None
        for ic in xrange(len(self.mIdcsToGroup)):
            if lastGroup<>self.mIdcsToGroup[ic]:
                fil.write('####grpname#%s\n'%self.mIdcsToGroup[ic])
                lastGroup = self.mIdcsToGroup[ic]
                                
            fil.write('%s\n'%self.lCmds[ic])
            
        fil.close()
    
    def countCommands(self):
        return len([0]+[ic for ic in xrange(len(self.lCmds)) if self.lCmdsValid[ic]]) - 1
    def countGroups(self):
        return len(self.lGroups)
        
    def logPath(self):
        (y,m,d)=time.localtime(time.time())[:3]
        logPath = os.path.join(os.path.join(tempPath, 'cluster.logs'),
                                            '%(y)d%(m)02d%(d)02d'%{'y':y,'m':m,'d':d})        
        
        if not os.path.exists(logPath):
            os.makedirs(logPath)
            
        return logPath
    
    def submitMkPath(self):
        (y,m,d)=time.localtime(time.time())[:3]
        scriptPath = os.path.join(os.path.join(tempPath, 'cluster.temp'),
                                            '%(y)d%(m)02d%(d)02d'%{'y':y,'m':m,'d':d})
        
        if not os.path.exists(scriptPath):
            os.makedirs(scriptPath)
            
        return scriptPath
       
    def submitlocal(self, jobName):
        logPath = self.logPath()
                                                               
        for grp in self.lGroups + (None in self.mIdcsToGroup and [None] or []):
            grpFullname = '%s%s'%(jobName, grp or '')
            
            lCmdSrcs = [self.lCmds[ic] for ic in self.mGroupToIdcs[grp] if self.lCmdsValid[ic]]
            
            filOut = open(os.path.join(logPath, '%s.local'%grpFullname), 'w')
            
            for cmd in lCmdSrcs:
                print 'run local: %s'%cmd
                
                proc = Popen( '%s 2>&1'%cmd, stdout=subprocess.PIPE, shell=True )
                os.waitpid( proc.pid, 0 )
                               
                for l in proc.stdout:
                    filOut.write(l)
                    
                filOut.flush()
                
            filOut.close()    
        
    def submit_using_tasks(self, jobName, priority, debug=False):
        logPath = self.logPath()
        scrPath = self.submitMkPath()
        
        lTaskFiles = []
        nCmdsTotal = 0
        
        for grp in self.lGroups + (None in self.mIdcsToGroup and [None] or []):
            grpFullname = '%s%s'%(jobName,grp or '')
           
            fnJob = tempfile.NamedTemporaryFile(
                mode='w', prefix=grpFullname, dir=scrPath).name
            filJob = open(fnJob,'w')
                        
            cmdStrs = []
            
            for cmd in [self.lCmds[ic] for ic in self.mGroupToIdcs[grp] if self.lCmdsValid[ic]]:
                #cmdStrs += ['echo "%s"; %s'%(cmd.replace('"','\\"'),cmd)]
                cmdStrs += [cmd]
                nCmdsTotal += 1                

            filJob.write( '\n'.join(cmdStrs) )
            filJob.close()
            
            lTaskFiles.append( filJob )
        
        fnAll = tempfile.NamedTemporaryFile(
            mode='w', prefix='taskified_%s'%grpFullname, dir=scrPath).name        
        filAll = open(fnAll,'w')
        filAll.write('%s\n'%('\n'.join(['sh %s'%t.name for t in lTaskFiles])))
        filAll.close()  
        print fnAll          
            
        fnTask = tempfile.NamedTemporaryFile(
            mode='w', prefix='taskified_%s'%grpFullname, dir=scrPath).name        
        filTask = open(fnTask,'w')            
        filTask.write(
"""#!/bin/bash%(optional_q)s
#$ -j y
#$ -o %(logPath)s
#$ -N %(grpFullname)s
#$ -p %(prio)d
#$ -cwd
#$ -V
if [[ -z "$SGE_TASK_ID" ]] ; then
    SGE_TASK_ID=1
fi
echo "task $SGE_TASK_ID"
CMD=`head -n $SGE_TASK_ID %(fnAll)s | tail -n 1`
echo "cmd: $CMD"
${CMD}
"""%{  'optional_q': o.queues and '\n#$ -q %s'%o.queues or '',
            'logPath':logPath,
                'prio': priority,
                'fnAll':fnAll,
            'grpFullname':grpFullname } )
        filTask.flush()
                
        print 'submitting taskset %s'%''.join(filTask.name)

        qsubCmd = '%s'%('qsub -S /bin/bash %s -t 1-%d %s > /dev/null'%(
            o.nThreads>0 and '-l thread=%d'%o.nThreads or '',
            len(lTaskFiles),
            filTask.name))

        
        if debug:
            print qsubCmd
            os.system('cat %s'%filTask.name)
        else:           
            print qsubCmd
            os.system( qsubCmd )
            
        filTask.close()
         
    def submit(self, jobName, priority, debug=False):
        logPath = self.logPath()
        scrPath = self.submitMkPath()
        
        for grp in self.lGroups + (None in self.mIdcsToGroup and [None] or []):
            grpFullname = '%s%s'%(jobName,grp or '')
           
            fnJob = tempfile.NamedTemporaryFile(
                mode='w', prefix=grpFullname,dir=scrPath).name
                
            filJob=open(fnJob,'w')
                        
            cmdStrs = []
                        
            for cmd in [self.lCmds[ic] for ic in self.mGroupToIdcs[grp] if self.lCmdsValid[ic]]:
                #cmdStrs += ['echo "%s"'%(cmd.replace('"','\\"'))]
                cmdStrs += [cmd]
       
# "#$ -S /bin/bash"
            
            filJob.write(
"""#!/bin/bash%(optional_q)s
#$ -j y
#$ -o %(logPath)s
#$ -N %(grpFullname)s
#$ -p %(prio)d
#$ -cwd
#$ -V
%(cmds)s
"""%{    'optional_q': o.queues and '\n#$ -q %s'%o.queues or '',
               'logPath':logPath,
                 'prio': priority,
                'grpFullname':grpFullname, 
                'cmds':'\n'.join( cmdStrs ) })
            filJob.flush()
            
            print 'submitting %s'%''.join(filJob.name)
            
            if debug:
                os.system('cat %s'%filJob.name)
            else:
                os.system('qsub -S /bin/bash %s %s > /dev/null'%(
                   o.nThreads > 0 and '-l thread=%d'%o.nThreads or '',
                   filJob.name,
                   ) )
            
            filJob.close()
        
if __name__=='__main__':
    help_string = """
        clustutil.py
            
        Manages cluster job submission.  Reads lists of commands from text files,
        optionally separates into groups, submits to cluster, and directs stderr/out
        to specified locations.
           
        OPTIONS: 
    
            -i --inFn:        input list of commands, one per line
            -o --outFn:       optional.  output command list, for instance, after grouping.  
            -g --groupSize [int]:    optional.  directs splitting of input tasks into sets of specified size
               --groupName [string] :  optional.  if input tasks spilt into groups, specifies group name prefix
            
            SUBMISSION            
            --submit          Submit job to cluster.
                              if no grouping specified, submit tasks as 
                                a single job to be completed serially.
                              if grouping applied, submits each subgroup of tasks
                                each as a single job to be completed in parallel
            --submit_tasks    Submit job to cluster as a task list.
                               if no grouping specified, task list has one item per input
                               if grouping applied, each task corresponds to a task subgroup
            --submit_local     Run jobs in a subprocess
            
            OTHER
            --jobname [string] Name of the cluster job.  Defaults to the basename of the input filename
            --priority [int]
            --nThreads[int]    How many processor 'threads' to reserve for each cluster job
            --queues [string]  Comma-separated list of queues to submit to
            --dryrun           Make submission scripts but do not submit
            
            --tempPath [string]  Specify a base location for storage of submission scripts and 
                              job stdout/err output.  Submission scripts are stored in a subdirectory 
                              called "cluster.temp" from this directory and output is stored in a 
                              subdir called "cluster.logs"
    """
    
    def help():
        print help_string
        sys.exit(0)  
    
    opts=OptionParser()
    opts.add_option('-i','--inFn',dest='inFn')         # task list input
    opts.add_option('-o','--outFn',dest='outFn')       # task list output (optional
        
    # if grouping a list of commands into individual tasks
    opts.add_option('-g','--groupSize',type=int,default=-1,dest='groupNPer')    # note: -g 1e9 to put all in one group.
    opts.add_option('','--groupName',default='group',dest='groupName')

    opts.add_option('','--submit',default=False,action='store_true',dest='submit')   
    opts.add_option('','--submit_tasks',default=False,action='store_true',dest='submitTaskify')
    opts.add_option('','--submit_local',default=False,action='store_true',dest='submitLocal')
    
    opts.add_option('','--jobname',default=None,dest='jobname')
    
    opts.add_option('','--priority',default=-1,type=int,dest='prio')
    opts.add_option('','--nThreads',default=0,dest='nThreads',type=int)
    opts.add_option('','--queues',default=None,dest='queues')
        
    opts.add_option('','--dryrun',default=False,action='store_true',dest='dryrun')

    opts.add_option('','--tempPath',default=os.path.expanduser('~/tmp/'),dest='tempPath')
    (o,args)=opts.parse_args()
    
    if not o.inFn or not os.path.exists(o.inFn):
        help()
            
    tempPath=os.path.exists(o.tempPath) and os.path.abspath(o.tempPath) or os.path.abspath('./')
        
    cl = CmdList( o.inFn )    
    print 'loaded %d commands, %d groups'%(cl.countCommands(), cl.countGroups())
    print 'temp path: %s'%cl.submitMkPath()
    print 'output log path: %s'%cl.logPath()
    
    
    if o.groupNPer>-1:
        cl.regroupNper(o.groupNPer, o.groupName)
        print 'regrouping into %d-item groups'%o.groupNPer
    elif o.groupNPer==1 and o.submitTaskify:
        cl.regroupNper(1, o.groupName)
        print 'regrouping into single-item groups'
        
    if o.outFn:
        print 'saving back to %s'%o.outFn
        cl.save(o.outFn)
    
    if o.submitTaskify:
        cl.submit_using_tasks(jobName = o.jobname or  (cl.lastFnLoaded and '.'.join(os.path.split(cl.lastFnLoaded)[-1].split('.')[:-1]) or 'job'),
                      priority=o.prio, debug=o.dryrun)
    elif o.submit:
        cl.submit(jobName = o.jobname or  (cl.lastFnLoaded and '.'.join(os.path.split(cl.lastFnLoaded)[-1].split('.')[:-1]) or 'job'),
                  priority=o.prio, debug=o.dryrun)
    elif o.submitLocal:
        cl.submitlocal( jobName = o.jobname or  (cl.lastFnLoaded and '.'.join(os.path.split(cl.lastFnLoaded)[-1].split('.')[:-1]) or 'job') )
           
        
