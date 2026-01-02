#include "Phenotype.hpp"
#include "GeneticsCore.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <functional>

struct Phenotype {
	std::string name;

	// Stack of modifiers applied to the phenotype
	using Modifier = std::function<void(std::string&)>;
	std::vector<Modifier> modifiers;

	void resolve() {
		for (auto it = modifiers.rbegin(); it != modifiers.rend(); ++it) {
			(*it)(name);
		}
	}

	void printPhenotypeDescription() const {
		if (name.empty()) {
			std::cout << "Unknown Phenotype" << std::endl;
		}
		else {
			std::cout << name << std::endl;
		}
	}
};