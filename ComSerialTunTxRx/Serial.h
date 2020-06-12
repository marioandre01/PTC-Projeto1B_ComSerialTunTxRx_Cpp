/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Serial.h
 * Author: marioandre
 *
 * Created on 19 de Agosto de 2018, 18:52
 */

#ifndef SERIAL_H
#define	SERIAL_H

#include <termios.h>

#include <termios.h>
#include <exception>
#include <string>

using std::exception;

class SerialException : public exception {
protected:
    std::string msg;
public:
  SerialException():msg("unknown serial exception") {}
  SerialException(const char * mesg):msg(mesg) {}
  SerialException(const SerialException & o) : exception(o), msg(o.msg) {}
  virtual ~SerialException() {}
  virtual const char* what() const throw() {
      return msg.c_str();
  }
};

class Serial {
public:
    Serial();
    
    // cria um objeto Serial associado a uma porta serial.
    // "path": path do dispositivo serial (ex: /dev/ttyS0)
    // "rate": taxa de transmissão (ex: B9600 ... ver termios.h)
    Serial(const char * path, int rate);
    
    // cria um objeto serial associado a um arquivo para fins de debug
    Serial(const char * path); // abre um arquivo ao invés da serial
    
    // construtor de cópia
    Serial(const Serial& orig);
    
    virtual ~Serial();
    
    // obtém o descritor de arquivo da porta serial
    int get() { return tty_fd;}
    
    // Experimental: faz "clear channel assessment" (testa se cana livre)
    bool cca();
    
    // envia dados pela serial
    int write(const char * buffer, int len);
    
    // lê até "len" dados da serial. Versão não-bloqueante
    int read(char * buffer, int len);
    
    // lê até "len" dados da serial.
    // "se block for true: bloqueia à espera de receber pelo menos um byte. Esse bloqueio
    // é limitado por um timeout, caso tenha sido especificado
    // se vblock false: não-bloqueante
    int read(char * buffer, int len, bool block);    
    
    // lê um único byte em modo bloqueante (também limitado por timeout)
    char read_byte();
    
    // define o valor de timeout. Se for "0" (zero), desativa o timeout
    // Valor inicial de timeout é "0"
    void set_timeout(int to); // timeout em milissegundos
private:
    int tty_fd;
    int timeout;
    timeval t;
    
    void init(const char * path, int rate);
};

#endif	/* SERIAL_H */
