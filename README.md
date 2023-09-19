# SO TP1

Repositorio para el **TP1** de la materia **Sistemas Operativos** en el **ITBA**

## Tabla de Contenidos

- [SO TP1](#so-tp1)
  - [Tabla de Contenidos](#tabla-de-contenidos)
  - [Autores](#autores)
  - [Requisitos](#requisitos)
  - [Instalación](#instalación)
    - [Docker](#docker)
      - [Descargar imagen](#descargar-imagen)
      - [Ejecutar el contenedor](#ejecutar-el-contenedor)
      - [Correr el contenedor](#correr-el-contenedor)
      - [Aliases](#aliases)
  - [Ejecución](#ejecución)
    - [Compilación](#compilación)
    - [Ejecución sin vista](#ejecución-sin-vista)
    - [Ejecución con pipe](#ejecución-con-pipe)
    - [Ejecución con 2 terminales](#ejecución-con-2-terminales)
  - [Resultados](#resultados)

## Autores
- Grupo 12:
  - Ben Deyheralde (Legajo: 63559)
  - Lucas Gonzalez Rouco (Legajo: 63366)
  - Matias Mutz (Legajo: 63590)

## Requisitos

Para la realizacion del trabajo se requieren los siguientes elementos:

- **docker:** Nos va a permitir compilar el TPE dentro de un contenedor

## Instalación

### Docker

La opcion mas facil va a ser ir a la pagina oficial de [docker](https://www.docker.com/products/docker-desktop/) donde podran elegir la version a instalar.

#### Descargar imagen

```bash
docker pull agodio/itba-so:2.0
```

Y pueden revisar que la imagen aparezca utilizando:

```bash
docker images
```

#### Ejecutar el contenedor

```bash
docker run -v ${PWD}:/root --privileged -ti --name SO agodio/itba-so:2.0
```

Esto debería mostrar un prompt como el siguiente:

```bash
root@c3285f863835:/#
```

#### Correr el contenedor

```bash
docker start SO
docker exec -ti SO bash
```

#### Aliases

Pueden armar un aliases para trabajar con la imagen:

```bash
alias dcrun="docker run -v ${PWD}:/root --privileged -ti --name SO agodio/itba-so:2.0"
alias dcexec="docker exec -ti SO bash"
alias dcstart="docker start SO"
alias dcstop="docker stop SO"
alias dcrm="docker rm -f SO"
alias dcall="dcstart && dcexec"
```

- `dcrun` abre la terminal como es usual para utilizar la imagen provista.
- Realizar exit dentro del docker o `dcstop` desde el host tienen el mismo efecto.
- `dcexec` en este punto falla porque no está corriendo, es necesario `dcstart` antes de utilizar `dcexec`.
- `dcrm` es opuesto a `dcrun`.
- `dcall` ejecuta `dcstart` y `dcexec`.

## Ejecución

### Compilación

Para compilar el proyecto se debe correr el siguiente comando:

```bash
make
```

### Ejecución sin vista

Para ejecutar el proyecto se debe correr el siguiente comando:

```bash
./app files/*
```

### Ejecución con pipe

Para ejecutar el proyecto se debe correr el siguiente comando:

```bash
./app files/* | ./view
```

### Ejecución con 2 terminales

Para ejecutar el proyecto se debe correr el siguiente comando:

En la primer terminal:

```bash
./app files/*
```

Y en la segunda terminal:

```bash
./view /shm
```

## Resultados

Los resultados obtenidos se encuentran en el archivo [output.txt](output.txt).

Si se ejecuto con una vista, los resultados también se imprimiran en la terminal
