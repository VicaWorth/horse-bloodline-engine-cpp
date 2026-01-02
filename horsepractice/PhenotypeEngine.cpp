#include "PhenotypeEngine.hpp"
#include "GeneticsCore.hpp" 

#include <utility>
#include <functional>
#include <string>

using Rule = std::pair<bool (Gene::*)(void) const, std::string>;

EpistasisGraph epistasisGraph;

static bool evaluateNode(
	Locus currentNode,
	const Gene& parentGene,
	const Genotype<Locus>& genotype,
	Phenotype& phenotype,
	std::set<Locus>& visited);

static bool replaceColor(
	std::string& name, // Takes by reference to modify it directly
	const std::string& target,
	const std::string& replacement);

std::function<void(const Gene&, const Gene&, Phenotype&)>
createGeneRule(
	const std::vector<Rule>& rules,
	std::string defaultPhenotype = "")
{
	return [rules, defaultPhenotype](const Gene& parentGene, const Gene& gene, Phenotype& phenotype) {
		for (const auto& [predicate, resultName] : rules) {
			if (std::invoke(predicate, gene)) {
				phenotype.modifiers.push_back([resultName](std::string& name) {
					name = resultName + " " + name;
					});
				return;
			}
		}

		// Default case if given
		if (!defaultPhenotype.empty()) {
			phenotype.modifiers.push_back([defaultPhenotype](std::string& name) {
				name = defaultPhenotype + " " + name;
				});
		}
		};
}

std::function<void(const Gene&, const Gene&, Phenotype&)> createGeneRule(
	std::function<void(const Gene&, const Gene&, Phenotype&)> complexLogic
) {
	return complexLogic;
}

Phenotype getPhenotype(
	const Genotype<Locus>& genotype,
	const EpistasisGraph& graph)
{
	Phenotype phenotype;

	Allele dummyAllele{ "U" };
	Gene root(dummyAllele, dummyAllele);

	std::set<Locus> visited;

	evaluateNode(Locus::Root, root, genotype, phenotype, visited);

	phenotype.resolve();

	return phenotype;
}

static bool evaluateNode(
	Locus currentNode,
	const Gene& parentGene,
	const Genotype<Locus>& genotype,
	Phenotype& phenotype,
	std::set<Locus>& visited)
{
	if (visited.contains(currentNode)) {
		return false;
	}
	visited.insert(currentNode);

	if (epistasisGraph.find(currentNode) == epistasisGraph.end()) {
		throw NonexistantAlleles("Graph logic missing for: " + genotype.locusToString(currentNode));
	}

	const EpistasisNode& node = epistasisGraph.at(currentNode);

	if (!node.entryCondition(parentGene)) {
		return false;
	}

	bool parentIsInhibited = false;

	// don't fetch parent gene if parent is root
	const Gene& contextForChildren = (currentNode == Locus::Root)
		? parentGene
		: genotype.getGene(currentNode);

	for (const GraphEdge& edge : node.children) {
		bool childExpressed = evaluateNode(edge.locus, contextForChildren, genotype, phenotype, visited);

		if (childExpressed && edge.inhibitsParent) {
			parentIsInhibited = true;
		}
	}

	bool nodeUpdatedModifiers = false;

	if (!parentIsInhibited && node.resolver) {
		const Gene& childGene = (currentNode == Locus::Root)
			? parentGene
			: genotype.getGene(currentNode);

		size_t sizeBefore = phenotype.modifiers.size();
		node.resolver(parentGene, childGene, phenotype);

		if (phenotype.modifiers.size() > sizeBefore) {
			nodeUpdatedModifiers = true;
		}
	}

	return nodeUpdatedModifiers;
}

static bool replaceColor(
	std::string& name, // Takes by reference to modify it directly
	const std::string& target,
	const std::string& replacement)
{
	size_t pos = name.find(target);
	if (pos != std::string::npos) {
		name.replace(pos, target.length(), replacement);
		return true;
	}
	return false;
}

void epistasisGraphConstructor() {
	//Root
	epistasisGraph.insert({
		Locus::Root,
		{
			[](const Gene&) { return true; },
			{
				{ Locus::Extension },
				{ Locus::Pangare },
				{ Locus::SplashWhite },
				{ Locus::FrameOvero },
				{ Locus::Leopard },
				{ Locus::KIT },
				{ Locus::Sooty },
				{ Locus::Gray },
			},
			createGeneRule([](const Gene&, const Gene&, Phenotype& p) {
				p.modifiers.push_back([](std::string& name) {
					name = "";
				});
			})
		}
		});

	// Extension
	epistasisGraph.insert({
		Locus::Extension,
		{
			[](const Gene&) { return true; },
			{
				{ Locus::Agouti, true },
				{ Locus::Cream },
				{ Locus::Flaxen },
				{ Locus::Silver },
				{ Locus::Dun },
				{ Locus::Champagne }
			},
			createGeneRule(
				std::vector<Rule>{
					{ &Gene::isRecessivePresent, "Chestnut" }
				},
				"Black"
			)
		}
		});

	// Agouti
	epistasisGraph.insert({
		Locus::Agouti,
		{
			[](const Gene& parentGene) { return parentGene.isDominantPresent(); },
			{
				{
					{ Locus::Cream },
					{ Locus::Dun },
					{ Locus::Champagne }
				}
			},
			createGeneRule({
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
			{ },
			createGeneRule({
				{ &Gene::isDominantPresent, "Silver"}
			})
		}
		});

	// Flaxen
	epistasisGraph.insert({
		Locus::Flaxen,
		{
			[](const Gene& parentGene) { return parentGene.isRecessivePresent(); },
			{ },
			createGeneRule({
				{ &Gene::isRecessivePresent, "Flaxen"}
			})
		}
		});

	// Pangare
	epistasisGraph.insert({
		Locus::Pangare,
		{
			[](const Gene& parentGene) { return true; },
			{ },
			createGeneRule({
				{ &Gene::isDominantPresent, "Pangare"}
			})
		}
		});

	// Splash White
	epistasisGraph.insert({
		Locus::SplashWhite,
		{
			[](const Gene& parentGene) { return true; },
			{ },
			createGeneRule({
				{ &Gene::isDominantPresent, "Splash White"}
			})
		}
		});

	// Frame Overo
	epistasisGraph.insert({
		Locus::FrameOvero,
		{
			[](const Gene& parentGene) { return true; },
			{ },
			createGeneRule({
				{ &Gene::isDominantPresent, "Frame Overo"}
			})
		}
		});

	// Sooty
	epistasisGraph.insert({
		Locus::Sooty,
		{
			[](const Gene& parentGene) { return true; },
			{ },
			createGeneRule({
				{ &Gene::isDominantPresent, "Sooty"}
			})
		}
		});

	// Gray
	epistasisGraph.insert({
		Locus::Gray,
		{
			[](const Gene& parentGene) { return true; },
			{ },
			createGeneRule({
				{ &Gene::isDominantPresent, "Gray"}
			})
		}
		});

	// KIT
	epistasisGraph.insert({
		Locus::KIT,
		{
			[](const Gene& parentGene) { return true; },
			{ },
			createGeneRule([](const Gene& parentGene, const Gene& childGene, Phenotype& p) {
				p.modifiers.push_back([childGene](std::string& name) {
				if (childGene.hasAllele("RN") && childGene.hasAllele("TO")) {
					if (replaceColor(name, "", "Roan Tobiano ")) return;
				}
				else if (childGene.hasAllele("RN") && childGene.hasAllele("SB1")) {
					if (replaceColor(name, "", "Sabino Roan ")) return;
				}
				else if (childGene.hasAllele("RN") && childGene.hasAllele("n")) {
					if (replaceColor(name, "", "Roan ")) return;
				}
				else if (childGene.hasAllele("TO") && childGene.hasAllele("SB1")) {
					if (replaceColor(name, "", "Tovero ")) return;
				}
				else if (childGene.hasAllele("TO")) {
					if (replaceColor(name, "", "Tobiano ")) return;
				}
				else if (childGene.hasAllele("SB1") && childGene.isHomozygous()) {
					if (replaceColor(name, "", "Sabino White ")) return;
				}
				else if (childGene.hasAllele("SB1")) {
					if (replaceColor(name, "", "Sabino ")) return;
				}
				});
			})
		}
		});

	// Leopard
	epistasisGraph.insert({
		Locus::Leopard,
		{
			[](const Gene& parentGene) { return true; },
			{ { Locus::MLeopard } },
			createGeneRule({
				{ &Gene::isDominantPresent, "Varnish Roan Appaloosa"}
			})
		}
		});

	// Leopard Pattern
	epistasisGraph.insert({
		Locus::MLeopard,
		{
			[](const Gene& parentGene) { return parentGene.isDominantPresent(); },
			{ },
			createGeneRule([](const Gene& parentGene, const Gene& childGene, Phenotype& p) {
				if (childGene.isDominantPresent()) {
					p.modifiers.push_back([parentGene](std::string& name) {
						if (parentGene.isHomozygous()) {
							if (replaceColor(name, "Varnish Roan Appaloosa", "Few Spot Leopard Appaloosa")) return;
						}
						else {
							if (replaceColor(name, "Varnish Roan Appaloosa", "Leopard Appaloosa")) return;
						}
					});
				}
			})
		}
		});

	// Dun
	epistasisGraph.insert({
		Locus::Dun,
		{
			[](const Gene& parentGene) { return true; },
			{ },
			createGeneRule([](const Gene& parentGene, const Gene& gene, Phenotype& p) {
				if (gene.isDominantPresent()) {
				p.modifiers.push_back([](std::string& name) {
					if (replaceColor(name, "Chestnut", "Red Dun")) return;
					if (replaceColor(name, "Palomino", "Dunalino")) return;
					if (replaceColor(name, "Cremello", "Cremello Dun")) return;
					if (replaceColor(name, "Bay", "Bay Dun")) return;
					if (replaceColor(name, "Buckskin", "Dunskin")) return;
					if (replaceColor(name, "Perlino", "Perlino Dun")) return;
					if (replaceColor(name, "Black", "Grullo")) return;
					if (replaceColor(name, "Smoky Black", "Smoky Grullo")) return;
					if (replaceColor(name, "Smoky Cream", "Smoky Cream Dun")) return;
					if (replaceColor(name, "Seal Brown", "Brown Dun")) return;
					if (replaceColor(name, "Wild Bay", "Wild Bay Dun")) return;
				});
				}
			})
		}
		});

	// Champagne
	epistasisGraph.insert({
		Locus::Champagne,
		{
			[](const Gene& parentGene) { return true; },
			{ },
			createGeneRule([](const Gene& parentGene, const Gene& gene, Phenotype& p) {
				if (gene.isDominantPresent()) {
				p.modifiers.push_back([](std::string& name) {
					if (replaceColor(name, "Chestnut", "Gold Champagne")) return;
					if (replaceColor(name, "Palomino", "Gold Cream")) return;
					if (replaceColor(name, "Black", "Classic Champagne")) return;
					if (replaceColor(name, "Smoky Black", "Classic Cream")) return;
					if (replaceColor(name, "Bay", "Amber Champagne")) return;
					if (replaceColor(name, "Buckskin", "Amber Cream")) return;
					if (replaceColor(name, "Seal Brown", "Sable Champagne")) return;
					if (replaceColor(name, "Wild Bay", "Wild Bay Champagne")) return;
				});
				}
			})
		}
		});

	// Cream
	epistasisGraph.insert({
		Locus::Cream,
		{
		[](const Gene& g) { return true; },
		{ },
		createGeneRule([](const Gene& parentGene, const Gene& gene, Phenotype& p) {
			int count = gene.getActiveCount();
			p.modifiers.push_back([count](std::string& name) {
				if (count == 1) { // Single Dilute
					if (replaceColor(name,"Chestnut", "Palomino")) return;
					if (replaceColor(name, "Bay", "Buckskin")) return;
					if (replaceColor(name, "Black", "Smoky Black")) return;
					if (replaceColor(name, "Seal Brown", "Smoky Brown")) return;
				}
 else if (count == 2) { // Double Dilute
  if (replaceColor(name, "Chestnut", "Cremello")) return;
  if (replaceColor(name, "Bay", "Perlino")) return;
  if (replaceColor(name, "Black", "Smoky Cream")) return;
  if (replaceColor(name, "Seal Brown", "Smoky Brown")) return;
}
});
})
}
		});
}