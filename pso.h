#include <cassert>
#include <cmath>
#include <ctime>
#include <random>
#include <vector>
#include <functional>
#include <algorithm>
#include <cstdlib>
#include <fmt/core.h>

const float inf = 100000000.f;

template <class InputIt1, class InputIt2, class InputIt3,
          class OutputIt, class TernaryOperation>
OutputIt transform(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt3 first3,
                   OutputIt d_first, TernaryOperation ternary_op){
  while(first1 != last1){
    *d_first++ = ternary_op(*first1++, *first2++, *first3++);
  }
  return d_first;
}


template <typename T>
struct Swarm{
  struct Best{
    std::vector<T> position; // Best X/positions/variables
    T fitness; // Best O, computed from objective
  };

  struct Particles{
    std::vector<T> position;
    std::vector<T> velocities;
    Best best_local;
  };

  Best best_global;
  std::vector<Particles> particles;
  Swarm()=default;
  Swarm(const int& n_var, const int& n_particles);
}; // end Swarm type

template <class T>
class PSO {
private:
  const int n_var = 2; // Number of position
  std::vector<T> lower_bound; // lower bound limit for position
  std::vector<T> upper_bound; // upper bound limit for position
  
  // ##############
  //  PSO PARAMETER
  const int number_of_particle = 10;
  const int max_iter = 500;
  const T min_inertia_weight = 0.2;
  const T max_inertia_weight = 0.9;
  const T weight_diff = max_inertia_weight - min_inertia_weight;
  const T c1 = 2; // cognitive component
  const T c2 = 2; // social component
  std::vector<T> min_velocity;
  std::vector<T> max_velocity;
  
  std::function<T(const std::vector<T>&)> func;
  // ###################
  //  Initialize default
  Swarm<T> swarm;
 public:
   PSO(const int &n_particle, const int &max_iter, const T &min_w,
       const T &max_w, const T &c1, const T &c2, const int &n_var,
       const std::vector<T> &lb, const std::vector<T> &ub,
       std::function<T(const std::vector<T> &)> f);

  // creates x number of uniformly distributed random
  const std::vector<T> random_float();

  std::vector<T> get_best_x() const;
  // Initializing particle
  void init(const std::vector<T>& ub, const std::vector<T>& lb);

  T update_velocity(const T& w, const T& v,
                    const T& c1, const T& c2,
                    const T& r1, const T& r2,
                    const T& P, const T& G,
                    const T& position);

  T update_position(const T& position, const T& velocity);

  // main loop that starts the optimization and return a global best
  void run();
};


//  LocalWords:  PSO
