#include "pso.h"
#include "timer.h"
#include <fmt/ranges.h>
typedef double type_;

template <class T>
T sphere(const std::vector<T> &vec) {
  // objective function to optimize
  return std::inner_product(vec.begin(), vec.end(), vec.begin(), static_cast<T>(0));
}

template <class T>
T schwefel(const std::vector<T> &vec){
  T temp_i = 0.0f;
  for(size_t i = 0; i < vec.size(); ++i){
    T temp_j = 0.0f;
    for(size_t j = 0; j < (i + 1); j++){
      temp_j += vec[j];
    }
    temp_i += temp_j * temp_j;
  }
  return temp_i;
}

template <typename T>
T table(const std::vector<T> &vec) {
  return 1*(vec[0]+vec[1]);
}

int main(){
  std::system("clear");
  Timer timer;
  int nVar = 5;
  std::vector<type_> upper_bound, lower_bound; // Initial search space variables
  upper_bound.resize(nVar, 10);
  lower_bound.resize(nVar, -10);

  timer.Start();
  PSO<type_> pso(
      300, 1000, 0.2, 0.9, 2, 2, nVar, lower_bound, upper_bound,
      static_cast<type_(*)(const std::vector<type_> &)>(schwefel));
  pso.run();
  timer.Stop();
  auto best_optimize_x = pso.get_best_x();
  fmt::print("Optimize X values\n");
  for(size_t i = 0; i < best_optimize_x.size(); ++i)
  {
    fmt::print("X[{}] = {}\n", i, best_optimize_x[i]);
  }
  timer.computeTime();
  return 0;
}
