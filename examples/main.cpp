#include "pso/pso.h"
#include "utils/timer.h"
#include <fmt/ranges.h>
typedef double type_;

double sphere(const std::vector<double> &vec) {
  // objective function to optimize
  return std::inner_product(vec.begin(), vec.end(), vec.begin(), static_cast<double>(0));
}

double schwefel(const std::vector<double> &vec){
  double temp_i = 0.0f;
  for(size_t i = 0; i < vec.size(); ++i){
    double temp_j = 0.0f;
    for(size_t j = 0; j < (i + 1); j++){
      temp_j += vec[j];
    }
    temp_i += temp_j * temp_j;
  }
  return temp_i;
}

double table(const std::vector<double> &vec) {
  return 1*(vec[0]+vec[1]);
}

int main(){
  std::system("clear");
  Timer timer;
  int nVar = 5;
  std::vector<type_> upper_bound, lower_bound; // Initial search space variables
  upper_bound.resize(nVar, 10);
  lower_bound.resize(nVar, -10);

  const auto configurations = toml::parse("../config/config.toml");
  int number_of_particles = toml::find<int>(configurations, "number_of_particles");
  int maximum_iterations = toml::find<int>(configurations, "maximum_iterations");

  timer.Start();
  PSO pso(
      number_of_particles, maximum_iterations, 0.2, 0.9, 2, 2, nVar, lower_bound, upper_bound,
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
