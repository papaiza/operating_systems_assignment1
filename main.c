/*# Family Name: Sitkovets
# Given Name: Anton
# Section: Z
# Student Number: 212118048
# CSE Login: cse31027 */


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
 
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

//MAIN OF CLASS WHERE USER WILL GIVE INPUT
int main(int argc, char *argv[]) {
    int i;
    for (i = 1; i <= argc-1; i++){
        
        FILE * fp;
        char * line = NULL;
        size_t len = 0;
        ssize_t read;
        int x = 0;
        //Array of structs for the vectors in the file.
        FileContents vectors[1000];
        //Array of structs for the comparisons done
        WordComparison comps[3];

        //Open and read the file given as argv
        fp = fopen(argv[i], "r");
        if (fp == NULL)
           exit(EXIT_FAILURE);
        
        int k;
        k = 0;
        char *token;
        
        //Read the file line by line and pars the lines using the space character as
        // a delimeter
        while ((read = getline(&line, &len, fp)) != -1) {

            float num;
            int j;
            j = 0;
            x+=1;

            token = strtok(line, " ");
            
            //Store the first string as word in vectors.
            //Store the rest of the strings as a float array
            while (token != NULL){
                int l;
                l = j - 1;
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


        //Compare each element in vectors with the rest of the elements in vector.
        //Calculate the cosine similarity and store it as well as the two words in an array
        //Only store the top three similarities. 
        //In order to be added to the array, the cosine similarity must be at least as large 
        //as the least significant element. Every time a new element is added, quicksort is run to preserve 
        //the order of the array.
        int p = 0;
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
        
        // Print the results of the computation.
        // Formatted as: 
        // Filename1 wordA11 wordB11 simA11B11 wordA12 wordB12 simA12B12 wordA13 wordB13 simA13B13  
        int q;
        printf("%s ", argv[i]);
        for(q = 2; q >= 0; q--){
            printf("%s %s %.6f ", comps[q].word1, comps[q].word2, comps[q].similarity);
        }
        printf("\n");
    
    }
    return 0;
    
    
}
