
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define DEFAULT_PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int sock, new_socket;
    struct sockaddr_in address;
    socklen_t addrlen;
    char buffer[BUFFER_SIZE];
    char send_buffer[BUFFER_SIZE];

    int port;
    char *endptr;

    if (argc == 1) {
        // Cap argument -> port per defecte
        port = DEFAULT_PORT;
        printf("Cap port indicat. Utilitzant valor per defecte: %d\n", port);
    } else if (argc == 2) {
        errno = 0;
        port = strtol(argv[1], &endptr, 10);

        // Comprovacions
        if (errno != 0 || *endptr != '\0' ||
            port < 1 || port > 65535) {
            fprintf(stderr, "Error: port incorrecte.\n");
            return EXIT_FAILURE;
        }
    } else {
        fprintf(stderr, "Us: %s [port]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Crear el socket
	// Afegiu comentari explicant els arguments
    sock = socket(AF_INET, SOCK_STREAM, 0); 
    /* socket() funció per crear el socket*/ 
    /* AF_INET especifica la la familia de direcció, en aquest cas indica que el socket utiliza el protocol IPv4*/ 
    /* SOCK_STREAM especifica el tipus de socket que utiliza*/
    /* 0 especifica el tipus especific dins de la familia de direcció, en aquest cas intendifica com*/ 
    if (sock < 0) {
        perror("Error en crear el socket");
        return EXIT_FAILURE;
    }

    /* Permetre reutilitzar el port */
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)) < 0) {
        perror("Error en setsockopt");
        close(sock);
        return EXIT_FAILURE;
    }

    // Afegiu comentari explicant què són aquests 3 paràmetres,
	// per què s'utilitzen htons() i htonl(),
	// i per què s'utilitzen els valors que hi ha
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    /* Espesifica la familia d'adreça de la xarxa d'aquesta estructura */
    address.sin_addr.s_addr = htonl(INADDR_ANY);// Per a INADDR_ANY no faria falta htonl(), però li posem per coherència
    /* Defineix la direcció IP a la qual es vincularà en el socket */
    /* INADDR_ANY incica que el servidor acceptar connexio des de qualsevol interficie de xarxa de la màquina local (Ethernet, Wi_Fi, etc) */
    /* htonl() converteix un sencer de 32bits de l'ordre de bytes 
    de l'host (Little-Endian en la majoria d'arquitectures x86/ARM) a l'ordre de xarxa (Big-Endian) */
    address.sin_port = htons(port);
    /* defineix el número de port en el servidor que acceptarà */
    /* 'port' serveix per determinar el punt d'accés la */
    /* converteix un sencer de 16 bits de l'ordre de bytes de l'host a l'ordre de xarxa (Big-Endian)
    És imprescindible perquè la xarxa interpreti correctament el número de port sense invertir els bytes */

    // Enllaçar el socket al port especificat
	// Afegiu comentari explicant els arguments
    if (bind(sock, (struct sockaddr *)&address,
    /* bind() atorga una identitat (IP i Port) al socket per poder rebre trànsit de la xarxa. */
    /* L'argument (struct sockaddr *) actua com a punter genèric per suportar diversos protocols. */
    /* sock: És el descriptor de fitxer del socket que hem creat prèviament.*/
    /* (struct sockaddr *)&address: Punter a la configuració d'adreça i port. Fem 
        un 'cast' (conversió de tipus) cap a 'struct sockaddr *' perquè la funció bind() 
        és genèrica i cal adaptar la nostra estructura 'sockaddr_in' (específica d'IPv4) al format genèric. */
    /* sizeof(address): La mida en bytes de l'estructura passada. */
             sizeof(address)) < 0) {
        perror("Error en fer el bind");
        close(sock);
        return EXIT_FAILURE;
    }

    // Escoltar connexions entrants
	// Afegiu comentari explicant els arguments
    if (listen(sock, 5) < 0) {
    /*listen() marca el socket 'sock' com a passiu perquè el sistema operatiu escolti la xarxa. */
    /* El segon argument (5, anomenat 'backlog') defineix la mida de la cua*/
        perror("Error en escoltar");
        close(sock);
        return EXIT_FAILURE;
    }

    printf("Servidor en funcionament al port %d.\n", port);

    while (1) {
        // Acceptar connexions de clients
		// Afegiu comentari explicant els arguments i per què hi ha i cal new_socket si ja tenim sock
        addrlen = sizeof(address);
        /* accept(sock, &addr, &addrlen) extrau un client de la cua i en guarda la IP a l'estructura.*/
        /* Crea i retorna un 'new_socket' dedicat exclusivament a la comunicació amb aquest client.
        Això és imprescindible perquè el 'sock' original ha de continuar lliure escoltant noves connexions.*/
        new_socket = accept(sock, (struct sockaddr *)&address,
                            &addrlen);

        if (new_socket < 0) {
            perror("Error en acceptar la connexio");
            continue;
        }

        printf("Client connectat.\n");

        while (1) {
            memset(buffer, 0, sizeof(buffer));
            memset(send_buffer, 0, sizeof(send_buffer));

            ssize_t n = recv(new_socket, buffer,
                             BUFFER_SIZE - 1, 0);
            // Llegir missatge del client
            if (n <= 0) {
                printf("Client desconnectat.\n");
                break;
            }

            buffer[n] = '\0';

            // Aquí haureu d'implementar l'anàlisi de la cadena rebuda per saber l'operació,
			// i si n'hi ha els arguments, executar-la i tornar el(s) resultat(s)

			// A continuació hi ha el codi corresponent a l'opció d'Enviar missatge, amb el retorn d'una cadena fixa,
			// i la de Sortida de la connexió/bucle si és el cas.
			// Modifiqueu la cadena de retorn per tal que sigui la que diu l'enunciat.


            printf("Missatge rebut: %s\n", buffer);

            // Comprovar si el client vol tancar la connexió
            if (strcmp(buffer, "EXIT") == 0) {
                snprintf(send_buffer, sizeof(send_buffer),
                         "Connexio tancada\n");

                send(new_socket, send_buffer,
                     strlen(send_buffer), 0);
                break; // Sortir del bucle
            }

            /* OPCIO 2: TEMPS DE VOL */
            else if (strncmp(buffer, "FLIGHT_TIME ", 12) == 0) {
                double distancia, velocitat, temps;

                if (sscanf(buffer, "FLIGHT_TIME %lf %lf",
                           &distancia, &velocitat) == 2 &&
                    distancia >= 0 && velocitat > 0) {

                    temps = distancia / velocitat;

                    snprintf(send_buffer, sizeof(send_buffer),
                             "%.2f h\n", temps);
                } else {
                    snprintf(send_buffer, sizeof(send_buffer),
                             "Error: dades incorrectes. "
                             "La velocitat ha de ser major que 0.\n");
                }
            }
			/* OPCIO 3: COMBUSTIBLE NECESSARI
               Format: FUEL temps consum */
            else if (strncmp(buffer, "FUEL ", 5) == 0) {
                double temps, consum, combustible;

                if (sscanf(buffer, "FUEL %lf %lf",
                           &temps, &consum) == 2 &&
                    temps >= 0 && consum >= 0) {

                    combustible = temps * consum;

                    snprintf(send_buffer, sizeof(send_buffer),
                             "%.2f L\n", combustible);
                } else {
                    snprintf(send_buffer, sizeof(send_buffer),
                             "Error: dades incorrectes per al combustible.\n");
                }
            }



            /* OPCIO 4: COST DEL COMBUSTIBLE
               Format: FUEL_COST combustible preu */
            else if (strncmp(buffer, "FUEL_COST ", 10) == 0) {
                double combustible, preu, cost;

                if (sscanf(buffer, "FUEL_COST %lf %lf",
                           &combustible, &preu) == 2 &&
                    combustible >= 0 && preu >= 0) {

                    cost = combustible * preu;

                    snprintf(send_buffer, sizeof(send_buffer),
                             "%.2f euros\n", cost);
                } else {
                    snprintf(send_buffer, sizeof(send_buffer),
                             "Error: dades incorrectes per al cost.\n");
                }
            }

            /* OPCIO 5: AUTONOMIA
               Format: RANGE combustible consum velocitat */
            else if (strncmp(buffer, "RANGE ", 6) == 0) {
                double combustible, consum, velocitat, autonomia;

                if (sscanf(buffer, "RANGE %lf %lf %lf",
                           &combustible, &consum, &velocitat) == 3 &&
                    combustible >= 0 && consum > 0 &&
                    velocitat >= 0) {

                    autonomia = (combustible / consum) * velocitat;

                    snprintf(send_buffer, sizeof(send_buffer),
                             "%.2f km\n", autonomia);
                } else {
                    snprintf(send_buffer, sizeof(send_buffer),
                             "Error: consum ha de ser major que 0 "
                             "i les altres dades no poden ser negatives.\n");
                }
            }

            

            /* OPCIO 1: MISSATGE NORMAL */
            else {
                snprintf(send_buffer, sizeof(send_buffer),
                         "Missatge rebut: %.1000s\n", buffer);
            }

            if (send(new_socket, send_buffer,
                     strlen(send_buffer), 0) < 0) {
                perror("Error en enviar la resposta");
                break;
            }
        }

        close(new_socket); // Tancar la connexió amb el client
    }

    close(sock);
    return 0;
}
