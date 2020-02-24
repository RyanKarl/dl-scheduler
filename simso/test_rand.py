#Test script with random task generation

import sys
from simso.core import Scheduler
import simso.core 
from simso.core import Model
import simso.configuration
import simso.generator.task_generator
from simso.configuration import Configuration

#This EDF is not the basic one - it will output training data
#Not portable - switch this if used on a different system
scheduler_filename = "/home/jonathan/Documents/grad_os/dl-scheduler/simso/EDF_NN_mimic.py"

def main(argv):

    print "Started main"

    #Variables
    num_periods = 3
    num_sets = 2
    num_tasks = 10
    period_min = 5
    period_max = 50
    round_to_int = True
    utilization_min = 3
    utilization_max = 7
    #Data in use
    compute = 3
    deadline = 7
    period = deadline + compute
    target_util = 0.2
    runtime = 420
    use_base_scheduler = True
    #Take in length of time to run as argument
    
    args = argv[1:]
    i = 0
    while args and args[0].startswith('-') and len(args[0]) > 1:
      arg = args.pop(0)
      if arg == '-r':
        runtime = int(args.pop(0))
        print "Runtime set as " + str(runtime)
      elif arg == '-s':
        sched = args.pop(0)
        if sched == 'f':
          use_base_scheduler = False
          print "Using file scheduler"
        else:
          use_base_scheduler = True
          print "Using base scheduler"  
      elif arg == '-n':
        num_sets = int(args.pop(0))
        print "Generating " + str(num_sets) + " sets"
      elif arg == '-u':
        target_util = float(args.pop(0))      
        print "Utilization is " + str(target_util)
          



    if len(argv) == 2:
        # Configuration load from a file.
        configuration = Configuration(argv[1])
    else:
        # Manual configuration:
        configuration = Configuration()

        configuration.duration = runtime * configuration.cycles_per_ms


        
        # Add tasks manually:
        configuration.add_task(name="T1", identifier=1, period=7,
                               activation_date=0, wcet=3, deadline=7)
        configuration.add_task(name="T2", identifier=2, period=12,
                               activation_date=0, wcet=3, deadline=12)
        configuration.add_task(name="T3", identifier=3, period=20,
                               activation_date=0, wcet=5, deadline=20)
        #configuration.add_task(name="T4", identifier=4, period=20,
                               #activation_date=0, wcet=5, deadline=20)                           
         
                       
        """                   
        #Create tasks
        #generated_task_sets = simso.generator.task_generator.gen_ripoll(num_sets, compute, deadline, period, target_util)
        #Order is (comp. time, deadline, period)   
        #print "num_sets " + str(num_sets) + " target_util " + str(target_util) + " num_tasks " + str(num_tasks)
        #generated_task_sets = simso.generator.task_generator.StaffordRandFixedSum(num_tasks, target_util, num_sets)
        print generated_task_sets
        #Add tasks
        i = 1
        for s in generated_task_sets:
          for t in s:
            #print t
            #Use this with ripoll
            configuration.add_task(name="T"+str(i), identifier = i, period = t[2], wcet = t[0], deadline = t[1])
            #Use this with StaffordRandFixedSum
            #configuration.add_task(name='T'+str(i), identifier = i, period = t, wcet = t, deadline = t)
            i = i+1
            """
        
        # Add a processor:
        configuration.add_processor(name="CPU 1", identifier=1)

        # Add a scheduler:
        if use_base_scheduler:
          configuration.scheduler_info.clas = "simso.schedulers.EDF"
        else:  
          configuration.scheduler_info.filename = scheduler_filename        

    # Check the config before trying to run it.
    configuration.check_all()

    # Init a model from the configuration.
    model = Model(configuration)

    # Execute the simulation.
    model.run_model()

    # Print logs.
    for log in model.logs:
        print(log)
        
    #Print to file    
    #for task in model.results.tasks:
    	#print(task.name + ":")
    	#for job in task.jobs:
      	    #print("%s %.3f ms %s %s %s" % (job.name, job.actual_computation_time, task.data, not job.aborted, job.end_date))

main(sys.argv)
