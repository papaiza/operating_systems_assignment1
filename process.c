/*# Family Name: Sitkovets
# Given Name: Anton
# Section: Z
# Student Number: 212118048
# CSE Login: cse31027 */


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
 
//Create a Struct to hold the file contents
// Hold the word in char* array 'word'
// Hold the vector values in float array 'values'
typedef struct FileContents {
    char* word[128];
    float values[1000];
}FileContents;

// Create Struct to hold Word Comparisons
// Hold the first word in the comparison in char* array 'word1'
// Hold the second word in the comparison in char* array 'word2'
// Hold the cosine similarity between the two words in the float 'similarity'
typedef struct WordComparison {
    char** word1;
    char** word2;
    float similarity;
} WordComparison;


//SORTING ALGORITHM FUNCTION
void qs(WordComparison x[], int low, int high){
    int i,j, pivot;
    WordComparison temp;
    
    if (low < high){
        pivot = low;
        i = low;
        j = high;
        while(i < j){
            //Check to see if the ith value is less than the pivot and high limit
            while (x[i].similarity <= x[pivot].similarity && i < high)
                i++;
            //check to see if the jth item's similarity greater than the pivot's
            while (x[j].similarity > x[pivot].similarity)
                j--;
            if (i < j){
                temp = x[i];
                x[i] = x[j];
                x[j] =  temp;
            }
        }
        
        temp = x[pivot];
        x[pivot] = x[j];
        x[j] = temp;
        qs(x, low, j -1);
        qs(x, j+1, high);
    }
}

//COSINE SIMILARITY CALCULATION FUNCTIONS

// Calculate the sum of all the values^2 in x and then square the result
float square_sum(float x[]){
    float sum = 0;
    long i;
    for (i = 0; i < 1000; i++){
        sum += pow(x[i], 2);
    }
    return sqrt(sum);
}
// Calculate the summation dot product of the x and y vectors.
// Loop through two vectors, multiply the ith item of each vector and add to sum. 
float dot_prod(float x[], float y[]){
    
    float sum = 0;
    long i;
    
    for( i = 0 ; i < 1000; i++){
        sum += x[i]*y[i];
    }
    return sum;
}
//Calculate the final cosine similarity
float cos_sim(float x[], float y[]){
    
    return (dot_prod(x, y))/((square_sum(x)*square_sum(y)));
}

//Array of structs for the comparisons done
WordComparison comps[3];

//Array of structs for the vectors in the file.
FileContents vectors[1000];

//MAIN OF CLASS WHERE USER WILL GIVE INPUT
int main(int argc, char *argv[]) {
    int i = 1;
    pid_t pid;
    int fd[argc - 1][2];
    char read_msg[500];
    for (i = 1; i < argc; i++){
    
        //Create the pipe
        if(pipe(fd[i - 1]) < 0){
            fprintf(stderr, "Pipe failed" );
            return 1;
        }

        pid = fork();

        if(pid < 0){
            fprintf(stderr, "Fork Failed!\n");
            exit(-1);
        }else if(pid == 0){ // Child Process
 
            FILE * fp;
            char * line = NULL;
            size_t len = 0;
            ssize_t read;
            int x = 0;
            
            int p = 0; //Stores how many lines in the text file
            //Open and read file giving in the command line as an argument
            fp = fopen(argv[i], "r");
            if (fp == NULL)
               exit(EXIT_FAILURE);
            
            int k;
            k = 0;
            char *token;

            

            //Read the file line by line
            while ((read = getline(&line, &len, fp)) != -1) {

                float num;
                int j;
                j = 0;
                x+=1;
                //Create a token that starts and ends with each space character
                token = strtok(line, " ");
                
                while (token != NULL){
                    int l;
                    l = j - 1;
                    // When j is 0, this is the start of the line, so the first
                    // character string is the word. If j != 0, then this must be a 
                    // number that is a part of the interupt vector
                    if (j == 0){
                        strcpy(vectors[k].word, token);
                    }else{
                        vectors[k].values[l] = atof(token);
                        
                    }
                    token = strtok(NULL, " ");
                    j++;
                }     
                
                k++;
            }

            if (line)
               free(line);
            
            rewind(fp);
            fclose(fp);

            // Compare each value in the array with every other value in the array.
            // Only store the top three highest cosine similarity pairs. 
            int s;
            int t;
            for ( s = 0; s < x; s++){
                for( t = s + 1; t < x; t++){
                    if(p < 3){
                        comps[p].word1 = (char **)malloc(10 * sizeof(char **));
                        strcpy(comps[p].word1, vectors[s].word);
                        comps[p].word2 = (char **)malloc(10 * sizeof(char **));
                        strcpy(comps[p].word2, vectors[t].word);
                        comps[p].similarity = cos_sim(vectors[s].values, vectors[t].values);
                        qs(comps, 0, p - 1);
                        p++;
                    }else{
                        float next;
                        next = cos_sim(vectors[s].values, vectors[t].values);
                        if(next > comps[0].similarity){
                            comps[p].word1 = (char **)malloc(sizeof(char **));
                            strcpy(comps[0].word1, vectors[s].word);
                            comps[p].word2 = (char **)malloc(sizeof(char **));
                            strcpy(comps[0].word2, vectors[t].word);
                            comps[0].similarity = next;
                            qs(comps, 0, p -1);
                        }
                    } 
                }
            }

            char answer[100];
            // Print the results of the computation.
            // Formatted as: 
            // Filename1 wordA11 wordB11 simA11B11 wordA12 wordB12 simA12B12 wordA13 wordB13 simA13B13
            sprintf(answer , "%s %s %s %.6f %s %s %.6f %s %s %.6f \n" ,argv[i], comps[2].word1, comps[2].word2, comps[2].similarity,comps[1].word1,comps[1].word2, comps[1].similarity, comps[0].word1, comps[0].word2, comps[0].similarity);

            close(fd[i - 1][0]);
            write(fd[i - 1][1], answer, (strlen(answer) + 1));            
            exit(0);
            
        }
    }
    if( pid > 0){ // Parent Process
            int status ;
            waitpid(pid, &status, 0);
            int j;
            for (j = 0; j < argc - 1; j++) {

                close(fd[j][1]);
                read(fd[j][0], read_msg, sizeof(read_msg));
                printf("%s", read_msg);
            }
            exit(0);
        
    }  
    
    return 0;
}

