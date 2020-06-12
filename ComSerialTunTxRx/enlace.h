/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   enlace.h
 * Author: marioandre
 *
 * Created on 25 de Novembro de 2018, 10:42
 */

#ifndef ENLACE_H
#define ENLACE_H

#include <iostream>
#include "Serial.h"
#include "Enquadramento.h"
#include "ARQ.h"
#include "Callback.h"
#include "poller.h"
#include <unistd.h>
#include "Tun.h"
//#include <sstream>
//#include <stdio.h>
//#include <string>
//#include <cstring>
//#include <stdlib.h> 

using namespace std;

class enlace {
public:
    enlace();
    enlace(const char * pathSerial, int rate, const char * ip_h1, const char * ip_h2, int bytes_min, int bytes_max);
    enlace(const enlace& orig);
    virtual ~enlace();

private:

};

#endif /* ENLACE_H */

class CallbackSerial : public Callback {
public:

    int n_bytes_recebidos = 0;
    int max_bytes = 256;
    Tun * _interfaceTun;
    Enquadramento * _enq;
    ARQ * _arq;
    Serial * ser;
    char buffer[256];
    bool quadroCompleto = false;
    bool verifQuadro = false;

    CallbackSerial(long tout, Tun * interface, Serial * s, Enquadramento * enq, ARQ * arq) : Callback(s->get(), tout) {

        _interfaceTun = interface;
        ser = s;
        _arq = arq;
        _enq = enq;
        cout << "fd: " << s->get() << endl;

    }

    //Executa esse método se receber algo no descritor de arquivo(fd) da Serial
    void handle() {
        
        //cout << "recebi algum byte" << endl;

        n_bytes_recebidos = ser->read(buffer, max_bytes);

        quadroCompleto = _enq->trataBytes(buffer, n_bytes_recebidos);

        //Se um quadro foi recebido e o CRC esta ok, pega o quadro sem flag e CRC e manda pra subcamada ARQ
        //Para verificar se é uma quadro de Dados ou Ack
        if (quadroCompleto == true) {
            //cout << "Um quadro recebido" << endl;
            char quadro[4094] = {};
            int n_bytes_quadro;
            n_bytes_quadro = _enq->repassa_quadro(quadro);
            _arq->recebe(quadro, n_bytes_quadro);
        }

        //Se um quadro de Dados foi recebido, pega o payload e envia para a aplicação com o send_frame
        if (_arq->payloadRecebido == true) {

            _arq->payloadRecebido = false;

            TunFrame payRecebido;

            //copia os bytes do payload para payRecebido.buffer
            memcpy(payRecebido.buffer, _arq->payload, _arq->n_bytes_payload);

            payRecebido.len = _arq->n_bytes_payload;

            cout << endl;

            cout << "Enviando payload para a aplicação" << endl;
            _interfaceTun->send_frame(&payRecebido);
        }
    }

    //Se nao receber nenhum byte vem pra esse método
    void handle_timeout() {     
        _enq->ocorreu_timeout = true;
    }

};

class CallbackTun : public Callback {
public:

    Tun * _interfaceTun;
    Enquadramento * _enq;
    ARQ * _arq;

    CallbackTun(long tout, Tun * interface, Enquadramento * enq, ARQ * arq_) : Callback(interface->get(), tout) {
        _interfaceTun = interface;
        _enq = enq;
        _arq = arq_;
    }

    //Executa esse método se receber algo no descritor de arquivo(fd) da interface Tun
    void handle() {

        TunFrame * payload;

        //Pega o conteudo do descritor de arquivo(fd) do Tun
        //para pegar a informação do ping
        payload = _interfaceTun->get_frame();

        //Se o Ack do payload anterior nao ter chegado nao envia o proximo
        if (_arq->recebeuAck == true) {
            cout << endl;
            cout << "Informação do tun a ser enviado: " << endl;
            
            //mostra o conteudo da informação do Tun no formato hexdecimal
            _arq->dump(payload->buffer, payload->len);
            cout << endl;
            
            //seta em false, só vai ficar true quando o ack for recebido
            //o ack recebido é verificado no callbackSerail, pela subcamada ARQ
            _arq->recebeuAck = false; 

            //passa para a subcamada ARQ a informação do Tun a ser enviada
            _arq->envia(payload->buffer, payload->len);
            
        } else {
            cout << "Ainda nao recebi confirmação do envio anterior" << endl;
        }

    }

    //Se não receber nenhum ping vem pra esse método
    void handle_timeout() {
        cout << "Timeout tun" << endl;
    }

};

class CallbackTimer : public Callback {
public:

    ARQ * _arq;

    CallbackTimer(long tout, ARQ * arq) : Callback(tout) {
        _arq = arq;
    }

    void handle() {
    }

    //Só executa esse método a cada x milisegundos passado na instacia do callback
    void handle_timeout() {
        _arq->timeOutArq();
        cout << "Timer !!!" << endl;
    }
};


