#include "pso/pso.h"

auto lambaGetVelocity(){
    // get_min_velocity = lower - upper
    // get_max_velocity = upper - lower
    return [](const double& first, const double& second) -> double
    {
      return 0.2 * (first - second);
    };
}

std::mt19937 Random::s_RandomEngine;
std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;

Swarm::Swarm(const int& n_var, const int& n_particles){
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


PSO::PSO(const int &n_particle, const int &max_iter, const double &min_w,
            const double &max_w, const double &c1, const double &c2, const int &n_var,
            const std::vector<double> &lb, const std::vector<double> &ub,
            std::function<double(const std::vector<double> &)> f)
    : n_var(n_var), lower_bound(lb), upper_bound(ub),
      number_of_particle(n_particle), max_iter(max_iter),
      min_inertia_weight(min_w), max_inertia_weight(max_w), c1(c1), c2(c2) {
    func = f;
    swarm = Swarm(n_var, number_of_particle);
    Random::Init();

    auto compute_lower_minus_upper = lambaGetVelocity();
    auto compute_upper_minus_lower = lambaGetVelocity();
    min_velocity.reserve(lb.size());
    max_velocity.reserve(ub.size());
    std::transform(lb.begin(),
                   lb.end(),
                   ub.begin(),
                   std::back_inserter(min_velocity),
                   compute_lower_minus_upper);

    std::transform(ub.begin(),
                   ub.end(),
                   lb.begin(),
                   std::back_inserter(max_velocity),
                   compute_upper_minus_lower);

    init(ub, lb);
}

void PSO::init(const std::vector<double>& ub, const std::vector<double>& lb){
    for(auto& particle:swarm.particles)
    {
        std::transform(ub.cbegin(),
                       ub.cend(),
                       lb.cbegin(),
                       particle.position.begin(),
                       [](double t_upper, double t_lower)
                       {
                           return (t_upper - t_lower) * Random::Double() + t_lower;
                       });
    }
}


double PSO::update_velocity(const double& w, const double& v,
                          const double& c1, const double& c2,
                          const double& r1, const double& r2,
                          const double& P, const double& G,
                          const double& position){
    return (w * v + c1 * r1 * (P - position) + c2 * r2 * (G - position));
}


double PSO::update_position(const double& position, const double& velocity){
    return position + velocity;
}


void PSO::run(){
    // Start iteration
    std::vector<double> track_gbest;
    track_gbest.reserve(max_iter);
    std::vector<int> track_iter;
    track_iter.reserve(max_iter);

    for(int curr_iter = 0; curr_iter < max_iter; ++curr_iter){
        for (auto& particle:swarm.particles)
        {
            auto current_x = particle.position;
            auto new_fitness = func(current_x);
            auto current_local_fitness = particle.best_local.fitness;

            if(new_fitness < current_local_fitness)
            {
                particle.best_local.position = current_x;
                particle.best_local.fitness = new_fitness;
            }

            auto current_global_fitness = swarm.best_global.fitness;
            if(new_fitness < current_global_fitness)
            {
                swarm.best_global.position = current_x;
                swarm.best_global.fitness = new_fitness;
            }
        }

        // update inertia weight
        const auto w = max_inertia_weight - weight_diff * curr_iter / max_iter;

        // update exploration and exploitation
        for(auto& particle:swarm.particles)
        {
            for (int i = 0; i < n_var; ++i) {
                particle.velocities[i] = update_velocity(
                    w, particle.velocities[i], c1, c2, Random::Double(), Random::Double(),
                    particle.best_local.position[i], swarm.best_global.position[i],
                    particle.position[i]); // update velocity vector
            }

            ::transform(particle.velocities.begin(),
                        particle.velocities.end(),
                        min_velocity.cbegin(),
                        max_velocity.cbegin(),
                        particle.velocities.begin(),
                        [](double x, double min, double max) {
                            return std::min(std::max(min, x), max);
                        });

            // update position
            std::transform(particle.position.begin(), particle.position.end(),
                           particle.velocities.cbegin(), particle.position.begin(),
                           std::plus<>());

            ::transform(particle.position.begin(),
                        particle.position.end(),
                        lower_bound.cbegin(),
                        upper_bound.cbegin(),
                        particle.position.begin(),
                        [](double x, double min, double max){
                            return std::min(std::max(min, x), max);
                        });
        }

        track_gbest.emplace_back(swarm.best_global.fitness);
        track_iter.emplace_back(curr_iter);
    }
}


std::vector<double> PSO::get_best_x() const
{
    return swarm.best_global.position;
}
