#include "labyrinthe.h"

void freeMap(int** arr, int height){
    for(int i=0;i<height;i++) free(arr[i]);
    free(arr);
}

void printMap(int** arr, int height, int width){
    for (int i = height-1; i >= 0; i--){
        for (int j = 0; j < width; j++) {
            printf("%s ", (arr[i][j])?"\u2B1B":"\u2B1C");
        }
        printf("\n");
    }
}

int** updateMap(int** arr, int height, int width){
    int flag=1;
    while (flag){
        flag=0;
        for (int k = 0; k < height; ++k) {
            for (int l = 0; l < width; ++l) {
                if (arr[k][l]!=0){
                    //droite
                    if (l+1<width && arr[k][l+1]<arr[k][l] && arr[k][l+1]!=0){
                        arr[k][l+1]=arr[k][l];
                        flag=1;
                    }
                    //bas
                    if (k+1<height && arr[k+1][l]<arr[k][l] && arr[k+1][l]!=0){
                        arr[k+1][l]=arr[k][l];
                        flag=1;
                    }
                    //haut
                    if (k-1>=0 && arr[k-1][l]<arr[k][l] && arr[k-1][l]!=0){
                        arr[k-1][l]=arr[k][l];
                        flag=1;
                    }
                    //gauche
                    if (l-1>=0 && arr[k][l-1]<arr[k][l] && arr[k][l-1]!=0){
                        arr[k][l-1]=arr[k][l];
                        flag=1;
                    }
                }
            }
        }
    }

    return arr;
}

int** initMap(int height, int width){
    //malloc
    int** arr = (int**)malloc(height * sizeof(int*));
    if (arr==NULL){
        ERROR("Initialisation de la map", strerror(errno));
    }
    for (int i = 0; i < height; i++) {
        arr[i] = (int*)malloc(width * sizeof(int));
        if (arr[i]==NULL){
            ERROR("Initialisation de la map", strerror(errno));
        }
    }

    int nbr_bloc=0;
    //init
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            arr[i][j] = 0;
            if (i%2==1 && j%2==1){
                arr[i][j] = rand()%(9-1 + 1) + 1;
                nbr_bloc+=1;
            }
        }
    }
    return arr;
}

int** fillMap(int** arr, int height, int width){
    for (int i = 1; i < height; i+=2) {
        for (int j = 1; j < width; j+=2) {
            //deux chances sur 3 de mettre a jour une case
            if (rand()%(3-1 + 1) + 1!=3){
                //droite
                if (j+2<width && arr[i][j+2]<arr[i][j]){
                    arr[i][j+2]=arr[i][j];
                    arr[i][j+1]=arr[i][j];
                    arr=updateMap(arr, height, width);
                }
                //bas
                if (i+2<height && arr[i+2][j]<arr[i][j]){
                    arr[i+2][j]=arr[i][j];
                    arr[i+1][j]=arr[i][j];
                    arr=updateMap(arr, height, width);
                }
                //haut
                if (i-2>=0 && arr[i-2][j]<arr[i][j]){
                    arr[i-2][j]=arr[i][j];
                    arr[i-1][j]=arr[i][j];
                    arr=updateMap(arr, height, width);
                }
                //gauche
                if (j-2>=0 && arr[i][j-2]<arr[i][j]){
                    arr[i][j-2]=arr[i][j];
                    arr[i][j-1]=arr[i][j];
                    arr=updateMap(arr, height, width);
                }
            }
        }
    }

    //si dimentions map impair
    if (width%2!=1){
        for (int i = 0; i < height; ++i) {
            arr[i][width-1]=0;
        }
    }
    if (height%2!=1){
        for (int i = 0; i < width; ++i) {
            arr[height-1][i]=0;
        }
    }

    //attribuer le chiffre 1 aux murs et 0 au sol
    for (int i = 0; i <height ; ++i) {
        for (int j = 0; j <width ; ++j) {
            arr[i][j]=(arr[i][j])?1:0;
        }
    }

    return arr;
}

int** createMap(int height, int width){

    int** arr= initMap(height, width);

    arr= fillMap(arr, height, width);
    return arr;
}