"""
Neural network scheduler
"""
from simso.core import Scheduler
from simso.schedulers import scheduler
from SimpleNeuralNetwork import Net
from random import shuffle
import random
import numpy

#Defined in EDF_NN.py - change if topology changes!
#NN is doing the same thing as with a RM scheduler
fname = "/home/jonathan/Documents/grad_os/dl-scheduler/simso/RM_net.txt"
"""
print "Starting NN init (base level)"
nn = Net(fname)
print "Finished NN init"
"""
num_procs = 10
dummy = 100
NN_init = False


def G_FLSort(p):
  return p.priority

@scheduler("simso.schedulers.G_FL_NN_mimic")
class G_FL_NN_mimic(Scheduler):
    """Earliest Deadline First"""
    def init(self):
        self.ready_list = []
        
    def get_job_priority(self, job):
      return job.activation_date + job.deadline - \
            ((len(self.processors) - 1.0) / len(self.processors)) * job.wcet

    def on_activate(self, job):
        job.priority = get_job_priority(self, job)
        self.ready_list.append(job)
        job.cpu.resched()

    def on_terminated(self, job):
        if job in self.ready_list:
            self.ready_list.remove(job)
        else:
            job.cpu.resched()
    def schedule(self, cpu):
    
        global num_procs
        global nn
        global dummy
        global fname
        global NN_init
        
        if not NN_init:
          nn = Net(fname)
          NN_init = True
        
        #Test output
        #print nn.pythonGetTopology()
        
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
            
            #Sort list by priority
            ready_jobs.sort(reverse=False, key=G_FLSort)
            #Get top 10 from the list
            list_size = min(num_procs, len(ready_jobs))
            NN_jobs = ready_jobs[:num_procs]
            #Get input to send to NN
            while(len(NN_jobs) < num_procs):
              #Randomly add duplicates
              NN_jobs.append(random.choice(NN_jobs))
            shuffle(NN_jobs)
            input_list = [x.priority for x in NN_jobs]

            #Send it to the net
            nn.pythonFeedForward(input_list)
            #Get output back
            output_list = nn.pythonGetResults()
            #Get index of maximal element
            best_idx = numpy.argmax(output_list)
            #Set job
            job = NN_jobs[best_idx]
            #Not our code - don't touch
            if (cpu_min.running is None or cpu_min.running.absolute_deadline > job.absolute_deadline):
                print(self.sim.now(), job.name, cpu_min.name)
                return (job, cpu_min)
