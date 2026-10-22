# The Resilient Scheduler — Simulation Engine

In order to build this program, open resilient-scheduler.slnx and compile it with f7 (or going into Build > Build Solution).
The unit tests can be run by using the shorcut Ctrl+R , A.

The project is executed by providing the following arguments:
--base_folder folder which job.csv, failures.csv and workers.csv are stored.
--workers name of the workers file inside the base folder.
--jobs name of the jobs file inside the base folder.
--failures name of the failures file inside the base folder.
--policy policy used, Possible values are the following: SJF, RR, FIFO, LATELAST
--recmode recovery mode used. Possible values are: RESUME, RESTART
--quantum any integer value above 1 and below int_max

Example command-line arguments:
 --base_folder C:\Users\gabri\Documents\Newcastle-Uni\CSC8501-Cpp\Coursework\CSC8501_Resilient_Scheduler_Support_Pack\data\assessment\01_arrival_bursts\ --workers workers.csv --jobs jobs.csv --failures failures.csv --policy LATELAST --recmode RESUME --quantum 1
 
 The program generates job and work traces in .csv format inside the base folder, and a summary in .json file in the same folder.