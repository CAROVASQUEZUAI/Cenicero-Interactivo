# Cenicero Interactivo IoT

## Descripción del proyecto

El **Cenicero Interactivo IoT** es un prototipo diseñado para reducir la
disposición inadecuada de colillas de cigarrillo en espacios públicos.
Utiliza dos sensores infrarrojos FC-51 para detectar y contabilizar las
colillas depositadas en dos contenedores independientes. Un ESP32
procesa las detecciones, actualiza una pantalla LCD y ofrece un
dashboard web mediante una red Wi-Fi propia.

## Integrantes

-   Montserrat Leiva
-   Martina Ureta
-   Carolina Vásquez

**Asignatura:** TEI201 - Taller de Diseño en Ingeniería\
**Institución:** Universidad Adolfo Ibáñez

## Problema que aborda

En el sector empresarial de El Golf, en Las Condes, algunas personas
desechan colillas de cigarrillo en calles y espacios peatonales, incluso
cuando existen basureros cercanos. Esto contribuye a la contaminación
del entorno y evidencia la necesidad de facilitar e incentivar una
disposición responsable de estos residuos.

El prototipo aborda esta problemática mediante un cenicero interactivo
que registra el uso de dos contenedores y entrega información para
apoyar su monitoreo, limpieza y mantenimiento. Además, busca fomentar
hábitos responsables y generar conciencia ambiental.

## Componentes necesarios para replicar el hardware

  Componente                  Cantidad Especificación / función
  ------------------------- ---------- -----------------------------------------------
  ESP32 DevKit V1                    1 Microcontrolador con Wi-Fi y Bluetooth
  Sensor infrarrojo FC-51            2 Detección digital del paso de colillas
  Pantalla LCD                       1 16×2 con comunicación I2C
  Protoboard blanca                  1 830 puntos; montaje de conexiones
  Protoboard naranja                 1 170 puntos; placa auxiliar
  Power bank                         1 5.000 mAh; alimentación portátil
  Cable USB                          1 Alimentación del ESP32
  Cables Dupont                  1 set Conexiones eléctricas
  MDF                            1 set Planchas de 5,5 mm y 3 mm para el encapsulado
  Acrílico                       1 set Contenedores removibles

### Estructura física

El encapsulado está construido con MDF de 5,5 mm y 3 mm. Tiene tres
compartimientos: los espacios laterales alojan los contenedores
removibles de acrílico y el compartimiento central protege el circuito
electrónico y la alimentación. Cada contenedor dispone de un sensor
infrarrojo FC-51.

## Instrucciones para cargar el firmware

### Requisitos

-   Arduino IDE instalado.
-   Archivo de firmware `.ino` ubicado en `firmware/`.
-   Controladores USB de la placa ESP32, si son necesarios.
-   Bibliotecas requeridas por el código fuente.

### Carga del programa

1.  Conectar el ESP32 al computador mediante un cable USB de datos.
2.  Abrir Arduino IDE y cargar el archivo `.ino` del proyecto.
3.  Revisar los `#include` del programa e instalar las bibliotecas que
    falten.
4.  En **Herramientas \> Placa**, seleccionar una placa ESP32
    compatible.
5.  Seleccionar el puerto correspondiente.
6.  Compilar el programa y resolver cualquier dependencia faltante.
7.  Presionar **Subir** y esperar a que finalice la carga.
8.  Alimentar el prototipo mediante la power bank.

**Importante:** verificar en el código fuente los GPIO asignados a los
sensores y los pines SDA/SCL de la pantalla I2C. Las conexiones deben
coincidir con el firmware y el esquemático eléctrico de `hardware/`.

## Conexión y uso del dashboard

El ESP32 configura una red Wi-Fi propia y ejecuta un servidor web local.
Por ello, el dashboard puede utilizarse sin conexión a internet.

1.  Encender el prototipo y esperar a que el ESP32 inicialice la red.
2.  Desde un teléfono o computador, conectarse a la red Wi-Fi creada por
    el ESP32.
3.  Abrir un navegador e ingresar la dirección `http://192.168.4.1`.
4.  Comprobar que se visualicen los conteos individuales y el total
    acumulado.
5.  Pasar una colilla por cada entrada para verificar la detección del
    sensor correspondiente.

**Datos de conexión:**

-   **Nombre de la red (SSID):** `CeniceroInteractivo`
-   **Contraseña:** `Cenicero26`
-   **Dirección del dashboard:** <http://192.168.4.1>

El dashboard muestra los conteos de ambos contenedores, sus porcentajes
de ocupación y una alerta configurable de llenado. La interfaz se
actualiza automáticamente.

> **Aviso:** si el repositorio será público, considera que cualquier
> persona podrá ver la contraseña de la red Wi-Fi al consultar este
> archivo. En ese caso, cambia la contraseña antes de publicar el
> repositorio o evita incluir credenciales de acceso.

## Información generada

El sistema genera:

-   Conteo individual del contenedor 1.
-   Conteo individual del contenedor 2.
-   Total acumulado de colillas.
-   Porcentaje de ocupación de cada contenedor.
-   Estado de las alertas de llenado.

**Almacenamiento:** el dashboard permite visualizar los registros
durante el funcionamiento del prototipo. La disponibilidad de los datos
después de apagar o reiniciar el ESP32 depende de que el firmware
implemente almacenamiento persistente; no debe asumirse que los conteos
quedan guardados si esa función no está implementada.

## Uso de herramientas de inteligencia artificial

Durante el desarrollo del proyecto se utilizaron herramientas de
inteligencia artificial como apoyo para comprender y revisar el código
del sistema, además de apoyar la generación de imágenes utilizadas en el
desarrollo o presentación del prototipo. Estas herramientas se emplearon
como recurso complementario; la selección de componentes, el montaje
físico, la integración del sistema y las pruebas del prototipo fueron
responsabilidad del equipo.

## Contenido del repositorio

``` text
cenicero-interactivo/
├── README.md
├── FUENTES.md
├── firmware/
│   ├── main.ino
│   └── libraries/
├── hardware/
│   ├── esquematico.pdf
│   └── BOM.xlsx
├── diseño-3d/
│   ├── encapsulado.f3d
│   ├── planos.pdf
│   └── renders/
├── testing/
│   ├── protocolo_pruebas.pdf
│   └── datos/
└── docs/
    ├── presentacion.pdf
    └── reporte_final.pdf
```

Esta es una estructura propuesta. Ajustar los nombres según los archivos
que efectivamente se incluyan.

## Licencia y uso

\[Indicar la licencia del proyecto o las condiciones de uso definidas
por el equipo.\]
