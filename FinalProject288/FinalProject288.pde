import processing.serial.*;
import java.util.ArrayList;
import processing.net.*; 

/**
* A timer for different actions
*/

Client myClient;
/**
* incoming serial data
*/
int serialData[];

String string = "no data";

ArrayList<block> blockList;

char currentKey = 0;
void setup(){

  size(800, 800);
  background(#044f6f);
  myClient = new Client(this, "192.168.1.1", 288);
  serialData = new int[20];
  clearSerialData();
  blockList = new ArrayList();
  shapeMode(CENTER);
  textMode(CENTER);
}

/**
* Controls: 
* w move forward 30 cm
* a turn left 10 degrees
* d turn right 10 degrees
* s move backwards 10 centimeters
* f sweep the turret
* q turn left 45 degrees
* e turn right 45 degrees
* r turn 180 degrees
* m play music
*/ 
void draw(){
  background(#044f6f);
  fill(255);
  ellipse(width/2, height/2, 16*6, 16*6);
  fill(100);
  arc(width/2, height/2, 16*6, 16*6, PI, TWO_PI);
  drawBlocks();
  
  //send data to the bot when a key is pressed
  delay(200);
  if(keyPressed){
    currentKey = key;
    myClient.write(key);
    if(key == 'a'){
      rotateBlocksRight(10);
    }
    if(key == 'd'){
      rotateBlocksLeft(10);
    }
    if(key == 's'){
      moveBlocks(-10);
    }
    if(key =='q'){
      rotateBlocksRight(45);
    }
    if(key == 'e'){
      rotateBlocksLeft(45);
    }
    if(key == 'r'){
      rotateBlocksLeft(180);
    }
    
  }
  
  //delete objects that are clicked on
  if(mousePressed){
    
  }
  
  
  
  //read and proccess incoming data
  if(myClient.available() > 1){
    //read the serial until the e
    string = myClient.readStringUntil('e');
    //parse the serial data
    readSerial();
    //make new blocks
    creatBlocks();
    //print incoming string
    println(string);
    //print serialData
    for(int i = 0; i < serialData.length; i++){
      println(serialData[i]);
    }
    println();
    
    //something was hit, don't move the blocks
    if(serialData[0] == -1){
      moveBlocks(30);
      currentKey = 0;
    }
    if(serialData[0] == 9){
      currentKey = 'o';
    }
  }
  
  if(currentKey == 'f'){
    text("SCANNING", width/2, (height/2) + 150);
  }
  
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
void readSerial() {
  //remove the e
  String dataIn = string.substring(0, string.length() - 1); 
  
  try{
    String[] split;
    split = dataIn.split(",");
    for(int i = 0; i < serialData.length && i < split.length; i++){
      serialData[i] = parseInt(split[i]); 
    }
  }
  catch(Exception e){println("something went wrong trying to read the serial data");}
}
/**
* clears all serial data
*/

void clearSerialData(){
  for(int i = 0; i < serialData.length; i++){
    serialData[i] = 0;
  }
}

/**
* draws all the blocks
*/
void drawBlocks(){
  //check to see if the blocks are out of the screen
  blockOutOfScreen();
  //draw the blocks
  for(int i = 0; i < blockList.size(); i++){
    blockList.get(i).drawUI();
  }
}

/**
* for manually removing blocks
*/
void removeBlock(){
  if(blockList.size() > 0){
    blockList.remove(0);
  }
}
/**
* removes the blocks if they are out of screen
*/
void blockOutOfScreen(){
  for(int i = 0; i < blockList.size(); i++){
    if(abs((blockList.get(i).getX() * 6)) > 400){
      blockList.remove(i);
    }
    else if(abs((blockList.get(i).getY() * 6)) > 400){
      blockList.remove(i);
    }
  }
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
void creatBlocks(){
  int type = serialData[0];
  float y = (float) serialData[1];
  y = (y/10);
  float x = 5;
  if(type == 1){
    blockList.add(new block(-x, y, type));
  }
  else if(type == 2){
    blockList.add(new block(x, y, type));
  }
  else if(type == 3){
    blockList.add(new block(0, y, type));
  }
  else if(type == 4){
    blockList.add(new block(x, y, type));
  }
  else if(type == 5){
    blockList.add(new block(x, y, type));
  }
  else if(type == 6){
    blockList.add(new block(0, y, type));
  }
  else if(type == 7){
    blockList.add(new block(x, y, type));
  }
  else if(type == 8){
    blockList.add(new block(-x, y, type));
  }
  //if it is incoming scanner data
  else if(type == 9){
    for(int i = 0; i < serialData[1]; i++){
      //make a small block
      if(serialData[2+i*3] == 0){
        blockList.add(new block(serialData[3+i*3], serialData[4+i*3], 9));
      }
      //make a large block
      else{
        blockList.add(new block(serialData[3+i*3], serialData[4+i*3], 10));
      }
    }
  }
}

void rotateBlocksRight(int deg){
  for(int i = 0; i < blockList.size(); i++){
    blockList.get(i).rotateRight(deg);
  }
}

void rotateBlocksLeft(int deg){
  for(int i = 0; i < blockList.size(); i++){
    blockList.get(i).rotateLeft(deg);
  }
}
void moveBlocks(int d){
  for(int i = 0; i < blockList.size(); i++){
    blockList.get(i).move(d);
  }
}
