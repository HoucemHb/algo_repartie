# Gestion Avancée des Processus et des Signaux en C sous Linux

## Description

Ce projet est un programme en C qui illustre la gestion avancée des processus et des signaux sous Linux. Le programme crée plusieurs processus fils à partir d'un processus père, gère la communication entre ces processus à l'aide de signaux, et met en œuvre des mécanismes de synchronisation pour coordonner les tâches des processus fils.

## Objectifs

1. Créer un processus père qui génère quatre processus fils.
2. Le processus père doit envoyer des signaux aux processus fils pour leur demander d'exécuter des tâches spécifiques.
3. Les processus fils doivent traiter les signaux reçus, effectuer des tâches complexes, et répondre en envoyant des signaux de confirmation au processus père.
4. Le processus père doit afficher un message à chaque fois qu'il reçoit une confirmation d'un processus fils.
5. Implémenter une synchronisation entre les processus fils pour qu'ils ne commencent leurs tâches qu'une fois qu'ils ont tous reçu un signal de départ.
6. Utiliser un mécanisme de sémaphore pour gérer la synchronisation.

## Contraintes

- Utiliser les appels système `fork`, `kill`, `signal`, et les sémaphores POSIX (`sem_open`, `sem_wait`, `sem_post`).
- Gérer les signaux `SIGUSR1`, `SIGUSR2`, `SIGTERM` pour la communication entre le père et les fils.
- Utiliser `sleep` pour simuler des tâches dans les processus fils.
- Assurer une bonne gestion des processus et des signaux pour éviter les conditions de course.
- Les processus fils doivent effectuer des calculs simulés par des boucles et ensuite envoyer un signal de confirmation.

## Fonctionnement du Programme

1. Le processus père initialise un sémaphore pour synchroniser les processus fils.
2. Le processus père crée quatre processus fils.
3. Chaque processus fils configure ses gestionnaires de signaux et attend les signaux.
4. Les processus fils indiquent qu'ils sont prêts en postant sur le sémaphore après avoir reçu `SIGUSR1`.
5. Une fois que tous les processus fils sont prêts, le père envoie `SIGUSR2` pour leur demander de commencer leurs tâches.
6. Chaque fils effectue sa tâche, puis envoie `SIGUSR1` au père pour confirmer l'achèvement.
7. Le père attend que tous les fils aient terminé avant de se terminer lui-même.


## Compilation et Exécution

Pour compiler et exécuter le programme, utilisez les commandes suivantes dans un terminal Linux :

```sh
gcc -o gestion_processus gestion_processus_signaux.c -lrt
./gestion_processus
