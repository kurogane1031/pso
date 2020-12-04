#include "pso.h"

template <typename T>
auto lambaGetVelocity(){
    // get_min_velocity = lower - upper
    // get_max_velocity = upper - lower
    return [](const T& first, const T& second) -> T
    {
      return 0.2 * (first - second);
    };
}

template <typename T>
Swarm<T>::Swarm(const int& n_var, const int& n_particles){
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

template <typename T>
PSO<T>::PSO(const int &n_particle, const int &max_iter, const T &min_w,
            const T &max_w, const T &c1, const T &c2, const int &n_var,
            const std::vector<T> &lb, const std::vector<T> &ub,
            std::function<T(const std::vector<T> &)> f)
    : n_var(n_var), lower_bound(lb), upper_bound(ub),
      number_of_particle(n_particle), max_iter(max_iter),
      min_inertia_weight(min_w), max_inertia_weight(max_w), c1(c1), c2(c2) {
    func = f;
    swarm = Swarm<T>(n_var, number_of_particle);

    auto compute_lower_minus_upper = lambaGetVelocity<T>();
    auto compute_upper_minus_lower = lambaGetVelocity<T>();
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

template <typename T>
const std::vector<T> PSO<T>::random_float(){
    std::vector<T> rand;
    rand.reserve(n_var);
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> distribution(0, 1);

    for(int idx = 0; idx < n_var; idx++){
        rand.emplace_back(distribution(mt));
    }

    return rand;
}


template <typename T>
void PSO<T>::init(const std::vector<T>& ub, const std::vector<T>& lb){
    for(auto& particle:swarm.particles)
    {
        std::vector<T> rand = random_float();
        ::transform(rand.cbegin(),
                    rand.cend(),
                    ub.cbegin(),
                    lb.cbegin(),
                    particle.position.begin(),
                    [](T t_random, T t_upper, T t_lower)
                    {
                        return (t_upper - t_lower) * t_random + t_lower;
                    });
    }
}

template <typename T>
T PSO<T>::update_velocity(const T& w, const T& v,
                          const T& c1, const T& c2,
                          const T& r1, const T& r2,
                          const T& P, const T& G,
                          const T& position){
    return (w * v + c1 * r1 * (P - position) + c2 * r2 * (G - position));
}

template <typename T>
T PSO<T>::update_position(const T& position, const T& velocity){
    return position + velocity;
}

template <typename T>
void PSO<T>::run(){
    // Start iteration
    std::vector<T> track_gbest;
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
        const std::vector<T> rand1 = random_float();
        const std::vector<T> rand2 = random_float();
        for(auto& particle:swarm.particles)
        {
            for (int i = 0; i < n_var; ++i) {
                particle.velocities[i] = update_velocity(
                    w, particle.velocities[i], c1, c2, rand1[i], rand2[i],
                    particle.best_local.position[i], swarm.best_global.position[i],
                    particle.position[i]); // update velocity vector
            }

            ::transform(particle.velocities.begin(),
                        particle.velocities.end(),
                        min_velocity.cbegin(),
                        max_velocity.cbegin(),
                        particle.velocities.begin(),
                        [](T x, T min, T max) {
                            return std::min(std::max(min, x), max);
                        });

            // update position
            std::transform(particle.position.begin(), particle.position.end(),
                           particle.velocities.cbegin(), particle.position.begin(),
                           std::plus<T>());

            ::transform(particle.position.begin(),
                        particle.position.end(),
                        lower_bound.cbegin(),
                        upper_bound.cbegin(),
                        particle.position.begin(),
                        [](T x, T min, T max){
                            return std::min(std::max(min, x), max);
                        });
        }

        track_gbest.emplace_back(swarm.best_global.fitness);
        track_iter.emplace_back(curr_iter);
    }
}

template <typename T>
std::vector<T> PSO<T>::get_best_x() const
{
    return swarm.best_global.position;
}

template class PSO<int>;
template class PSO<double>;
template class PSO<float>;

