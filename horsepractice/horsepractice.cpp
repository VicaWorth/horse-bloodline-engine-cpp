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
	Genotype(int id, std::initializer_list<std::pair<Locus, Alleles>> genes)
		: m_id(id), m_genotype(genes) { }

	Alleles getGene(const Locus l) const {
		return m_genotype.at(l);
	}
};

struct Horse {
	char sex;
	Genotype<Locus> genotype;
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
		
	} catch (const std::exception& e) {
		std::cerr << "Caught an exception: " << e.what() << std::endl;
	}
};

// Looks at punnett square and generates frequency map
// takes frequency map and gets weighted distribution
// returns alleles associated with that
Alleles resolvePunnettSquare(const Punnett<Alleles> &p) {
	std::unordered_map<std::string, float> frequencies;
	for (auto& cell : p.punnett | std::views::join) {
		frequencies[cell]++;
	}
	
	std::unordered_map<std::string, float> weights(4);
	for (auto& cell : frequencies | std::views::join) {
		frequencies[cell] = frequencies[cell]/4; //magic number here represents total number of squares
	}

	std::random_device rd;
	std::mt19937_64 generator(rd());

	std::discrete_distribution<> distribution(frequencies.begin(), frequencies.end());

	int chosen_index = distribution(generator);

	return frequencies[chosen_index];
}

Genotype<Locus> generateOffspringGenotype(
	const Genotype<Locus>& sGenotype,
	const Genotype<Locus>& dGenotype) {
	auto sireExtension = sGenotype.getGene(Locus::Extension);
	auto damExtension = dGenotype.getGene(Locus::Extension);

	auto a = resolvePunnettSquare(generatePunnett(sireExtension, damExtension));
	Genotype<Locus> foalGenotype(0, a);
	return Horse{ 'M', foalGenotype };
}

Horse generateOffspring(const Horse& sire, const Horse& dam) {
	if (sire.sex != 'M' || dam.sex != 'F') {
		throw std::invalid_argument("First horse must be male and second must be female.");
	}

	// Attempts to breed horses
	try {
		auto foalGenotype = generateOffspringGenotype(sire.genotype, dam.genotype);
	} catch (const std::exception& e) {
		std::cerr << "Caught an exception: " << e.what() << std::endl;
	}

	Genotype<Locus> placeholderGenotype(0, {}); // An empty genotype with ID 0
	Horse offspring{ 'U', placeholderGenotype }; // 'U' for unknown sex
	return offspring;
}

int main()
{
	

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
