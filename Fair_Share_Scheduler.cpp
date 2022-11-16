// Nov 10 2022
// C++ program to implement a fair-share scheduler

// Mohamed Allalou 
// Ayman Chaoui 


#include <iostream>
#include <thread>
#include <fstream> 
#include <string>
#include <vector>
#include <mutex>


using namespace std;

int Global_Time = 0;
mutex mtx;

class User
{
    public:
        string name; // User Name
        vector<int> processRT; // Process Release Times
        vector<int> processET; // Process Execution Times
        vector<string> process_ST; // Process Statuses (running, finished, etc...)
    
    // Overloaded constructer receives 2 arguments; name and number of processes.
    // Initializes name and status vector for the user
    User(string a, int b){
        name = a;
        for (int c = 0; c < b; c++){
          process_ST.emplace_back("Ready");
        }
    }
};

// Processor function receives a reference to a process that needs to be executed
// by the cpu. It performs the execution if the parameters are met (global time, etc...)
void Processor(vector<User>& Users_Vector_Arg_Processor, int& user_number, int& process_number, int& time_quantum_arg, bool& a_process_ran_arg)
{
    // Global time check against the process release time
    if (Global_Time >= Users_Vector_Arg_Processor[user_number].processRT[process_number]) {

        // First time start
        // Set status to "Started"
        // Print stack trace
        if (Users_Vector_Arg_Processor[user_number].process_ST[process_number] == "Ready") {
            Users_Vector_Arg_Processor[user_number].process_ST[process_number] = "Started";
            cout << "Time " << Global_Time << ", User " << Users_Vector_Arg_Processor[user_number].name << ", Process " << process_number << ", " << Users_Vector_Arg_Processor[user_number].process_ST[process_number] << endl;
        }

        // This process is now resuming
        Users_Vector_Arg_Processor[user_number].process_ST[process_number] = "Resumed";
        cout << "Time " << Global_Time << ", User " << Users_Vector_Arg_Processor[user_number].name << ", Process " << process_number << ", " << Users_Vector_Arg_Processor[user_number].process_ST[process_number] << endl;
        
        // Check to see if this will be the last run
        // If so, we add the remaining execution time to the global time
        // We set the remaining execution time to 0
        // We notify the schdeuler that a process ran
        if (Users_Vector_Arg_Processor[user_number].processET[process_number] <= time_quantum_arg) {
            Global_Time = Global_Time + Users_Vector_Arg_Processor[user_number].processET[process_number];
            Users_Vector_Arg_Processor[user_number].processET[process_number] = 0;
            a_process_ran_arg = true;
        }

        // Otherwise we simply decrease the remaining execution time by the allocated quantum
        // We notify the schdeuler that a process ran
        else {
            Global_Time = Global_Time + time_quantum_arg;
            Users_Vector_Arg_Processor[user_number].processET[process_number] = Users_Vector_Arg_Processor[user_number].processET[process_number] - time_quantum_arg;
            a_process_ran_arg = true;
        }

        // Pause the Process
        // Print stack trace
        Users_Vector_Arg_Processor[user_number].process_ST[process_number] = "Paused";
        cout << "Time " << Global_Time << ", User " << Users_Vector_Arg_Processor[user_number].name << ", Process " << process_number << ", " << Users_Vector_Arg_Processor[user_number].process_ST[process_number] << endl;
        
        // Completion check
        // If finished - Print stack trace
        if (Users_Vector_Arg_Processor[user_number].processET[process_number] == 0){
            Users_Vector_Arg_Processor[user_number].process_ST[process_number] = "Finished";
            cout << "Time " << Global_Time << ", User " << Users_Vector_Arg_Processor[user_number].name << ", Process " << process_number << ", " << Users_Vector_Arg_Processor[user_number].process_ST[process_number] << endl;
        }
    }
}

// Scheduler function receives a reference to vector storing all the users
// and all the respective information (processes, times, etc...).
// It then schedules each process for every user to use the processor
// according to fair share scheduling policy
void Scheduler(vector<User>& Users_Vector_Arg_Scheduler, int& time_quantum)
{
    // Get time quantum per User
    // Initialize some local helper variables
    int quantum_per_user = time_quantum/Users_Vector_Arg_Scheduler.size(); 
    int quantum_per_process = 0;
    bool done = false;
    bool a_process_ran = false;
    
    while (done == false) {
               
        a_process_ran = false;

        // For all users in the vector container
        for (int i = 0; i < Users_Vector_Arg_Scheduler.size(); i++){
            
            // Calculate the time quantum for each process of that User
            quantum_per_process = quantum_per_user/Users_Vector_Arg_Scheduler[i].processRT.size();
            
            // Edge case safety check
            // Make sure each process is allocated a minimum of 1 time
            if (quantum_per_process < 1) {
                quantum_per_process = 1;
            }
            
            // Now go through each process for that User
            for (int j = 0; j < Users_Vector_Arg_Scheduler[i].processRT.size(); j++) {
                
                // If the process is not finished, launch a child thread and process it
                if (Users_Vector_Arg_Scheduler[i].process_ST[j] != "Finished") {
                    
                    // Lock the processor
                    mtx.lock();
                    
                    Processor(Users_Vector_Arg_Scheduler, i, j, quantum_per_process, a_process_ran);
                    
                    // Unlock the processor so other threads can use it
                    mtx.unlock();
                }
            }
        }

        // Check to see if a process ran
        // If not, advance the time
        if (a_process_ran == false) {
            Global_Time++;
        }

        // Check to see if all processes are finished.
        // We set done to true then go through all the processes.
        // If one of them is not Finished, then done gets set to
        // false and the scheduler will continue the loop
        done = true;
        for (int i = 0; i < Users_Vector_Arg_Scheduler.size(); i++){
            for (int j = 0; j < Users_Vector_Arg_Scheduler[i].processRT.size(); j++) {
                if (Users_Vector_Arg_Scheduler[i].process_ST[j] != "Finished") {
                    done = false;
                }
            }
        }

        // If all processes are done, done stays true. The exit
        // condition is valid and we have finished processing
    }
}

// Driver code
int main()
{
    // We begin by creating a vector of user objects
    // This container will store the users, processes for each user,
    // and thier respective release and execution times, etc...
    vector<User> Users_Vector {};

    // Some temporary variables to help read the Input file
    // and parse it into objects of user class
    int User_Number = 0;
    int time_quantum = 0;
    string parsed_name = "";
    int number_of_processes = 0;
    int release_time = 0;
    int execution_time = 0;

    // Open Input.txt 
    ifstream Input_File("Input.txt");

    // Now read the file and begin to copy elements
    // First line is time quantum
    Input_File >> time_quantum;

    // Now we enter a loop to create users and processes
    // as we read the rest of the file. The second line is
    // the user name and number of processes. The first 
    // element is the user name. It is read by the below line.
    while (Input_File >> parsed_name){

        // second element is number of processes for that user
        Input_File >> number_of_processes;
        
        // We create the user object
        Users_Vector.emplace_back(User(parsed_name, number_of_processes));
    
        // now according to the number of processes
        // we read that number of lines down
        for (int i = 0; i < number_of_processes; i++) {

            // first half is release time for that process
            // second half is required execution time
            Input_File >> release_time;
            Input_File >> execution_time;
            
            // We store the values in the user vector in thier appropriate positions
            Users_Vector.back().processRT.emplace_back(release_time);
            Users_Vector.back().processET.emplace_back(execution_time);
        }
        
    // If the end of file is reached, the while statement wont execute
    // and the loop will run again, creating another user, then
    // reading his processes, etc...
    }
    //  end of file

    // Launch threads
    thread master_thread(Scheduler, ref(Users_Vector), ref(time_quantum));
    
    // Wait for all threads to finish
    master_thread.join();
    
    // Congrats! Your processes have all finished processing :)
    return 0;
}
