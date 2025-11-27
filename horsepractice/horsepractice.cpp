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
template <typename T> class Genotype;
struct Phenotype;
struct EpistasisNode;

enum class Dominance {
	Dominant,
	Recessive,
	Codominant,
	InDominant  //IncompletelyDominant
};

enum class Locus {
	Root,
	Extension,
	Agouti,
	KIT,
	Silver,
	Cream,
	Pearl, // apparenty pearl is on the same locus at Cream so this will have to be updated
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

struct EpistasisNode {
	std::function<bool(const Gene&)> entryCondition;
	std::vector<Locus> children;
	std::function<bool(const Genotype<Locus>&, Phenotype&)> resolver;
};

struct Phenotype {
	std::unordered_map<std::string, bool> activeTraits;

	void flipSwitch(const std::string& traitName) {
		activeTraits[traitName] = true;
	}

	bool isSwitchedOn(const std::string& traitName) const {
		if (activeTraits.count(traitName)) {
			return activeTraits.at(traitName);
		}
		return false;
	}

	void printPhenotypeDescription() {
		for (const auto& pair : activeTraits) {
			std::cout << pair.first << " all " << std::endl;
			if (pair.second == true) {
				std::cout << pair.first << " ";
			}
		}
		std::cout << std::endl;
	}
};

struct Allele {
	std::string symbol; // Like 'A' or "Sty"
	Dominance dominance;
	Locus locus;

	std::unordered_map<std::string, Locus> symbolToLocusTable = {
		{"E", Locus::Extension},
		{"A", Locus::Agouti},
		{"A+", Locus::Agouti},
		{"AT", Locus::Agouti},
		{"RN", Locus::KIT},
		{"TO", Locus::KIT},
		{"SB1", Locus::KIT},
		{"Z", Locus::Silver},
		{"CR", Locus::Cream},
		{"PRL", Locus::Pearl},
		{"DUN", Locus::Dun},
		{"CH", Locus::Champagne},
		{"G", Locus::Gray},
		{"SW", Locus::SplashWhite},
		{"O", Locus::FrameOvero},
		{"LP", Locus::Leopard},
		{"PATN1", Locus::MLeopard},
		{"F", Locus::Flaxen},
		{"P", Locus::Pangare},
		{"STY", Locus::Sooty},
		{"U", Locus::Unknown} // add others later
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

	constexpr bool hasAllele(std::string a) const {
		return (alleles.first.symbol == a);
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

	std::string locusToString(Locus l) const noexcept {
		switch (l) {
		case Locus::Root:         return "Root";
		case Locus::Extension:    return "Extension";
		case Locus::Agouti:       return "Agouti";
		case Locus::KIT:          return "KIT";
		case Locus::Silver:       return "Silver";
		case Locus::Cream:        return "Cream";
		case Locus::Pearl:        return "Pearl";
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
	Genotype<Locus> genotype;

	Horse(char s, Genotype<Locus> g)
		: sex(s), genotype(g) {
	}

	void showGenes(Locus l) {
		std::cout << "   " << genotype.getGene(l).toString() << std::endl;
	}
};

// Locus Graph Related
using Rule = std::pair<bool (Gene::*)(void) const, std::string>;

std::function<bool(const Genotype<Locus>&, Phenotype&)>
createGeneRule(
	Locus locus, 
	const std::vector<Rule>& rules, 
	const std::string& defaultPhenotype = "")
{
	return [locus, rules, defaultPhenotype](const Genotype<Locus>& genotype, Phenotype& phenotype) {
		const Gene& gene = genotype.getGene(locus);

		// Iterates thru all 'if,ifelse,else'
		for (const auto& [predicate, phenotypeName] : rules) {
			if (std::invoke(predicate, gene)) {
				phenotype.flipSwitch(phenotypeName);
				return true;
			}
		}

		// If no rules apply, go to else case 
		if (!defaultPhenotype.empty()) {
			phenotype.flipSwitch(defaultPhenotype);
			return true;
		}
		return false;
	};
}

using EpistasisGraph = std::unordered_map<Locus, EpistasisNode>;
EpistasisGraph epistasisGraph;

void epistasisGraphConstructor() {
	//Root
	epistasisGraph.insert({
	Locus::Root,
	{
		[](const Gene&) { return true; },
		{ Locus::Extension/*, Locus::SplashWhite, Locus::FrameOvero, Locus::Sooty, Locus::Gray, Locus::KIT, Locus::Pearl, Locus::Flaxen, Locus::Pangare*/},
		std::function<bool(const Genotype<Locus>&, Phenotype&)>()
	}
	});

	// Extension
	epistasisGraph.insert({
	Locus::Extension,
	{ 
		[](const Gene& parentGene) { return true; },
		{ Locus::Agouti,/* Locus::Champagne, Locus::Dun, Locus::Cream,*/ Locus::Silver},
		createGeneRule(Locus::Extension, {
			{ &Gene::isRecessivePresent, "Chestnut" }
		},
			"Black")
		}
	});

	// Agouti
	epistasisGraph.insert({
		Locus::Agouti,
		{
			[](const Gene& parentGene) { return parentGene.isDominantPresent(); },
			{ /* Locus::Champagne, Locus::Dun, Locus::Cream */ },
			createGeneRule(Locus::Agouti, {
				{ &Gene::hasAlleleA_plus, "Wild Bay" },
				{ &Gene::hasAlleleA,     "Bay"      }, 
				{ &Gene::hasAlleleAt,    "Seal Brown" }
			})
		}
	});

	// Silver
	epistasisGraph.insert({
		Locus::Silver,
		{
			[](const Gene& parentGene) { return parentGene.isDominantPresent(); },
			{},
			createGeneRule(Locus::Silver, {
				{ &Gene::isDominantPresent, "Silver"}
			})
		}
	});
}

static bool evaluateNode(
	Locus currentNode,
	const Gene& parentGene, // The context from the parent
	const Genotype<Locus>& genotype,
	Phenotype& phenotype)
{

	if (epistasisGraph.find(currentNode) == epistasisGraph.end()) {
		std::string locusName = genotype.locusToString(currentNode);
		throw NonexistantAlleles("Epistasis Graph is missing logic for Locus: " + locusName);
	}


	const EpistasisNode& node = epistasisGraph.at(currentNode);

	if (!node.entryCondition(parentGene)) {
		return false; 
	}

	bool childFlippedASwitch = false;

	const Gene& contextForChildren = (currentNode == Locus::Root)
		? parentGene 
		: genotype.getGene(currentNode);

	for (Locus childLocus : node.children) {
		childFlippedASwitch = evaluateNode(childLocus, contextForChildren, genotype, phenotype) || childFlippedASwitch;
	}

	if (childFlippedASwitch) {
		return true;
	} else {
		if (node.resolver) {
			return node.resolver(genotype, phenotype);
		}
	}

	return childFlippedASwitch;
}

/*
This code predicts what the horse will look like using it's genotype
*/
Phenotype getPhenotype(
	const Genotype<Locus>& genotype, 
	const EpistasisGraph& graph) 
{
	Phenotype phenotype; 
	
	Allele dummyAllele{"U"};
	Gene root(dummyAllele, dummyAllele);

	evaluateNode(Locus::Root, root, genotype, phenotype);

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
	epistasisGraphConstructor();
	// --- HORSE CREATION ---
	std::cout << "   " << "Getting horses" << std::endl;

	// To create a Genotype, you must first create the map
	std::unordered_map<Locus, Gene> sireGenes = {
		{ Locus::Extension, Gene(Allele("E"), Allele("E")) },
		{ Locus::Agouti,    Gene(Allele("a"), Allele("a")) },
		{ Locus::Silver,    Gene(Allele("Z"), Allele("Z")) }
	};
	Genotype<Locus> sireG(0, std::move(sireGenes));

	std::unordered_map<Locus, Gene> damGenes = {
		{ Locus::Extension, Gene(Allele("E"), Allele("e")) },
		{ Locus::Agouti,    Gene(Allele("a"), Allele("a")) },
		{ Locus::Silver,    Gene(Allele("Z"), Allele("Z")) }
	};
	Genotype<Locus> damG(0, std::move(damGenes));

	auto sire = Horse('M', sireG);
	auto dam = Horse('F', damG);

	// --- GENOTYPE DISPLAY ---
	std::cout << "Sire's Genotype" << std::endl;
	sire.showGenes(Locus::Extension);
	sire.showGenes(Locus::Agouti);
	sire.showGenes(Locus::Silver);

	std::cout << "Dam's Genotype" << std::endl;
	dam.showGenes(Locus::Extension);
	dam.showGenes(Locus::Agouti);
	dam.showGenes(Locus::Silver);

	// --- OFFSPRING GENERATION & PHENOTYPE RESOLUTION ---
	std::cout << "Offspring's Genotype" << std::endl;
	auto offspring = generateOffspring(sire, dam);
	offspring.showGenes(Locus::Extension);
	offspring.showGenes(Locus::Agouti);
	offspring.showGenes(Locus::Silver);

	try {
		Phenotype finalPhenotype = getPhenotype(offspring.genotype, epistasisGraph);

		std::cout << "Offspring's Phenotype Traits:" << std::endl;
		finalPhenotype.printPhenotypeDescription();
	}
	catch (const NonexistantAlleles& e) {
		std::cerr << "\nPHENOTYPE ERROR: " << e.what() << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "\nAn unexpected error occurred: " << e.what() << std::endl;
	}

	return 0;
}