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
	FrameOvero
};

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

Punnett<Alleles> generatePunnett(
	const Alleles& sAlleles,
	const Alleles& dAlleles) {

	try {
		validateAlleles(sAlleles); validateAlleles(dAlleles);
		// later insert a function here that ensures ordering of Alleles

		// Fix later
		Punnett<Alleles> cPunnett;
		cPunnett.punnett[0][0] = Alleles(sAlleles.alleles.first, dAlleles.alleles.first);
		cPunnett.punnett[0][1] = Alleles(sAlleles.alleles.first, dAlleles.alleles.second);
		cPunnett.punnett[1][0] = Alleles(sAlleles.alleles.second, dAlleles.alleles.first);
		cPunnett.punnett[1][1] = Alleles(sAlleles.alleles.second, dAlleles.alleles.second);

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

	std::string chosenAllele = resolvePunnettSquare(generatePunnett(sireExtension, damExtension)); 
	auto extensionAlleles = Alleles(std::string(1, chosenAllele[0]), std::string(1, chosenAllele[1]));

	//defintely fix this later
	Genotype<Locus> foalGenotype(0, {{Locus::Extension, extensionAlleles}});
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

int main()
{
	std::cout << "Getting horses" << std::endl;
	auto sireA = Alleles(std::string(1, 'E'), std::string(1, 'e'));
	auto damA  = Alleles(std::string(1, 'e'), std::string(1, 'e'));
	auto sireG = Genotype<Locus>(0, { {Locus::Extension, sireA} });
	auto damG  = Genotype<Locus>(0, { {Locus::Extension, damA} });

	auto sire = Horse('M', sireG);
	auto dam  = Horse('F', damG);

	std::cout << "attempting to print" << std::endl;
	auto offspring = generateOffspring(sire, dam);
	offspring.showGenes(Locus::Extension);


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
