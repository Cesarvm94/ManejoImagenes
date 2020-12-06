#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#define DATA_OFFSET_OFFSET 0x000A
#define WIDTH_OFFSET 0x0012
#define HEIGHT_OFFSET 0x0016
#define BITS_PER_PIXEL_OFFSET 0x001C
#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
#define NO_COMPRESION 0
#define MAX_NUMBER_OF_COLORS 0
#define ALL_COLORS_REQUIRED 0
typedef unsigned int int32;
typedef short int16;
typedef unsigned char byte; 
 
void ReadImage(const char *fileName,byte **pixels, int32 *width, int32 *height, int32 *bytesPerPixel)
{
		//En este primer bloque vamos a sacar información del header
		//Abro el archivo en modo rb (lectura de binarios)
		FILE *imageFile = fopen(fileName, "rb"); 
        int32 dataOffset;
        /*Permite hacer un salto a una posición especifica del fichero, es decir, desplaza la posición actual de lectura del fichero, el orden de los parametros recibidos serán:
        el indentificador del archivo, es decir, el puntero definido para la apertura del archivo, luego el desplazamiento y finalmente la posición de origen, en este caso, iniciaremos desde
        la primer posicion del archivo (comienzo de dicho archivo) esto lo hago con SEEK_SET que es una "palabra" reservada que define la posicion del puntero donde inicia dicho archivo
        */
        /*Se corre a DATA_OFFSET_OFFSET--->0x000A, es decir 10 en decimal, porque en dicha posición es donde empiezan los datos de pixeles del archivo.... 
        */
        fseek(imageFile, DATA_OFFSET_OFFSET, SEEK_SET);
        /*
        Permite definir unos parametros de lectura, para ello usamos primero un puntero que me va a permitir definir un espacio para guardar lo leído, luego el tamaño de lo leido en bytes,
        posteriormente el numero de items a ser leidos, finalmente el puntero que me dice donde está el archivo a leer. Me va a devolver en la variable dataoffset el numero de elementos leidos
        */
        fread(&dataOffset, 4, 1, imageFile);
        /*
        Actualizo mi posicion a la posicion donde esta ubicada la información del ancho de la imagen en pixeles       
        */
        fseek(imageFile, WIDTH_OFFSET, SEEK_SET);
        /*
        Leo la informacion relacionada al ancho de la imagen en pixeles       
        */
        fread(width, 4, 1, imageFile);
        
        
        /*
        Actualizo mi posicion a la posicion donde esta ubicada la información de la altura de la imagen en pixeles       
        */
        fseek(imageFile, HEIGHT_OFFSET, SEEK_SET);
        /*
        Leo la informacion relacionada a la altura de la imagen en pixeles       
        */
        fread(height, 4, 1, imageFile);
        
        int16 bitsPerPixel;
        /*
        Actualizo mi posicion a la posicion donde esta ubicada la información del numero de bits por pixel    
        */
        fseek(imageFile, BITS_PER_PIXEL_OFFSET, SEEK_SET);
        /*
        Leo la informacion contenida en la direccion previamente asignada con fseek, esta vez hago deferenciacion a la variable creada "bitsPerPixel" y hago una lectura de un elemento de tamaño
        de 2 bytes, dicho tamaño está determinado en la documentación
        */
        fread(&bitsPerPixel, 2, 1, imageFile);
        /*
        El dato recuperado está expresado en bits, lo transformo en bytes y lo asigno a la variable "bytesPerPixel" apuntada por dicho puntero.
        */
        *bytesPerPixel = ((int32)bitsPerPixel) / 8;
        int m=*bytesPerPixel;
        
 		//Fin de información del header
 		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 		//Ceil aproxima al entero mayor más cercano 
 		/*
 		Calculo ajustado de bytes en una fila, asegurarme que cada 4 pixeles voy a tener una distribucion uniforme de bytes, es decir, hacer su longitud multiplo de 4
 		*/
        int paddedRowSize = (int)(4 * ceil((float)(*width) / 4.0f))*(*bytesPerPixel);         
        //Ceil aproxima al entero mayor más cercano 
 		/*
 		Calculo de bytes en una fila (numero de elementos (pixeles) en fila x bytes por pixel)
 		*/        
		int unpaddedRowSize = (*width)*(*bytesPerPixel);
		/*
		Calculo de bytes totales de la imagen (anchoajustado x alto)
		*/
        int totalSize = unpaddedRowSize*(*height);
        /*
        Arreglo de pixeles con el tamaño de bytes de la imagen completa
        */
        *pixels = (byte*)malloc(totalSize);
        /*
        Puntero que va a apuntar a la ultima fila del arreglo de pixeles, esto lo hace con sumandole a la variable inicial el tamaño de la altura -1 * el numero de filas...  
        ¿Le resta 1 porque el indice empieza en 0?
        */
        
        int i=0;
		byte *currentRowPointer = *pixels+((*height-1)*unpaddedRowSize);
        for (i = 0; i < *height; i++)
        {
        	
        	//Me empiezo a mover por el archivo tantas veces como filas tenga 
        	/*
        	Identificador del archivo, desplazamiento, comienzo en el inicio del archivo
        	
        	El desplazamiento se hará cogiendo el dataOffSet para omitir el header innecesario y a ello le sumo el producto del elemento iterador por el ancho en bytes de una fila
        	Se asume que se coge el tamaño ajustado para asegurarse que no queda ningun byte sobrando en la fila cometiendo errores en los saltos
        	Es decir, que en cada desplazamiento voy a cubrir una fila, haciendo una lectura fila a fila.
        	*/
        	
            fseek(imageFile, dataOffset+(i*paddedRowSize), SEEK_SET);
            /*
            Realizo la lectura de abajo hacia arriba ya que currentRowPointer fue apuntado al final del archivo, haré una lectura de byte por byte y tomaré tantos bytes
            como elementos no ajustados tiene la fila, todo ello del archivo "imageFile"
            */
            fread(currentRowPointer, 1, unpaddedRowSize, imageFile); 
            /*
            El decremento se lleva a cabo acá, donde empiezo en el final del archivo y empiezo a ir de adelante hacia atrás.
            */
            currentRowPointer -= unpaddedRowSize;
            //currentRowPointer -= 3;
        }
        
        
        
 		//Cerrado de archivo
        fclose(imageFile);
}

void WriteImage(const char *fileName, byte *pixels, int32 width, int32 height,int32 bytesPerPixel)
{
        //Abriendo archivo
		FILE *outputFile = fopen(fileName, "wb");
        //Variable usada para construir parte del header
        const char *BM = "BM";
        //Empiezo a moverme a traves del archivo para construir el header completo
        fwrite(&BM[0], 1, 1, outputFile);
        fwrite(&BM[1], 1, 1, outputFile);
        int paddedRowSize = (int)(4 * ceil((float)width/4.0f))*bytesPerPixel;
        //Tamaño del archivo usado en el header
        int32 fileSize = paddedRowSize*height + HEADER_SIZE + INFO_HEADER_SIZE;
        fwrite(&fileSize, 4, 1, outputFile);
        int32 reserved = 0x0000;
        fwrite(&reserved, 4, 1, outputFile);
        int32 dataOffset = HEADER_SIZE+INFO_HEADER_SIZE;
        fwrite(&dataOffset, 4, 1, outputFile);
 
        
        int32 infoHeaderSize = INFO_HEADER_SIZE;
        fwrite(&infoHeaderSize, 4, 1, outputFile);
        fwrite(&width, 4, 1, outputFile);
        fwrite(&height, 4, 1, outputFile);
        int16 planes = 1; //always 1
        fwrite(&planes, 2, 1, outputFile);
        int16 bitsPerPixel = bytesPerPixel * 8;
        fwrite(&bitsPerPixel, 2, 1, outputFile);
        //write compression
        int32 compression = NO_COMPRESION;
        fwrite(&compression, 4, 1, outputFile);
        //write image size (in bytes)
        int32 imageSize = width*height*bytesPerPixel;
        fwrite(&imageSize, 4, 1, outputFile);
        int32 resolutionX = 11811; //300 dpi
        int32 resolutionY = 11811; //300 dpi
        fwrite(&resolutionX, 4, 1, outputFile);
        fwrite(&resolutionY, 4, 1, outputFile);
        int32 colorsUsed = MAX_NUMBER_OF_COLORS;
        fwrite(&colorsUsed, 4, 1, outputFile);
        int32 importantColors = ALL_COLORS_REQUIRED;
        fwrite(&importantColors, 4, 1, outputFile);
        /*
        Termina de construirse el header
        */
		int j=0,k=0;
		///////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////
        /*for (j=0;j<sizeof(pixels);j++){
        	for (k=0; k<sizeof(pixels[j]);k++){
        		printf("%d",pixels[j][k]);
			}
        	
		}*/
        int i = 0;
        int unpaddedRowSize = width*bytesPerPixel;
        printf("%d",bytesPerPixel);
        int kernel[3][3]= { { -1, -1, -1 }, { -1, 8, -1 }, {-1,-1,-1} }; //Se establece el kernel
        //Se intenta recorrer pixels posicion a posicion para que haga las operaciones con el kernel
        for (i= 1; i < height - 1 ; i++){
        	int pixelOffset=((height-i)-1)*unpaddedRowSize; //Posicion de primer byte de ultima fila, el ciclo la lleva hasta primer fila
            pixels[pixelOffset]= ((pixels[pixelOffset-(unpaddedRowSize+1)]*kernel[0][0]) + (pixels[pixelOffset-unpaddedRowSize]*kernel[0][1]) +  (pixels[pixelOffset-(unpaddedRowSize-1)]*kernel[0][2]) 
			+  (pixels[pixelOffset-1]*kernel[1][0]) + (pixels[pixelOffset]*kernel[1][1]) + (pixels[pixelOffset+1]*kernel[1][2]) + (pixels[pixelOffset+(unpaddedRowSize-1)]*kernel[2][0])+
			(pixels[pixelOffset-unpaddedRowSize]*kernel[2][1]) + (pixels[pixelOffset-(unpaddedRowSize+1)]*kernel[2][2]) );
        	for (j=1;j<unpaddedRowSize;j++){  //Recorro la fila correspondiente
            	pixels[pixelOffset+j]=((pixels[pixelOffset+j-(unpaddedRowSize+1)]*kernel[0][0]) + (pixels[pixelOffset+j-unpaddedRowSize]*kernel[0][1]) +  (pixels[pixelOffset+j-(unpaddedRowSize-1)]*kernel[0][2]) 
			+  (pixels[pixelOffset+j-1]*kernel[1][0]) + (pixels[pixelOffset+j]*kernel[1][1]) + (pixels[pixelOffset+j+1]*kernel[1][2]) + (pixels[pixelOffset+j+(unpaddedRowSize-1)]*kernel[2][0])+
			(pixels[pixelOffset+j+unpaddedRowSize]*kernel[2][1]) + (pixels[pixelOffset+j+(unpaddedRowSize+1)]*kernel[2][2]) ); //Escribo byte a byte, elemento a elemento de toda la fila
			}
		}
        
        
        
        for ( i = 0; i < height ; i++)
        {
            int pixelOffset=((height-i)-1)*unpaddedRowSize; //Posicion de primer byte de ultima fila, el ciclo la lleva hasta primer fila
            fwrite(&pixels[pixelOffset], 1, 1, outputFile);
			//printf("%d",pixels[100]);
			//Escribo ese dato en esa posicion
            for (j=1;j<width*bytesPerPixel;j++){  //Recorro la fila correspondiente
            	fwrite(&pixels[pixelOffset+j], 1, 1, outputFile); //Escribo byte a byte, elemento a elemento de toda la fila
			}
        }
        //Si tengo un chorizo de datos, pues buscar un patrón que describa la convolución, cada posición se va a desplazar
        //la misma cantidad de bytes para buscar los elementos de la convolución
        
        /*
        int index=height;
        
        for ( i = 0; i < height; i++)
        {
            //int pixelOffset=((height-i)-1)*unpaddedRowSize; //Posicion de primer byte de ultima fila
            index-=1; //Indice que se comporta de forma adecuada respecto el ciclo para llenar las filas de la matriz
            
            //fwrite(&matriz[index][0], 1, 1, outputFile); //Escribo la primer fila
            for (j=0;j<768;j++){
            	fwrite(&matriz[index][j], 1, 1, outputFile);//Escribo el resto de la fila
			}
        }
        /*
        for ( i = 1; i < 20; i++) //Quitando bordes
        {
            for (j=1;j<50;j++){ //Quitando bordes
            	printf("%d",matriz[i][j]);
			}
			printf("\n");
        }
        printf("%d",matriz[0][0]);
        int kernel[3][3]= { { -1, -1, -1 }, { -1, 8, -1 }, {-1,-1,-1} };
        
        for ( i = 1; i < height-1; i++) //Quitando bordes
        {
            for (j=1;j<767;j++){ //Quitando bordes
            	matriz[i][j]= ( (matriz[i-1][j-1]*kernel[0][0]) + (matriz[i-1][j]*kernel[0][1]) + (matriz[i-1][j+1]*kernel[0][2]) + (matriz[i][j-1]*kernel[1][0]) + (matriz[i][j]*kernel[1][1]) 
				+ (matriz[i][j+1]*kernel[1][2]) + (matriz[i+1][j-1]*kernel[2][0]) + (matriz[i+1][j]*kernel[2][1]) + (matriz[i+1][j+1]*kernel[2][2]) ); //Leyendo 
			}
        }
        printf("%d",matriz[0][0]);
        /*
		for ( i = 0; i < height; i++)
        {
            int pixelOffset=((height-i)-1)*unpaddedRowSize; //Posicion de primer byte de ultima fila
            index-=1; //Indice que se comporta de forma adecuada respecto el ciclo para llenar las filas de la matriz
            fwrite(&matriz[index][0], 1, 1, outputFile); //Escribo la primer fila
            for (j=1;j<768;j++){
            	fwrite(&matriz[index][j], 1, 1, outputFile);//Escribo el resto de la fila
			}
        }*/
        fclose(outputFile);
}
 
int main()
{
        byte *pixels;
        int32 width;
        int32 height;
        int32 bytesPerPixel;
        ReadImage("img1.bmp", &pixels, &width, &height,&bytesPerPixel);
        WriteImage("img2.bmp", pixels, width, height, bytesPerPixel);
        free(pixels);
        return 0;
}
