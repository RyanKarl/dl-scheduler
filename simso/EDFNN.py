"""
Implementation of the Global-EDF (Earliest Deadline First) for multiprocessor
architectures.
"""
from simso.core import Scheduler
from simso.schedulers import scheduler
from random import shuffle

num_procs = 10
parameters_per_proc = 1
num_inputs = num_procs*parameters_per_proc
num_outputs = 10
filename = "EDF_out.txt"
file_opened = False
"""May overwrite preexisting file"""
filestream = open(filename, "w")

def EDFSort(p):
  return p.absolute_deadline
  

@scheduler("simso.schedulers.EDFNN")
class EDFNN(Scheduler):
    """Earliest Deadline First"""
    def on_activate(self, job):
        job.cpu.resched()        

    def on_terminated(self, job):
        job.cpu.resched()

    def schedule(self, cpu):
    
        print "Modified version"
    
        #Declare globals
        global filestream
        global file_opened
    
        #Open file, if not already opened
        if not file_opened:
          filestream = open(filename, "w")
          #Hardcoded - change if the topology of the NN changes!
          filestream.write("topology: 10 7 8 10\n")
          file_opened = True
        
    
        # List of ready jobs not currently running:
        ready_jobs = [t.job for t in self.task_list
                      if t.is_active() and not t.job.is_running()]

        if ready_jobs:
            # Select a free processor or, if none,
            # the one with the greatest deadline (self in case of equality):
            key = lambda x: (
                1 if not x.running else 0,
                x.running.absolute_deadline if x.running else 0,
                1 if x is cpu else 0
            )
            cpu_min = max(self.processors, key=key)

            # Select the job with the least priority:
            job = min(ready_jobs, key=lambda x: x.absolute_deadline)
            
            #Sort list by priority
            ready_jobs.sort(reverse=False,key=EDFSort)
            #Get top 10 from the list
            list_size = min(num_procs, len(ready_jobs))
            NN_jobs = ready_jobs[:10]
            #Randomize order so network doesn't learn to pick the first one
            shuffle(NN_jobs)
            #Output to file
            #Inputs
            filestream.write("input: ")
            for j in NN_jobs:
              filestream.write(str(j.absolute_deadline) + " ")
            filestream.write("\n")
            #Outputs
            #Get earliest deadline
            earliest_deadline = job.absolute_deadline
            output_list = [str(1.0) if x.absolute_deadline == earliest_deadline else str(0.0) for x in NN_jobs]
            filestream.write("output: ")
            for y in output_list:
              filestream.write(y + " ")
            filestream.write("\n")  

            if (cpu_min.running is None or
                    cpu_min.running.absolute_deadline > job.absolute_deadline):
                print(self.sim.now(), job.name, cpu_min.name)
                return (job, cpu_min)
