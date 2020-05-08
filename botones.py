from tkinter import *
import serial
import time
import sys
import struct

ser= serial.Serial(port='COM6', baudrate=9600, timeout=0.2)

#ser.write(";".encode())

caracter = "0"
arribaJ1 = 0
derechaJ1 = 0
izquierdaJ1 = 0
abajoJ1 = 0

arribaJ2 = 0
derechaJ2 = 0
izquierdaJ2 = 0
abajoJ2 = 0

cadenaJ1 = [arribaJ1, derechaJ1, izquierdaJ1, abajoJ1]
cadenaJ2 = [arribaJ2, derechaJ2, izquierdaJ2, abajoJ2]
##def dato():
##        EnvioDatos = Valor.get()
##        ser.write(struct.pack('>B', int(EnvioDatos.get())))

def enviarDatos(datos):
    ser.write(';'.encode())
    for x in datos:
        ser.write(str(x).encode())
        print(str(x) + ' ')
    print(' ')

def presionarArribaJ1():
    derechaJ1 = 0
    izquierdaJ1 = 0
    abajoJ1 = 0
    arribaJ1 = 1
    cadenaJ1 = [arribaJ1, derechaJ1, izquierdaJ1, abajoJ1]
    #print(cadenaJ1+cadenaJ2)
    enviarDatos(cadenaJ1+cadenaJ2)
    
def presionarDerechaJ1():
    arribaJ1 = 0
    derechaJ1 = 1
    izquierdaJ1 = 0
    abajoJ1 = 0
    cadenaJ1 = [arribaJ1, derechaJ1, izquierdaJ1, abajoJ1]
    #print(cadenaJ1+cadenaJ2)
    enviarDatos(cadenaJ1+cadenaJ2)
    
def presionarIzquierdaJ1():
    arribaJ2 = 0
    derechaJ2 = 0
    izquierdaJ1 = 1
    abajoJ1 = 0
    cadenaJ1 = [arribaJ1, derechaJ1, izquierdaJ1, abajoJ1]
    #print(cadenaJ1+cadenaJ2)
    enviarDatos(cadenaJ1+cadenaJ2)

def presionarAbajoJ1():
    arribaJ1
    derechaJ1 = 0
    izquierdaJ1 = 0
    abajoJ1 = 1
    cadenaJ1 = [arribaJ1, derechaJ1, izquierdaJ1, abajoJ1]
    #print(cadenaJ1+cadenaJ2)
    enviarDatos(cadenaJ1+cadenaJ2)
    
def presionarArribaJ2():
    derechaJ2 = 0
    izquierdaJ2 = 0
    abajoJ2 = 0
    arribaJ2 = 1
    cadenaJ2 = [arribaJ2, derechaJ2, izquierdaJ2, abajoJ2]
    #print(cadenaJ1+cadenaJ2)
    enviarDatos(cadenaJ1+cadenaJ2)

def presionarDerechaJ2():
    izquierdaJ2 = 0
    abajoJ2 = 0
    arribaJ2 = 0
    derechaJ2 = 1
    cadenaJ2 = [arribaJ2, derechaJ2, izquierdaJ2, abajoJ2]
    #print(cadenaJ1+cadenaJ2)
    enviarDatos(cadenaJ1+cadenaJ2)
    
def presionarIzquierdaJ2():
    derechaJ2 = 0
    abajoJ2 = 0
    arribaJ2 = 0
    izquierdaJ2 = 1
    cadenaJ2 = [arribaJ2, derechaJ2, izquierdaJ2, abajoJ2]
    #print(cadenaJ1+cadenaJ2)
    enviarDatos(cadenaJ1+cadenaJ2)

def presionarAbajoJ2():
    derechaJ2 = 0
    arribaJ2 = 0
    izquierdaJ2 = 0
    abajoJ2 = 1
    cadenaJ2 = [arribaJ2, derechaJ2, izquierdaJ2, abajoJ2]
    #print(cadenaJ1+cadenaJ2)
    enviarDatos(cadenaJ1+cadenaJ2)



#aca empieza la configuracion de mi interfaz grafica
ventana = Tk ()
EnvioDatos = StringVar()

color = "#00A2FC"

ventana.title ("Controles de videojuego")
ventana.geometry("800x500")
ventana.config(bg=color)

nombre = Label(ventana, text = "CONTROLES", bg=color, font = ("Arial", 18))
nombre.place  (x = 330, y = 20)

j1 = Label(ventana, text = "JUGADOR 1", bg=color, font = ("Arial", 12))
j1.place  (x = 150, y = 90)

j2 = Label(ventana, text = "JUGADOR 2", bg=color, font = ("Arial", 12))
j2.place  (x = 550, y = 90)

ArribaJ1 = Button(ventana, text = "Arriba", command = presionarArribaJ1,  height=3,width=8).place(x=160, y=120)
DerechaJ1 = Button(ventana, text = "Derecha", command = presionarDerechaJ1,  height=3,width=8).place(x=250, y=180)
IzquierdaJ1 = Button(ventana, text = "Izquierda", command = presionarIzquierdaJ1,  height=3,width=8).place(x=70, y=180)
AbajoJ1 = Button(ventana, text = "Abajo", command = presionarAbajoJ1,  height=3,width=8).place(x=160, y=240)

ArribaJ2 = Button(ventana, text = "Arriba", command = presionarArribaJ2,  height=3,width=8).place(x=560, y=120)
DerechaJ2 = Button(ventana, text = "Derecha", command = presionarDerechaJ2,  height=3,width=8).place(x=650, y=180)
IzquierdaJ2 = Button(ventana, text = "Izquierda", command = presionarIzquierdaJ2,  height=3,width=8).place(x=470, y=180)
AbajoJ2 = Button(ventana, text = "Abajo", command = presionarAbajoJ2,  height=3,width=8).place(x=560, y=240)

   
#aca termina la interfaz grafica
