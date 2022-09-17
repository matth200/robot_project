#!/usr/bin/python3
from matplotlib import pyplot as plt
import numpy as np
from os import listdir
from os.path import isfile, join

STATS_FOLDER = "resources/stats/"

onlyfiles = [f for f in listdir(STATS_FOLDER) if isfile(join(STATS_FOLDER, f))]

print("Choisissez un fichier à afficher:")
for i,file in enumerate(onlyfiles):
    print("-->",i,",",file,end="")
    if i!=len(onlyfiles)-1:
        print()
if len(onlyfiles)==0:
    print("Il n'y a pas de stat à afficher")
    exit(0)
ask = ""
continuer = True
while continuer:
    ask = input("Entrez le numéro(ou 'q' pour quitter):")
    
    try:
        if ask=='q':
            print("Byee")
            continuer = False
            break
        ask = int(ask)
        if(not(ask>=0 and ask<len(onlyfiles))):
            print("Le nombre n'est pas correcte")
            continue
        break   
    except:
        print("Ce n'est pas un nombre")

if continuer:
    data = []
    X = []
    Y = []
    #construction des listes
    with open(join(STATS_FOLDER, onlyfiles[ask])) as file:
        data = file.read().split("\n")[:-1]
        liste = []
        for elt in data:
            tmp = elt.split(" ")
            liste.append([int(tmp[0]),int(tmp[1])])
            X.append(liste[-1][0])
            Y.append(liste[-1][1])
        data = liste

    plt.ylim(0,800000)
    plt.plot(X,Y)
    plt.xlabel("génération")
    plt.ylabel("score max")
    plt.title("Evolution population à travers les générations")
    plt.show()

