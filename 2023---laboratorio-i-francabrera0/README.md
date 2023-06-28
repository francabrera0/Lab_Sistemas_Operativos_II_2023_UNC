# 2023---laboratorio-i-francabrera0
2023---laboratorio-i-francabrera0 created by GitHub Classroom

# Objetivo
El presente informe pretende definir conceptualmente el diseño implementado para resolver la problemática planteada.

# Planteo del problema
Se debe diseñar un sistema cliente servidor en el cual se diferencien tres tipos de clientes. La diferencia entre cada uno de ellos es el método de IPC que utilizan para comunicarse con el servidor.
En este caso, los métodos utilizados son memoria compartida, colas de mensajes y fifo.
El servidor deberá antender las solicitudes de múltiples clientes de cualquier tipo.

# Solución planteada
En la siguiente imágen se plantea la arquitectura general del sistema, donde puede notarse que inicialmente, los clientes cuentan con un canal (cola de mensajes) para enviar peticiones al servidor.
El servidor, al recibir una nueva petición, devolverá al cliente un mensaje indicando por qué canal se debe comunicar (en función del tipo de cliente).
Luego puede observarse que los clientes de tipo 1 se comunican por una cola de mensajes, los tipo 2 por memoria compartida y finalmente los tipo 3 por fifo.

![image](https://user-images.githubusercontent.com/83674694/227240706-cd21fc4c-2450-421f-bd1b-e17904352b70.png)

Cabe aclarar, que puede haber más de una instancia del mismo cliente, por lo tanto se deberá controlar el acceso al recurso compartido (canal de comunicación) para evitar problemas de concurrencia.

### Semáforos
Debido a que ninguno de los métodos anteriormente nombrados es threadSafe, es necesario sincronizar el acceso a los mismos para evitar problemas de concurrencia. 
Por lo tanto, se definió un semáforo para cada uno de los canales de comunicación. Este semáforo es tomado por el cliente al momento de querer enviar un mensaje, una vez enviado lo libera, permitiendo que otro proceso cliente lo haga.

## HandShake inicial
  Cómo se mencionó anteriormente, se implementó una rutina de solicitud por parte de los clientes al servidor. Cuando un proceso nuevo es creado, lo primero que hace es hacer uso de la dirección de la cola de peticiones (obtenida del archivo de configuración) para enviar una nueva solicitud.
  En el mensaje, se coloca el tipo de cliente que se instancio. En función de esto, el servidor responde a dicho proceso con la dirección correspondiente al canal que se tiene que conectar. Esta dirección es representada por la ruta a un archivo que se utilizará para crear la clave de los IPC de systemV o la fifo según corresponda. Estos archivos, serán creados al iniciar un server tomando las rutas del archivo de configuración.
  Una vez que el cliente recibe este mensaje, si no es un rechazo, se conecta al canal correspondiente y puede comenzar a transmitir.

# Ejecuión
  Al correr el comando `make`, dentro del directorio de trabajo, se generaran todos los archivos necesarios para ejecutar el programa. Los binarios correspondientes se encuentran en el directorio `/bin`.
  Para realizar una primera prueba, se puede utilizar el script `launcher.sh` que se encuentra en el directorio raiz. Este script instanciará el servidor y un cliente de cada tipo.
  Luego pueden crearse nuevos procesos cliente desde otra terminal simplemente lanzando un nuevo cliente. A continuación se presenta una muestra.
 
 ![image](https://user-images.githubusercontent.com/83674694/227248956-cae6b6be-0054-4254-ad96-528b7bdb3add.png)

 Puede observarse en la imágen los id de los diferentes canales de comunicación, y los mensajes enviados por cada proceso. Estos mensajes simplemente son un contador concatenado con su nombre.
 
 Al finalizar el servidor, los clientes son desconectados de los canales de comunicación, se eliminan todos los canales y se imprime en pantalla un totalizador de mensajes por canal a modo estadístico.
 
# Errores conocidos y mejoras
* Demasiado código en el archivo server.c, se debería descomponer en diferentes archivos juntando funcionalidades que estén relacionadas
