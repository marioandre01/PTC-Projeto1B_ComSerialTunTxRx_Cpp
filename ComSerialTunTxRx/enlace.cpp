/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   enlace.cpp
 * Author: marioandre
 * 
 * Created on 25 de Novembro de 2018, 10:42
 */

#include "enlace.h"

enlace::enlace() {
}

enlace::enlace(const enlace& orig) {
}

enlace::enlace(const char * pathSerial, int rate, const char * ip_h1, const char * ip_h2, int bytes_min, int bytes_max) {

    Serial dev(pathSerial, rate); //argv = /dev/pts/2 , taxa= B9600  ////Instanciando Sub Serial

    Enquadramento proto(dev, bytes_min, bytes_max); //Instanciando Sub camada Enquadramento

    ARQ subCaArq(proto); //Instanciando Sub camada ARq

    Tun iface("tun0", ip_h1, ip_h2); //criando interface Tun0
    iface.start(); //iniciando interface Tun0

    cout << "Criou interface tun: fd=" << iface.get() << endl;

    //Instanciando Callbscks
    CallbackTun cbtun(1000, &iface, &proto, &subCaArq); //1000 = 1 segundo
    CallbackSerial cbSerial(500, &iface, &dev, &proto, &subCaArq);
    CallbackTimer cbTimer(1000, &subCaArq);

    //Instanciando Poller - Loop Para execução dos Callbacks 
    Poller sched;

    //Adicionando callbacks criadso ao poller
    sched.adiciona(&cbtun);
    sched.adiciona(&cbSerial);
    sched.adiciona(&cbTimer);

    //Iniciando o Poller - 
    sched.despache();
    
    //Depois de executar o despache() o codigo vai ficar preso em um loop, só executando os métodos
    //dos callbacks criados.
}

enlace::~enlace() {
}


