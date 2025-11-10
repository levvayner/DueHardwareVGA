#include "Arduino.h"
#include "hw/video/VRAM.h"

void setup(){
    Serial.begin(115200);
    Serial.println("Started Due Hardware VGA");
    graphics.begin();
}


void scale(){
  char buf[20];
  graphics.clear();
  graphics.drawText(260,25,"Fourier series",255,1); 
  graphics.drawLine(100,300,799,300,255);
  graphics.drawText(400,340,"Time",255);
  for(int n=0;n<=100;n++){
    int l=4;
    if((n%5)==0)l+=4;
    if((n%10)==0){
      l+=4;
      sprintf(buf,"%d",n/10);
      graphics.drawText(96+n*7,315,buf,255);
    }      
    graphics.drawLine(100+n*7,300,100+n*7,300+l,255);
  }
  
  graphics.drawLine(100,0,100,599,240);
  graphics.drawText(45,330,"Amplitude",255,0,1);
    for(int n=0;n<=40;n++){
    int l=4;
    if((n%5)==0){
      l+=4;
      if((n%10)==0) l+=4;
      sprintf(buf,"%0.1f",(-n+20.0)/10.0);
      graphics.drawText(60,n*15-4,buf,254);
    }      
    graphics.drawLine(100,n*15,100-l,n*15,240);
  }
}

void loop() {
  scale();  
  graphics.drawText(300,40,"Sawtooth",255,0); 
  for(int h=1;h<19;h++){
    double ox=0,oy=0,x,y;
    for(int xi=100;xi<800;xi++){
      x=(xi-100.0)/70.0 - (h-1)/3.0;
      y=0;
      for(int i=1;i<=h;i++)y+=sin(x*i)/i;
      graphics.drawLine(xi-1,300-oy*150,xi,300-y*150,1);
      ox=x;oy=y;
    }
  }  
  graphics.setReady();
  ///delay(3000);
  
  scale(); 
  graphics.drawText(300,40,"Square",255,0); 
  for(int h=1;h<38;h+=2){
    double ox=0,oy=0,x,y;
    for(int xi=100;xi<800;xi++){
      x=(xi-100.0)/70.0 - (h-1)/6.0;
      y=0;
      for(int i=1;i<=h;i+=2)y+=sin(x*i)/i;
      graphics.drawLine(xi-1,300-oy*150,xi,300-y*150,240);
      ox=x;oy=y;
    }
  }  
  graphics.setReady();
  ///delay(3000);
  
  scale();  
  graphics.drawText(300,40,"Triangle",255,0);   
  for(int h=1;h<38;h+=2){    
    double ox=0,oy=0,x,y;
    for(int xi=100;xi<800;xi++){
      double m=1;
      x=(xi-100.0)/70.0 - (h-1)/6.0;
      y=0;
      for(int i=1;i<=h;i+=2){y+=m*sin(x*i)/(i*i);m*=-1;}
      graphics.drawLine(xi-1,300-oy*150,xi,300-y*150,240);
      ox=x;oy=y;
    }
  }  
  graphics.setReady();
  //delay(3000);
  
}