/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ARQ.h
 * Author: marioandre
 *
 * Created on 26 de Setembro de 2018, 21:43
 */

#ifndef ARQ_H
#define ARQ_H

#include "Enquadramento.h"

class ARQ {
public:
      
    ARQ();
    ARQ(Enquadramento & a_enq);
    ARQ(const ARQ& orig);
    virtual ~ARQ();
    
    void envia(char * buffer, int bytes);  
    void recebe(char * buffer, int bytes);
    void add_proto(char *buf, int b);
    void add_controle(char *buf, int b, char contr);
    void mostra_buffer(char * buf, int bytes);
    void dump(char * buffer, int len);
    void limpar_buffer(char * b, int len);
    void pega_payload(char * b, int len);
    void verificaQuadro(char * buffer, int bytes);
    void timeOutArq();    
    
    bool recebeuAck = true;
    bool respostaEnvio = false;
    bool quadroEnviado = false;
    char * copiaQuadroEnv;
    int bytesCopiaQuadroEnv;
    bool payloadRecebido = false;
    char payload[1500] = {}; // Todos os elementos 0, em C++
    int n_bytes_payload = 0;
    
private:
    
    Enquadramento enq;
    //long timeout;
   // bool quadro_chegou;
    
    // estado atual da MEF
    int estado;
    
    uint8_t val_ini_controle = 0b00001011; //0x0B   
    int bytes_recebidos = 0;
    char quadro[4094] = {}; // Todos os elementos 0, em C++
    
    int n_bytes_quadro_arq = 0;
    int num_timeout_ocorridos = 0;
    //int tempo_timeout = 1000; //1000 milisegundos = 1 segudo
    
    bool quadro_seq_0 = 0;
    bool quadro_seq_1 = 1;
    bool ultimo_quadro_recebido = 1;
    
    
    char controle;
    char proto;
    
  enum Estados {Ocioso, Espera}; //Ocioso = 0; RX = 1; ESC = 2
  enum TipoEvento {Payload, Quadro, Timeout};
 
  // esta struct descreve um Evento
  struct Evento {
    TipoEvento tipo;
    char * ptr;
    int bytes;
    
 
    // construtor sem parâmetros: cria um Evento Timeout
    Evento(){ tipo = Timeout;}
 
    // construtor com parâmetros: cria um evento Payload ou Quadro
    Evento(TipoEvento t, char * p, int len) : tipo(t), ptr(p), bytes(len) {}
  };
 
  // executa a MEF, passando como parâmetro um evento
  void handle(Evento & e);

};

#endif /* ARQ_H */

