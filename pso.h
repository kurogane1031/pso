#include "matplotlib-cpp/matplotlibcpp.h"
#include <cassert>
#include <cmath>
#include <ctime>
#include <random>
#include <vector>
#include <functional>
#include <algorithm>
#include <cstdlib>
#include <fmt/core.h>

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
T schwefel(const std::vector<T> &vec){
  T temp_i = 0.0f;
  for(size_t i = 0; i < vec.size(); ++i){
    T temp_j = 0.0f;
    for(size_t j = 0; j < (i + 1); j++){
      temp_j += vec.at(j);
    }
    temp_i += temp_j * temp_j;
  }
  return temp_i;
}

template<class T>
void print_sth();

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

  Swarm(const Swarm &swarm)
      : best_global(swarm.best_global), particles(swarm.particles){
    std::cout << "Copied!\n";
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
   PSO(const int &n_particle, const int &max_iter, const T &min_w,
       const T &max_w, const T &c1, const T &c2, const int &n_var,
       const std::vector<T> &lb, const std::vector<T> &ub,
       std::function<T(const std::vector<T> &)> f)
       : n_var(n_var), lower_bound(lb), upper_bound(ub),
         number_of_particle(n_particle), max_iter(max_iter),
         min_inertia_weight(min_w), max_inertia_weight(max_w), c1(c1), c2(c2) {
     // assert(swarm.particles.size() == number_of_particle);
     // func = static_cast<T (*)(const std::vector<T> &)>(f);
     func = f;

     std::transform(lb.begin(),
                    lb.end(),
                    ub.begin(),
                    std::back_inserter(min_velocity),
                    get_min_velocity<T>);

     std::transform(lb.begin(),
                    lb.end(),
                    ub.begin(),
                    std::back_inserter(max_velocity),
                    get_max_velocity<T>);

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
        it->position.at(iter) = (ub.at(iter) - lb.at(iter)) * rand.at(iter) + lb.at(iter);
        it->velocities.at(iter) = 0.0;
        it->best_local.position.at(iter) = 0.0;
      }
    }
  }

  T update_velocity(const T& w, const T& v,
                    const T& c1, const T& c2,
                    const T& r1, const T& r2,
                    const T& P, const T& G,
                    const T& position){
    return (w * v + c1 * r1 * (P - position) + c2 * r2 * (G - position));
  }

  // void update_velocity(const T& weight,
  //                      const T& c1,
  //                      const T& c2,
  //                      std::vector<T> velocity,
  //                      const std::vector<T>& rand1,
  //                      const std::vector<T>& rand2,
  //                      const std::vector<T>& position,
  //                      const std::vector<T>& global_best
  //                      ){

  // }

  T update_position(const T& position, const T& velocity){
    return position + velocity;
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

      T lbb = lower_bound.at(0);
      T upp = upper_bound.at(0);
      // update exploration and exploitation
      const std::vector<T> rand1 = random_float();
      const std::vector<T> rand2 = random_float();
      for (auto it = swarm.particles.begin(); it != swarm.particles.end(); ++it){

        for (int i = 0; i < n_var; i++) {
          it->velocities.at(i) = update_velocity(
              w, it->velocities.at(i), c1, c2, rand1.at(i), rand2.at(i),
              it->best_local.position.at(i), swarm.best_global.position.at(i),
              it->position.at(i)); // update velocity vector
        }

        // std::transform(it->velocities.begin(),
        //                it->velocities.end(),
        //                it->velocities.begin(),
        //                [minn = std::cbegin(min_velocity)](auto vel) mutable {
        //                  if(std::less<T>{}(vel, *minn)){
        //                    return *minn++;
        //                  }
        //                  else{
        //                    minn++;
        //                    return vel;
        //                  }
        //                });

        // std::transform(it->velocities.begin(), it->velocities.end(),
        //                it->velocities.begin(),
        //                [maxx = std::cbegin(max_velocity)](auto vel) mutable {
        //                  if (std::greater<T>{}(vel, *maxx)) {
        //                    return *maxx++;
        //                  }
        //                  else {
        //                    maxx++;
        //                    return vel;
        //                  }
        //                });

        std::transform(it->velocities.begin(), it->velocities.end(),
                       it->velocities.begin(),
                       [minn = std::cbegin(min_velocity),
                        maxx = std::cbegin(max_velocity)](auto vel) mutable {
                         if (std::less<T>{}(vel, *minn)) {
                           maxx++;
                           return *minn++;
                         } else if (std::greater<T>{}(vel, *maxx)) {
                           minn++;
                           return *maxx++;
                         } else {
                           minn++;
                           maxx++;
                           return vel;
                         }
                       });

        // std::replace_if(it->velocities.begin(),
        //                 it->velocities.end(),
        //                 [&minn](const T& v){
        //                   return v < minn;
        //                 },
        //                 minn);
        
        //update position
        std::transform(it->position.begin(),
                       it->position.end(),
                       it->velocities.begin(),
                       it->position.begin(),
                       std::plus<T>());

        std::replace_if(it->position.begin(),
                        it->position.end(),
                        [&lbb](const T &p){
                          return p < lbb;
                        },
                        lbb);
        
        std::replace_if(it->position.begin(),
                        it->position.end(),
                        [&upp](const T &p){
                          return p > upp;
                        },
                        upp);
      }

      track_gbest[curr_iter] = swarm.best_global.fitness;
      track_iter[curr_iter] = curr_iter;
      std::system("clear");
      
      fmt::print("Iteration# {} \t Global Best O = {} \n", curr_iter+1, swarm.best_global.fitness);
      fmt::print("Best global X = \n");

      for(const auto& gX:swarm.best_global.position){
        fmt::print("{}\n", gX);
      }
    }

    // plt::semilogy(track_iter, track_gbest);
    // plt::xlabel("Iteration");
    // plt::ylabel("Global Best Objective");
    // plt::show();
  }
};

//  LocalWords:  PSO
