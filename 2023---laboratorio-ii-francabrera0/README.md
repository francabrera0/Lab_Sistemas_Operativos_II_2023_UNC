# 2023---soii---laboratorio-ii-francabrera0
2023---soii---laboratorio-ii-francabrera0 created by GitHub Classroom

# Laboratorio II - Interprocess communication advanced

[GitHub repository](https://github.com/ICOMP-UNC/2023---soii---laboratorio-ii-francabrera0)


## Ejecución<a name="ejecucion"></a>
  En primer lugar, debemos compilar el proyecto, para esto descargar o clonar el repositorio y ejecutar

```
mkdir build
cd build
cmake ..
make
```
  Es necesaria la instalación de la librería cJSON.
  
  Luego puede usarse el script `launcher.sh` para lanzar una ejecución del servidor con un cliente de cada tipo.
  
## Diseño conceptual del proyecto <a name="diseño"></a>
  El sistema, modela una arquitectura cliente servidor en la cual diferentes tipos de clientes acceden a diferentes tipos de servicios que brinda el servidor.
  Los clientes se diferencian por las características del socket que utilizan para la comunicación, en función de esto, es que pueden acceder a uno u otro servicio.

### Server <a name="server"></a>
El servidor, prestará diferentes servicios a diferentes tipos de clientes. Al momento de instanciar un nuevo servidor, se le deben indicar los siguientes parámetros
```
./Server <IPV4Port> <IPv6Port> <pathUnix>
```
En el caso de las direcciones IP para los sockets de internet, se definen como las direcciones de loopback del sistema en el que corren.

Inicialmente el servidor crea tres sockets (IPv4, IPv6 y UNIX) a través de tres procesos diferentes. Una vez que esto se realiza, cada proceso queda esperando por conexiones de clientes. 
Al momento en el que llega una nueva conexión, se genera un proceso hijo que se encargará de atender dicha conexión, mientras que el padré seguirá esperando por nuevas conexiones.
El modo de comunicación elegido para este sistema es orientado a la conexión, lo que quiere decir que una vez que se establece la conexión, esta quedará abierta hasta que sea cerrada, y no será necesario explicitar la dirección de destino u origen cada vez que se quiera enviar o recibir un mensaje.

### IPv4 Clients
Estos clientes, utilizan un socket de internet con dirección IPv4 en un puerto especificado para comunicarse con el servidor. Cuentan con una CLI que permite enviar una serie de argumentos al servidor. Este tomará estos argumentos para realizar una ejecución del comando `journalctl` y devolverá al cliente la salida de esta ejecución.
El tipo de cliente, la dirección IPv4 y puerto del servidor, deben ser ingresados como argumento al momento de inicializar el cliente, de la siguiente forma
```
./Client ipv4 <ipv4Address> <port>
```
En el caso que el cliente corra de manera local, la dirección IPv4 del server es la dirección de loopback (127.0.0.1, localhost). Nótese que debe conocerse el puerto en el que el server presta el servicio.

### IPv6 Clients
Estos clientes, utilizan un socket de internet con dirección IPv6 en un puerto especificado para comunicarse con el servidor. Cuentan con una CLI que permite enviar una serie de argumentos al servidor. Este tomará estos argumentos para realizar una ejecución del comando `journalctl` y devolverá al cliente la salida de esta ejecución pero comprimida mediante gzip. A diferencia del cliente anterior, este cliente, guardará en un archivo la información comprimida que recibió. DIchos archivos, se encuentran en un directorio llamado `/tmp`. 
El tipo de cliente, la dirección IPv6 y puerto del servidor, deben ser ingresados como argumento al momento de inicializar el cliente, de la siguiente forma
```
./Client ipv6 <ipv6Address> <port>
```
En el caso que el cliente corra de manera local, la dirección IPv6 del server es la dirección de loopback IPv6 (::1). Nótese que debe conocerse el puerto en el que el server presta el servicio.

### Unix Clients
Estos clientes, utilizan un socket Unix. Requiere la dirección a un archivo que utilizará para comunicarse con el servidor. Estos clientes, al momento de generar la conexión comenzarán a recibir un reporte del loadAverage y freeMemory del sistema.
El tipo de cliente y el path al archivo se deben ingresar como argumento al momento de inicializar el cliente, de la siguiente forma
```
./Client unix <path>
```
Se debe conocer el path en el que el servidor está prestando el servicio.

## Ejemplo de ejecución
### Server
Puede observarse que inicialmente el servidor está listo para recibir conexiones, y luego ya indica cuales son los procesos hijos que se encargarán de las conexiones que llegaron al servidor (una de cada tipo).
![image](https://user-images.githubusercontent.com/83674694/231828336-b6220d2f-55dc-41e4-83e6-00772044a4ae.png)

En la siguiente imágen, podemos observar el servidor recibiendo requests de los diferentes clientes
![image](https://user-images.githubusercontent.com/83674694/231828998-e689fc18-d337-48b5-830d-b9df636056ff.png)

### IPV4 Clients
  A modo ejemplo, se envía los parámetros -n 1, y obtiene la salida desde el servidor 
  ![image](https://user-images.githubusercontent.com/83674694/231829239-5cf72260-1c82-4fa9-87e5-bc05c6511404.png)
  
### IPV6 Clients
  En este caso, la salida se guarda en un archivo comprimido
  ![image](https://user-images.githubusercontent.com/83674694/231829398-515a57db-69c1-4826-ba99-e91a8999aaa1.png)
  ![image](https://user-images.githubusercontent.com/83674694/231829503-c76afbf3-28b4-4693-8da4-34f3ec6b07f3.png)
  ![image](https://user-images.githubusercontent.com/83674694/231829560-aa31507e-7872-4c54-8314-8e781bfb0b54.png)

  Si lo descomprimimos, podemos ver la salida.
  
### UNIX Clients
  Estos clientes reciben periódicamente un reporte del loadAverage y freeMemory
  ![image](https://user-images.githubusercontent.com/83674694/231829756-62a513f8-ded6-4732-a1ba-4b1e134ecf16.png)
  
### Finalización
  En caso de interrumpir a los clientes, se cierran las conexiones generadas 
  ![image](https://user-images.githubusercontent.com/83674694/231829938-88a1892a-dff2-4618-a148-658a96e083b1.png)
  
### Test de creación de n clientes
  En el directorio Tests, se creo un pequeño script en C que recibe como parámetro la cantidad de clientes que se desean crear. Para poder ejecutarlo, se pueden seguir los diferentes pasos
  1. Compilar el proyecto según se indica en la primera sección
  2. Utilizar el script `launcher.sh` para crear una instancia de server con un cliente de cada tipo
  3. Ir al directorio `Tests` y compilar el archivo `test.c` con `gcc -o test test.c`
  4. Correr el binario `test` indicando la cantidad de clientes que se desean crear
  5. Podrá observarse en el servidor que se crearan la cantidad de clientes que se han pasado como argumento, estos recibirán datos durante 10 segundos y luego cerrarán la conexión con el server. El server seguirá con los tres clientes que tenía originalmente.
  
  ![image](https://user-images.githubusercontent.com/83674694/232635658-a66c07ba-81f2-4cb3-b6da-a67cc4cedcc2.png)
  
  ![image](https://user-images.githubusercontent.com/83674694/232635682-2749b41a-c008-4444-ba76-51ddead41399.png)


 
## BUGS y Faltantes
  - Cuando se envían argumentos inválidos al journalcrl, el cliente IPv6 guarda la salida debido a que no identifica que hubo un problema.
  - Mejorar implementación del checksum
  - Mejorar el manejo de los procesos hijos en el servidor


