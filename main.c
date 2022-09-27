#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leak_detector_c.h"

typedef struct monster {
  char *name;
  char *element;
  int population;
} monster;

typedef struct region {
  char *name;
  int nmonsters;
  int total_population;
  monster **monsters;
} region;

typedef struct itinerary {
  int nregions;
  region **regions;
  int captures;
} itinerary;

typedef struct trainer {
  char *name;
  itinerary *visits;
} trainer;

monster **readMonsters(int *monsterCount);
monster *makeMonster(char *name, char *element, int population);
region **readRegions(int *countRegions, monster **monsterList,
                     int monsterCount);
trainer *loadTrainers(int *tranierCount, region **regionList, int countRegions);
void processInputs(monster **monsterList, int monsterCount, region **regionList,
                   int regionCount, trainer *trainerList, int trainerCount);

void releaseMemory(monster **monsterList, int monsterCount, region **regionList,
                   int regionCount, trainer *trainerList, int trainerCount);

int main() {

  // Initialize memory leak report
  atexit(report_mem_leak);

  int monsterCount;
  int countRegions;
  int trainerCount;

  monster **monsterList = readMonsters(&monsterCount);

  region **regionList = readRegions(&countRegions, monsterList, monsterCount);

  trainer *trainerList = loadTrainers(&trainerCount, regionList, countRegions);

  processInputs(monsterList, monsterCount, regionList, countRegions,
                trainerList, trainerCount);

  releaseMemory(monsterList, monsterCount, regionList, countRegions,
                trainerList, trainerCount);

  return 0;
}

monster *makeMonster(char *name, char *element, int population) {

  monster *m = malloc(sizeof(monster));

  m->name = malloc((strlen(name) + 1) * sizeof(char));

  m->element = malloc((strlen(element) + 1) * sizeof(char));

  strcpy(m->name, name);
  strcpy(m->element, element);

  m->population = population;

  return m;
}

monster **readMonsters(int *monsterCount) {
  char monsterName[51];
  char monsterElement[51];
  int pop;
  char garbage[51];

  // garbage is strings
  scanf("%d %s", monsterCount, garbage);
  monster **ptr = malloc((*monsterCount) * sizeof(monster *));

  // Readinput from the user
  for (int i = 0; i < *monsterCount; i++) {
    scanf("%s %s %d", monsterName, monsterElement, &pop);
    ptr[i] = makeMonster(monsterName, monsterElement, pop);
  }
  return ptr;
}

region **readRegions(int *countRegions, monster **monsterList,
                     int monsterCount) {
  char garbage[51];
  char regionName[51];
  int mCount;
  char mName[51];
  int result;

  // input number of regions
  scanf("%d %s", countRegions, garbage);

  // allocate array of region pointers
  region **ptr2 = malloc((*countRegions) * sizeof(region *));

  // Allocate each region pointer
  for (int i = 0; i < *countRegions; i++) {
    ptr2[i] = malloc(sizeof(region));

    scanf("%s", regionName);          // ask for region name
    scanf("%d %s", &mCount, garbage); // ask for monster count

    // Allocate each region name
    ptr2[i]->name = malloc((strlen(regionName) + 1) * sizeof(char));
    strcpy(ptr2[i]->name, regionName);
    ptr2[i]->nmonsters = mCount;
    // allocate memory for array of monster pointers for region i so that we can
    // store previously dynamically allocated monsters
    ptr2[i]->monsters = malloc(mCount * sizeof(monster *));

    int total_population = 0;

    // Fill up monster at region
    for (int j = 0; j < mCount; j++) { // go though each monster

      scanf("%s", mName); // get monster's name
      for (int z = 0; z < monsterCount; z++) {
        result = strcmp(
            mName,
            monsterList[z]->name); // compare region's monster with monsterList
        if (result == 0) { // if they are equal set the reion's monsters member
          ptr2[i]->monsters[j] = monsterList[z];
          total_population += monsterList[z]->population;
        }
      }
    }
    ptr2[i]->total_population = total_population;
  }

  return ptr2;
}

trainer *loadTrainers(int *tranierCount, region **regionList,
                      int countRegions) {
  char garbage[51];
  char trainerName[51], regionName[51];
  int captures;

  // Ask for number of trainer
  scanf("%d %s", tranierCount, garbage);

  // allocated array of trainers
  trainer *ptr3 = malloc((*tranierCount) * sizeof(trainer));

  // Fill up each trainer
  for (int i = 0; i < *tranierCount; i++) {
    scanf("%s", trainerName); // asked trainer name

    // -- assign the trainer name-----
    int length = strlen(trainerName) + 1;
    ptr3[i].name = malloc(length * sizeof(char));
    strcpy(ptr3[i].name, trainerName);

    // first let's build the visits itinerary and then assign it later
    // this just makes it easier to work with
    itinerary *visits = malloc(sizeof(itinerary));
    scanf("%d %s", &visits->captures, garbage);
    scanf("%d %s", &visits->nregions, garbage);

    // store the number of regions in a varibable for ease of use
    int nregions = visits->nregions;
    region **TrainerRegions = malloc(nregions * sizeof(region *));

    // now for every region that the trainor did go through we look for
    // a possible matching region in the region list so we can just copy its
    // data
    for (int j = 0; j < nregions; j++) {
      scanf("%s", regionName); // asked for regionName

      // go over the region list
      for (int z = 0; z < countRegions;
           z++) { // check if the regionName = name of region from regionList
        int result = strcmp(regionName, regionList[z]->name);
        if (result == 0) {
          TrainerRegions[j] = regionList[z];
          break;
        }
      }
    }

    // -------------------- NOW we assign the filled trainer regions as needed
    // ------------
    visits->regions = TrainerRegions;
    ptr3[i].visits = visits;
  }

  return ptr3;
}

void processInputs(monster **monsterList, int monsterCount, region **regionList,
                   int regionCount, trainer *trainerList, int trainerCount) {

  // we are working with trainers so the most outer loop should process the
  // input based on iterating over the available trainers, so we chose to go
  // over the trainerlist first through this loop
  for (int t = 0; t < trainerCount; t++) {

    // for each current trainor we print their name first
    printf("%s\n", trainerList[t].name);

    // then we get the number of regions the current trainer has covered, so we
    // can loop through all of the regions covered by the trainer, we store that
    // count for ease of use in a variable
    int trainer_nregion = trainerList[t].visits->nregions;

    // we go over the available regions for the current trainer
    for (int tr = 0; tr < trainer_nregion; tr++) {

      // as we go through each of the regions covered by the current trainor we
      // firstprint the name of the current region we are processing
      printf("%s\n", trainerList[t].visits->regions[tr]->name);

      // after printing the name of the region
      // for every region we need to check the monster available in that region
      // so for each region we iterate over the available monsters in it
      // through this loop
      for (int trm = 0; trm < trainerList[t].visits->regions[tr]->nmonsters;
           trm++) {

        int count = round(
            (1.0 *
             trainerList[t].visits->regions[tr]->monsters[trm]->population /
             trainerList[t].visits->regions[tr]->total_population) *
            trainerList[t].visits->captures);

        // check if the count actually exists and that its not 0 as if it is 0
        // we simply don't care about it
        if (count != 0) {
          printf("%d#%s\n", count,
                 trainerList[t].visits->regions[tr]->monsters[trm]->name);
        }
      }
    }

    // we only add new lines if we are not at the end
    // this if for passing th output test case only
    // here check if you are at the last index trainer count - 1, if you are
    // then don't print a new line, otherwise print a new line, simple if
    // statement :)
    if (t != (trainerCount - 1)) {
      printf("\n");
    }
  }
}

void releaseMemory(monster **monsterList, int monsterCount, region **regionList,
                   int regionCount, trainer *trainerList, int trainerCount) {
  for (int m = 0; m < monsterCount; m++) {
    free(monsterList[m]->name);
    free(monsterList[m]->element);
    free(monsterList[m]);
  }
  free(monsterList);

  for (int r = 0; r < regionCount; r++) {
    free(regionList[r]->name);
    free(regionList[r]->monsters);
    free(regionList[r]);
  }
  free(regionList);

  for (int t = 0; t < trainerCount; t++) {
    free(trainerList[t].name);
    free(trainerList[t].visits->regions);
    free(trainerList[t].visits);
  }
  free(trainerList);
}
