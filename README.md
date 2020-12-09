# TrabajoColaborativo-1

Programa en C que -intenta- leer un archivo .bmp y a partir de esta lectura crea uno idéntico.
Programa en Python que lee un archivo .bmp, aplica un kernel y devuelve la imagen con el kernel aplicado.

Cambios:
-Ya no se hace lectura por bloque de filas sino byte a byte y la escritura en el nuevo archivo lo hace satisfactoriamente.

Pendiente:
-Al definir un kernel y aplicar la convolución sobre los bytes recorridos genera una imagen dañada.
-Paralelizar

Importante: lena.bmp tiene 1 byte por pixel. Por otro lado img1.bmp tiene 3 bytes por pixel.
