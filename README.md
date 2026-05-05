# Simulation d'une application réseau de menu de restauration

Projet de programmation système réalisé dans le cadre de la 3e année de la filière informatique de l'ESIEE Paris / Université Gustave Eiffel.

## Description

Ce projet simule une application de gestion de menus de restauration. Un client envoie une requête contenant l'identifiant d'un lieu et d'un menu à un serveur de routage, qui la transmet à un serveur de données. Ce dernier retrouve le menu correspondant et renvoie le résultat au client. La communication entre les trois entités est assurée par des tubes nommés (FIFOs).

## Prérequis

- Linux ou WSL (Windows Subsystem for Linux)
- GCC
- Make

## Compilation

```bash
make
```

Cela génère trois binaires : `client`, `routeur` et `dataserveur`.

## Utilisation

Lancer les trois binaires dans trois terminaux séparés, dans cet ordre :

**Terminal 1 — Serveur de données**

```bash
./dataserveur
```

**Terminal 2 — Routeur**

```bash
./routeur
```

**Terminal 3 — Client**

```bash
./client
```

Le client attend une requête au format suivant :

```
|code_serveur|code_lieu|code_menu|
```

Par exemple :

```
|0|1234|5678|
```

### Sous Windows (PowerShell)

Un script `run.ps1` est disponible pour lancer automatiquement les trois binaires depuis PowerShell via WSL :

```powershell
.\run.ps1
```

## Nettoyage

```bash
make clean
```

## Auteurs

- Jules Delobelle
- Léo Denooz
