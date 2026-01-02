#pragma once

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

class Gene;
template <typename T> class Genotype;

struct Allele;
class Gene;