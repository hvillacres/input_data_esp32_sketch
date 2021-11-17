# input_data_esp32_sketch
Lo primero a realizar es un sketch para nuestro esp32, el cual se encargará de recoger los datos obtenidos con el sensor MPU6050. 
Esta codificación la realizaremos utilizando platformio. Para este proyecto se hará uso del acelerómetro y del giroscopio. 
Un punto muy importante a tener en cuenta es que, al recolectar un conjunto de datos, cada uno de ellos debe tener un tiempo (para todos será el mismo, en este caso 3 segundos), 
el cual se utilizará para detectar el movimiento, generando así series temporales.

Una vez cargado el Sketch, utilizando Python, obtendremos los datos del módulo esp32, Recordar que tenemos 3 segundos para ejecutar cada movimiento. 
Un punto muy importante es que tenemos que generar una gran cantidad de datos para obtener un mejor modelo.
