/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ARQ.cpp
 * Author: marioandre
 * 
 * Created on 26 de Setembro de 2018, 21:43
 */

#include "ARQ.h"

ARQ::ARQ() {
}

ARQ::ARQ(Enquadramento & a_enq) {
    enq = a_enq;
    estado = Ocioso;
}

ARQ::ARQ(const ARQ& orig) {
}

ARQ::~ARQ() {
}

//Maquina de estado do ARQ

void ARQ::handle(Evento & e) {

    switch (estado) {
        case Ocioso:
            //cout << "Estou no estado Ocioso" << endl;

            if (e.tipo == Payload) { //evento Payload
                //cout << "Estou no evento payload" << endl;

                cout << "add proto: ";
                add_proto(e.ptr, e.bytes);
                //mostra_buffer(e.ptr, e.bytes + 1);
                //dump(e.ptr, e.bytes + 1);

                cout << "add controle: ";

                val_ini_controle ^= 0b00001000; //XOR no bit 3 para ficar alternando o bit em 0 e 1

                add_controle(e.ptr, (e.bytes + 1), val_ini_controle);
                //mostra_buffer(e.ptr, (e.bytes + 2));
                // dump(e.ptr, (e.bytes + 2));

                cout << "Campo controle(HEX): ";
                printf("%02X ", val_ini_controle);
                cout << endl;

                cout << "Sequencia do quadro: ";
                if (val_ini_controle & 0b00001000) {
                    cout << "1" << endl;
                } else {
                    cout << "0" << endl;
                }

                n_bytes_quadro_arq = e.bytes + 2;
                //enq.set_timeout(tempo_timeout);

                //Faz uma copia do e.ptr e n_bytes_quadro_arq para que caso seja necessario
                //fazer o reenvio dos dados
                copiaQuadroEnv = e.ptr;
                bytesCopiaQuadroEnv = n_bytes_quadro_arq;

                //seta essa variavel em true pra avisar para o envia do enquadramento que 
                //dever ser feita uma copia do numero de bytes enviados
                enq.enviePayload = true;

                enq.envia(e.ptr, n_bytes_quadro_arq);

                //usado no método timeOutArq() para saber se deve reenviar o quadro depois que ele foi enviado
                quadroEnviado = true;

                estado = Espera;

            } else { //senao evento Quadro

                verificaQuadro(e.ptr, e.bytes);

            }

            break;
        case Espera:
            //cout << "Estou no estado Espera" << endl;

            if (e.tipo == Payload) { //evento Payload

                verificaQuadro(e.ptr, e.bytes);

            }

            if (e.tipo == Quadro) { //evento Quadro
                //Verica o quadro ja sem CRC e vê se é um quadro de Dados ou Ack
                verificaQuadro(e.ptr, e.bytes);

            }

            if (e.tipo == Timeout) { //Timeout
                //                num_timeout_ocorridos++;
                //                cout << "Timeout " << num_timeout_ocorridos << endl;
                //                cout << "O tempo de espera da resposta terminou!" << endl;
                //                cout << "Setando timeout!" << endl;
                //                enq.set_timeout(tempo_timeout);
                //                enq.c = 0;

            }

            break;
    }
}

void ARQ::envia(char * buffer, int bytes) { 

    Evento te(Payload, buffer, bytes);

    handle(te); //maquina de estado do ARQ

}

void ARQ::recebe(char * buffer, int bytes) { 

    Evento te(Quadro, buffer, bytes);

    handle(te); //maquina de estado do ARQ

}

void ARQ::add_proto(char *buf, int b) { 

    int posicaoZero = 0;

    for (int i = b; i > posicaoZero; i--) {
        buf[i] = buf[i - 1];
    }
    buf[0] = '1'; //proto - valor qualquer por enquanto

}

void ARQ::add_controle(char *buf, int b, char contr) { 
    int posicaoZero = 0;


    for (int i = b; i > posicaoZero; i--) {
        buf[i] = buf[i - 1];
    }

    buf[0] = contr;
}

void ARQ::mostra_buffer(char * buf, int bytes) { 

    for (int i = 0; i < bytes; i++) {
        cout << buf[i];
    }
    cout << endl;
}

void ARQ::dump(char * buffer, int len) { 
    int m = 0, line = 0;

    while (m < len) {
        //printf("%02X: ", line * 16);

        for (int n = 0; n < 32 and m < len; n++, m++) {
            int x = (unsigned char) buffer[m]; //(unsigned char), não permite armazenar valores negativos, e portanto pode representar números em um intervalo que vai de 0 até 255.
            printf("%02X ", x);
        }
        puts("");
        line++;
    }
}

void ARQ::limpar_buffer(char * b, int len) { 
    for (int j = 0; j < len; j++) {
        b[j] = 0;
    }
}

void ARQ::pega_payload(char * b, int len) { 
    int id = 0;
    limpar_buffer(payload, 1500);

    for (int i = 2; i < len; i++) {
        payload[id] = b[i];
        id++;
    }

    n_bytes_payload = len - 2;

}

//Verica o quadro ja sem CRC e vê se é um quadro de Dados ou Ack
void ARQ::verificaQuadro(char * buffer, int bytes) { 

    uint8_t campo_controle = buffer[0];
    uint8_t campo_proto = buffer[1];

    //cout << "Campo controle(HEX): ";
    // printf("%02X ", campo_controle);
    // cout << endl;
    //  cout << "Campo proto(HEX): ";
    // printf("%02X ", campo_proto);
    // cout << endl;

    if (!(campo_controle & 0b10000000)) { //testa o bit 7 - se for data - valor 0

        cout << "Tipo de quadro: Dados" << endl;
        char ack[8] = {}; //inicia com todos os valoes 0
        uint8_t campo_ack;

        if (campo_controle & 0b00001000) { //testa o bit 3 - se for sequecia 1 
            cout << "Sequencia do quadro: 1" << endl;
            //sequencia 1

            if (quadro_seq_1 == ultimo_quadro_recebido) {

                cout << "Quadro com sequencia 1 ja recebido!" << endl;

                ack[0] = 0b10001000; //0x88
                campo_ack = ack[0];

                cout << endl;
                cout << "Tipo de resposta: ack1" << endl;
                cout << "Gerando ack1..." << endl;
                cout << "ack1: ";
                //mostra_buffer(ack, 1);
                dump(ack, 1);
                cout << "ack1(HEX): ";
                printf("%02X ", campo_ack);
                cout << endl;

                enq.envia(ack, 1);
                cout << "ack 1 enviado!" << endl << endl;
                limpar_buffer(payload, 1500);

            } else {

                ack[0] = 0b10001000; //0x88
                campo_ack = ack[0];

                cout << endl;
                cout << "Tipo de resposta: ack1" << endl;
                cout << "Gerando ack1..." << endl;
                cout << "ack1: ";

                //mostra_buffer(ack, 1);
                dump(ack, 1);
                cout << "ack1(HEX): ";
                printf("%02X ", campo_ack);
                cout << endl;

                enq.envia(ack, 1);
                cout << "ack 1 enviado!" << endl << endl;

                pega_payload(buffer, bytes);

                payloadRecebido = true;

                ultimo_quadro_recebido = 1;

            }

        } else { //testa o bit 3 - se for sequecia 0
            cout << "Sequencia do quadro: 0" << endl;
            //sequencia 0

            ack[0] = 0b10000000; //0x80
            campo_ack = ack[0];

            if (quadro_seq_0 == ultimo_quadro_recebido) {

                cout << "Quadro com sequencia 0 ja recebido!" << endl;

                cout << endl;
                cout << "Tipo de resposta: ack0" << endl;
                cout << "Gerando ack0..." << endl;
                cout << "ack0: ";
                //mostra_buffer(ack, 1);
                dump(ack, 1);
                cout << "ack0(HEX): ";
                printf("%02X ", campo_ack);
                cout << endl;

                enq.envia(ack, 1);
                cout << "ack 0 enviado!" << endl << endl;
                limpar_buffer(payload, 1500);

            } else {

                cout << endl;
                cout << "Tipo de resposta: ack0" << endl;
                cout << "Gerando ack0..." << endl;
                cout << "ack0: ";
                //mostra_buffer(ack, 1);
                dump(ack, 1);
                cout << "ack0(HEX): ";
                printf("%02X ", campo_ack);
                cout << endl;

                enq.envia(ack, 1);
                cout << "ack 0 enviado!" << endl << endl;

                pega_payload(buffer, bytes);

                payloadRecebido = true;

                ultimo_quadro_recebido = 0;

            }

        }
        estado = Ocioso;
        //quadro_chegou = true;

    } else { // senão é ack - bit 7 com valor 1

        cout << "Tipo de quadro: Ack" << endl << endl;

        if ((val_ini_controle & 0b00001000) == (campo_controle & 0b00001000)) {

            if (campo_controle & 0b00001000) { //testa o bit 3 , se for 1 é ack1
                //sequencia 1 - ack 1
                cout << "ack 1 recebido! " << endl << endl;

            } else { //testa o bit 3 , se for 0 é ack0
                //sequencia 0 - ack0
                cout << "ack 0 recebido! " << endl << endl;

            }

            recebeuAck = true;
            quadroEnviado = false;
            enq.reenviar = false;
            estado = Ocioso;


        } else {
            cout << "Ack recebido nao corresponde ao quadro enviado!" << endl;

            enq.reenviar = true;

            cout << "Reenviando o quadro!" << endl;
            //enq.envia(e.ptr, enq.n_bytes_buf_enviado);
            enq.envia(copiaQuadroEnv, bytesCopiaQuadroEnv);

        }

    }
}

void ARQ::timeOutArq() { 

    if (quadroEnviado == true && recebeuAck == false) {
        cout << "Timeout ARQ: " << num_timeout_ocorridos << endl;
        num_timeout_ocorridos++;

        cout << "Reenviando o quadro!" << endl;

        enq.reenvia(copiaQuadroEnv, enq.reenvio_n_bytes_buf_enviado);
    }
}
