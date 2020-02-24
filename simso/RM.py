#RM scheduler modified to output training data to a scheduler


from simso.core import Scheduler
from simso.schedulers import scheduler

num_procs = 10
parameters_per_proc = 1
num_inputs = num_procs*parameters_per_proc
num_outputs = 10
filename = "RM_short_jobs.txt"
file_opened = False
"""May overwrite preexisting file"""
filestream = open(filename, "w")

def RMSort(p):
  return p.period

@scheduler("simso.schedulers.RM")
class RM(Scheduler):
    """ Rate monotonic """
    def init(self):
        self.ready_list = []

    def on_activate(self, job):
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
    
        decision = None
        if self.ready_list:
            # Get a free processor or a processor running a low priority job.
            key = lambda x: (
                0 if x.running is None else 1,
                -x.running.period if x.running else 0,
                0 if x is cpu else 1
            )
            cpu_min = min(self.processors, key=key)

            # Job with highest priority (final answer).
            job = min(self.ready_list, key=lambda x: x.period)
            
            #Sort list by priority
            ready_jobs = self.ready_list
            ready_jobs.sort(reverse=False,key=RMSort)
            #Get top 10 from the list
            list_size = min(num_procs, len(ready_jobs))
            NN_jobs = ready_jobs[:num_procs]
            #Pad with extra data
            while len(NN_jobs) < num_procs:
              NN_jobs.append(random.choice(NN_jobs))
            #Randomize order so network doesn't learn to pick the first one
            shuffle(NN_jobs)
            #Output to file
            #Inputs
            filestream.write("in: ")
            for j in NN_jobs:
              filestream.write(str(j.period) + " ")
            filestream.write("\n")
            #Outputs
            #Get highest priority
            first_period = job.period
            output_list = [str(1.0) if x.period == first_period else str(0.0) for x in NN_jobs]
            filestream.write("out: ")
            for y in output_list:
              filestream.write(y + " ")
            filestream.write("\n")  
  

            if (cpu_min.running is None or
                    cpu_min.running.period > job.period):
                self.ready_list.remove(job)
                if cpu_min.running:
                    self.ready_list.append(cpu_min.running)
                decision = (job, cpu_min)

        return decision
