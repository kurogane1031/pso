#include "matplotlib-cpp/matplotlibcpp.h"
#include <cassert>
#include <cmath>
#include <ctime>
#include <iostream>
#include <random>
#include <vector>
#include <functional>

namespace plt=matplotlibcpp;
const float inf = 100000000.f;

template <class T>
T get_max_velocity(T lower, T upper) {
  return 0.2 * (upper - lower);
}

template <class T>
T get_min_velocity(T lower, T upper) {
  return 0.2 * (lower - upper);
}

template <class T>
T sphere(const std::vector<T> &vec) {
  // objective function to optimize
  T temp = 0.0f;
  for (auto it=vec.begin(); it != vec.end(); ++it) {
    temp += std::pow(*it, 2);
  }
  return temp;
}

template <class T>
T table(const std::vector<T> &vec) {
  return 1*(vec[0]+vec[1]);
}

template <class T>
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

  Swarm() = default;

  Swarm(const int& n_var, const int& n_particles){
    particles.resize(n_particles);
    for(auto& particle:particles){
      particle.position.resize(n_var, 0.0);
      particle.velocities.resize(n_var, 0.0);
      particle.best_local.position.resize(n_var, 0.0);
      particle.best_local.fitness = inf;
    }
    best_global.position.resize(n_var, 0.0);
    best_global.fitness = inf;

  }
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
  Swarm<T> swarm = Swarm<T>(n_var, number_of_particle);

 public:
  PSO(const int& n_particle, const int& max_iter, const T& min_w, const T& max_w,
      const T& c1, const T& c2, const int& n_var,
      const std::vector<T>& lb, const std::vector<T>& ub,
      std::function<T(const std::vector<T>&)> f)
      : number_of_particle(n_particle),
        max_iter(max_iter),
        min_inertia_weight(min_w),
        max_inertia_weight(max_w),
        c1(c1), c2(c2), n_var(n_var),
        lower_bound(lb), upper_bound(ub){
    
    assert(swarm.particles.size() == number_of_particle);
    // func = static_cast<T (*)(const std::vector<T> &)>(f);
    func = f;
    std::transform(lb.begin(), lb.end(), ub.begin(),
                       std::back_inserter(min_velocity), get_min_velocity<T>);
    std::transform(lb.begin(), lb.end(), ub.begin(),
                   std::back_inserter(max_velocity), get_max_velocity<T>);

    init(ub, lb);
  }

  // creates x number of uniformly distributed random
  const std::vector<T> random_float(){
    std::vector<T> rand(n_var, 0.0);
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<T> distribution(0, 1);

    for(auto& i:rand){
      i = distribution(mt);
    }
    return rand;
  }

  // Initializing particle
  void init(const std::vector<T>& ub, const std::vector<T>& lb){
    for (auto it = swarm.particles.begin(); it != swarm.particles.end(); ++it){
      std::vector<T> rand = random_float();
      for(int iter = 0; iter < n_var; ++iter){
        it->position[iter] = (ub[iter] - lb[iter]) * rand[iter] + lb[iter];
        it->velocities[iter] = 0.0;
        it->best_local.position[iter] = 0.0;
      }
    }
  }

  auto personal_best(const T& C1, const T& R1, const T& P, const T& position){
    return C1 * R1 * (P - position);
  }

  auto global_best(const T& C2, const T& R2, const T& G, const T& position){
    return C2 * R2 * (G - position);
  }

  // main loop that starts the optimization and return a global best
  void run(){
    // Start iteration
    std::vector<T> track_gbest(max_iter);
    std::vector<int> track_iter(max_iter);
    for(int curr_iter = 0; curr_iter < max_iter; ++curr_iter){
      for(auto it = swarm.particles.begin(); it != swarm.particles.end(); ++it){
        std::vector<T> curr_X = it->position;

        T curr_O = func(curr_X);

        // update PBEST if current objective value is better than personal objective value
        if(curr_O < it->best_local.fitness){
          it->best_local.position = curr_X;
          it->best_local.fitness = curr_O;
        }

        // update GBEST if current objective value is better than global objective value
        if(curr_O < swarm.best_global.fitness){
          swarm.best_global.position = curr_X;
          swarm.best_global.fitness = curr_O;
        }
      }

      // update inertia weight
      const T w = max_inertia_weight - weight_diff * curr_iter / max_iter;

      // update exploration and exploitation
        const std::vector<T> r1 = random_float();
        const std::vector<T> r2 = random_float();
      for (auto it = swarm.particles.begin(); it != swarm.particles.end(); ++it){
        for (int i = 0; i < n_var; i++) {
          it->velocities[i] = w * it->velocities[i] + personal_best(c1, r1[i], it->best_local.position[i], it->position[i])
                     + global_best(c2, r2[i], swarm.best_global.position[i], it->position[i]); // update velocity vector

          if (it->velocities[i] < min_velocity[i]) {
            it->velocities[i] = min_velocity[i];
          }
          if (it->velocities[i] > max_velocity[i]) {
            it->velocities[i] = max_velocity[i];
          }
          it->position[i] = it->position[i] + it->velocities[i]; //update position vector
          if(it->position[i] < lower_bound[i]){
            it->position[i] = lower_bound[i];
          }
          if(it->position[i] > upper_bound[i]){
            it->position[i] = upper_bound[i];
          }
        }
      }
      track_gbest[curr_iter] = swarm.best_global.fitness;
      track_iter[curr_iter] = curr_iter;
      std::cout << "Iteration# " << curr_iter + 1 <<"\t" << "G_Best_O = " << swarm.best_global.fitness << "\n";
    }
    // plt::plot(track_gbest);
    plt::semilogy(track_iter, track_gbest);
    // plt::ylim(0, 100);
    plt::xlabel("Iteration");
    plt::ylabel("Global Best Objective");
    plt::show();
  }
};

//  LocalWords:  PSO
