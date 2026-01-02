#pragma once

#include <functional>
#include <set>

#include "Phenotype.hpp"

std::function<void(const Gene&, const Gene&, Phenotype&)>
createGeneRule(
	const std::vector<Rule>& rules,
	std::string defaultPhenotype = "");

std::function<void(const Gene&, const Gene&, Phenotype&)> createGeneRule(
	std::function<void(const Gene&, const Gene&, Phenotype&)> complexLogic
);

Phenotype getPhenotype(
	const Genotype<Locus>& genotype,
	const EpistasisGraph& graph);

void epistasisGraphConstructor();

extern EpistasisGraph epistasisGraph;