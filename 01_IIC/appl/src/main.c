#include "8051.h"
#include "8052.h"
#include "timer51.h"

__sbit __at (0xA0) SDA;	//将P2.0管脚和4C02芯片的数据线相连
__sbit __at (0xA1) SCL;	//将P2.1管脚和4C02芯片的时钟线相连

void main(void){
    while(1){
    LED1 = 1; //LED1端口设置为高电平
    delay();
    LED1 = 0; //LED1端口设置为低电平
    delay();
    }
}

 
/**
 * 适用芯片海天芯 HT24C02
 * 函数: I2C_Start()
 * 一个_nop_耗时，一个机器周期，针对89C52就是12个时钟周期，晶振为11.0592M的话
 * 一个_nop_就好是12/11.0592=1.085us,5个_nop_，就是5.43us
*/
void I2C_Start()
{
  SDA = 1;  //SDA为高电平
  _nop_();  //这个延时放着也没关系，从图中可以看出SDA比SCL先高电平
        
  SCL = 1; //把时钟线拉高
  _nop_(); //SCL拉高之后，要保持一段时间，这个高电平要持续Tsu:STA时间,至少也是4700ns
    
  SDA = 0; //把 SCL 为高电平的时候把SDA拉低
  _nop_(); //拉低之后需要维持一段时间，T_HD:STA,也是4000ns以上
    
  SCL = 0; //再把SCL拉低
}
 
/**
 * 适用芯片海天芯 HT24C02
 * 函数: I2C_Stop()
 * 功能: 停止i2c
 * 一个_nop_耗时，一个机器周期，针对89C52就是12个时钟周期，晶振为11.0592M的话
 * 一个_nop_就好是12/11.0592=1.085us,5个_nop_，就是5.43us
 *
*/
void I2C_Stop()
{
  SCL = 0; //SCL先拉低
  _nop_(); 
    
  SDA = 0; //SDA也要拉低，
  _nop_(); 
    
  SCL = 1;//SCL拉高，
  _nop_(); //SCL拉高维持时间T_SU:STO，
    
  SDA = 1; //SDA拉高
  _nop_(); //SDA拉高持续时间T_BUF
}

 
void I2C_Ack(void)
{
    SCL = 0;
    SDA = 0;
    nops(); //在第9个脉冲到来之前的低电平期间，把SDA拉低
 
    SCL = 1; //SCL高电平，表示第9个脉冲
    nops();
 
    SCL = 0 //SCL拉低，方便后续的读写
}

 
void I2C_Nack(void)
{
    SCL = 0;
    SDA = 1;
    nops(); //在第9个脉冲到来之前的低电平期间，释放SDA总线，让上拉电阻把SDA拉高
 
    SCL = 1; //SCL高电平，表示第9个脉冲
    nops();
 
    SCL = 0 //SCL拉低，方便后续的读写
}

 
unsigned char I2C_Wait_Ack(void)
{
    unsigned char ucErr = 0;
    SDA = 1;
    nops();
    SCL = 1;
    nops();
    while(SDA)  //SDA为高电平，就表示没有检查到ACK，
    {
        ucErr++;
        if(ucErr > 250) //等一段时间，还没有ack，就停止总线
        {
            I2C_Stop();
            return 1;
        }
    }
    SCL = 0;
    return 0; //能到这里就表示检测到应答信号了
}

 
void I2C_Send_Byte(unsigned char dat)
{
    unsigned char i;
    SCL = 0; //拉低时钟，准备数据
    
    for(i=0;i<8;i++)
    {
        SDA = (dat & 0x80) >> 7;//先发高位，再发低位，这个逻辑自己分析下
        dat << 1 //数据的高位发出去了，左移一位，接着发送次高位......
        nops();  //数据要保持一段时间，让SCL是高电平的时候，数据已经稳定了
        
        SCL = 1; //SCL 拉高
        nops(); //针对Microchip的24C02B，延时要4700ns以上
        scl = 0; //SCL 拉低
        nops(); //拉低也要持续一段时间
    }
    
}

 
unsigned char I2C_Read_Byte(unsigned char ack)
{
    unsigned char i;
    unsigned char receive = 0;
    
    for(i=0; i<8; i++)
    {
        SCL = 0;
        nops();
        
        SCL = 1;//时钟高电平的时候，接收的数据移位
        receive  <<= 1;
        
        if(SDA) //如果SDA是高电平，receive +1
        {
            receive++;
        }
            
        nops(); //
    }
    
    if(!ack)  //非应答信号
    {
        I2C_Nack();
    } else{ //应答信号
        I2C_Ack();
    }
    
    return receive;
}

 
//MCU 向24C02的某个寄存器写一个字节的数据
//很多人不喜欢检查从机的应答信号，其实在工程上是不规范的；
//虽然大部分情况下，不检查应答信号，也能正常使用
//就像国家免检产品一样，默认你不会出问题,一出问题就是大问题
void eeprom_Write_Byte(unsigned char addr, unsigned char dat)
{
    //1、MCU发出起始信号
    I2C_Start();
    
    //2、MCU发送从设备的地址，最后一位是写
    I2C_Send_Byte(DEV_ADDR | WR);
    
    //3、MCU检查从机的应答
    I2C_Wait_Ack();
    
    //4、MCU发出从机的寄存器地址
    I2C_Send_Byte(addr);
    
    //5、MCU检查从机的应答
    I2C_Wait_Ack();
    
    //6、MCU发出要写的数据
    I2C_Send_Byte(dat);
    
    //7、MCU检查从机的应答
    I2C_Wait_Ack();
    
    //8、MCU发出停止信号
    I2C_Stop();
}

 
//多次调用写一个字节的函数来实现写多个字节
eeprom_Write_NBytes(unsigned char addr, *pdata, size)
{
    unsigned char i ;
    for(i=0;i<size;i++)
    {
        eeprom_Write_Byte(addr, pdata[i]);
        addr++; //寄存器地址自动增加
    }
}


 
//读24C02一个字节
void eeprom_Read_Byte(unsigned char addr, *pdata)
{
    //1、MCU发出起始信号
    I2C_Start();
    
    //2、MCU发送从设备的写地址
    I2C_Send_Byte(DEV_ADDR | WR);
    
    //3、MCU检查从机的应答
    I2C_Wait_Ack();
    
    //4、MCU发出从机的寄存器地址
    I2C_Send_Byte(addr);
    
    //5、MCU检查从机的应答
    I2C_Wait_Ack();
    
    //6、MCU再次发出开始信号
    I2C_Start();
    //7、MCU发送从机的读地址
    I2C_Send_Byte(DEV_ADDR | RD);
    
    //8、MCU检测从机应答
    I2C_Wait_Ack();
    
    //9、从机返回寄存器的数据
    //10、MCU发出非应答NACK信号
    *pdata = I2C_Read_Byte(NACK);
    
    //11、MCU发出结束信号
    I2C_Stop();
}

 
//读eeprom多个字节
//就是把读一个字节重复几遍
void eeprom_Read_NBytes(unsigned char addr, *pdata, size)
{
    unsigned char i;
    for(i=0;i<size;i++)
    {
        eeprom_Read_Byte(addr, &pdata[i]);
        addr++;
    }
}
