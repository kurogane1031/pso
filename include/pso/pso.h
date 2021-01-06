#include <cassert>
#include <cmath>
#include <ctime>
#include <random>
#include <vector>
#include <functional>
#include <algorithm>
#include <cstdlib>
#include <fmt/core.h>
#include <toml.hpp>

const double inf = std::numeric_limits<double>::max();

template <class InputIt1, class InputIt2, class InputIt3,
          class OutputIt, class doubleernaryOperation>
OutputIt transform(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt3 first3,
                   OutputIt d_first, doubleernaryOperation ternary_op){
  while(first1 != last1){
    *d_first++ = ternary_op(*first1++, *first2++, *first3++);
  }
  return d_first;
}

class Random
{
public:
	static void Init()
	{
		s_RandomEngine.seed(std::random_device()());
	}

	static double Double()
	{
		return (double)s_Distribution(s_RandomEngine) / (double)std::numeric_limits<uint64_t>::max();
	}

private:
	static std::mt19937 s_RandomEngine;
	static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
};

struct Swarm
{
  struct Best{
    std::vector<double> position; // Best X/positions/variables
    double fitness; // Best O, computed from objective
  };

  struct Particles{
    std::vector<double> position;
    std::vector<double> velocities;
    Best best_local;
  };

  Best best_global;
  std::vector<Particles> particles;
  Swarm()=default;
  Swarm(const int& n_var, const int& n_particles);
}; // end Swarm type

class PSO {
private:
  const int n_var = 2; // Number of position
  std::vector<double> lower_bound; // lower bound limit for position
  std::vector<double> upper_bound; // upper bound limit for position
  
  // ##############
  //  PSO PARAMEdoubleER
  const int number_of_particle = 10;
  const int max_iter = 500;
  const double min_inertia_weight = 0.2;
  const double max_inertia_weight = 0.9;
  const double weight_diff = max_inertia_weight - min_inertia_weight;
  const double c1 = 2; // cognitive component
  const double c2 = 2; // social component
  std::vector<double> min_velocity;
  std::vector<double> max_velocity;
  
    std::function<double(const std::vector<double>&)> func {nullptr};
  // ###################
  //  Initialize default
  Swarm swarm;
 public:
   PSO(const int &n_particle, const int &max_iter, const double &min_w,
       const double &max_w, const double &c1, const double &c2, const int &n_var,
       const std::vector<double> &lb, const std::vector<double> &ub,
       std::function<double(const std::vector<double> &)> f = nullptr);

  std::vector<double> get_best_x() const;
  // Initializing particle
  void init(const std::vector<double>& ub, const std::vector<double>& lb);

  double update_velocity(const double& w, const double& v,
                    const double& c1, const double& c2,
                    const double& r1, const double& r2,
                    const double& P, const double& G,
                    const double& position);

  double update_position(const double& position, const double& velocity);

  // main loop that starts the optimization and return a global best
  void run();
};


//  LocalWords:  PSO
