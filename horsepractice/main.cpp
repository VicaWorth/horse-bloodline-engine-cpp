#include <iostream>

// Custom Includes
#include "PhenotypeEngine.hpp"
#include "GeneticsCore.hpp"
#include "CustomExceptions.hpp"
#include "Simulation.hpp"
#include "Phenotype.hpp"

int main()
{
	try {
		epistasisGraphConstructor();
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
		Genotype<Locus> sireG(0, std::move(sireGenes));

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
		Genotype<Locus> damG(0, std::move(damGenes));

		auto sire = Horse('M', sireG);
		auto dam = Horse('F', damG);

		std::cout << "Sire's Genotype" << std::endl;
		sire.showAllGenes();

		std::cout << "Dam's Genotype" << std::endl;
		dam.showAllGenes();

		std::cout << "Offspring's Genotype" << std::endl;
		auto offspring = generateOffspring(sire, dam);
		offspring.showAllGenes();
	
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