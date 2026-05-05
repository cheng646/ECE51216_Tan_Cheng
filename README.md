# ECE51216_Tan_Cheng

to run tests through uf20-91

Be in main folder and use cmd runTests.sh

To run the executable. Be in the main folder and run
"./SAT Solver/mySAT" "Benchmark Files/uf20-91/uf20-01.cnf"

To run with dlis. Add the --dlis flag after the file. For example:
"./SAT Solver/mySAT" "Benchmark Files/uf20-91/uf20-01.cnf" --dlis

to run a full folder of files and see assignments, enter "runTestsAssign.sh" and edit the path to go to the folder that contains the .cnf files. Add "--dlis" to the main call to run with dlis (There is a comment above the required line with instructions)
be in the main folder and run "./runTestsAssign.sh" You may need to run chmod +x on the .sh file to make it become an executable bash.
The test_results.txt will contain all of the assignment details.

to run a full folder of files and see total run time and memory usage, enter "runTests.sh" and edit the path to go to the folder that contains the .cnf files. Add "--dlis" to the main call to run with dlis (There is a comment above the required line with instructions)
be in the main folder and run "./runTests.sh" You may need to run chmod +x on the .sh file to make it become an executable bash.


To remake "mysat" executable. Enter the SAT Solver folder and run
make clean
make mySAT