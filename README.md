# horse-bloodline-engine-cpp
A cpp application that allows you to simulate horse genetics. 

### Horse Genetics: The Extension Locus (E)

The Extension Locus is a key gene that determines a horse's base coat color by controlling the production of black (eumelanin) and red (pheomelanin) pigment. The allele 'E' is dominant over the recessive 'e' allele.

*   **E (Dominant Allele):** Allows for the production of black pigment.
*   **e (Recessive Allele):** Restricts pigment production to red only.

Here is a summary of how the genotypes at the Extension Locus translate to the horse's base color:

| Genotype | Dominance Explained | Resulting Phenotype (Base Color) |
| :--- | :--- | :--- |
| **E_** | The presence of at least one dominant 'E' allele allows for the production of black pigment. | Black-Based (e.g., Black, Bay) |
| **ee** | With two recessive 'e' alleles, the horse cannot produce black pigment, resulting in a red coat. | Red-Based (e.g., Chestnut) |

*_Note: The underscore in "E_" indicates that the second allele can be either 'E' or 'e'; the dominant 'E' will still dictate a black-based coat._

### Horse Genetics: The Agouti Locus (A)

Here is how the Agouti and Extension loci work together:

| Genotype | Dominance Explained | Resulting Phenotype (Color) |
| :--- | :--- | :--- |
| **E_ A_** | The horse is black-based, and the dominant 'A' allele restricts black pigment to the points. | Bay |
| **E_ aa** | The horse is black-based, and the absence of a dominant 'A' allows black pigment to cover the body. | Black |
| **ee __** | The horse is red-based. Agouti has no effect on red pigment, so its genotype is irrelevant to the coat color. | Chestnut |

*_Note: The Agouti gene only affects black pigment (eumelanin). On a red-based horse (ee), the Agouti genotype (AA, Aa, or aa) is "silent" and does not change the horse's chestnut appearance._
