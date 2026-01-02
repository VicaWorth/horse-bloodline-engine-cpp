#include "Simulation.hpp"
#include "GeneticsCore.cpp"

#include <array>
#include <iostream>
#include <random>

// need to control that sex has to be a 'F' or a 'M'.
struct Horse {
	char sex;
	Genotype<Locus> genotype;

	Horse(char s, Genotype<Locus> g)
		: sex(s), genotype(g) {
	}

	void showGene(Locus l) {
		std::cout << "   " << genotype.getGene(l).toString() << std::endl;
	}

	void showAllGenes() {
		for (const auto& [locus, gene] : genotype) {
			showGene(locus);
		}
	}
};

template <typename A>
struct Punnett {
	std::array<std::array<A, 2>, 2> punnett;

	Punnett(const A& tl, const A& tr, const A& bl, const A& br)
		: punnett{ {
			{ tl, tr },
			{ bl, br }
		} }
	{
	}
};

Punnett<Gene> generatePunnett(const Gene& sAlleles, const Gene& dAlleles) {
	// This is the correct aggregate initialization for a struct containing a std::array.
	Punnett<Gene> cPunnett(
		Gene(sAlleles.getAllele1(), dAlleles.getAllele1()),
		Gene(sAlleles.getAllele1(), dAlleles.getAllele2()),
		Gene(sAlleles.getAllele2(), dAlleles.getAllele1()),
		Gene(sAlleles.getAllele2(), dAlleles.getAllele2())
	);
	return cPunnett;
}

// Looks at punnett square and generates frequency map
// takes frequency map and gets weighted distribution
// returns gene associated with that
Gene resolvePunnettSquare(const Punnett<Gene>& p) {
	static std::random_device rd;
	static std::mt19937 generator(rd());

	std::uniform_int_distribution<> distribution(0, 3);
	int choice = distribution(generator);

	int row = choice / 2;
	int col = choice % 2;

	return p.punnett[row][col];
}

Genotype<Locus> generateOffspringGenotype(
	const Genotype<Locus>& sGenotype,
	const Genotype<Locus>& dGenotype) {

	// consider lambda here
	std::unordered_map<Locus, Gene> foalGenes;
	for (const auto& [locus, sireGene] : sGenotype) {
		foalGenes.emplace(locus, std::move(resolvePunnettSquare(generatePunnett(sireGene, dGenotype.getGene(locus)))));
	}

	Genotype<Locus> foalGenotype(0, std::move(foalGenes));
	return foalGenotype;
}

Horse generateOffspring(const Horse& sire, const Horse& dam) {
	if (sire.sex != 'M' || dam.sex != 'F') {
		throw std::invalid_argument("First horse must be male and second must be female.");
	}

	auto foalGenotype = generateOffspringGenotype(sire.genotype, dam.genotype);
	Horse offspring{ 'U', foalGenotype }; // 'U' for unknown sex
	return offspring;
}