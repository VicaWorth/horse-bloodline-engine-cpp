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

class InvalidAlleles : public std::exception {
private:
	std::string message;
public:
	// Constructor to initialize the exception message
	InvalidAlleles(const std::string& msg) : message(msg) {}

	// Override the virtual what() method to return the exception message
	// The noexcept specifier indicates that this method will not throw exceptions.
	virtual const char* what() const noexcept override {
		return message.c_str();
	}
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
	MLeopard
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

struct Alleles {
	std::pair<std::string, std::string> alleles;

	Alleles() = default;

	Alleles(std::string a1, std::string a2)
		: alleles(a1, a2) { }

	std::string toString() const {
		return (alleles.first + alleles.second);
	};
};

template <typename a>
struct Punnett {
	std::array<std::array<a, 2>, 2> punnett;
};

template <typename T>
class Genotype {
private:
	int m_id;
	std::unordered_map<Locus, Alleles> m_genotype;

public: 
	Genotype(int id, std::initializer_list<std::pair<const Locus, Alleles>> genes)
		: m_id(id), m_genotype(genes) { }

	Alleles getGene(const Locus l) const {
		return m_genotype.at(l);
	}
};

// possibly make sex an optional
struct Horse {
	char sex;
	Genotype<Locus> genotype;

	Horse(char s, Genotype<Locus> g) 
		: sex(s), genotype(g) { }

	void showGenes(Locus l) {
		std::cout << genotype.getGene(l).toString() << std::endl;
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
	1. Traverse graph for all nodes. When a node has a dependency
	*/
}

// Offspring Related

// Convert these two functions into one function using templates
// checking if second one is dominant
bool validateAlleles(const std::string a1, const std::string a2) {
	if (a1 > a2) {
		return false;
	}
	return true;
};

bool validateAlleles(const Alleles& a) {
	// this part will need to be updated to properly work with different Alleles 
	if (a.alleles.first > a.alleles.second) {
		throw InvalidAlleles("Alleles are invalid. Ensure dominant gene is always first.");
	}
	return true;
};

Alleles ensureProperOrdering(const std::string& sAlleles, const std::string& dAlleles) {
	std::cout << sAlleles << " " << dAlleles << std::endl;
	if (!std::isupper(sAlleles[0]) && std::isupper(dAlleles[0])) {
		return Alleles(dAlleles, sAlleles);
	}
	return Alleles(sAlleles, dAlleles);
}

Punnett<Alleles> generatePunnett(
	const Alleles& sAlleles,
	const Alleles& dAlleles) {

	try {
		validateAlleles(sAlleles); validateAlleles(dAlleles);
		// later insert a function here that ensures ordering of Alleles

		// Fix later
		Punnett<Alleles> cPunnett;
		cPunnett.punnett[0][0] = ensureProperOrdering(sAlleles.alleles.first, dAlleles.alleles.first);
		cPunnett.punnett[0][1] = ensureProperOrdering(sAlleles.alleles.first, dAlleles.alleles.second);
		cPunnett.punnett[1][0] = ensureProperOrdering(sAlleles.alleles.second, dAlleles.alleles.first);
		cPunnett.punnett[1][1] = ensureProperOrdering(sAlleles.alleles.second, dAlleles.alleles.second);

		return cPunnett;

	} catch (const std::exception& e) {
		std::cerr << "Caught an exception: " << e.what() << std::endl;
	}
};

// Looks at punnett square and generates frequency map
// takes frequency map and gets weighted distribution
// returns alleles associated with that
// REFACTOR : returning a string here is not best, 
//            later should become a gene type or something
//            also generally needs to be cleaned up
std::string resolvePunnettSquare(const Punnett<Alleles> &p) {
	try {
		std::unordered_map<std::string, float> frequencies(4);
		for (auto& cell : p.punnett | std::views::join) {
			frequencies[cell.toString()]++;
		}

		int i = 0;
		std::array<std::string, 4> alleles{ {} };
		std::array<float, 4> weights{ {} };
		for (auto& [key, value] : frequencies) {
			alleles[i] = key;
			weights[i] = frequencies[key] / 4; //magic number here represents total number of squares
			std::cout << "Chances at " << key << " are " << weights[i] << " chance " << std::endl;
 			i++;
		}

		std::random_device rd;
		std::mt19937_64 generator(rd());

		std::discrete_distribution<> distribution(weights.begin(), weights.end());

		return alleles[distribution(generator)];
	} catch (const std::exception& e) {
		std::cerr << "Caught an exception: " << e.what() << std::endl;
	}
}

Genotype<Locus> generateOffspringGenotype(
	const Genotype<Locus>& sGenotype,
	const Genotype<Locus>& dGenotype) {
	auto sireExtension = sGenotype.getGene(Locus::Extension);
	auto damExtension = dGenotype.getGene(Locus::Extension);

	std::string eAllele = resolvePunnettSquare(generatePunnett(sireExtension, damExtension)); 
	auto extensionAlleles = Alleles(std::string(1, eAllele[0]), std::string(1, eAllele[1]));

	auto sireAgouti = sGenotype.getGene(Locus::Agouti);
	auto damAgouti = dGenotype.getGene(Locus::Agouti);

	std::string aAllele = resolvePunnettSquare(generatePunnett(sireAgouti, damAgouti));
	auto agoutiAlleles = Alleles(std::string(1, aAllele[0]), std::string(1, aAllele[1]));

	//defintely fix this later
	Genotype<Locus> foalGenotype(0, { {Locus::Extension, extensionAlleles}, {Locus::Agouti, agoutiAlleles} });
	return foalGenotype;
}

Horse generateOffspring(const Horse& sire, const Horse& dam) {
	if (sire.sex != 'M' || dam.sex != 'F') {
		throw std::invalid_argument("First horse must be male and second must be female.");
	}

	// Attempts to breed horses
	try {
		auto foalGenotype = generateOffspringGenotype(sire.genotype, dam.genotype);
		Horse offspring{ 'U', foalGenotype }; // 'U' for unknown sex
		return offspring;
	} catch (const std::exception& e) {
		std::cerr << "Caught an exception: " << e.what() << std::endl;
	}
}

// QoL Horse Generation


int main()
{
	std::cout << "Getting horses" << std::endl;
	auto sireAE = Alleles(std::string(1, 'E'), std::string(1, 'e'));
	auto damAE  = Alleles(std::string(1, 'E'), std::string(1, 'e'));
	auto sireAA = Alleles(std::string(1, 'a'), std::string(1, 'a'));
	auto damAA = Alleles(std::string(1, 'A'), std::string(1, 'a'));
	auto sireG = Genotype<Locus>(0, { {Locus::Extension, sireAE}, {Locus::Agouti, sireAA} });
	auto damG  = Genotype<Locus>(0, { {Locus::Extension, damAE}, {Locus::Agouti, damAA} });

	auto sire = Horse('M', sireG);
	auto dam  = Horse('F', damG);

	std::cout << "attempting to print" << std::endl;
	auto offspring = generateOffspring(sire, dam);
	offspring.showGenes(Locus::Extension);
	offspring.showGenes(Locus::Agouti);

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
