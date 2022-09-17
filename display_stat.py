#!/usr/bin/python3
from matplotlib import pyplot as plt
import numpy as np
from os import listdir
from os.path import isfile, join

STATS_FOLDER = "resources/stats/"

onlyfiles = [f for f in listdir(STATS_FOLDER) if isfile(join(STATS_FOLDER, f))]

print("Choisissez un fichier à afficher:")
for i,file in enumerate(onlyfiles):
    print("-->",i,",",file)

if len(onlyfiles)==0:
    print("Il n'y a pas de stat à afficher")
    exit(0)
ask = ""
while True:
    ask = input("Entrez le numéro:")
    
    try:
        ask = int(ask)
        if(not(ask>=0 and ask<len(onlyfiles))):
            print("Le nombre n'est pas correcte")
            raise ValueError
        break   
    except:
        print("Ce n'est pas un nombre")


data = []
with open(join(STATS_FOLDER, onlyfiles[ask])) as file:
    data = file.read()
    print("data:", data)