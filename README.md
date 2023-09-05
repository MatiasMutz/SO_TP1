# SO TP1

Repositorio para el **TP1** de la materia **Sistemas Operativos** en el **ITBA**

## Tabla de Contenidos

  - [SO TP1](#so-tp1)
    - [Autores](#autores)
    - [Tabla de Contenidos](#tabla-de-contenidos)
    - [Requisitos](#requisitos)
    - [Instalacion](#instalacion)
      - [Docker](#docker)
        - [Descargar imagen](#descargar-imagen)
        - [Ejecutar el contenedor](#ejecutar-el-contenedor)
        - [Correr el contenedor](#correr-el-contenedor)
        - [Aliases](#aliases)

## Autores

- Ben Deyheralde (Legajo: 63559)
- Matias Mutz (Legajo: 63590)
- Lucas Rouco (Legajo: 63366)

## Requisitos

Para la realizacion del trabajo se requieren los siguientes elementos:

- **docker:** Nos va a permitir compilar el TPE dentro de un contenedor

## Instalacion

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
