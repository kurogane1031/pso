#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <cstdlib>
#include "timer.h"
struct Member{
  std::string DNA;
  int fitness;
};

struct Population{
  std::vector<Member> members = std::vector<Member>(50000);
};

int main(){
  Timer timer;
  const std::string DNA = "Hi, this is a string that we want to find using GA. Gratze";
  bool SequenceFound = false;
  const int MutationRate = 10;

  srand(time(NULL));

  Population population;

  // Create a population and initialize it with random DNA
  // Set fitness to 0
  for (int popIdx = 0; popIdx < population.members.size(); ++popIdx) {
    population.members.at(popIdx).DNA.resize(DNA.size());
    for(int dnaIdx = 0; dnaIdx < DNA.size(); ++dnaIdx){
      population.members.at(popIdx).DNA.at(dnaIdx) = static_cast<unsigned char>(rand() % 96 + 32);
    }
    population.members.at(popIdx).fitness = 0;
  }

  int generation = 0;

  while(!SequenceFound){
    generation++;

    // clears the fitness and reevaluate each member.
    // checks if fitness reaches maximum
    for (int popIdx = 0; popIdx < population.members.size(); ++popIdx){
      population.members.at(popIdx).fitness = 0;
      for(int dnaIdx = 0; dnaIdx < population.members.at(popIdx).DNA.size(); ++dnaIdx){
        if (population.members.at(popIdx).DNA.at(dnaIdx) == DNA.at(dnaIdx)){
          population.members.at(popIdx).fitness += 10;
        }
      }
      if(population.members.at(popIdx).fitness == DNA.size() * 10){
        SequenceFound = true;
      }
    }

    // sort the population by fitness, from highest to lowest
    std::sort(population.members.begin(), population.members.end(),
              [](Member const &lhs, Member const &rhs) {
                return lhs.fitness > rhs.fitness;
              });

    //select x amount of highest fitness member to pair from
    // lets use 2 parents in this case
    std::vector<Member> Parents{population.members.at(0), population.members.at(1)};

    // perform gene permutation and mating
    for(int popIdx = 0; popIdx < population.members.size(); ++popIdx){
      for(int dnaIdx = 0; dnaIdx < population.members.at(popIdx).DNA.size(); ++dnaIdx){

        // use an equal chance to take each parents gene (on a per gene basis)
        // later random mutation will be applied
        int tempSelectionIdx = rand() % Parents.size();
        population.members.at(popIdx).DNA.at(dnaIdx) = Parents.at(tempSelectionIdx).DNA.at(dnaIdx);


        // apply random mutation
        if(rand() % 1000 < MutationRate){
          population.members.at(popIdx).DNA.at(dnaIdx) = static_cast<unsigned char>(rand() % 96 + 32);
        }
      }
    }
    std::system("clear");
    std::cout << "Generation : " << generation
              << " Highest fitness: " << Parents.at(0).fitness
              << " with sequence: " << Parents.at(0).DNA.c_str() << "\n";
  }

  std::cout << "Generation " << generation << " Evolved to the full sequence.\n";
  return 0;
}
