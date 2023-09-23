// Program 2: Food Web Analysis With Dynamic Memory
// Course: CS 211, Fall 2023, UIC
// System: Advanced zyLab
// Author: Shaan Kohli

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Org_struct {
    char name[20];
    int* prey; //dynamic array of indices  
    int numPrey;
} Org;

// Adds a predator-prey relation to the food web
void buildWeb(Org* web, int numOrg, int predInd, int preyInd) {
  
    Org* predator = &web[predInd];

    // allocates memory for 1 index if predator's prey array is empty
    if (predator->prey == NULL) {
        predator->prey = (int*)malloc(sizeof(int));
        predator->numPrey = 1;
        predator->prey[0] = preyInd;
    }
    else {
        // allocate memory for new prey array
        int* newPrey = (int*)malloc((predator->numPrey + 1) * sizeof(int));

        // create new prey array
        for (int i = 0 ; i < predator->numPrey; i++) {
            newPrey[i] = predator->prey[i];
        }

        // appends new prey index as last element
        newPrey[predator->numPrey] = preyInd;

        free(predator->prey);

        // updates predator's prey pointer and numPrey
        predator->prey = newPrey;
        predator->numPrey++;
    }
}

// recursive function to get proper height 
int calculateHeightRecursively(Org* web, int* heights, int idx) {

    Org* organism = &web[idx];

    int maxHeight = 0;

    // loop through organism's prey
    for (int i = 0; i < organism->numPrey; i++) {
        int preyIndex = organism->prey[i];

        // recursively calculate height
        int preyHeight = calculateHeightRecursively(web, heights, preyIndex);

        // get maximum height
        if (preyHeight > maxHeight) {
            maxHeight = preyHeight;
        }
    }
    heights[idx] = maxHeight + 1;
    return heights[idx];
}

// Calculates height of each organism
void calculateHeight(Org* web, int numOrgs) {
    
    // array to store all height values
    int* heights = (int*)malloc(numOrgs * sizeof(int)); 

    // intializes all elements in heights to 0
    for (int i = 0; i < numOrgs; i++) {
        heights[i] = 0;
    }

    // loop through all organisms and caclculate the height
    for (int j = 0; j < numOrgs; j++) {
        if (heights[j] == 0) {
            calculateHeightRecursively(web, heights, j);
        }
    }

    for (int k = 0; k < numOrgs; k++) {
        printf("  %s: %d\n", web[k].name, heights[k] - 1);
    }

    // free allocated memory in heights
    free(heights);    
}

// removes organism from food web and makes necessary changes in web
void extinction(Org** web, int* numOrgs, int index) {

    // sets web to NULL and numOrgs to 0 if after extinction there are no more organisms
    if (*numOrgs - 1 == 0) {
        free((*web)[index].prey);
        free((*web));
        *web = NULL;
        *numOrgs = 0;
        return;
    }

    // frees prey array from the extinct organism
    free((*web)[index].prey);

    // allocates memory for new web, which contains 1 less organism
    Org* newWeb = (Org*)malloc((*numOrgs - 1) * sizeof(Org));
    
    // copies organisms from old web to new web, exlcuding extinct organism
    for (int i = 0, j = 0; i < *numOrgs; i++) {
        if (i != index) {
            newWeb[j]  = (*web)[i];
            j++;
        }
    }

    // frees memory of old web
    free(*web);
    
    // updates web pointer to the new web
    *web = newWeb;

    // decrements number of organisms
    (*numOrgs)--;   

    // updates prey indices in organisms to the left
    for (int i = 0; i < *numOrgs; i++) {

        Org* organism = &(*web)[i];

        // loops through all prey for given organism
        for (int j = 0; j < organism->numPrey; j++) {

            // finds organism that is going extinct
            if (organism->prey[j] == index) {

                // checks edge case if the only prey for an organism is going extinct
                if (organism->numPrey == 1) {
                    organism->prey = NULL;
                    organism->numPrey = 0;
                }
                else {
                    // allocates memory for new prey array with 1 less prey
                    int* newPrey = (int*)malloc((organism->numPrey - 1) * sizeof(int));

                    // copies prey indices, exlcuding the index for the extinct organism
                    for (int k = 0, z = 0; k < organism->numPrey; k++) {
                        if (k != j) {
                            newPrey[z] = organism->prey[k];
                            z++;
                        }
                    }

                    // frees old prey array and updates pointer
                    free(organism->prey);
                    organism->prey = newPrey;
                    organism->numPrey--;
                }
            }
        }

        // updates prey indices that are higher than extinct organim's index
        for (int p = 0; p < organism->numPrey; p++) {
            if (organism->prey[p] > index) {
                organism->prey[p]--;
            }
        }
    }
}

// identifies all apex predators before extinction
void identifyApexPredators(Org* web, int numOrgs) {

    // loop through each organism in food web
    for (int i = 0; i < numOrgs; i++) {
        Org* organism = &web[i];
        
        bool isPred = true;

        // loops through all organisms to check if current organism is prey at all
        for (int j = 0; j < numOrgs; j++) {
            if (i != j) {

                Org* possiblePrey = &web[j];

                // loops through possible predator's prey
                for (int z = 0; z < possiblePrey->numPrey; z++) {
                    if (possiblePrey->prey[z] == i) {
                        isPred = false;
                        break;
                    }
                }
                if (!isPred) {
                    break;
                }
            }
        }
        if (isPred) {
            printf("  %s\n", organism->name);
        }
    }
}

// identifies the most flexible eaters before extinction
void identifyFlexibleEaters(Org* web, int numOrgs) {
    int maxCount = 0;
    int flexibleEaters[numOrgs];
    int flexibleEaterCount = 0;

    // loops through each organism in web
    for (int i = 0; i < numOrgs; i++) {
        Org* organism = &web[i];
        
        // gets prey count for each organism
        int preyCount = organism->numPrey;

        // finds the max prey count
        if (preyCount > maxCount) {
            maxCount = preyCount;
            flexibleEaterCount = 1;
            flexibleEaters[0] = i;
        }
        else if (preyCount == maxCount) {
            flexibleEaters[flexibleEaterCount] = i;
            flexibleEaterCount++;
        }
    }

    for (int j = 0; j < flexibleEaterCount; j++) {
        printf("  %s\n", web[flexibleEaters[j]].name);
    }
}

// identifies all producers before extinction
void identifyProducers(Org* web, int numOrgs) {

    for (int i = 0; i < numOrgs; i++) {
        // gets access to each organism
        Org* organism = &web[i];
        if (organism->numPrey == 0) {
            printf("  %s\n", organism->name);
        }
    }
}

// finds tastiest food in web
void identifyTastiestFood(Org* web, int numOrgs) {
    int maxCount = 0;
    int tastiestFoods[numOrgs];
    int tastiestFoodCount = 0;

    // loops through all organisms in web
    for (int i = 0; i < numOrgs; i++) {
        Org* organism = &web[i];

        int eatenCount = 0;

        // loops through all organisms
        for (int j = 0; j < numOrgs; j++) {
            Org* possiblePred = &web[j];

            // loops through possible prey for predators
            for (int z = 0; z < possiblePred->numPrey; z++) {
                if (possiblePred->prey[z] == i) {
                    eatenCount++;
                    break;
                }
            }
        }

        // gets max count of organisms that eat this
        if (eatenCount > maxCount) {
            maxCount = eatenCount;
            tastiestFoodCount = 1;
            tastiestFoods[0] = i;
        }
        else if (eatenCount == maxCount) {
            tastiestFoods[tastiestFoodCount] = i;
            tastiestFoodCount++;
        }
    }

    for (int k = 0; k < tastiestFoodCount; k++) {
        printf("  %s\n", web[tastiestFoods[k]].name);
    }
}

// prints out all carnivores in web
void listCarnivores(Org* web, int numOrgs) {
    printf("  Carnivores:\n");

    // loops through all organisms in web
    for (int i = 0; i < numOrgs; i++) {
        Org* organism = &web[i];

        // makes sure organism actually has prey
        if (organism->numPrey > 0) {
            bool eatsNonProd = true;

            // loops through all the prey
            for (int j = 0; j < organism->numPrey; j++) {
                int preyIndex = organism->prey[j];

                Org* prey = &web[preyIndex];

                // checks if its a producer
                if (prey->numPrey == 0) {
                    eatsNonProd = false;
                    break;
                }
            }

            if (eatsNonProd) {
                printf("    %s\n", organism->name);
            }
        }
    }
}

// lists all herbivores, which only eat producers
void listHerbivores(Org* web, int numOrgs) {
    // finds all herbivores in food web
    printf("  Herbivores:\n");
    for (int j = 0; j < numOrgs; j++) {
        Org* organism = &web[j];

        // makes sure it isn't a producer
        if (organism->numPrey > 0) {
            bool isHerb = true;

            // loops through prey and makes sure prey is a producer
            for (int k = 0; k < organism->numPrey; k++) {
                int preyIndex = organism->prey[k];
                Org* prey = &web[preyIndex];

                if (prey->numPrey > 0) {
                    isHerb = false;
                    break;
                }
            }

            if (isHerb) {
                printf("    %s\n", organism->name);
            }
        }
    }
}

// lists all omnivores in the web
void listOmnivores(Org* web, int numOrgs) {
    printf("  Omnivores:\n");

    // loops through all organisms
    for (int i = 0; i < numOrgs; i++) {
        Org* organism = &web[i];

        if (organism->numPrey > 0) {
            bool eatsProd = false;
            bool eatsNonProd = false;

            // loops through all prey in the web
            for (int j = 0; j < organism->numPrey; j++) {
                int preyIndex = organism->prey[j];

                Org* prey = &web[preyIndex];

                // updates producer/nonproducer flag
                if (prey->numPrey == 0) {
                    eatsProd = true;
                }
                else {
                    eatsNonProd = true;
                }
            }

            if (eatsProd && eatsNonProd) {
                printf("    %s\n", organism->name);
            }
        }
    }
}

// lists all producers in web
void listProducers(Org* web, int numOrgs) {
    printf("  Producers:\n");

    // loops through each organism
    for (int i = 0; i < numOrgs; i++) {
        Org* organism = &web[i];

        // makes sure organism has no prey, which makes it a producer
        if (organism->numPrey == 0) {
            printf("    %s\n", organism->name);
        }
    }
}

// prints food web
void printFoodWeb(Org* web, int numOrgs) {

    for (int i = 0; i < numOrgs; i++) {
        Org* organism = &web[i];

        printf("  %s", organism->name);

        // makes sure organism has prey
        if (organism->numPrey > 0) {
            printf(" eats ");

            for (int j = 0; j < organism->numPrey; j++) {
                int preyIdx = organism->prey[j];
                printf("%s", web[preyIdx].name);

                // puts comma and space if not last prey
                if (j < organism->numPrey - 1) {
                    printf(", ");
                }
            }
        }
        printf("\n");
    }
}

int main(void) {

    int numOrgs;
    printf("Welcome to the Food Web Application\n");
    printf("--------------------------------\n");
    printf("Enter number of organisms:\n");
    scanf("%d",&numOrgs);

    Org* web = NULL;
    if(numOrgs > 0) { //Do not malloc an empty array, leave it pointing to NULL
        web = (Org*)malloc(numOrgs * sizeof(Org));
    }
    
    printf("Enter names for %d organisms:\n", numOrgs);
    for (int i = 0; i < numOrgs; ++i) {
        scanf("%s",web[i].name);
        web[i].prey = NULL;
        web[i].numPrey = 0;
    }

    printf("Enter number of predator/prey relations:\n");
    int numRels;
    scanf("%d",&numRels);

    printf("Enter the pair of indices for the %d predator/prey relations\n",numRels);
    printf("the format is [predator index] [prey index]:\n");
    
    int predInd, preyInd;
    for (int i = 0; i < numRels; ++i) {
        scanf("%d %d",&predInd, &preyInd);
        buildWeb(web,numOrgs,predInd,preyInd);
    }
    printf("--------------------------------\n\n");
    
    printf("Food Web Predators & Prey:\n");
    printFoodWeb(web, numOrgs);
    printf("\n");

    // PRINTS ALL PREDATORS ON SCREEN
    printf("Apex Predators:\n");
    identifyApexPredators(web, numOrgs);
    printf("\n");

    // PRINTS ALL PRODUCERS ON SCREEN
    printf("Producers:\n");
    identifyProducers(web, numOrgs);
    printf("\n");

    // PRINTS MOST FLEXIBLE EATERS ON SCREEn
    printf("Most Flexible Eaters:\n");
    identifyFlexibleEaters(web, numOrgs);
    printf("\n");

    // PRINTS TASTIEST FOOD ON SCREEN
    printf("Tastiest Food:\n");
    identifyTastiestFood(web, numOrgs);
    printf("\n");

    // PRINTS HEIGHTS ON SCREEN
    printf("Food Web Heights:\n");
    calculateHeight(web, numOrgs);
    printf("\n");

    // PRINTS VORE TYPES ON SCREEN
    printf("Vore Types:\n");
    listProducers(web, numOrgs);
    listHerbivores(web, numOrgs);
    listOmnivores(web, numOrgs);
    listCarnivores(web, numOrgs);
    printf("\n");

    printf("--------------------------------\n");
    int extInd;
    printf("Enter extinct species index:\n");
    scanf("%d",&extInd);
    printf("Species Extinction: %s\n", web[extInd].name);
    extinction(&web,&numOrgs,extInd);
    printf("--------------------------------\n\n");

    // UPDATES WEB AFTER EXTINCTION
    printf("UPDATED Food Web Predators & Prey:\n");
    printFoodWeb(web, numOrgs);
    printf("\n");

    // UPDATES PREDATORS AFTER EXTINCTION
    printf("UPDATED Apex Predators:\n");
    identifyApexPredators(web, numOrgs);
    printf("\n");

    // UPDATES PRODUCERS AFTER EXTINCTION
    printf("UPDATED Producers:\n");
    identifyProducers(web, numOrgs);
    printf("\n");

    // UPDATES FLEXIBLE EATERS AFTER EXTINCTION
    printf("UPDATED Most Flexible Eaters:\n");
    identifyFlexibleEaters(web, numOrgs);
    printf("\n");

    // UPDATES TASTIEST FOOD AFTER EXTINCTION
    printf("UPDATED Tastiest Food:\n");
    identifyTastiestFood(web, numOrgs);
    printf("\n");

    // UPDATES HEIGHTS AFTER EXTINCTION
    printf("UPDATED Food Web Heights:\n");
    calculateHeight(web, numOrgs);
    printf("\n");

    // UPDATES VORE TYPES AFTER EXTINCTION
    printf("UPDATED Vore Types:\n");
    listProducers(web, numOrgs);
    listHerbivores(web, numOrgs);
    listOmnivores(web, numOrgs);
    listCarnivores(web, numOrgs);
    printf("\n");
    printf("--------------------------------\n");

    // frees all malloc'd memory to prevent potential leaks
    for (int i = 0; i < numOrgs; i++) {
        free(web[i].prey);
    }
    free(web);

    return 0;
}
