> VU Nam – 1SN-N

# Rendu final
## Conception
Le fichier `Q*.c` est composé de la boucle principale et des handlers. \
La liste des `jobs` et sa gestion sont définies par `process`. \
Les fonctions intégrées au shell sont définies par `builtin`.

Pour la compilation, le `Makefile` est inclus.

## Questions 6 et 7
Je suis parti sur le scénario avec réinterprétation des signaux. \
Les `SIGSTP` sont transformés en `SIGSTOP` et les `SIGINT` en `SIGKILL`. \
`SIGSTP` et `SIGINT` sont masqués dans le fils.

## Questions 8, 9 et 10
Les redirections et les pipes fonctionnent avec les commandes externes *et internes*.
exemple fonctionnel: cat | tr a b | tr b c | tr c d < test > test2

## Tests
Je n'ai pas vraiment traité la Q9 j'étais passé direct à la 10...

Pour Q1-Q7 j'ai fait mes tests essentiellement avec des `sleep`. \
Pour la Q8, des `echo` et `cat`. \
Sur les Q9-Q10 des `echo` et `tr`.
