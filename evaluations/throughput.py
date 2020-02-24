#!/usr/bin/env python3

import sys

def Average(lst):
  return sum(lst)/len(lst)

#Maps task ID to start time
task_dict = dict()

avg_list = []
task_list = []

line = ""
start = 0
stop = 0


filename = sys.argv[1]

with open(filename, "r") as f:
  for line in f:
    
    found_activated = "Activated" in line
    found_terminated = "Terminated" in line
    
    if found_activated or found_terminated:
    
      line = line.replace("(", "")
      line = line.replace(")", "")    
      line = line.replace("[", "")
      line = line.replace("]", "")
      line = line.replace("'", "")
      #print line

      task_list = line.split(",")
      #task_list[0] is the time, task_list[1] is the task ID
    
      middle_section = task_list[1].lstrip()
      #print middle_section
      taskID = middle_section.split(' ')[0]
      #print taskID
      event_time = int(task_list[0])

      if found_terminated:
        completion_time = event_time - task_dict[taskID]
        avg_list.append(completion_time)
        #print str(taskID) + ' ' + str(completion_time)
      if found_activated:
        task_dict[taskID] = event_time

      
#Print averages at end
print "Average: " + str(Average(avg_list))    
      

