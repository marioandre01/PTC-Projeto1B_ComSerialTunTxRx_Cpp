/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: marioandre
 *
 * Created on 31 de Outubro de 2018, 23:58
 */

#include <iostream>
#include "enlace.h"

using namespace std;

int main(int argc, char** argv) {

    if(argc < 5){
        cout << "É necessário informar a porta serial, a taxa de transmissão, o IP do host 1 e o Ip do host 2" << endl;
        cout << "Ex-Tx: sudo ./app /dev/pts/1 B9600 10.0.0.1 10.0.0.2" << endl;
        cout << "Ex-Rx: sudo ./app /dev/pts/2 B9600 10.0.0.2 10.0.0.1" << endl;
        cout << "Ex-Rx-VirtualBox: sudo ./app /dev/ttyS0 B9600 10.0.0.2 10.0.0.1" << endl;
        return 0;
    }
    
    //Dados para criar o enlace
    const char * caminhoSerial = argv[1]; // /dev/pts/2
    int taxa = atoi(argv[2]); //converter o argumento B9600 de char * para int
    const char * ip_host1 = argv[3];  // 10.0.0.1
    const char * ip_host2 = argv[4];  // 10.0.0.2
    int tam_min_bytes = 8; //tamanho bytes minimo
    int tam_max_bytes = 256; //tamanho bytes máximo

    //Instanciando o enlace
    enlace enlace_p(caminhoSerial, taxa, ip_host1, ip_host2, tam_min_bytes, tam_max_bytes);

    return 0;
}





