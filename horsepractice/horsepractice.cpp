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
#include <functional>
#include <set>

class InvalidAlleles : public std::exception {
private:
	std::string message;
public:
	InvalidAlleles(const std::string& msg) : message(msg) {}

	virtual const char* what() const noexcept override {
		return message.c_str();
	}
};

class NonexistantAlleles : public std::exception {
private:
	std::string message;
public:
	NonexistantAlleles(const std::string& msg) : message(msg) {}

	virtual const char* what() const noexcept override {
		return message.c_str();
	}
};

class Gene;
class Genotype;
struct Phenotype;

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
	Dun,
	Champagne,
	Gray,
	SplashWhite,
	FrameOvero,
	Leopard,
	MLeopard,
	Flaxen,
	Pangare,
	Sooty,
	Unknown
}; 

struct Phenotype {
	
};

struct Allele {
	std::string symbol; // Like 'A' or "Sty"
	Dominance dominance;
	Locus locus;

	std::unordered_map<std::string, Locus> symbolToLocusTable = {
		{"E",   Locus::Extension},
		{"A",   Locus::Agouti}, 
		{"A+",  Locus::Agouti}, // wild bay
		{"AT",  Locus::Agouti}, // seal
		{"RN",  Locus::KIT},
		{"TO",  Locus::KIT},
		{"SB1", Locus::KIT},
		{"Z",   Locus::Silver},
		{"CR",  Locus::Cream},
		{"PRL", Locus::Cream},
		{"SNO", Locus::Cream},
		{"SUN", Locus::Cream},
		{"D",   Locus::Dun},
		{"CH",  Locus::Champagne},
		{"G",   Locus::Gray},
		{"SW",  Locus::SplashWhite},
		{"O",   Locus::FrameOvero},
		{"LP",  Locus::Leopard},
		{"PATN1", Locus::MLeopard},
		{"F",   Locus::Flaxen},
		{"P",   Locus::Pangare},
		{"STY", Locus::Sooty},
		{"U",   Locus::Unknown}
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
			if (s == "CR") {
				dominance = Dominance::InDominant;
			} else {
				dominance = Dominance::Recessive;
			}		
		}

		this->symbol = std::move(symbol);
	}
};

class Gene {
private:
	std::pair<Allele, Allele> alleles;
public:
	const Allele& getAllele1() const { return alleles.first; }
	const Allele& getAllele2() const { return alleles.second; }
	
	Gene(Allele a1, Allele a2)
		: alleles(std::move(a1), std::move(a2)) 
	{
		if (alleles.first.locus != alleles.second.locus) {
			throw InvalidAlleles("Alleles are invalid. They are not apart of the same Locus.");
		}

		if (alleles.second.dominance == Dominance::Dominant) {
			std::swap(alleles.first, alleles.second);
		}

		if (alleles.first.dominance == Dominance::InDominant
			&& alleles.second.dominance == Dominance::InDominant) {
			if (std::isupper(alleles.second.symbol[0])) {
				std::swap(alleles.first, alleles.second);
			}
		}
	}

	std::string toString() const {
		return (alleles.first.symbol + alleles.second.symbol);
	};

	// for indominant/codominant
	constexpr int getActiveCount() const {
		if (alleles.first.dominance != Dominance::Dominant
			|| alleles.first.dominance != Dominance::Recessive
			|| alleles.second.dominance != Dominance::Dominant
			|| alleles.second.dominance != Dominance::Recessive) {
			if (std::isupper(alleles.first.symbol[0])
				&& std::isupper(alleles.second.symbol[0])) {
				return 2;
			} else if (std::isupper(alleles.first.symbol[0])
				|| std::isupper(alleles.second.symbol[0])) {
				return 1;
			}
			return 0;
		}
		throw InvalidAlleles("getActiveCount only works on indominant/codominant");
	}

	constexpr int getDominantCount() const {
		if (alleles.first.dominance == Dominance::Dominant
			&& alleles.second.dominance == Dominance::Dominant) {
			return 2;
		} else if (alleles.first.dominance == Dominance::Dominant
			|| alleles.second.dominance == Dominance::Dominant) {
			return 1;
		}
		return 0;
	};

	constexpr bool hasAllele(std::string a) const {
		return (alleles.first.symbol == a
			|| alleles.second.symbol == a);
	};

	constexpr bool hasAlleleA_plus() const {
		return (alleles.first.symbol == "A+");
	};

	constexpr bool hasAlleleA() const {
		return (alleles.first.symbol == "A");
	};

	constexpr bool hasAlleleAt() const {
		return (alleles.first.symbol == "At");
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

	std::string locusToString(Locus l) const noexcept {
		switch (l) {
		case Locus::Extension:    return "Extension";
		case Locus::Agouti:       return "Agouti";
		case Locus::KIT:          return "KIT";
		case Locus::Silver:       return "Silver";
		case Locus::Cream:        return "Cream";
		case Locus::Dun:          return "Dun";
		case Locus::Champagne:    return "Champagne";
		case Locus::Gray:         return "Gray";
		case Locus::SplashWhite:  return "SplashWhite";
		case Locus::FrameOvero:   return "FrameOvero";
		case Locus::Flaxen:       return "Flaxen";
		case Locus::Pangare:      return "Pangare";
		case Locus::Sooty:        return "Sooty";
		case Locus::Leopard:      return "Leopard";
		case Locus::MLeopard:     return "MLeopard";
		case Locus::Unknown:      return "Unknown";
		default:                  return "[[Unhandled Locus Value]]";
		}
	}

	Gene getGene(const Locus l) const {
		try {
			return m_genotype.at(l);
		}
		catch (const std::out_of_range& oor) {
			std::string e = "Attempted to access non-existent gene from locus: " + locusToString(l);
			throw NonexistantAlleles(e);
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
		}} 
	{ }
};

// need to control that sex has to be a 'F' or a 'M'.
struct Horse {
	char sex;
	Genotype genotype;

	Horse(char s, Genotype g)
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

/*
This code predicts what the horse will look like using it's genotype
*/
Phenotype getPhenotype(
	const Genotype& genotype) 
{
	Phenotype phenotype; 
	
	Allele dummyAllele{"U"};

	return phenotype;
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
    static std::random_device rd;
    static std::mt19937 generator(rd());
    
    std::uniform_int_distribution<> distribution(0, 3);
    int choice = distribution(generator);

    int row = choice / 2; 
    int col = choice % 2; 

    return p.punnett[row][col];
}

Genotype generateOffspringGenotype(
	const Genotype& sGenotype,
	const Genotype& dGenotype) {

	// consider lambda here
	std::unordered_map<Locus, Gene> foalGenes;
	for (const auto& [locus, sireGene] : sGenotype) {
		foalGenes.emplace(locus, std::move(resolvePunnettSquare(generatePunnett( sireGene, dGenotype.getGene(locus) ))) );
	}

	Genotype foalGenotype(0, std::move(foalGenes));
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

int main()
{
	try {
		std::cout << "   " << "Getting horses" << std::endl;

		std::unordered_map<Locus, Gene> sireGenes = {
			{ Locus::Extension, Gene(Allele("e"), Allele("e")) },
			{ Locus::Agouti,    Gene(Allele("A"), Allele("a")) },
			{ Locus::Silver,    Gene(Allele("Z"), Allele("Z")) },
			{ Locus::Flaxen,    Gene(Allele("F"), Allele("f")) },
			{ Locus::Cream,     Gene(Allele("cr"), Allele("Cr")) },
			{ Locus::Pangare,   Gene(Allele("P"), Allele("p")) },
			{ Locus::Dun,       Gene(Allele("D"), Allele("D")) },
			{ Locus::SplashWhite,Gene(Allele("SW"), Allele("sw")) },
			{ Locus::FrameOvero, Gene(Allele("O"), Allele("o")) },
			{ Locus::Leopard,   Gene(Allele("LP"), Allele("lp")) },
			{ Locus::MLeopard,  Gene(Allele("PATN1"), Allele("patn1")) },
			{ Locus::Sooty,     Gene(Allele("STY"), Allele("sty")) },
			{ Locus::KIT,       Gene(Allele("RN"), Allele("TO")) },
			{ Locus::Gray,      Gene(Allele("G"), Allele("g")) },
			{ Locus::Champagne, Gene(Allele("Ch"), Allele("ch")) },
		};
		Genotype sireG(0, std::move(sireGenes));

		std::unordered_map<Locus, Gene> damGenes = {
			{ Locus::Extension, Gene(Allele("E"), Allele("e")) },
			{ Locus::Agouti,    Gene(Allele("A"), Allele("a")) },
			{ Locus::Silver,    Gene(Allele("z"), Allele("Z")) },
			{ Locus::Flaxen,    Gene(Allele("F"), Allele("f")) },
			{ Locus::Cream,     Gene(Allele("Cr"), Allele("Cr")) },
			{ Locus::Pangare,   Gene(Allele("P"), Allele("p")) },
			{ Locus::Dun,		Gene(Allele("D"), Allele("d")) },
			{ Locus::SplashWhite,Gene(Allele("SW"), Allele("sw")) },
			{ Locus::FrameOvero, Gene(Allele("O"), Allele("o")) },
			{ Locus::Leopard,   Gene(Allele("LP"), Allele("lp")) },
			{ Locus::MLeopard,  Gene(Allele("PATN1"), Allele("patn1")) },
			{ Locus::Sooty,     Gene(Allele("STY"), Allele("sty")) },
			{ Locus::KIT,       Gene(Allele("RN"), Allele("TO")) },
			{ Locus::Gray,      Gene(Allele("G"), Allele("g")) },
			{ Locus::Champagne, Gene(Allele("Ch"), Allele("ch")) },
		}; 
		Genotype damG(0, std::move(damGenes));

		auto sire = Horse('M', sireG);
		auto dam = Horse('F', damG);

		std::cout << "Sire's Genotype" << std::endl;
		sire.showAllGenes();

		std::cout << "Dam's Genotype" << std::endl;
		dam.showAllGenes();

		std::cout << "Offspring's Genotype" << std::endl;
		auto offspring = generateOffspring(sire, dam);
		offspring.showAllGenes();
	
		Phenotype finalPhenotype = getPhenotype(offspring.genotype);

		std::cout << "Offspring's Phenotype Traits:" << std::endl;
		
	}
	catch (const NonexistantAlleles& e) {
		std::cerr << "\nPHENOTYPE ERROR: " << e.what() << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "\nAn unexpected error occurred: " << e.what() << std::endl;
	}

	return 0;
}