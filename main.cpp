#include "pso.h"

int main(){
  // srand(time(NULL));
  int nVar = 10;
  std::vector<double> upper_bound, lower_bound; // Initial search space variables
  upper_bound.resize(nVar, 10);
  lower_bound.resize(nVar, -10);

  PSO<double> pso(30, 500, 0.2, 0.9, 2, 2, nVar, lower_bound, upper_bound);
  pso.run();

  return 0;
}
