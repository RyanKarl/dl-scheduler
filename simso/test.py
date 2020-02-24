import sys
from simso.core import Scheduler
import simso.core 
from simso.core import Model
import simso.configuration
from simso.configuration import Configuration

scheduler_filename = "./EDFNN.py"

def main(argv):
    if len(argv) == 2:
        # Configuration load from a file.
        configuration = Configuration(argv[1])
    else:
        # Manual configuration:
        configuration = Configuration()

        configuration.duration = 420 * configuration.cycles_per_ms

        # Add tasks:
        configuration.add_task(name="T1", identifier=1, period=7,
                               activation_date=0, wcet=7, deadline=7)
        configuration.add_task(name="T2", identifier=2, period=12,
                               activation_date=0, wcet=3, deadline=12)
        configuration.add_task(name="T3", identifier=3, period=20,
                               activation_date=0, wcet=5, deadline=20)

        # Add a processor:
        configuration.add_processor(name="CPU 1", identifier=1)

        # Add a scheduler:
        configuration.scheduler_info.filename = scheduler_filename
        #configuration.scheduler_info.clas = "simso.schedulers.EDF_NN"

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
