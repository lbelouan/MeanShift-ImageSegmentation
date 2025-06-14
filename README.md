# Mean Shift - Algorithme de Segmentation et Classification d'Image

## Avantages et Inconvénients

### Avantages
- Le nombre de classes ne doit pas être renseigné.
- Sa mise en œuvre est relativement simple.

### Inconvénients
- Son principal désavantage est son temps d’exécution qui est long.

---

## Principe de Fonctionnement

![Principe du Mean Shift](https://github.com/user-attachments/assets/1cc357c8-6e4f-490c-9aba-b949f04f25cc)

1. **Sélection d’un point initial** : On sélectionne un pixel (ou point) de l’image.
2. **Définition de la région de recherche** : On prend tous les pixels situés dans une fenêtre spatiale centrée sur ce point (définie par un rayon `hs`).
3. **Filtrage colorimétrique** : Parmi ces pixels, on ne garde que ceux dont la couleur est proche de celle du pixel initial, selon une fenêtre de couleur (définie par un rayon `hc`).
4. **Calcul du nouveau centre** : On calcule la moyenne pondérée des positions des pixels restants (en prenant en compte leurs couleurs et positions). Ce nouveau centre devient le centre de la fenêtre de recherche.
5. **Convergence** : On répète l’étape 4 tant que la position du centre change de manière significative.
6. **Traitement de tous les pixels** : On applique ce processus à l’ensemble des pixels de l’image pour regrouper les régions de couleurs similaires et segmenter l’image.

---

## Points d'Attention

- Il est nécessaire de fixer les paramètres de fenêtre spatiale (`hs`) et de fenêtre de couleur (`hc`).
  - **Si `hs` est grand** : L'algorithme se concentre moins sur les détails, ce qui peut en omettre certains.
  - **Si `hs` est trop petit** : L'algorithme peut introduire du bruit en générant de nombreux petits clusters (faux positifs).
  - **Si `hc` est trop grand** : Les couleurs sont homogénéisées, ce qui peut entraîner une perte de détails.
  - **Si `hc` est trop faible** : On risque une sur-segmentation avec une distinction excessive des zones.

- **Problème aux bordures**  
  Étant donné que l’algorithme fait glisser une fenêtre sur l’image, il faut l’agrandir de `hs` sur les côtés pour éviter des problèmes aux bordures.  
  Un **zero padding** simple est ajouté : bordures noires d’épaisseur `hs` sur tous les côtés (ligne 61 du code).

- **Gestion des clusters proches en couleur**  
  Lors de l'exécution, des moyennes légèrement différentes peuvent apparaître.  
  - Une liste est utilisée pour comparer chaque pixel aux éléments déjà stockés.  
  - Si un pixel a une couleur proche d’un élément existant, il prend sa valeur et n'est pas ajouté à la liste.  
  - Sinon, il est ajouté, ce qui limite le nombre de clusters (ligne 138 du code).

- **Optimisation du temps d’exécution**  
  L’algorithme est lent, donc on propose à l’utilisateur de **réduire la taille de l’image** en divisant ses dimensions par un entier (`resize`).  
  - Le Mean Shift est appliqué sur l’image réduite pour accélérer le traitement.  
  - Ensuite, un **redimensionnement à la taille originale** est effectué avec une **interpolation linéaire** pour éviter un flou excessif.

- **Polarisation de l’image**  
  Pour comparer l’image segmentée à une image en noir et blanc, une polarisation est appliquée :
  - La moyenne des clusters est calculée.
  - Si la valeur d’un cluster est supérieure ou inférieure à cette moyenne, il est classé en noir ou en blanc (ligne 178 du code).

---

## Évaluation de la segmentation

Les métriques suivantes sont utilisées pour évaluer la segmentation :

- **Précision (`Pmine`)**  
  - Une **précision élevée** signifie que la majorité des prédictions positives sont correctes.
  - Une **précision faible** indique la présence de nombreux faux positifs.

- **Sensibilité (`Smine`)**  
  - Une **sensibilité élevée** signifie que presque tous les éléments positifs ont été détectés.
  - Une **sensibilité faible** signifie que de nombreux éléments positifs ont été oubliés.

- **Dice Similarity Coefficient (`DSCmine`)**  
  - Une valeur proche de **1** indique une correspondance parfaite entre la segmentation obtenue et la référence.
  - Une valeur proche de **0** signifie une correspondance très faible.

---

## Exemples d'Utilisation sur des Images de Texture (512 × 512)

| Référence | Segmentation Mean Shift |
|-----------|------------------------|
| ![Texture 3](https://github.com/user-attachments/assets/ea9c2051-356f-400d-bddf-ba2672cc2781) | ![Texture 3 Mine](https://github.com/user-attachments/assets/c8f221d5-3e63-4bbb-a23f-2251b6d4ec55) |
| ![Texture 3 VT](https://github.com/user-attachments/assets/bd374dc4-3210-4dd6-81f8-76adbf3ffd8c) | ![Texture 3 VT Mine](https://github.com/user-attachments/assets/50f51724-9401-48ca-9db9-cfc8db33465c) |

---

## Résultats

- **Facteur de compression** : 1.5
- **Nombre de clusters différents** : 3
- **Précision (`Pmine`)** : 0.598684
- **Sensibilité (`Smine`)** : 0.939337
- **Dice Similarity Coefficient (`DSCmine`)** : 0.731285
- **Temps d'exécution** : 382.033 secondes
- **On remarque une précision faible on a en effet plus de cases noires que dans la référence, on pourrait alors modifier notre binarisation en conséquence**

---

| Référence | Segmentation Mean Shift |
|-----------|------------------------|
|![image](https://github.com/user-attachments/assets/6a07cf5f-b27a-43ff-82e5-0a3b37cffce1) | ![image](https://github.com/user-attachments/assets/5a117051-b30b-4b87-828b-b668327fd9f3)|
|![image](https://github.com/user-attachments/assets/acbbca15-ed42-4859-a429-99d2db7a74a3)| ![image](https://github.com/user-attachments/assets/d7a3bf24-2a6a-47e8-8f20-29f8a1aa940f)|

---

## Résultats

- **Facteur de compression** : 1
- **Nombre de clusters différents** : 4
- **Précision (`Pmine`)** : 0.434241
- **Sensibilité (`Smine`)** : 0.38475
- **Dice Similarity Coefficient (`DSCmine`)** : 0.408
- **Temps d'exécution** : 442.947 secondes
- **Le résultat en couleur n'est pas très  bon. Cela est du au zero-padding.**

---

| Référence | Segmentation Mean Shift |
|-----------|------------------------|
| ![Texture 3](https://github.com/user-attachments/assets/ea9c2051-356f-400d-bddf-ba2672cc2781) | ![image](https://github.com/user-attachments/assets/2bbfdff8-e58b-49a6-8bf0-91ba016089da)|
|  ![Texture 3 VT](https://github.com/user-attachments/assets/bd374dc4-3210-4dd6-81f8-76adbf3ffd8c) |![image](https://github.com/user-attachments/assets/12ee765a-8c81-4676-bbc9-28fdc113402d) |


---

## Résultats

- **Facteur de compression** : 2
- **Nombre de clusters différents** : 3
- **Précision (`Pmine`)** : 0.59947
- **Sensibilité (`Smine`)** : 0.935847
- **Dice Similarity Coefficient (`DSCmine`)** : 0.730809
- **Temps d'exécution** : 102.208 secondes
- **Le résultat reste bon malgré une compression de 2. En passant la compression de 1 à 2 on va environ 4 fois plus vite ( 2 au carré )**

  ---

| Référence | Segmentation Mean Shift |
|-----------|------------------------|
| ![Texture 3](https://github.com/user-attachments/assets/ea9c2051-356f-400d-bddf-ba2672cc2781) | ![image](https://github.com/user-attachments/assets/2ac6a8ef-7bcd-45fe-9c2d-5e0d86503d57)|
|  ![Texture 3 VT](https://github.com/user-attachments/assets/bd374dc4-3210-4dd6-81f8-76adbf3ffd8c) |![image](https://github.com/user-attachments/assets/d2c51ea9-f0c3-4e82-b55e-9a2e83bc1ca5)|

---

## Résultats

- **Facteur de compression** : 10
- **Nombre de clusters différents** : 2
- **Précision (`Pmine`)** : 0.551907
- **Sensibilité (`Smine`)** :  0.876181
- **Dice Similarity Coefficient (`DSCmine`)** : 0.677228
- **Temps d'exécution** : 4.13957 secondes
- **Le résultat devient moyen avec une compression de 10 mais le temps de computation est très court. En passant la compression de 1 à 10 on va environ 100 fois plus vite ( 10 au carré )**

  ---


## Sources

- **Cours ENSEEIHT - Sylvie Chambon**
