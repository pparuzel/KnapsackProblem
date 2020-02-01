#include <iostream>
#include <vector>
#include <random>
#include <algorithm>


using std::vector;
using std::size_t;
using chromosome_t = vector<bool>;
using population_t = vector<chromosome_t>;

struct Solution
{
    chromosome_t chromosome;
    size_t       fitness;
};

struct Item
{
    size_t weight;
    size_t value;
};

class KnapsackSolver
{
public:
    KnapsackSolver(
        vector<Item> items,
        size_t       capacity,
        size_t       population_size,
        size_t       iterations,
        float        mutation_rate)

        : items_{std::move(items)}
        , capacity_{capacity}
        , population_(population_size)
        , iterations_{iterations}
        , mutation_rate_{mutation_rate}
        , solution_{chromosome_t(items_.size(), {}), 0}
        , rng{[]{
            const auto seed = std::random_device{}();
            std::cout << "Seed=" << seed << '\n';
            return seed;
          }()}
    {
    }

    const Solution& solution() const
    {
        return solution_;
    }

    auto solve() -> decltype(solution())
    {
        vector<size_t> fitnesses(population_.size());
        // Generate the first population
        generate(
            begin(population_),
            end(population_),
            [this] { return generate_chromosome(); });

        for (size_t i = 0; i < iterations_; ++i) {
            std::cout << "Generation: " << i << '/' << iterations_;
            std::cout << '\r' << std::flush;
            // Evaluate fitness values
            transform(
                cbegin(population_), cend(population_),
                begin(fitnesses), [&] (auto&& chromosome) {
                    return evaluate(chromosome);
                });
            // Find the fittest chromosome
            auto best = max_element(begin(fitnesses), end(fitnesses));
            const auto best_fitness = *best;
            if (best_fitness > solution_.fitness) {
                solution_.fitness = best_fitness;
                solution_.chromosome = population_[best - begin(fitnesses)];
                std::cout << "New solution found: " << solution_.fitness;
                std::cout << " iteration=" << i << '\n';
            }
            // Start a new population with the current best chromosome
            population_t new_population = {solution_.chromosome};
            new_population.reserve(population_.size());
            const auto fitness_sum = accumulate(
                cbegin(fitnesses), cend(fitnesses), size_t{});
            // Generate remaining chromosomes based on selection and crossover
            generate_n(
                back_inserter(new_population), population_.size() - 1,
                [&] {
                    auto chromosome_1 = select(fitnesses, fitness_sum);
                    auto chromosome_2 = select(fitnesses, fitness_sum);
                    return crossover(chromosome_1, chromosome_2);
                });
            // Mutate some of the chromosomes
            for (auto& chromosome : new_population) { mutate(chromosome); }
            // Prepare the new population for next iterations
            population_ = std::move(new_population);
        }
        return solution();
    }

private:
    chromosome_t generate_chromosome() const
    {
        std::bernoulli_distribution coin_flip{0.5};

        chromosome_t chromosome;
        chromosome.reserve(items_.size());
        generate_n(
            back_inserter(chromosome),
            items_.size(),
            [&] { return coin_flip(rng); });
        return chromosome;
    }

    size_t evaluate(const chromosome_t& osobnik) const
    {
        // Get value of all items or 0 if weight exceeds capacity
        size_t weight = 0;
        size_t value = 0;
        for (const auto& item : items_) {
            if (osobnik[&item - items_.data()]) {
                weight += item.weight;
                value += item.value;
                if (weight > capacity_) {
                    return 0;
                }
            }
        }
        return value;
    }

    const chromosome_t& select(const vector<size_t>& fitnesses, size_t max) const
    {
        // Wheel roulette, find a chromosome based on some probability value 
        std::uniform_int_distribution<size_t> random_value{0, max};

        const auto it = find_if(
            cbegin(fitnesses),
            cend(fitnesses) - 1,
            [offset = size_t{}, pick = random_value(rng)]
            (auto fitness) mutable {
                return pick < (offset += fitness);
            });
        return population_[it - cbegin(fitnesses)];
    }

    void mutate(chromosome_t& chromosome)
    {
        // Flip random bits
        std::uniform_real_distribution<float> random_chance{};
        for (auto&& has_item : chromosome) {
            if (random_chance(rng) < mutation_rate_) {
                has_item = !has_item;
            }
        }
    }

    chromosome_t crossover(
        const chromosome_t& chromosome_1,
        const chromosome_t& chromosome_2)
    {
        // Slice two chromosomes at a point and join the left side
        // ...  of the first with the right side of the second
        std::uniform_int_distribution<size_t> random_index{0, items_.size()};

        auto slice = random_index(rng);
        chromosome_t new_chromosome(items_.size());
        copy(
            begin(chromosome_2) + slice,
            end(chromosome_2),
            copy_n(
                begin(chromosome_1),
                slice,
                begin(new_chromosome))
        );
        return new_chromosome;
    }

private:
    vector<Item> items_;
    size_t       capacity_;
    population_t population_;
    size_t       iterations_;
    float        mutation_rate_;
    Solution     solution_;

    mutable std::default_random_engine rng;
};

int main(int argc, char* argv[])
{
    auto items = vector<Item>{
        // waga,  wartosc
        {382745,  825594},
        {799601, 1677009},
        {909247, 1676628},
        {729069, 1523970},
        {467902,  943972},
        {44328 ,   97426},
        {34610 ,   69666},
        {698150, 1296457},
        {823460, 1679693},
        {903959, 1902996},
        {853665, 1844992},
        {551830, 1049289},
        {610856, 1252836},
        {670702, 1319836},
        {488960,  953277},
        {951111, 2067538},
        {323046,  675367},
        {446298,  853655},
        {931161, 1826027},
        {31385 ,   65731},
        {496951,  901489},
        {264724,  577243},
        {224916,  466257},
        {169684, 369261}};
    auto solver = KnapsackSolver(
        std::move(items),
        6404180,  // capacity
        1000,     // population size
        1000,     // iterations
        0.001);    // mutation rate

    auto result = solver.solve();

    std::cout << "Current solution: " << result.fitness << ", ";
    std::cout << "Chromosome: ";
    for (bool bit : result.chromosome) { std::cout << bit; }
    std::cout << "\nGlobal solution:  13549094, ";
    std::cout << "Chromosome: 110111000110100100000111";
    std::cout << std::endl;
}
