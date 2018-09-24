### PRAELECTOR

<a name="index">index</a>    

- [Navigation dans la phrase](#nav)
- [La syntaxe ](#syntaxe)
- [Préparer les phrases à lire](#phrases)
- [Morphologie et traduction du mot](#mots)
- [Les liens syntaxiques](#liens)
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

[index](#index)
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

[index](#index)
## <a id="phrases">Préparer les phrases à lire</a>
Il y a deux méthodes pour charger dans Praelector la phrase qu'on a
l'intention de lire. La plus simple est de cliquer sur le lien
_Saisir une phrase_, et de la saisir dans le dialogue que s'ouvre. Le plus
simple est même de la copier à partir d'un document ou d'une page web, et
de la coller dans le dialogue. Dès qu'on valide la saisie, la lecture peut
commencer. Toute ligne commençant par un point d'exclamation sera affichée,
mais ne sera pas considérée comme une phrase latine à lire.

L'autre méthode, plus organisée, consiste à construire un fichier où chaque
phrase occupe une ligne. On placera ce fichier dans le répertoire _corpus/_.
Lorqu'on choisira ensuite ce fichier en cliquant sur _choisir une phrase_,
toutes les phrases apparaîtront numérotées, et il suffira d'en sélectionner une.

## <a id="mots">Morphologies et traductions du mot</a>

Comme le français, les mots latins sont souvent ambigus. En français, un
mot comme _suis_ peut ête une forme du verbe _être_, mais aussi du verbe
_suivre_. Le mot _avions_ est le plus souvent l'imparfait du verbe _avoir_,
mais il est aussi le pluriel du nom _avion_. En latin, le mot _arma_ est
une forme de _arma, orum _: « les armes », mais c'est aussi, beaucoup plus
rarement, l'impératif du verbe _armo_. _cano_ est un verbe qui signifie
« chanter », mais c'est aussi une forme de l'adjectif _canus_, « blanc ».

D'autre part, plusieurs morphologies du même mot peuvent avoir la même
graphie. _uitae_, par exemple peut être le génitif singulier, le datif
singulier, le nominatif pluriel ou le vocatif pluriel de _uita_.

Praelector propose, en essayant de les ordonner par fréquences, toutes les
possibilités d'interprétation d'une forme. On peut choisir définitivement
l'une d'entre elles, ou en rejeter une ou plusieurs.

Dès qu'un choix est fait dans cette partie de l'interface, les liens
partant et aboutissant à ce mot sont mis à jour.

Praelector essaie de traduire chaque possibilité en français, et cette
traduction, dans la mesure du possible, est _fléchie_, c'est à dire
accordée ou conjuguée. Lorsque c'est possible, on peut choisir une
traduction, ou même en ajouter une.

[index](#index)
## <a id="liens">Les liens syntaxiques</a>
À la lecture d'un mot, c'est à dire le premier quand la lecture commence,
et le suivant si on demande la suite de la lecture, Praelector fait deux
opérations et affiche les résultats.

* Il consulte la liste des syntagmes incomplets, et exæmine si le mot
pourrait convenir pour que le syntagme soit complet. IL affiche ensuite
toutes les possibilités. Quelquefois, il n'y en a qu'une, qu'il ne faudra
pas nécessairement valider. Il y en aura plus souvent plusieurs, et il est
conseillé de rejeter les solutions qui sont manifestement fausses.
Lorsqu'on est sûr qu'une solution est la bonne, on la valide, et si le mot
est un subordonné, tous les autres candidats pour être son noyau disparaissent.
Chåque syntagme possible est donc affiché, accompagné de sa traduction. Si
plusieurs traductions sont possibles, le lien _tr. suiv._ permet d'en changer.

* Il consulte sa liste de règles, et retient celles dont le mot pourrait
être le noyau, ou le subordonné. Mais le syntagme que formerait le mot avec
un autre, qu'il soit noyau ou subordonné, est incomplet. Il lance donc
l'hypothèse. Ces hypothèse sont souvent très nombreuses, parce que la base
des règles est abondante. Cette liste n'est donc pas affichée.

* Il récapitule la traduction de la phrase, en regroupant les syntagmes
autour des noyaux, et en essayant de respecter la syntaxe française, avec,
hélas, des erreurs encore trop nombreuses.

[index](#index)
## <a id="enr">Enregistrement et relecture</a>
Le lien _enregistrer_ permet de consigner dans le fichier homonyme du
répertoire _enr/_ la lecture d'une phrase, avec tous ses choix,
ses allers et retours et ses traductions. Pour relire cet enregistrement,
il faut suivre le lien _choisir une phrase enregistrée_. La fenêtre
ressemblera alors à la fenêtre habituelle, mais le lien sélectionné par le
lecteur qui a enregistré sera surligné en rouge, on pourra le sélectionner
et poursuiver sa lecture.

[index](#index)
## <a id="prog">Pour les programmeurs</a>
Toutes les données utilisées par Praelectore sont dans _data/_. Parmi elles,
quelques fichiers sont issus de Collatinus. Les autres servent aux règles et
au traductions. Leur format est commenté en tête de chaque fichier.
