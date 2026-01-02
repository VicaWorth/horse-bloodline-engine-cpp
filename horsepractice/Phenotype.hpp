#pragma once 

#include "GeneticsCore.hpp"

struct Phenotype;
struct EpistasisNode;
struct GraphEdge;
struct EpistasisNode;

struct GraphEdge {
	Locus locus;
	bool inhibitsParent; // If true child expresses, parent does not, false and both express
};
struct EpistasisNode {
	std::function<bool(const Gene&)> entryCondition;
	std::vector<GraphEdge> children;
	std::function<void(const Gene&, const Gene&, Phenotype&)> resolver;
};

using EpistasisGraph = std::unordered_map<Locus, EpistasisNode>;