# horse-bloodline-engine-cpp
A cpp application that allows you to simulate horse genetics. 

## Supported Loci
The engine currently simulates the following genetic markers:
*   **Extension (E)**
*   **Agouti (A)** - Including Wild Bay ($A^+$) and Seal Brown ($A^t$)
*   **KIT** - Tobiano, Roan, Sabino 1, Dominant White
*   **Silver (Z)**
*   **Cream (Cr)**
*   **Dun (D)**
*   **Champagne (Ch)**
*   **Gray (G)**
*   **Splash White (SW)**
*   **Frame Overo (O)**
*   **Leopard (LP)** and **MLeopard (PATN1)**
*   **Flaxen (f)**
*   **Pangare (PNG)**
*   **Sooty (STY)**

---

### Extension ($E$) and Agouti ($A$)

The **Extension** locus determines if a horse is black-based or red-based. The **Agouti** locus then determines where that black pigment is distributed. Agouti has no visual effect on red-based ($ee$) horses.

**Agouti Dominance Hierarchy:** $A^{+} > A > A^{t} > a$

| Genotype | Base Color | Phenotype Description |
| :--- | :--- | :--- |
| **$ee$ \_\_** | Chestnut | Red-based; Agouti is present but hidden. |
| **$E\\_$ $A^+$** | Wild Bay | Black is restricted to very low points on the legs (fetlocks). |
| **$E\\_ A\\_$** | Bay | Black is restricted to "points" (mane, tail, lower legs). |
| **$E\\_ A^t\\_$** | Seal Brown | Black is mostly unrestricted, with tan/red muzzle, eyes, and flanks. |
| **$E\\_$ $aa$** | Black | Black pigment covers the entire body. |

### The Cream Locus ($Cr, prl, sun, sno$)
This locus controls the primary dilution of the horse. A single copy of Cream (**$nCr$**) creates a single dilute. Two copies of Cream (**$CrCr$**), or one copy of Cream paired with any recessive allele (**$Crprl$**, **$Crsun$**, **$Crsno$**), create a phenotype nearly identical to a double-dilute.

| Base Genetics | Base Name | Single Cream (**$nCr$**) | Double Cream (**$CrCr$**) | Cream Pseudo-Double (**$Crprl, Crsun, Crsno$**) |
| :--- | :--- | :--- | :--- | :--- |
| **$ee \\\_\\\_$** | Chestnut | **Palomino** | **Cremello** | **Pseudo-Double Chestnut** |
| **$E\\\_ A^{+\\\_}$** | Wild Bay | **Wild Buckskin** | **Wild Perlino** | **Pseudo-Double Wild Bay** |
| **$E\\\_ A\\\_$** | Bay | **Buckskin** | **Perlino** | **Pseudo-Double Bay** |
| **$E\\\_ A^{t\\\_}$** | Seal Brown | **Brown Buckskin** | **Brown Perlino** | **Pseudo-Double Brown** |
| **$E\\\_ aa$** | Black | **Smoky Black** | **Smoky Cream** | **Pseudo-Double Black** |

#### Homozygous Recessive Dilutions
These phenotypes occur only when a horse carries two copies of the same recessive allele. Heterozygous recessive horses (e.g., **$nprl$**, **$nsun$**, **$nsno$**) show no dilution and appear as their base color.

| Base Genetics | Base Name | Double Pearl (**$prlprl$**) | Double Sun (**$sunsun$**) | Double Snowdrop (**$snosno$**) |
| :--- | :--- | :--- | :--- | :--- |
| **$ee$ \_\_** | Chestnut | **Apricot** | **Sun Chestnut** | **Snowdrop Chestnut** |
| **$E\\\_ A\\\_$** | Bay | **Bay Pearl** | **Sun Bay** | **Snowdrop Bay** |
| **$E\\\_ aa$** | Black | **Black Pearl** | **Sun Black** | **Snowdrop Black** |

#### Recessive Interaction Pseudo-Doubles ($prl, sun, sno$)
When two different recessive alleles are paired on the same locus (e.g., **$prlsun$**, **$prlsno$**, or **$sunsno$**), they interact to create a **Pseudo-Double Dilute**. In the equine community, when these combinations result in a phenotype identical to a double-dilute, they are referred to by the corresponding Cream names.

| Base Genetics | Base Name | Mixed Pseudo-Double (**$prlsun, prlsno, sunsno$**) |
| :--- | :--- | :--- |
| **$ee$ \_\_** | Chestnut | **Cremello** |
| **$E\\\_ A^{+\\\_}$** | Wild Bay | **Wild Perlino** |
| **$E\\\_ A\\\_$** | Bay | **Perlino** |
| **$E\\\_ A^{t\\\_}$** | Seal Brown | **Brown Perlino** |
| **$E\\\_ aa$** | Black | **Smoky Cream** |

### The Dun Locus ($D, nd1, nd2$)
The Dun locus determines both body dilution and primitive markings (dorsal stripes, leg bars). Horses carrying **$nd1$** are often referred to as **"False Duns"** because they possess primitive markings but lack the body color dilution seen in true Duns. 

The interaction between Dun and Snowdrop (**$nSno$**) creates the **Snowdrop** phenotype, characterized by a highly desaturated, near-white body color that makes the dark primitive markings stand out sharply.

| Base Genetics | Base Name | Dun Phenotype (**$D\\_$**) | False Dun (**$nd1\\\_$**) |
| :--- | :--- | :--- | :--- |
| **$ee$ \_ \_** | Chestnut | **Red Dun** | **Primitive Chestnut** |
| **$E\\\_ A^{+\\\_}$** | Wild Bay | **Wild Dun** | **Primitive Wild Bay** |
| **$E\\\_ A\\\_$** | Bay | **Bay Dun** | **Primitive Bay** |
| **$E\\\_ A^{t\\\_}$** | Seal Brown | **Brown Dun** | **Primitive Brown** |
| **$E\\\_ aa$** | Black | **Grullo** | **Primitive Black** |

##### ND1 IS NOT SUPPORTED AT THIS TIME

#### Complex Interactions: Dun and Cream
When the Dun gene (**$D$**) or the False Dun gene (**$nd1$**) interact with Cream (**$Cr$**), they create specific "Double Dilute" phenotypes. While True Duns exhibit both body dilution and markings, False Duns (**$nd1$**) display primitive markings on a non-diluted base.

| Base Genetics | + Dun (**$D\\\_$**) | + False Dun (**$nd1\\\_$**) | + Dun & Cream (**$D\\\_ nCr$**) | + Double Cream (**$CrCr$**) | + Dun & Double Cream (**$D\\\_ CrCr$**) |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **$ee$ \_ \_** (Chestnut) | **Red Dun** | **Primitive Chestnut** | **Dunalino** | **Cremello** | **Dunello** |
| **$E\\\_ A^{+\\\_}$** (Wild Bay) | **Wild Dun** | **Primitive Wild Bay** | **Wild Dunskin** | **Wild Perlino** | **Wild Perlino Dun** |
| **$E\\\_ A\\\_$** (Bay) | **Bay Dun** | **Primitive Bay** | **Dunskin** | **Perlino** | **Perlino Dun** |
| **$E\\\_ A^{t\\\_}$** (Brown) | **Brown Dun** | **Primitive Brown** | **Brown Dunskin** | **Brown Perlino** | **Brown Perlino Dun** |
| **$E\\\_ aa$** (Black) | **Grullo** | **Primitive Black** | **Smoky Grullo** | **Smoky Cream** | **Smoky Cream Dun** |

##### ND1 IS NOT SUPPORTED AT THIS TIME

### Pangare ($P$)
Primarily affects red pigment (pheomelanin).

| Base Color | Effect of Pangare ($P\\_$) | Visual Description |
| :--- | :--- | :--- |
| **Chestnut** | Mealy Chestnut | Pale flaxen muzzle, belly, and inner legs. |
| **Wild Bay** | Mealy Wild Bay | Significant lightening of the red body; very pale muzzle. |
| **Bay** | Mealy Bay | Pale "mealy" areas on muzzle and underbelly. |
| **Seal Brown** | Mealy Brown | Highlights the tan areas of the muzzle and flanks. |
| **Black** | Minimal/None | Usually invisible as there is no red pigment to lighten. |

### Sooty ($Sty$)
Adds black hairs to the coat, usually concentrated on the topline.

| Base Color | Effect of Sooty ($STY\\_$) | Visual Description |
| :--- | :--- | :--- |
| **Chestnut** | Sooty Chestnut | Deepens the coat; can look like a "Liver Chestnut." |
| **Palomino** | Sooty Palomino | Can create "chocolate" palominos or fake dapples. |
| **Buckskin** | Sooty Buckskin | Darkens the back; often creates a "countershaded" look. |
| **Bay** | Sooty Bay | Turns a bright bay into a dark or "mahogany" bay. |
| **Black** | Hidden | Invisible, as the horse is already maximum black. |

### Silver Dapple ($Z$)
The Silver gene dilutes black pigment (eumelanin). It does not affect red pigment (pheomelanin).

| Base Color | Genotype | Resulting Phenotype |
| :--- | :--- | :--- |
| **Black ($E\\_ aa$)** | **$Z\\_$** | **Silver Black** (Chocolate with flaxen mane) |
| **Bay ($E\\_ A\\_$)** | **$Z\\_$** | **Silver Bay** (Red body, flaxen mane/legs) |
| **Chestnut ($ee$)** | **$Z\\_$** | **Chestnut** (Carrier - No visual change) |

### Flaxen ($f$)
A recessive modifier that only affects the mane and tail of red-based horses.

| Base Color | Genotype | Resulting Phenotype |
| :--- | :--- | :--- |
| **Chestnut ($ee$)** | **$ff$** | **Flaxen Chestnut** (Blonde mane and tail) |
| **Chestnut ($ee$)** | **$F\\_$** | **Chestnut** (Red mane and tail) |
| **Black/Bay ($E\\_$)** | **$ff$** | **Black/Bay** (Hidden - No effect on black pigment) |

### Champagne ($Ch$)
A dominant dilution that creates a metallic sheen, hazel eyes, and pinkish/lavender freckled skin.

| Base Color | Genotype | Resulting Phenotype |
| :--- | :--- | :--- |
| **Black ($E\\_ aa$)** | **$Ch\\_$** | **Classic Champagne** |
| **Bay ($E\\_ A\\_$)** | **$Ch\\_$** | **Amber Champagne** |
| **Seal Brown ($E\\_ A^t\\_$)** | **$Ch\\_$** | **Sable Champagne** |
| **Chestnut ($ee$)** | **$Ch\\_$** | **Gold Champagne** |

### Gray ($G$)
Gray is an epistatic gene. It acts as a "mask" that slowly covers the horse's birth color with white hair over time.

| Genotype | Resulting Phenotype |
| :--- | :--- |
| **$G\\_$** | **Gray** (Horse lightens with age until white) |
| **$gg$** | **No Gray** (Horse retains its base color) |

### The KIT Locus ($KIT$)
The following four patterns exist on the same gene. A horse can only have **two** alleles total from this list (e.g., a horse can be $TO/RN$, but cannot be $TO/RN/SB1$).

| Pattern | Genotype | Visual Effect |
| :--- | :--- | :--- |
| **Tobiano** | **$TO\\_$** | Large, smooth vertical white patches crossing the spine. |
| **Roan** | **$RN\\_$** | White hairs ticked into the coat; head and legs stay dark. |
| **Sabino 1** | **$SB1\\_$** | Lacy white markings, belly spots, and high white stockings. |
| **Dominant White**| **$W\\_$** | Ranges from high-white markings to a completely white horse. |

### White Spotting: Splash and Frame ($SW$ & $O$)
These patterns are independent of the KIT locus.

| Pattern | Genotype | Phenotype / Note |
| :--- | :--- | :--- |
| **Splash White** | **$SW\\_$** | "Dipped in paint" look; often causes blue eyes. |
| **Frame Overo** | **$nO$** | Jagged horizontal white patches on the side of the neck/barrel. |
| **Frame Overo** | **$OO$** | **LETHAL.** (Lethal White Overo Syndrome - foal dies at birth). |

### Leopard Complex ($LP$ & $PATN1$)
The **LP** gene is the "master switch" for Appaloosa traits, while **PATN1** (MLeopard) determines the amount of white coverage.

| LP Genotype | PATN1 Genotype | Resulting Phenotype |
| :--- | :--- | :--- |
| **$Lplp$** | **$patn1 / patn1$** | **Varnish Roan** (Characteristic Appaloosa mottling) |
| **$Lplp$** | **$PATN1\\_$** | **Leopard Appaloosa** (White with dark spots) |
| **$LpLp$** | **$patn1 / patn1$** | **Snowcap / Blanket** (Solid white over the hips) |
| **$LpLp$** | **$PATN1\\_$** | **Few-Spot Leopard** (Nearly all white) |
