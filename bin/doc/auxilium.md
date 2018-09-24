### PRAELECTOR

- [Navigation dans la phrase](#nav)
- [La syntaxe ](#syntaxe)
- [Les noms](#noms)
- [Les verbes](#verbes)
- [Traduction](#traduction)
- [Enregistrement et relecture](#enr)
- [Pour les programmeurs](#prog)


## <a id="nav">Navigation dans la phrase</a>
Une fois lancé, Praelector ressemble beaucoup à une page web. Les choix se
font en cliquant sur des hyperliens colorés en bleu, comme dans un
navigateur.  Si l'on est plus à l'aise au clavier, la touche '/' le
programme affiche devant chaque lien un raccourci qui permet de se passer
de la souris.

La méthode de lecture la plus adaptée à Praelector est celle qui a été
décrite par William Gardner Hale en 1902, _The Art of Reading Latin: How to
Teach It_. S'il fallait résumer la en une phrase : **Le latin se lit de
gauche à droite, en s'interdisant de passer au mot suivant avant d'avoir
tiré le maximum des précédents.**

On avance donc dans la phrase avec les liens _mot suivant_ et _mot
précédent_, et tous les choix morphologiques, sémantiques et syntaxiques
possibles sont proposés par des liens. Ces choix ne peuvent pas être
toujours exhaustifs ni exacts, et l'on peut les corriger.

## <a id="syntaxe">La syntaxe : noyau, subordonné</a>
Les principes syntaxiques appliqués sont très classiques, et s'inspirent de
la grammaire de dépendance : à un mot peuvent être subordonnés un ou
plusieurs autres mots. L'ensemble formé par ce mot et ses subordonnés est
un _groupe syntaxique_. La nomenclature des grammaires est très variées :
le mot dont dépendent les autres mots du groupe est tantôt le _noyau_,
tantôt le _mot-tête_, ou plus simplement _tête_. Les subordonnés deviennent
parfois des _satellites_. Dans le code source de Praelector, j'ai adopté
les termes _super_ et _sub_. Un noyau et un subordonné forment un _syntagme_.

Quoi qu'il en soit, un syntagme est porteur de sens, et c'est la
combinaison de ces sens qui produit le sens de la phrase. Praelector
propose donc, à chaque mot lu, tous les syntagmes possibles après
l'application des règles qu'on lui a fournies, et par conséquent, avec
chaque syntagme, une traduction. Il essaie aussi de classer ces
propositions par probabilité, c'est à dire que les syntagmes les plus
probables apparaissent en premier.

Il s'agit alors, en s'aidant du sens et de la logique, de rejeter les
syntagmes qu'on juge impossible, ou de choisir celui dont on est sûr, ce
qui a pour conséquence d'éliminer tous les autres. Un mot, en effet, ne
peut être subordonné qu'à un seul autre mot. La seule exception est celle
du pronom relatif, qui est subordonné à la fois à son antécédent, et à un
autre mot de la proposition relative.

## <a id="noms">Les noms</a>

## <a id="verbes">Les verbes</a>

## <a id="traduction">La traduction</a>

## <a id="enr">Enregistrement et relecture</a>

## <a id="prog">Pour les programmeurs</a>
