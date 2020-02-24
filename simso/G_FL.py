"""
Implementation of the Global-Fair Lateness as presented by Erickson and
Anderson in Fair lateness scheduling: Reducing maximum lateness in G-EDF-like
scheduling.

This version has been modified to output data to a neural network.
JST, RK, and CC
"""
from simso.core import Scheduler
from simso.schedulers import scheduler
import random
from random import shuffle

num_procs = 10
parameters_per_proc = 1
num_inputs = num_procs*parameters_per_proc
num_outputs = 10
filename = "G_FL_short_jobs.txt"
file_opened = False
"""May overwrite preexisting file"""
filestream = open(filename, "w")

def G_FLSort(p):
  return p.priority

@scheduler("simso.schedulers.G_FL")
class G_FL(Scheduler):
    """Earliest Deadline First"""
    def init(self):
        self.ready_list = []
        
    def get_job_priority(self, job):
      return job.activation_date + job.deadline - \
            ((len(self.processors) - 1.0) / len(self.processors)) * job.wcet    

    def on_activate(self, job):
        job.priority = self.get_job_priority(job)
        self.ready_list.append(job)
        job.cpu.resched()

    def on_terminated(self, job):
        if job in self.ready_list:
            self.ready_list.remove(job)
        else:
            job.cpu.resched()

    def schedule(self, cpu):
    
    
        #Declare globals
        global filestream
        global file_opened
    
        #Open file, if not already opened
        if not file_opened:
          filestream = open(filename, "w")
          #Hardcoded - change if the topology of the NN changes!
          filestream.write("topology: 10 7 8 10\n")
          file_opened = True

        ready_jobs = [j for j in self.ready_list if j.is_active()]

        if ready_jobs:
            # Key explanations:
            # First the free processors
            # Among the others, get the one with the greatest deadline
            # If equal, take the one used to schedule
            key = lambda x: (
                1 if not x.running else 0,
                x.running.priority if x.running else 0,
                1 if x is cpu else 0
            )
            cpu_min = max(self.processors, key=key)
            #This job is the final answer
            job = min(ready_jobs, key=lambda x: x.priority)
            
            #Sort list by priority
            ready_jobs.sort(reverse=False,key=G_FLSort)
            #Get top 10 from the list
            list_size = min(num_procs, len(ready_jobs))
            NN_jobs = ready_jobs[:num_procs]
            #Pad with extra data if need be
            while len(NN_jobs) < num_procs:
              NN_jobs.append(random.choice(NN_jobs))
            #Randomize order so network doesn't learn to pick the first one
            shuffle(NN_jobs)
            #Output to file
            #Inputs
            filestream.write("in: ")
            for j in NN_jobs:
              filestream.write(str(j.priority) + " ")
            filestream.write("\n")
            #Outputs
            #Get earliest deadline
            top_priority = job.priority
            output_list = [str(1.0) if x.priority == top_priority else str(0.0) for x in NN_jobs]
            filestream.write("out: ")
            for y in output_list:
              filestream.write(y + " ")
            filestream.write("\n")

            if (cpu_min.running is None or
                    cpu_min.running.priority > job.priority):
                self.ready_list.remove(job)
                if cpu_min.running:
                    self.ready_list.append(cpu_min.running)
                return (job, cpu_min)
