#include <stdio.h>   /* Стандартные объявления ввода/вывода */
#include <string.h>  /* Объявления строковых функций */
#include <unistd.h>  /* Объявления стандартных функций UNIX */
#include <fcntl.h>   /* Объявления управления файлами */
#include <errno.h>   /* Объявления кодов ошибок */
#include <termios.h> /* Объявления управления POSIX-терминалом */
#include <unistd.h>
//#include <QtWidgets>

//for TIMEUTS
#define UTC_NANO_4b 8
#define UTC_YEAR_2b 12
#define UTC_MONTH 14
#define UTC_DAY 15
#define UTC_HOUR 16
#define UTC_MINUTE 17
#define UTC_SEC 18

// for NAV_Pvt
#define LON_4b 24
#define LAT_4b 28

//NAV-POSECEF ECEF X coordinate
#define ecefX 4//ECEF X coordinate
#define ecefY 8//ECEF Y coordinate
#define ecefZ 12//ECEF Z coordinate

//RXM-SFRBX
#define svid 1 //Satellite identifier
#define numWords 4//



int main()
{
   // QApplication app(argc, argv);//GUI
   // QLabel    l1("QLabel-l1");
int MESS_POINTER=0;

    int fd;

    fd=open("/dev/ttyACM0",O_RDWR | O_NOCTTY | O_NDELAY);//создание файлового десриптора
    if (fd == -1)
    {
        perror("open_port: Unable to open /dev/ttyACM0 - ");
    }
    else fcntl (fd, F_SETFL, 0);

    fcntl(fd,F_SETFL);


    for(;;)
    {
        
        read(fd,&MESS_POINTER,1);






     if(MESS_POINTER==181)//Begin message. 1 byte
      {  //printf("\n UBX");//out 1 byte
     //move 2 byte
        read(fd,&MESS_POINTER,1);//read byte
       if(MESS_POINTER==98)
       {
          printf(" MES");//output 2 byte

           //create length var
           int MESLEN=0, MESLEN2=0;
           printf(" CLASS(");
           int CLASS=0,ID=0,CLID=0;//class & id for parsing
            
		 read(fd,&MESS_POINTER,1);//read byte 3
		 CLASS=MESS_POINTER;
            	 printf("0x%X ",MESS_POINTER);//output 3 byte
             read(fd,&MESS_POINTER,1);//read byte 4
             ID=MESS_POINTER;
            printf("0x%X",MESS_POINTER);//output 4 byte
        printf(")");

         CLID=CLASS*1000+ID;

         read(fd,&MESLEN,1);//read byte 5
         printf("%X ",MESLEN);
         read(fd,&MESLEN2,1);//read byte 6
         printf("%X",MESLEN2);
          MESLEN=MESLEN+MESLEN2;//message length
  // printf(" LEN(%i)",MESLEN);//print lenght


          int* pmes=new int(MESLEN);//payload message


          for(int i=0;i<MESLEN;i++)//payload read and print
            {
                   pmes[i]=0;
                read(fd,&pmes[i],1);//read byte in payload
            //    printf("%X ",pmes[i]);//output byte



            }


         switch (CLID)
         {
         case 1033:
         {
             printf("\nNAV-TIMEUTC ");
             printf(" year %i",pmes[UTC_YEAR_2b]+(pmes[UTC_YEAR_2b+1]<<8));
             printf(" month %i",pmes[UTC_MONTH]);
             printf(" day %i",pmes[UTC_DAY]);
             printf(" hour %i",pmes[UTC_HOUR]);
             printf(" minute %i",pmes[UTC_MINUTE]);
             printf(" nanosec %i",pmes[UTC_NANO_4b]+(pmes[UTC_NANO_4b+1]<<8)+(pmes[UTC_NANO_4b+2]<<16)+(pmes[UTC_NANO_4b+3]<<24));


             break;
         }
         case 1007:
         {
         printf("\nNAV-PVT ");
  printf("\n longitude %i",pmes[LON_4b]+(pmes[LON_4b+1]<<8)+(pmes[LON_4b+2]<<16)+(pmes[LON_4b+3]<<24));
  printf("\n latitude %i",pmes[LAT_4b]+(pmes[LAT_4b+1]<<8)+(pmes[LAT_4b+2]<<16)+(pmes[LAT_4b+3]<<24));

         break;
         }

         case 2021:
         {
         printf("\nRXM-RAWX ");
int numMeas=0;
numMeas=int((MESLEN-16)/32);

if(numMeas==pmes[11])
    printf("right");
else  printf("right %i <- %i",pmes[11], numMeas);

printf("lenght message %i\n",MESLEN);
for(int d=0; d<=numMeas;d++)
{
    printf("sat id %i\n",pmes[37+32*d]);
    printf("pseudorange %i\n",pmes[16+32*d]);
    printf("phase measure %i\n",pmes[24+32*d]);


}


         break;
         }
         case 1053:
         {
         printf("\nNAV-SAT ");
int numSvs=0;
numSvs=int((MESLEN-8)/12);
if(numSvs==pmes[5])
    printf(",my solve right");
else  printf("right %i <- %i",pmes[5], numSvs);

printf("lenght message %i\n",MESLEN);
for(int d=0; d<=numSvs;d++)
{
    printf("sat id %i\n",pmes[9+12*d]);
    printf("GNSS sat %i\n",pmes[8+32*d]);
    printf("POSITION SATELLITE\n");
    printf("Elevet %i\n",pmes[11+12*d]);
    printf("azimut %i\n",pmes[12+12*d]);
    printf("psr sat %i\n",pmes[14+12*d]);


}


         break;
         }

         case 1001:
         {
             printf("NAV-POSECEF");
             printf("\n ECEF X %i",pmes[ecefX]+(pmes[ecefX+1]<<8)+(pmes[ecefX+2]<<16)+(pmes[ecefX+3]<<24));
             printf("\n ECEF Y %i",pmes[ecefY]+(pmes[ecefY+1]<<8)+(pmes[ecefY+2]<<16)+(pmes[ecefY+3]<<24));
             printf("\n ECEF Z %i",pmes[ecefZ]+(pmes[ecefZ+1]<<8)+(pmes[ecefZ+2]<<16)+(pmes[ecefZ+3]<<24));


             break;
         }

         case 2019:
         {
             printf("\nRXM-SFRBX\n");
             printf("satellite number %i",pmes[svid]);

             for(int k=0;k<pmes[numWords];k++)
             printf("\n his data %X %X %X %X",pmes[8 + 4*(k+3)],pmes[8 + 4*(k+2)],pmes[8 + 4*(k+1)],pmes[8 + 4*(k+0)]);



             break;
         }

                             default:
         {
             printf("-");

         }


                             }

         delete pmes;

//-------------------------------------------


         // int *pd=new int(MESLEN);//create dynamic payload


          int a=0,b=0;
           read(fd,&a,1);//read byte payload
           read(fd,&b,1);//read byte payload
          printf("checksum(%X %X)\n",a, b);
//-------------------------------------------

       }

     }

    }


    int fcl;
    fcl=close(fd);
            if (fcl=0)
            printf("\n file no closed");

    return 1;//app.exec();
}







