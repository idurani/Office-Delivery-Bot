class block{
  
  float x, y;
  int type;
  int w;
  
  public block(float x, float y, int type){
    this.x = x;
    this.y = y;
    this.type = type;
    w = 6;
  }
  
  public block(float x, float y, int type, int w){
    this.x = x;
    this.y = y;
    this.type = type;
    this.w = w;
  }
  
  /**
* dataType: 
* nothing hit -1
* bumpLeft 1
* bumpRight 2
* cliffFront ect 3
* cliffRight 4
* cliffLeft 5
* edgeFront 6
* edgeRight 7
* edgeLeft 8
* scannedData 9
* (dataType),data,data,data(followed by the char e)
*/
  public void drawUI(){
    shapeMode(CENTER);
    float edgeSize = 4;
    float bumperSize = 6.5;
    if(type == 1 || type == 2){
      fill(255);
      arc((width/2) + (x*6), (height/2) - (y*6), bumperSize*6, bumperSize*6, PI, TWO_PI);
    }
    else if(type == 3 || type == 4 || type == 5){
      fill(0);
      rect((width/2) + (x*6), (height/2) - (y*6), edgeSize*6, edgeSize*6);
    }
    else if(type == 6 || type == 7 || type == 8){
      fill(255);
      rect((width/2) + (x*6), (height/2) - (y*6), edgeSize*6, edgeSize*6);
    }
    else if(type == 9){
      fill(0);
      ellipse((width/2) + (x*6), (height/2) - (y*6), w*6, w*6);
    }
    else if(type == 10){
      fill(255);
      ellipse((width/2) + (x*6), (height/2) - (y*6), w*6, w*6);
    }
    
  }
  
  public void updateCords(float x, float y){
    this.x = x;
    this.y = y;
  }
  
  public void rotateLeft(int deg){
    float r = sqrt(pow(x, 2) + pow(y, 2));
    float theta = atan(y/x);;
    if((x > 0) && (y > 0)){
      theta = atan(y/x);
    }
    else if((x < 0) && (y > 0)){
      theta = PI + atan(y/x);
    }
    else if((x < 0) && (y < 0)){
      theta = PI + atan(y/x);
    }
    else if((x > 0) && (y < 0)){
      theta = TWO_PI + atan(y/x);
    }
    
    theta = theta + (deg*(PI/180));
    x = r*cos(theta);
    y = r*sin(theta);
  }
  
  public void rotateRight(int deg){
    float r = sqrt(pow(x, 2) + pow(y, 2));
    float theta = atan(y/x);;
    if((x > 0) && (y > 0)){
      theta = atan(y/x);
    }
    else if((x < 0) && (y > 0)){
      theta = PI + atan(y/x);
    }
    else if((x < 0) && (y < 0)){
      theta = PI + atan(y/x);
    }
    else if((x > 0) && (y < 0)){
      theta = TWO_PI + atan(y/x);
    }
    
    theta = theta - (deg*(PI/180));
    x = r*cos(theta);
    y = r*sin(theta);
  }
  
  public void move(int d){
    y = y - d;
  }
  
  float getX(){
    return x;
  }
  
  float getY(){
    return y;
  }
  
  int getType(){
    return type;
  }
  
}
