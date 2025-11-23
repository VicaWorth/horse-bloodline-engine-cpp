#include <concepts>
#include <utility>
#include <iostream>
#include <string>
#include <initializer_list>
#include <unordered_map>
#include <stdexcept>
#include <exception>
#include <algorithm>
#include <ranges>   
#include <random>
#include <array>
#include <cctype>

class InvalidAlleles : public std::exception {
private:
	std::string message;
public:
	InvalidAlleles(const std::string& msg) : message(msg) {}

	virtual const char* what() const noexcept override {
		return message.c_str();
	}
};

enum class MaskLayers {
	Gray,
	White,
	Dilution,
	Base 
};

enum class Dominance {
	Dominant,
	Recessive,
	Codominant,
	InDominant  //IncompletelyDominant
};

enum class Locus {
	Extension,
	Agouti,
	KIT,
	Silver,
	Cream,
	Pearl,
	Dun,
	Champange,
	Gray,
	SplashWhite,
	FrameOvero,
	Flaxen,
	Pangare,
	Sooty,
	Leopard,
	MLeopard,
	Unknown
};

struct EpistasisNode {
	//Locus id;
	std::vector<Locus> locusParents;
	MaskLayers maskLayer;
};

using EpistasisGraph = std::unordered_map<Locus, EpistasisNode>;
EpistasisGraph epistasisDependencies;

struct Allele {
	std::string symbol; // Like 'A' or "Sty"
	Dominance dominance;
	Locus locus;

	std::unordered_map<std::string, Locus> symbolToLocusTable = {
		{"E", Locus::Extension},
		{"A", Locus::Agouti},
		{"RN", Locus::KIT},
		{"TO", Locus::KIT},
		{"SB1", Locus::KIT},
		{"Z", Locus::Silver},
		{"CR", Locus::Cream},
		{"NCR", Locus::Cream},
		{"PRL", Locus::Pearl} // add others later
	};

	Allele(std::string symbol) 
		: symbol(symbol)
	{
		std::string s = symbol;
		std::transform(s.begin(), s.end(), s.begin(),
			[](unsigned char c) { return std::toupper(c); });

		locus = symbolToLocusTable.at(s);
		if (isupper(symbol[0])) {
			dominance = Dominance::Dominant;
		} else {
			dominance = Dominance::Recessive;
		}

		if (locus == Locus::KIT) {
			// Represents Alleleic Series
			if (s == "W") {
				dominance = Dominance::Dominant;
			} else if (s == "RN") {
				dominance = Dominance::Codominant;
			} else if (s == "SB1") {
				dominance = Dominance::InDominant;
			} else if (s == "TO") {
				dominance = Dominance::Dominant;
			} else {
				dominance = Dominance::Recessive;
			}
		}

		if (locus == Locus::Cream) {
			dominance = Dominance::InDominant;
		}

		this->symbol = std::move(symbol); // no idea if this actually does anything
	}
};

class Gene {
private:
	std::pair<Allele, Allele> alleles;
public:
	const Allele& getAllele1() const { return alleles.first; }
	const Allele& getAllele2() const { return alleles.second; }

	//implement rule of 5
	Gene(Allele a1, Allele a2)
		: alleles(std::move(a1), std::move(a2)) 
	{
		if (alleles.first.locus != alleles.second.locus) {
			throw InvalidAlleles("Alleles are invalid. They are not apart of the same Locus.");
		}

		if (alleles.second.dominance == Dominance::Dominant) {
			std::swap(alleles.first, alleles.second);
		}
	}

	/*Gene(const Gene& other) = default;
	Gene& operator=(const Gene& other) = default;*/

	std::string toString() const {
		return (alleles.first.symbol + alleles.second.symbol);
	};

	constexpr bool isDominantPresent() const {
		return (alleles.first.dominance == Dominance::Dominant);
	};

	constexpr bool isRecessivePresent() const {
		return (alleles.first.dominance == Dominance::Recessive);
	};

	constexpr bool isCodominantPresent() const {
		return (alleles.first.dominance == Dominance::Codominant);
	};

	constexpr bool isInDominantPresent() const {
		return (alleles.first.dominance == Dominance::InDominant);
	};

	constexpr bool isHomozygous() const {
		return (alleles.first.symbol == alleles.second.symbol);
	};

	constexpr bool isHeterozygous() const {
		return (alleles.first.symbol != alleles.second.symbol);
	};
};

template <typename T>
class Genotype {
private:
	int m_id;
	std::unordered_map<Locus, Gene> m_genotype;

public:
	Genotype(int id, std::unordered_map<Locus, Gene>&& genotype_map)
		: m_id(id), m_genotype(std::move(genotype_map)) {
	}

	// Source - https://stackoverflow.com/a
	// Posted by Hajo Kirchhoff
	// Retrieved 2025-11-09, License - CC BY-SA 4.0

	auto begin() { return m_genotype.begin(); }
	auto end()   { return m_genotype.end(); }

	auto begin() const { return m_genotype.begin(); }
	auto end()   const { return m_genotype.end(); }

	Gene getGene(const Locus l) const {
		return m_genotype.at(l);
	}
};

template <typename A>
struct Punnett {
	std::array<std::array<A, 2>, 2> punnett;

	Punnett(const A& tl, const A& tr, const A& bl, const A& br) 
		: punnett{ {
			{ tl, tr },
			{ bl, br }
		}} 
	{ }
};

// need to control that sex has to be a 'F' or a 'M'.
struct Horse {
	char sex;
	Genotype<Locus> genotype;

	Horse(char s, Genotype<Locus> g)
		: sex(s), genotype(g) {
	}

	void showGenes(Locus l) {
		std::cout << "   " << genotype.getGene(l).toString() << std::endl;
	}
};

// Locus Graph Related
void epistasisGraphConstructor() {
	epistasisDependencies.insert({
		Locus::Extension, { 
			{}, 
			MaskLayers::Base 
		}
	});
	epistasisDependencies.insert({
		Locus::Agouti, {
			{ Locus::Extension }, 
			MaskLayers::Dilution
		}
	});
	epistasisDependencies.insert({
		Locus::KIT, {
			{ },
			MaskLayers::White
		}
	});
	epistasisDependencies.insert({
		Locus::Silver, {
			{ },
			MaskLayers::Dilution
		}
	});
	epistasisDependencies.insert({
		Locus::Cream, {
			{ Locus::Agouti },
			MaskLayers::Dilution
		}
	});
	epistasisDependencies.insert({
		Locus::Pearl, {
			{ },
			MaskLayers::Dilution
		}
	});
}

std::unordered_map<MaskLayers, std::vector<Locus>> createMaskBuckets(
	std::vector<Locus>& lociToSort,
	const EpistasisGraph& epGraph)
{

	std::sort(lociToSort.begin(), lociToSort.end(),
		[&epGraph](Locus a, Locus b) {
			const EpistasisNode& nodeA = epGraph.at(a);
			const EpistasisNode& nodeB = epGraph.at(b);

			// lower enum number means higher on the mask. 
			return static_cast<int>(nodeA.maskLayer) < static_cast<int>(nodeB.maskLayer);
		}
	);

	std::unordered_map<MaskLayers, std::vector<Locus>> maskBuckets;
	for (const Locus& locus : lociToSort) {
		MaskLayers currentLayer = epGraph.at(locus).maskLayer;
		maskBuckets[currentLayer].push_back(locus);
	}

	return maskBuckets;
}

/*
This code predicts what the horse will look like using it's genotype
*/
void getPhenotype(const Horse& h) {
	std::cout << "   " << "Phenotype: ";
	std::unordered_map<MaskLayers, std::vector<Locus>> maskBuckets = createMaskBuckets(h.genotype, epistasisDependencies);

	for (const auto& [maskLayer, loci] : maskBuckets) {
		if (maskLayer == MaskLayers::White) {
			for (const auto& locus : loci) {
				
			}
		}
	};
}

static Punnett<Gene> generatePunnett(const Gene& sAlleles, const Gene& dAlleles) {
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
	std::unordered_map<std::string, float> frequencies(4);
	for (auto& cell : p.punnett | std::views::join) {
		frequencies[cell.toString()]++;
	}

	std::vector<std::string> outcomes;
	std::vector<double> weights;
	for (const auto& [key, value] : frequencies) {
		outcomes.push_back(key);
		weights.push_back(static_cast<double>(value));
	}

	std::random_device rd;
	std::mt19937 generator(rd());
	std::discrete_distribution<> distribution(weights.begin(), weights.end());

	std::string chosenSymbols = outcomes[distribution(generator)];

	Allele a1(std::string(1, chosenSymbols[0]));
	Allele a2(std::string(1, chosenSymbols[1]));

	return Gene(a1, a2);
}

// Probably defunct
Gene generateOffspringGene(
	const Genotype<Locus>& sGenotype,
	const Genotype<Locus>& dGenotype,
	const Locus& L) {
	return resolvePunnettSquare(generatePunnett(sGenotype.getGene(L), dGenotype.getGene(L)));
}

Genotype<Locus> generateOffspringGenotype(
	const Genotype<Locus>& sGenotype,
	const Genotype<Locus>& dGenotype) {

	// consider lambda here
	std::unordered_map<Locus, Gene> foalGenes;
	for (const auto& [locus, sireGene] : sGenotype) {
		foalGenes.emplace(locus, std::move(resolvePunnettSquare(generatePunnett( sireGene, dGenotype.getGene(locus) ))) );
	}

	Genotype<Locus> foalGenotype(0, std::move(foalGenes));
	return foalGenotype;
}

Horse generateOffspring(const Horse& sire, const Horse& dam) {
	if (sire.sex != 'M' || dam.sex != 'F') {
		throw std::invalid_argument("First horse must be male and second must be female.");
	}

	std::vector<Locus> L = { Locus::Extension, Locus::Agouti };
	auto foalGenotype = generateOffspringGenotype(sire.genotype, dam.genotype);
	Horse offspring{ 'U', foalGenotype }; // 'U' for unknown sex
	return offspring;
}

int main()
{
	std::cout << "   " << "Getting horses" << std::endl;
	auto sireAE = Gene(Allele("e"), Allele("E"));
	auto damAE  = Gene(Allele("E"), Allele("e"));
	auto sireAA = Gene(Allele("A"), Allele("a"));
	auto damAA = Gene(Allele("a"), Allele("A"));
	auto sireG = Genotype<Locus>(0, { {Locus::Extension, sireAE}, {Locus::Agouti, sireAA} });
	auto damG  = Genotype<Locus>(0, { {Locus::Extension, damAE}, {Locus::Agouti, damAA} });

	auto sire = Horse('M', sireG);
	auto dam  = Horse('F', damG);

	std::cout << "Sire's Genotype" << std::endl;
	sire.showGenes(Locus::Extension);
	sire.showGenes(Locus::Agouti);

	std::cout << "Dam's Genotype" << std::endl;
	dam.showGenes(Locus::Extension);
	dam.showGenes(Locus::Agouti);

	std::cout << "Offsprings's Genotype" << std::endl;
	auto offspring = generateOffspring(sire, dam);
	offspring.showGenes(Locus::Extension);
	offspring.showGenes(Locus::Agouti);

	getPhenotype(offspring);

	return 0;
}