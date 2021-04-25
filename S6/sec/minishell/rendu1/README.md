> VU Nam – 1SN-N

# Rendu 1
## Conception
Le fichier `Q*.c` est composé de la boucle principale et des handlers.  
La liste des `jobs` et sa gestion sont définies par `process`.  
Les fonctions intégrées au shell sont définies par `builtin`.

## Bloquages / Questions 6 & 7
Pour la question 6 le ctrl+z ne fonctionne pas convenablement.  
Pour la question 7 il me semble qu'il "suffirait" de définir un handler pour sigint qui le transmet au fils seulement si ce dernier est en avant plan. (facile dit comme ça, mais j'ai pas encore réussi la 6 correctement...)
