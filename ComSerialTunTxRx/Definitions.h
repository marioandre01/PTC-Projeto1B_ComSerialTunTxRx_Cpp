/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Definitions.h
 * Author: marioandre
 *
 * Created on 1 de Novembro de 2018, 00:14
 */

#ifndef DEFINITIONS_H
#define	DEFINITIONS_H

#define MTU 256
#define MASK "255.255.255.252"
#define FrameTimeout 1000 // 750 ms
#define ByteTimeout 50 // 20 ms
#define BackoffSlot 50 // 20 ms
#define CWmax 15 // backoff máximo
#define MAX_TRANSMISSIONS 4
#define QueueLen 10 // comprimento da fila da tun (txqueue)
#define MAX_SERIAL_BYTES 64

#endif	/* DEFINITIONS_H */


