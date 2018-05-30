#include "common.h"
#include "BL144002.h"
#include "LCD.h"
#include "ASCII.h"  

u8  images[60][80]={0};

void LCD_Init(u16 colour)
{
    Site_type site={0,0};
    Size_type size={LCD_W,LCD_H};
        lcd_initialize();                       //��ʼ��L
    LCD_Rectangle(site,size,colour);        //��ʼ������
}


/******************************************************
 * ��������LCD_Rectangle
 * ����  ����ָ�����괦��ʾָ������ľ���
 * ����  : -x1      ���������� 0~319
           -y1      ���������� 0~239
           -len     ���򳤶� 0~319
           -wid     ������ 0~239
           -rgb565  �����ɫ
 * ���  ����
 * ����  ��    LCD_Rectangle(10,10,120,160,0);
            LCD_Rectangle(200,200,120,40,0);
 * ע��  ��    �Ѳ���
*********************************************************/    
void LCD_Rectangle(Site_type site,Size_type size,u16 rgb565)
{                    
    u32 n, temp;
    
    LCD_PTLON(site,size);               //����
    
    temp = (u32)size.W * size.H;  
    LCD_RAMWR();                        //д�ڴ�
    for(n=0; n<temp; n++)
    {
        LCD_WR_16Data( rgb565 );        //д��λ
    } 
}



/******************************************************
 * ��������LCD_Point
 * ����  ����ָ�����괦��ʾһ����
 * ����  : -x ������ʾλ�� 0~319
           -y ������ʾλ�� 0~239
 * ���  ����
 * ����  ��    LCD_Point(100,200);
            LCD_Point(10,200);
            LCD_Point(300,220);
 * ע��  ��    (0,0)λ��ΪҺ�������Ͻ� �Ѳ���
*********************************************************/    
void LCD_Point(Site_type site,u16 rgb565)
{
    Size_type size={1,1};
        LCD_PTLON(site,size);   
    LCD_RAMWR();                    //д�ڴ�
    LCD_WR_16Data(rgb565);          //д����
} 
void LCD_A_Point(u8 sit,u8 sie,u16 rgb565)
{
    Site_type site={sit,sie};
        Size_type size={1,1};
        LCD_PTLON(site,size);   
    LCD_RAMWR();                    //д�ڴ�
    LCD_WR_16Data(rgb565);          //д����
} 
/************************************************************
 * ��������LCD_Char
 * ����  ����Һ������(x,y)��,��ʾһ��8*16��С���ַ�
 * ����  :    -x 0~(239-6)
 *          -y 0~(319-12)
 *          -acsii    Ҫд����ַ�
 *          -Color    �ַ���ɫ
 *          -bkColor  ������ɫ
 * ���  ����
 * ����  ����
 * ע��  ���ַ�����ʾ����Ϊ�� �Ѳ���
******************************************************************/    
u8 LCD_Char(Site_type site,const u8 ascii, u16 Color, u16 bkColor)
{       
#define MAX_CHAR_POSX (LCD_W-8)
#define MAX_CHAR_POSY (LCD_H-16)
    
    u8 temp, t, pos;   
    Size_type size={8,16};
    
    if(site.x > MAX_CHAR_POSX || site.y > MAX_CHAR_POSY)
    {
        return 0;
    }
       
    LCD_PTLON(site,size); 
    
    LCD_RAMWR();                    //д�ڴ�
    
    for (pos=0;pos<16;pos++)
    {
        temp=ascii_8x16[((ascii-0x20)*16)+pos];
        
        for(t=0; t<8; t++)
        {
            
            if(temp & 0x80)              
            {
                LCD_WR_16Data(Color);
            }
            else    
            {
                LCD_WR_16Data(bkColor);  
            }            
            temp <<= 1; 
        }
    }
    return 1;
#undef MAX_CHAR_POSX
#undef MAX_CHAR_POSY   
}


/*����*/
void LCD_Str_8x16(Site_type site,u8 *Str, u16 Color, u16 bkColor)
{ 
#define MAX_CHAR_POSX (LCD_W-8)
#define MAX_CHAR_POSY (LCD_H-16) 
    while(*Str != '\0')
    {       
        if(site.x > MAX_CHAR_POSX ) 
        {   
                    //����
           site.x = 1;
           //site.y += 16;
        }
        if(site.y > MAX_CHAR_POSY ) 
        {   
                    //һ��
           site.y = 1;
           //site.x = 0;   
        }
        
        LCD_Char(site, *Str,Color, bkColor);
        site.x += 8;
        Str ++ ;
    }
#undef MAX_CHAR_POSX
#undef MAX_CHAR_POSY   
}

/*����Ϊx,y����*/
void LCD_Str_8x16_X(u8 sit,u8 sie,u8 *Str, u16 Color, u16 bkColor)
{ 
    Site_type size={sit,sie};
    LCD_Str_8x16(size,Str,Color,bkColor);           //ʵ�ʲɼ�����
}




/************************************************************
 * ��������LCD_Str_R
 * ����  ����Һ������(x,y)��,��ʾһ��16*8��С���ַ���,�Ҷ���
 * ����  : -x 0~(239-6)
 *         -y 0~(319-12)
 *         -str �ַ�����ַ
           -len �ַ�������
           -strColor �ַ���ɫ
           -bkColor  ������ɫ
 * ���  ����
 * ����  ��LCD_Str_R(304,59,��12345��,14,0x0000,0xffff)
 * ע��  ���ַ�����ʾ����Ϊ�� �Ѳ���
******************************************************************/    
void LCD_Str_R(Site_type site,const u8 *str,u16 Color,u16 bkColor)    //�Ҷ��룬����35λ������ʾ���35λ
{
    u16 i=1,strlen=0;   
    Site_type sitetemp;
    sitetemp.y=site.y;
    
    while(str[strlen] != 0 )strlen++; 
    while(str[strlen-i])
    {
        sitetemp.x=site.x-8*i;
        if ( LCD_Char(sitetemp,str[strlen-i],Color,bkColor) ==0 ) return;  //��ʾ�����߽磬ֱ�ӷ���
        i++;
        if( i>strlen)return;
  }   
}




void LCD_Num_8x16(Site_type site,u32 num, u16 Color, u16 bkColor)     
{      
    u32 res=num;       
    u8 t=0;
    Site_type sitetemp;
    sitetemp.y=site.y;
    
    if( num == 0 )
    {
        LCD_Char(site,'0', Color, bkColor);
        return;
    }   
    while( res )  /*�õ����ֳ���t*/
    {
        res/=10;
        t++;
    }
    
    while(num)
    {
        sitetemp.x=site.x+(8*(t--)-8);
        LCD_Char(sitetemp,(num%10)+'0', Color, bkColor);
        num /= 10 ;
    }                
}


void LCD_Num_8x16_C(Site_type site,u32 num, u16 Color, u16 bkColor)   
{      
    u16 res=num;       
    u8 t=0;
    Site_type sitetemp;
    Size_type size;
        
    sitetemp.y=site.y;
    
    if( num == 0 )
    {
        LCD_Char(site,'0', Color, bkColor);
        
        site.x += 8;
        size.H  = 16;
        size.W  = 8*(MAX_NUM_BIT-1);
        LCD_Rectangle(site,size,bkColor);

        return;
    }   
    while( res )            /*�õ����ֳ���t*/
    {
        res/=10;
        t++;
    }
    if(t >= MAX_NUM_BIT )    //������󳤶�
    {
        t = MAX_NUM_BIT;
    }
    
    res = t;  
    
    while( t !=0 )
    {
        sitetemp.x=site.x+(8*(t--)-8);
        LCD_Char(sitetemp,(num%10)+'0', Color, bkColor);
        num /= 10 ;
    }
    
    if(res != MAX_NUM_BIT )
    {
        size.W = 8*(MAX_NUM_BIT - res);
        site.x += (8*res);
        size.H  = 16;
        LCD_Rectangle(site,size,bkColor);
    }
}

void LCD_Num_8x16_X(u8 sit,u8 sie,u32 num, u16 Color, u16 bkColor)    
{
        Site_type size={sit,sie};
        LCD_Num_8x16_C(size,num,Color,bkColor);           //ʵ�ʲɼ�����
}

void LCD_Img_RGB565 (Site_type site,Size_type size,u16 *img)
{
    u32 n, temp;
    
    LCD_PTLON(site,size);               //����
    temp = (u32)size.W * size.H;  
    LCD_RAMWR();                        //д�ڴ�
    for(n=0; n<temp; n++)
    {
        LCD_WR_16Data_BIG( *img );      
        img++;
    }
}

void LCD_Img_RGB565_Z(Site_type site,Size_type size,u16 *img,Size_type imgsize)
{
    
    u16 x,y;
    u16 X,Y;
    u32 temp, tempY;
    LCD_PTLON(site,size);                       //����
    
    //temp = (u32)size.W * size.H;  
    LCD_RAMWR();                                //д�ڴ�
    for( y=0; y < (size.H) ; y++  )             //�ڿ������������ɨ��
    {
        Y=( (y * imgsize.H + (size.H>>1)-1)/size.H);    //�����img��ĸ߶ȣ�+ size.H>>1����������
        tempY = Y * imgsize.W ;
        for( x=0; x< size.W ; x++  )
        {
            X =( (x * imgsize.W + (size.W>>1)-1)/size.W);
            temp = tempY + X;

#if 1
            LCD_WR_Data( (u8 ) ( img [temp]) )     ;    //����ͷ���ȸ�λ���λ
            LCD_WR_Data(  (u8 ) (img  [temp+1]))  ;
#elif   1
            
            LCD_WR_Data( ( (u8 *) img) [temp] )     ;   //��֣�����ͷ���������ʾ����
            LCD_WR_Data(  ((u8 *) img ) [temp+1])  ;
#else
            LCD_WR_16Data_BIG( img[temp] );     //дͼ������
#endif
        }
    }
}

void LCD_Img_Binary_Z(Site_type site,Size_type size,u16 *img,Size_type imgsize)
{
    u16 x,y;
    u16 X,Y;
    u32 temp, tempY;
    //u16 tempflag;
    u8 * pimg = (u8 *)img;
    u16     bitflag;
    LCD_PTLON(site,size);                       //����
//  const u16 bitnums[]={B16(10000000,00000000),B16(11000000,00000000),B16(11100000,00000000),B16(11110000,00000000),
//                         B16(11111000,00000000),B16(11111100,00000000),B16(11111110,00000000),B16(11111111,00000000)};
    const u8 bitnums[]={B8(10000000),B8(11000000),B8(11100000),B8(11110000),
                         B8(11111000),B8(11111100),B8(11111110),B8(11111111)};
    
    //const u16 bitnums_2[]={B8(00000001),B8(00000011),B8(00000111),B8(00001111),B8(00011111),B8(00111111),B8(01111111),B8(11111111)};
    

    
    LCD_RAMWR();                                //д�ڴ�
    
    bitflag = imgsize.W/size.W;
    ASSERT(bitflag > 0);
    //tempflag = bitnums_2[bitflag-1];
    bitflag = bitnums[bitflag-1];
    
    for( y=0; y < (size.H) ; y++  )             //�ڿ������������ɨ��
    {
        Y=( (y * imgsize.H + (size.H>>1)-1)/size.H);    //�����img��ĸ߶ȣ�+ size.H>>1����������
        tempY = Y * imgsize.W ;
        for( x=0; x< size.W ; x++  )
        {
            X =( (x * imgsize.W + (size.W>>1)-1)/size.W);
            temp = tempY + X;
            
            if( (pimg[temp>>3] & (bitflag >> (temp & 0x07)) ) == (bitflag >> (temp & 0x07)) )       //1Ϊ�׵㣬���ͱ�����ɫ
            {
                LCD_WR_16Data_BIG( BINARY_BGCOLOR );        //дͼ������
            }
            else
            {
                LCD_WR_16Data_BIG( BINARY_COLOR );      //дͼ������
            }
        }
    }
    
}

/******************************************************
 * ��������LCD_Img_Binary
 * ����  ����ӡ������ͼ��
 * ����  : ��ѹǰͼ��
 * ���  ��void
 * ע��  ��    ��������
*********************************************************/  
void LCD_Img_Binary(u8 *img)
{
    Site_type site={127-80,0};                      //��ʾͼ�����Ͻ�λ��
    Size_type size={80,60};                 //��ʾ����ͼ���С
        
    u8    imgtemp;
    u8    bitindex;
    u8  * pimg=(u8 *)img;
    u8    i,j;
    //imgtemp     = *(pimg++);
    
    LCD_PTLON(site,size);                       //����    
    LCD_RAMWR();                                //д�ڴ�
    for(i=59;i>0;i--)
    {
        for(j=0;j<10;j++)
        {
            imgtemp     = *(pimg++);
            bitindex    = 8;
            while(bitindex--)
            {
                if( imgtemp & (0x01<<bitindex) )
                {
                    images[i][j*8+7-bitindex]=1;            //дͼ������BLACK
                }
                else
                {
                    images[i][j*8+7-bitindex]=0;            //дͼ������WHITE
                }
            }
        }
    }
/*****************************/        
    for(i=59;i>0;i--)
    {
        for(j=0;j<80;j++)
        {
            if(images[i][j]==1)
            {
                LCD_WR_16Data(BLACK);       //дͼ������
            }else
            {
                LCD_WR_16Data(WHITE);       //дͼ������
            }  
            BL14002_DELAY();
        }
    }
/******************************/
}


/******************************************************
 * ��������LCD_Img
 * ����  ����ӡ�����ͼ������������Ԫ��ֵ����ͬ���������ò�ͬ��ɫ��
 * ����  : ��ѹ��ͼ������
 * ���  ��void
 * ע��  ��    ��������
*********************************************************/  
void LCD_Img(u8 img[80][60]){
    for(int j = 0;j < 59;j++){
        for(int i = 0;i < 79;i++){
            u16 color = WHITE;
            switch(img[i][j]){
                case 1:  color = BLACK; break;      //��ʾͼ��ڵĵط�
                case 2:  color = RED;   break;      //��ʾ�߽�
                case 3:  color = BLUE;  break;      //��ʾ������
                case 4:  color = GREEN; break;      //��ʾŨ����ĵ�
                default: color = WHITE; break;      //��ʾͼ��׵ĵط�
            }
            LCD_A_Point(i + 128 - 80, j, color);
        }
    }
}


/******************************************************
 * ��������LCD_Clear
 * ����  ��ˢ��lcd
 * ����  : ��ɫ
 * ���  ��void
 * ע��  ��    ��������
*********************************************************/  
void LCD_Clear(u16 colour)
{
    Site_type site={0,0};
    Size_type size={LCD_W,LCD_H};
    LCD_Rectangle(site,size,colour);        //��ʼ������
}