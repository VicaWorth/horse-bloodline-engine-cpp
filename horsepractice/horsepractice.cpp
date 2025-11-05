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

struct LocusNode {
	// this id probably doesn't need to exist
	Locus id;
	// std::string alleleName;
	// possible other information

	// What this node is dependent on
	std::vector<Locus> dependencies;
};

// represents the group of Loci
// Meant to help organize masking order
// Gray masks White masks Dilution masks Base Coat
struct maskNode {
	std::vector<Locus> Loci;
};

using GeneGraph = std::unordered_map<Locus, LocusNode>;
GeneGraph LocusDependencies;
using MaskGraph = std::unordered_map<std::string, maskNode>;
MaskGraph maskingOrder;

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
		}
		else {
			dominance = Dominance::Recessive;
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
	Genotype(int id, std::initializer_list<std::pair<const Locus, Gene>> genes)
		: m_id(id), m_genotype(genes) {
	}

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
void locusGraphConstructor() {
	LocusDependencies.insert({ Locus::Extension, LocusNode{Locus::Extension, {} } });
	LocusDependencies.insert({ Locus::Agouti, LocusNode{Locus::Agouti, {Locus::Extension} } });
	/*
	LocusDependencies.insert({ Locus::KIT, LocusNode{Locus::KIT, {Locus::Agouti} } });
	LocusDependencies.insert({ Locus::Silver, LocusNode{Locus::Silver, {Locus::Agouti} } });
	LocusDependencies.insert({ Locus::Cream, LocusNode{Locus::Cream, {Locus::Agouti} } });
	LocusDependencies.insert({ Locus::Pearl, LocusNode{Locus::Pearl, {} } });
	LocusDependencies.insert({ Locus::Dun, LocusNode{Locus::Dun, {Locus::Agouti} } });
	LocusDependencies.insert({ Locus::Champange, LocusNode{Locus::Champange, {Locus::Agouti} } });
	LocusDependencies.insert({ Locus::Gray, LocusNode{Locus::Gray, {} } });
	LocusDependencies.insert({ Locus::SplashWhite, LocusNode{Locus::SplashWhite, {} } });
	LocusDependencies.insert({ Locus::FrameOvero, LocusNode{Locus::FrameOvero, {} } });
	LocusDependencies.insert({ Locus::Flaxen, LocusNode{Locus::Flaxen, {Locus::Extension} } });
	LocusDependencies.insert({ Locus::Pangare, LocusNode{Locus::Pangare, {Locus::Extension} } });
	LocusDependencies.insert({ Locus::Sooty, LocusNode{Locus::Sooty, {Locus::Extension} } });
	LocusDependencies.insert({ Locus::Leopard, LocusNode{Locus::Leopard, {} } });
	LocusDependencies.insert({ Locus::MLeopard, LocusNode{Locus::MLeopard, {Locus::Leopard} } });
	*/
}

void maskGraphConstructor() {
	/*
	maskingOrder.insert({ "Gray", maskNode{ {Locus::Gray} }});
	maskingOrder.insert({ "White", maskNode{ {Locus::KIT, Locus::Leopard, Locus::MLeopard, Locus::FrameOvero, Locus::SplashWhite} } });
	*/
	maskingOrder.insert({ "Dilution", maskNode{ {Locus::Agouti, Locus::Cream, Locus::Dun, Locus::Silver, Locus::Champange, Locus::Pearl, Locus::Sooty, Locus::Pangare, Locus::Flaxen} } });
	maskingOrder.insert({ "Base Coat", maskNode{ {Locus::Extension} } });
}

void getPhenotype(const Horse& h) {
	// need to add check to make sure horse exists

	/*
	1. Traverse graph for all nodes. When a node has a dependency, check its parent.
		for (const auto& pair : h.genotype) {}
	*/
	auto const& e = h.genotype.getGene(Locus::Extension);
	auto const& a = h.genotype.getGene(Locus::Agouti);
	std::cout << "   " << "Phenotype: ";
	if (a.isDominantPresent() && e.isDominantPresent()) {
		std::cout << "Bay" << std::endl;
	}

	if (a.isRecessivePresent() && e.isDominantPresent()) {
		std::cout << "Black" << std::endl;
	}

	if (e.isRecessivePresent()) {
		std::cout << "Chestnut" << std::endl;
	}
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

Gene generateOffspringGene(
	const Genotype<Locus>& sGenotype,
	const Genotype<Locus>& dGenotype,
	const Locus& L) {
	return resolvePunnettSquare(generatePunnett(sGenotype.getGene(L), dGenotype.getGene(L)));
}

Genotype<Locus> generateOffspringGenotype(
	const Genotype<Locus>& sGenotype,
	const Genotype<Locus>& dGenotype,
	const std::vector<Locus> L) {

	/*
	Possibly get some kind of lambda function that will
	- go through each Locus
	- call generateOffspringGene
	- add it to the Genotype Object
	*/

	Gene eAllele = generateOffspringGene(sGenotype, dGenotype, L[0]);
	Gene aAllele = generateOffspringGene(sGenotype, dGenotype, L[1]);

	//defintely fix this later
	Genotype<Locus> foalGenotype(0, { {Locus::Extension, eAllele}, {Locus::Agouti, aAllele} });
	return foalGenotype;
}

Horse generateOffspring(const Horse& sire, const Horse& dam) {
	if (sire.sex != 'M' || dam.sex != 'F') {
		throw std::invalid_argument("First horse must be male and second must be female.");
	}

	std::vector<Locus> L = { Locus::Extension, Locus::Agouti };
	auto foalGenotype = generateOffspringGenotype(sire.genotype, dam.genotype, L);
	Horse offspring{ 'U', foalGenotype }; // 'U' for unknown sex
	return offspring;
}

// QoL Horse Generation


int main()
{
	std::cout << "   " << "Getting horses" << std::endl;
	auto sireAE = Gene(Allele("E"), Allele("e"));
	auto damAE  = Gene(Allele("E"), Allele("e"));
	auto sireAA = Gene(Allele("a"), Allele("A"));
	auto damAA = Gene(Allele("a"), Allele("a"));
	auto sireG = Genotype<Locus>(0, { {Locus::Extension, sireAE}, {Locus::Agouti, sireAA} });
	auto damG  = Genotype<Locus>(0, { {Locus::Extension, damAE}, {Locus::Agouti, damAA} });

	auto sire = Horse('M', sireG);
	auto dam  = Horse('F', damG);

	std::cout << "attempting to print" << std::endl;
	auto offspring = generateOffspring(sire, dam);
	offspring.showGenes(Locus::Extension);
	offspring.showGenes(Locus::Agouti);

	getPhenotype(offspring);

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
