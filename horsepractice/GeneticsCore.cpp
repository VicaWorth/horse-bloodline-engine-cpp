#include "GeneticsCore.hpp"
#include "CustomExceptions.hpp"

#include <string>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>

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
		{"PRL", Locus::Pearl},
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
		{"U",   Locus::Unknown} // add others later
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

		if (locus == Locus::KIT) {
			// Represents Alleleic Series
			if (s == "W") {
				dominance = Dominance::Dominant;
			}
			else if (s == "RN") {
				dominance = Dominance::Codominant;
			}
			else if (s == "SB1") {
				dominance = Dominance::InDominant;
			}
			else if (s == "TO") {
				dominance = Dominance::Dominant;
			}
			else {
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

		if (alleles.first.dominance == Dominance::InDominant
			&& alleles.second.dominance == Dominance::InDominant) {
			if (std::isupper(alleles.second.symbol[0])) {
				std::swap(alleles.first, alleles.second);
			}
		}
	}

	/*Gene(const Gene& other) = default;
	Gene& operator=(const Gene& other) = default;*/

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
			}
			else if (std::isupper(alleles.first.symbol[0])
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
		}
		else if (alleles.first.dominance == Dominance::Dominant
			|| alleles.second.dominance == Dominance::Dominant) {
			return 1;
		}
		return 0;
	};

	constexpr bool hasAllele(std::string a) const {
		return (alleles.first.symbol == a
			|| alleles.second.symbol == a);
	};

	constexpr bool hasAlleleA_plus() const { // should be fixed
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
	auto end() { return m_genotype.end(); }

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