#include "matplotlib-cpp/matplotlibcpp.h"
#include <cassert>
#include <cmath>
#include <ctime>
#include <iostream>
#include <random>
#include <vector>
#include <iomanip>


namespace plt=matplotlibcpp;
const float inf = 100000000.f;

template <class T>
struct Swarm{
  const int number_of_particles = 1;
  struct Particles{
    std::vector<T> X;
    std::vector<T> V;
    std::vector<T> P_BEST_X;
    T P_BEST_O = (T)inf;
  };
  std::vector<T> G_BEST_X;
  T G_BEST_O = (T)inf;
  std::vector<Particles> particles;

  Swarm(){
    particles.reserve(number_of_particles);
  }

  Swarm(const int& nVar, const int& number_of_particles)
      : number_of_particles(number_of_particles){
    particles.resize(number_of_particles);
    for(auto it = particles.begin(); it != particles.end(); ++it){
      it->X.resize(nVar, 0.0);
      it->V.resize(nVar, 0.0);
      it->P_BEST_X.resize(nVar, 0.0);
      it->P_BEST_O = inf;
    }
    G_BEST_O = inf;
    G_BEST_X.resize(nVar, 0.0);
  }
};

template <class T>
T get_max_velocity(T lower, T upper) {
  return 0.2 * (upper - lower);
}

template <class T>
T get_min_velocity(T lower, T upper) {
  return 0.2 * (lower - upper);
}

template <class T>
T objective_sphere(const std::vector<T> &vec) {
  // objective function to optimize
  T temp = 0.0f;
  for (auto it=vec.begin(); it != vec.end(); ++it) {
    temp += std::pow(*it, 2);
  }
  return temp;
}

template <class T>
T objective_table(const std::vector<T> &vec) {
  return 1*(vec[0]+vec[1]);
}

template <class T>
class PSO {
private:
  const int nVar = 2; // Number of variables
  std::vector<T> lower_bound; // lower bound limit for variables
  std::vector<T> upper_bound; // upper bound limit for variables
  
  // ##############
  //  PSO PARAMETER
  const int number_of_particle = 30;
  const int max_iter = 500;
  const T min_inertia_weight = 0.2;
  const T max_inertia_weight = 0.9;
  const T weight_diff = max_inertia_weight - min_inertia_weight;
  const T c1 = 2; // cognitive component
  const T c2 = 2; // social component
  std::vector<T> min_velocity;
  std::vector<T> max_velocity;
  
  // ###################
  //  Initialize default
  Swarm<T> swarm = Swarm<T>(nVar, number_of_particle);
 public:
  PSO(const int& n_particle, const int& max_iter, const T& min_w, const T& max_w,
      const T& c1, const T& c2, const int& nVar,
      const std::vector<T>& lb, const std::vector<T>& ub)
      : number_of_particle(n_particle),
        max_iter(max_iter),
        min_inertia_weight(min_w),
        max_inertia_weight(max_w),
        c1(c1), c2(c2), nVar(nVar),
        lower_bound(lb), upper_bound(ub){
    
    assert(swarm.particles.size() == number_of_particle);

    std::transform(lb.begin(), lb.end(), ub.begin(),
                   std::back_inserter(min_velocity), get_min_velocity<T>);
    std::transform(lb.begin(), lb.end(), ub.begin(),
                   std::back_inserter(max_velocity), get_max_velocity<T>);

    init(ub, lb);
  }

  // creates x number of uniformly distributed random
  const std::vector<T> random_float(){
    std::vector<T> rand(nVar, 0.0);
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
      for(int iter = 0; iter < nVar; ++iter){
        it->X[iter] = (ub[iter] - lb[iter]) * rand[iter] + lb[iter];
        it->V[iter] = 0.0;
        it->P_BEST_X[iter] = 0.0;
      }
    }
  }

  // main loop that starts the optimization and return a global best
  void run(){
    // Start iteration
    std::vector<T> track_gbest(max_iter);
    std::vector<int> track_iter(max_iter);
    for(int curr_iter = 0; curr_iter < max_iter; ++curr_iter){
      for(auto it = swarm.particles.begin(); it != swarm.particles.end(); ++it){
        std::vector<T> curr_X = it->X;

        T curr_O = objective_sphere(curr_X);

        // update PBEST if current objective value is better than personal objective value
        if(curr_O < it->P_BEST_O){
          it->P_BEST_X = curr_X;
          it->P_BEST_O = curr_O;
        }

        // update GBEST if current objective value is better than global objective value
        if(curr_O < swarm.G_BEST_O){
          swarm.G_BEST_X = curr_X;
          swarm.G_BEST_O = curr_O;
        }
      }

      // update inertia weight
      const T w = max_inertia_weight - weight_diff * curr_iter / max_iter;

      // update exploration and exploitation
        const std::vector<T> r1 = random_float();
        const std::vector<T> r2 = random_float();
      for (auto it = swarm.particles.begin(); it != swarm.particles.end(); ++it){
        for (int i = 0; i < nVar; i++) {
          it->V[i] = w * it->V[i] + (c1 * r1[i] * (it->P_BEST_X[i] - it->X[i])) + (c2 * r2[i] * (swarm.G_BEST_X[i] - it->X[i])); // update velocity vector
          if (it->V[i] < min_velocity[i]) {
            it->V[i] = min_velocity[i];
          }
          if (it->V[i] > max_velocity[i]) {
            it->V[i] = max_velocity[i];
          }
          it->X[i] = it->X[i] + it->V[i]; //update position vector
          if(it->X[i] < lower_bound[i]){
            it->X[i] = lower_bound[i];
          }
          if(it->X[i] > upper_bound[i]){
            it->X[i] = upper_bound[i];
          }
        }
      }
      track_gbest[curr_iter] = swarm.G_BEST_O;
      track_iter[curr_iter] = curr_iter;
      // std::cout << "Iteration# " << curr_iter + 1 <<"\t" << "G_Best_O = " << swarm.G_BEST_O << "\n";
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
