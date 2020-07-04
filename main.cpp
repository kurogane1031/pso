#include "pso.h"

typedef double type_;

int main(){
  // srand(time(NULL));
  int nVar = 10;
  std::vector<type_> upper_bound, lower_bound; // Initial search space variables
  upper_bound.resize(nVar, 10);
  lower_bound.resize(nVar, -10);

  PSO<type_> pso(
      30, 500, 0.2, 0.9, 2, 2, nVar, lower_bound, upper_bound,
      static_cast<type_(*)(const std::vector<type_> &)>(sphere));
  pso.run();

  return 0;
}
