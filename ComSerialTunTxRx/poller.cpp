/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   poller.cpp
 * Author: marioandre
 * 
 * Created on 1 de Novembro de 2018, 00:10
 */

#include <sys/time.h>
#include "poller.h"

// valor mto grande ...
const long MaxTimeout = 1000000000;


Poller::Poller() {
}

Poller::~Poller() {
    for (auto cb: cbs_to) delete cb;
    for (auto & par: cbs) delete par.second;
}


void Poller::adiciona(Callback * cb) {
    if (cb->filedesc() < 0) {
        // callback para timer:
        // verifica se já existe um callback equivalente cadastrado
        // se existir, sobrescreve-o com o novo callback
        for (auto & c : cbs_to) {
            if (*c == *cb) {
                c = cb;
                return;
            }
        }
        // adiciona novo callback timer
        cbs_to.push_back(cb);
    } else cbs[cb->filedesc()] = cb; // adiciona callback para descritor de arquivo
}

void Poller::remove(int fd) {
    cbs.erase(fd);
}

void Poller::remove(Callback * cb) {
    int fd = cb->filedesc();
    
    if (fd >= 0) remove(fd);
    else {
        for (auto it = cbs_to.begin(); it != cbs_to.end(); it++) {
            auto c = *it;
            if (*c == *cb) {
                cbs_to.erase(it);
                break;
            }
        }
    }
}

void Poller::limpa() {
    cbs.clear();
    cbs_to.clear();
}

void Poller::despache() {
    while (true) despache_simples();
}

void Poller::despache_simples() {
    pollfd fds[MAX_FDS];
    int nfds = 0;
    
    // identifica o menor timeout dentre todos os timers
    int min_timeout = MaxTimeout;
    Callback * cb_tout = nullptr;
    
    for (auto cb : cbs_to) {
        int fd = cb->filedesc();
        int timeout = cb->timeout();
        
        if (timeout < min_timeout) {
            min_timeout = timeout;
            cb_tout = cb;
        }
    }
    
    // gera o vetor de descritores a ser usado na chamada poll
    // verifica tb se o timeout de algum callback desses é menor do que o
    // timer mais próximo 
    for (auto & par : cbs) {
        int fd = par.first;
        int timeout = par.second->timeout();
        
        if (timeout < min_timeout) {
            min_timeout = timeout;
            cb_tout = par.second;
        }

        if (fd >= 0) {
            if (nfds == MAX_FDS) throw -1; // erro: excedeu qtde de descritores vigiados
            fds[nfds].fd = fd;
            fds[nfds].events = POLLIN;
            nfds++;            
        }
    }

    // lê o relógio, para saber o instante em que o poll iniciou
    timeval t1, t2;
    gettimeofday(&t1, NULL);
    
    int n = poll(fds, nfds, min_timeout);
    
    // lê novamente o relógio, para saber o instante em que o poll retornou
    gettimeofday(&t2, NULL);
        
    // determina o tempo decorrido até ocorrência do evento
    long dt = (t2.tv_usec - t1.tv_usec)/1000 + (t2.tv_sec-t1.tv_sec)*1000;
    
    // se nenhum descritor está pronto para ser lido, ocorreu timeout
    // Na verdade, um timer disparou: chama o callback desse timer
    if (n == 0) {
        cb_tout->handle_timeout(); // timeout        
        cb_tout->reload_timeout();
    } else {
        // caso contrário, chama o callkback de cada descritor pronto para ser acessado
        for (int i=0; i < nfds and n > 0; i++) {
            auto cb = cbs[fds[i].fd];
            if (fds[i].revents & POLLIN) {                
                cb->handle();
                cb->reload_timeout();
                n--;
            }/* else {
		cb->update(dt);
	    }*/
        }
    }
    for (auto & cb: cbs_to) {
      if (cb != cb_tout) cb->update(dt);
    }
    for (auto & par: cbs) {
      par.second->update(dt);
    }
}
