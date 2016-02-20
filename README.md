# operating_systems_assignment1
Operating Systems assignment parsing large text files, calculating the cosine similarity and then returning the top 3 similarities. Done in 1 process, multiple processes and multiple threads. 

Here all three programs do the same thing. But main.c does so in 1 process, process.c uses multiple processes and thread.c usese multithreading

To test the code locally run :

   gcc main.c
  
   ./a.out wordvector1.txt wordvector2.txt wordvector3.txt
  
The tester can choose how many arguments to give to the program.
