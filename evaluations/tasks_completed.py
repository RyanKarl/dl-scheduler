#!/usr/bin/env python3

import sys

def Average(lst):
  return sum(lst)/len(lst)

#Maps task ID to start time
task_dict = dict()

avg_list = []
task_list = []
completed = []

line = ""
last_epoch = 0
number_completed = 0
last_time = 0
NUM_MILLISECONDS = 1000


#Error handling for input

if len(sys.argv) != 3:
  print "Usage: python " + str(sys.argv[0]) + " filename time_bound"
  sys.exit()
  
filename = sys.argv[1]
time_bound = int(sys.argv[2])
  
if time_bound < 1:
  print "Positive time_bound required"
  sys.exit()  

with open(filename, "r") as f:
  for line in f:

    #found_activated = "Activated" in line
    found_terminated = "Terminated" in line
    #found_time = time_bound in line    

    if found_terminated:
    
      line = line.replace("(", "")
      line = line.replace(")", "")    
      line = line.replace("[", "")
      line = line.replace("]", "")
      line = line.replace("'", "")
      #print line

      task_list = line.split(",")
      #task_list[0] is the time, task_list[1] is the task ID

      #middle_section = task_list[1].lstrip()
      
      #print middle_section
      #taskID = middle_section.split(' ')[0]
      
      #print taskID
      
      #Get time of termination
      event_time = int(task_list[0])
      #Increment number of tasks completed
      number_completed = number_completed+1
      #Store most recent time
      last_time = event_time
      #Print out tasks completed after this amount of time
      current_epoch = event_time // time_bound
      if current_epoch > last_epoch:
        print str(last_epoch) + ","  + str(number_completed - 1)
        last_epoch = current_epoch

      
#Print number completed before deadline at end
#print str(last_time) + ","  + str(number_completed) 
      

