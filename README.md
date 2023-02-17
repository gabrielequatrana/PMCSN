# Progetto PMCSN
Questa repository contiene il codice che permette di eseguire una simulazione relativa a un servizio cloud, per valutare il numero ottimo di server nell'arco di una giornata.

- La cartella ```Code``` contiene il codice del programma di simulazione.
- La cartella ```Estimator``` contiene gli eseguibili che permettono di generare le varie statistiche a partire dai csv prodotti nelle simulazioni.

## Guida all'uso
- Spostarsi sulla cartella ```Code```
- Impostare in ```config.h``` i parametri da utilizzare per la simulazione.
- Impostare in ```config.c``` nella funzione ```init_config()``` la configurazione di serventi che si vuole utilizzare per la simulazione.
- Compilare l'eseguibile tramite il comando ```make```.
- Eseguire il programma con il comando:
    ```bash
    ./simulator <FINITE/INFINITE> <TIME_SLOT>
    ``` 
- I risultati della simulazione vengono salvati nella cartella ```Results```.
- È possibile valutare le statistiche delle simulazione tramite i programmi presenti in ```Estimator```:

    - ```uvs < file.csv```: calcola media e varianza.
    - ```estimate < file.csv```: calcola media e l'intervallo di confidenza al 95%.
    - ```acs < file.csv```: calcola l'autocorrelazione del campione.

