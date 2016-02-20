/*
 Detects Secret Ring Pattern and Triggers Diode if pattern is correct
 
 
 //slight changes in this version
 
 arrays werden nciht uebergeben, da global ausserdem ist das array nun eindimensional
 defines statt consts...
 Statt einem Button wird nun die Klingelschaltung als Klingelinput verwendet mit threshold
 die haustuer wird ueber eine funktion geoeffnet
 die wohnungstuer wird mit den servos geoeffnet
 
 
	by Tobias Hoelzer
	Version 0.1.1
`


	Digital Pin 10 	Ringing Bell Diode Input for Ringing the Bell
        Digital Pin 04 Program Pin to Programm a new Pattern
        Digital Pin 09 Pin to show current pattern
        
	Digital Pin 13	Diode Signaling Ring
	Digital Pin 12 Diode Green to show if pattern is correct
	Digital Pin 08 Diode Red to show if pattern is incorrect or to signal programming mode
        
//WORKING



 */
 
 
 //SERVO
 
 //includes
 
 #include <Servo.h>
 
 
 
 //create servo objects to control them
 Servo servooben;
 Servo servounten;
 
 //variable to store servo position
 
 int servoposition=0;
 
 //constants to set the servo pins
 
#define ServoObenPin     5
#define ServoUntenPin    6
 
 
// constants used here to set pin numbers:



#define RingPin      A0            // the number of RingPin
#define ledPin       13           // the number of the LED pin
#define ProgramPin   11          //the number of the ProgrammButtonPin
#define greenledPin  7        //the number of the green LED pin	
#define redledPin    4	      //the number of the red LED pin
#define ShowPattern  8        // the number Showpattern pin to show the pattern
#define relaisPin    10      //the number of the Pin that controls the relais for the main door


//#define NumberofRings 5
const int NumberofRings=5;	//max number of Rings possible
//const does not work properly with no apparent reason
//reason was that it was used in function definition blinkpattern(int blabla[NumberofRings*2]){...
//dont know why not working




//other constants 

const int ringfinished=3000;	 //time available to ring again
const int Sensibility=10;        //Sensibility to the right timing
const float threshold=1.00f;        //Threshold for the incoming Ring in Volts





// variables will change:


boolean RingState =        false;                               // variable for reading the ringing status, is the ringbutton currently pressed?
float   RingStateVolts=    0.00f;                              //variable for reading the ringin Voltage
boolean ProgramState=      false;                             //variable for reading the Progam Status.Programm mode on?
boolean ShowPatternState=  false;                         //variable for the showpatternbutton, Show Pattern if Pressed

int RingArray[NumberofRings*2];                            //variable Array for the incoming ring
int MasterArray[NumberofRings*2];                          //variable array for the secret pattern


boolean Codechecker=false;                              //variable for code is correct/incorrect







void setup() {
  
          //Input Pins
  
          pinMode(RingPin, INPUT);      // initialize the pushbutton pin as an input: 
          pinMode(ProgramPin, INPUT);  //initialize the Program pin as an input
          pinMode(ShowPattern,INPUT);  //initialize the Show Pattern pin as an input     
          
          //Output Pins
  
          pinMode(ledPin, OUTPUT);      // initialize the LED pin as an output: 
          pinMode(greenledPin, OUTPUT); // initialize the  green LED pin as an output:
          pinMode(redledPin, OUTPUT);   // initialize the red LED pin as an output:
          pinMode(relaisPin, OUTPUT); //initialize the Relais Pin as an output;
          
     
          Serial.begin(9600);          // initialize serial communications at 9600 bps:  
}



void loop(){
  

                          //first, check all the input pins
                          
                        RingState=MeasureRingState();    //checks if the voltage is above threshold,if so it is considered on        
                        
                        ProgramState=digitalRead(ProgramPin);          //read the state of the Programpin value
                        
                        ShowPatternState=digitalRead(ShowPattern);    //read the state of the showpatternpin value
                        
                        
                        //if Show PatternPin is active, show pattern
                        
                        if(ShowPatternState){
                          
                                                 Blink(MasterArray); 
                         
                        }//endofif
                        
                        
                        
                        
                        //if Programstate Pin is active signal it by red LED meaning in Programming Mode, otherwise not 
                        
                        if(ProgramState){
                          
                                                  digitalWrite(redledPin,HIGH);   
                          
                        } else {
                          
                                                   digitalWrite(redledPin,LOW);
                          
                        }//endofif
                        
                        
                        
                        
                        
                      
                        // check if there is a Ring
                        // if it is, the RingState is HIGH:
                                                
                        
                        if (RingState) {     
                          
                                          
                                           ListenToRing();                         //listen to the ringings
                                           MapArray();                            //and map them
                                           
                                           
                                           //now check if program state to save new pattern or usual state to open the door
                                           
                                           if(ProgramState){              //if in PROGRAM STATE
                                           
                                           
                                                              ChangeMasterArray();    // write current pattern into the master Array
                                                              Blink(MasterArray);                          // and show the new pattern
                                              
                                              
                                              
                                           }else{                        //if in USUAL STATE 
                       
                       
                                                          Codechecker=CheckSecretCode(RingArray,MasterArray);  //check if the current pattern matches the Secret Pattern
                                                          
                                                          delay(500);                                          //Wait 500 millis to increase tension
                                                                
                                                       
                                                       
                                                        
                                                        
                                                       
                                                               //now check if the pattern was right
                                                               
                                                          if(Codechecker){  //if code was right blink the green led and reset the codecheck variable to default false;
                                                            
                                                                            digitalWrite(greenledPin,HIGH);
                                                                            
                                                                            delay(100);
                                                                            
                                                                            digitalWrite(greenledPin,LOW);
                                                                            
                                                                            //open the doors
                                                                            openmaindoor();
                                                                            delay(5000);
                                                                            openfloordoor();
                                                                            delay(1000);
                                                                            //and close them again
                                                                            closefloordoor();
                                                                            
                                                                
                                                                           Codechecker=false;                          //reset codecheck
                                                                           
                                                                           
                                                          } else{ //if code was not right just blink the red led
                                                          
                                                          
                                                                            digitalWrite(redledPin,HIGH);
                                                                            
                                                                            delay(100);
                                                                            
                                                                            digitalWrite(redledPin,LOW);
                                                                            
                                                                            
                                                          }//endofif codechecker
                        
                                     }//endofif programstate
                       
                        
                      
                       }  //end of if ringstate

}//endof loop









///////////////////functions///////////////
 
 
 //MeasureRingState
 //Measures the Voltage at the Ringpin and calculates if it is considered a Ring
 //if so it returns true, else false
 boolean MeasureRingState(){
   
   
                       RingStateVolts=analogRead(RingPin);              //reads the value of volts between 0 and 1023
                       RingStateVolts=float(map(RingStateVolts,0,1023,0,500))/100;//convert to volts (should be between 0V and 5V)
                       
                       
                       
                       // read the state of the Ringpin value, if it is above the threshold it is considered on
                       //otherwise off
                       
                       if(RingStateVolts>threshold){
                         
                                                    return true;
                       } else {
           
                                                    return false;
                            
                       }    
                       
                       
                       //default
                       Serial.println("Something went wrong in MeasureRingState()");
                       return false;
                       
 }//endof MeasureRingState()





 

// Listen to Ring
//Listens to the incoming Ringing Pattern and writes it into the Ringing Array

void ListenToRing(){
  
                                       Serial.println("in Listen to ring");    //signal the functionstart


                                       
                                      //define time at start of the listening and current time
                                      
                                      int timestart=millis();                //time at the beginning
                                      int now=millis();                      //current time
                                      
                                      
                                      //define 
                                      
                                      boolean Ringstatebefore=false;         //state of input before to check if the state before was ringon/ringoff
                                      boolean change=false;                  //change of input?
                                      int ring =0;                           //ring inkrementer
                            




	                            //fill Ringingarray empty


                                      for(int i=0;i<2*NumberofRings;i++){
                                	                               
                                  	                                  RingArray[i]=0;	                     	
                                		                                            
                                	                              }




	    //as long as the time between two rings is shorter than ringfinished and the amount of rings is smaller than maxrings
            //read the pattern

                	        do{
                  
                  
                		               RingState=MeasureRingState();      //read the inputstate of the Ring shoud be true if on
                		
                		
                		                
                                                //check if there was a change from ring to no ring or from no ring to ring
                                                //so first time Ringstatebefore is false-->change is true
       
                                                  if(RingState==Ringstatebefore){
                                    
                                                                                change=false;
                                                          
                                                  }  else {
                                                    
                                                                                change=true;
                                                          
                                                  }//and check if it changed
                		
                
                
                
                                      		//signal for the eyes
                                      
                                      		if(RingState && (change)){//if it changed from off to on signal on
                                        
                                                                          digitalWrite(ledPin, HIGH);
                                                          
                                                }
                                                
                                                
                                                
                                                      
                                      		if((RingState==false) && (change)){//if it changed from on to off
                                      
                                                                                  digitalWrite(ledPin, LOW);
                                                                                  
                                                }
                                                
                                                
                                                
                                                
                                                
                
                                    		//if the signal changed
                                    		if(change){
                                      
                                        			now=millis();//read the time
                                        
                                                                
                                        			
                                        			RingArray[ring]=(now-timestart);//and fill the entry 
                                        
                                        			Ringstatebefore=RingState;//reset the Beforerringstate
                                        
                                        			timestart=now;		//reset the time
                                        
                                                                 ring++;                //increase ring
                                        
                                        
                                        
                                                           /*
                                                               //if it is was ringed signal to computer and increase the ring
                                                               //stupid bc now it is only one array
                                                               
                                                             if(Ringstatebefore==false){
                                                              
                                                                	        	Serial.println(ring);
                                                                
                                                                                        ring++;                //increase ring
                                        		    }//endofif Ringstatebefore
                                                            */
                                    
                                    
                                    
                                    		}//endif change
                
                
                		                  now=millis(); //reset the time
                
                                                  delay(10);    // wait for the signal to fade
                
                
                	} while  (((now-timestart)<ringfinished) && (ring<NumberofRings*2));//end do while time to ring again



}//endof ListenToRing


// function MapArray
//Maps RingArray from 0 to 100, 100 beeing the sum of all


void MapArray(){
                                Serial.println("MapArray");
  
                                int Sum=0;
                                
                                
                                //calculate cumulated time
                                
                               
                               for(int i=0;i<NumberofRings*2;i++){
                                                                   
                                                                   Sum=Sum+RingArray[i];
                                                                   
                               } 
                                                                
                                
                                
                                
                                //Map Pattern fom 0  to 100
                                
                                for(int i=0;i<NumberofRings*2;i++){
                                                                   
                                                                    RingArray[i]=int(float(RingArray[i])*100/float(Sum));
                                                                    
                                }
                                

}//endof MapArray



//function ChangeMasterArray
// write the current array into the master array


void ChangeMasterArray(){
                         
 
                          Serial.println("ChangemasterArray");
                          
                          
                          for(int i=0;i<NumberofRings*2;i++){
                                                                                 
                                                          MasterArray[i]=RingArray[i];
                                                
                          } 
                          
  
  
}//endof ChangeMAsterArray





//function CheckSecretCode checks if the ringing pattern matches the secret code

boolean CheckSecretCode(int RingArray[],int MasterArray[]){
  
                                        Serial.println("Codecheck");  
                                        
                                        boolean checkthecode=false;                 //variable if the code is correct 
                                        boolean checkthecodearray[2*NumberofRings];  //array for checkthecode in order not to interrupt the distance measurement 
                                        
                                        int distance=0;                      //distance between the Masterarray and the Input
                                        int distancearray[2*NumberofRings];  //to check at the computer     
                                        
                                       
                                        //fill the distancearray empty
                                        
                                        for(int i=0;i<NumberofRings*2;i++){
                                                                           distancearray[i]=0;
                                        }                                  
                                         
                                     
                                     
                                        // check the whole array and calculate the difference to the masterarray
                                        
                                        
                                        for(int i=0;i<NumberofRings*2;i++){
                                                                        
                                                  
                                                                                             distance=abs(RingArray[i]-MasterArray[i]);    //calculate distance
                                                                                             distancearray[i]=distance;
                                                                                                                       
                                                                                             if(distance<=Sensibility){
                                                                                             
                                                                                                                       checkthecodearray[i]=true;
                                                                                               
                                                                                             } else {
                                                                                          
                                                                                                     checkthecodearray[i]=false;
                                                                                               
                                                                                             }//endifelse         
                                                   
                                                                                            
                                                                         
                                        } //endoffor
                                        
                                         
                                         Printpattern(distancearray);  //to check at the computer
                                         
                                         //need new array to check true,nottrue at the computer
                                         int checkarray [2*NumberofRings];
                                         for(int i=0;i<2*NumberofRings;i++){
                                             
                                                                            if(checkthecodearray[i]==true){
                                                                            
                                                                                                        checkarray[i]=1;
                                                                            }else{
                                                                              
                                                                                                        checkarray[i]=0;
                                                                            }//endofifelse
                                         }//endoffor
                                                                            
                                                                            
                                         Printpattern(checkarray);//to check at the computer
                                         
                                         
                                         
                                         
                                         //now check if there is a false in the checkthecodearray, if not the code is correct
                                         
                                         for(int i=0; i<2*NumberofRings;i++){
                                                                             
                                                                            //if the entry is true, checkthecode is true, if there is one false entry, return false directly
                                                                    
                                                                            if(checkthecodearray[i]==true){
                                                                              
                                                                                                            checkthecode=true;
                                                                              
                                                                            } else {
                                                                              
                                                                                    return false;
                                                                                    
                                                                            }//endofif
                                           
                                         }//endoffor
                                         
                                         
                                         
                                         
                                         
                                         //is true if the pattern is correct
                                          
                                          return checkthecode;
  
}//endof Check Secret Code





//function openmaindoor 
//opens the main door

void openmaindoor(){
  
                    Serial.println("open main door");
                    
                    digitalWrite(relaisPin, HIGH);   // set the LED on
                    
                    delay(3000);
                    
                    // wait for a second
                    digitalWrite(relaisPin, LOW);    // set the LED off
                    Serial.println("AUS");
                    delay(3000);              // wait for a second
 

  
}//endof openmaindoor()



//function openfloordoor()
//opens the entrance door to the flat

void openfloordoor(){
                      //if the door is already open, servooben is at 1 and servounten at 179 degrees
                      //theat means if they are not there, move them
                          
                       if(servooben.read()!=1 || servounten.read()!=179){
  
                                                                       Serial.println("Door Opens");
                                                                              
                                                                                 
                                                                       servooben.attach(ServoObenPin);//initialize the Servo that is above
                                                                       servounten.attach(ServoUntenPin);//initialize the Servo that is beyond
                                                                 
                                                                       //slowly move to open position  
                                                                         
                                                                       for(servoposition=0;servoposition<180;servoposition++){
    
                                                                                                                              servooben.write(180-servoposition);
                                                                                                                              servounten.write(servoposition);
                                                                                                                              delay(15);
    
                                                                        }//endof for
                                                                        
                                                                        //and detach to not make a sound while doing nothing
                                                                        
                                                                        servooben.detach();
                                                                        servounten.detach();
                                                                        
                       }else{
                         
                                                                        Serial.println("Door is already open");
                             
                       }
                      
}//endof openfloordoor()
  
  
  //function closefloordoor()
  //closes the entrance door to the flat
  
  
  void closefloordoor(){
   
                     //if the door is already open, servooben is at 179 and servounten at 1 degrees
                     //theat means if they are not there, move them 
    
                     if(servooben.read()!=179 || servounten.read()!=1){
                      
                                                                       Serial.println("Door Closes"); 
                                                                        
                                                                        
                                                                       servooben.attach(ServoObenPin);//initialize the Servo that is above
                                                                       servounten.attach(ServoUntenPin);//initialize the Servo that is beyond
                                                                         
                                                                       //and move them to position  
                                                                       for(servoposition=0;servoposition<180;servoposition++){
                                                      
                                                                                                                               servooben.write(servoposition);
                                                                                                                               servounten.write(180-servoposition);
                                                                                                                               delay(15);
                                                      
                                                                        }//endof for
                                                    
                                                                        //then detach the pins again s that they dont make noise
                                                                        servooben.detach();
                                                                        servounten.detach();
                                                                        
                      }else{
                         
                                                                        Serial.println("Door is already closed");
                             
                      }
  
  }//endof closefloordoor
  
  
  
  
  
  
  
  
  





//function Blink blinks the Pattern that is send to it

void Blink(int Blinkpattern []){
  
  
                          int delayI=0;
                          
                          
                          for(int i=0;i<NumberofRings*2;i++){
                          
                                                              //Off
                                                            digitalWrite(ledPin,LOW);
                                                            delayI=Blinkpattern[i]*20;
                                                            delay(delayI);
                                                             
                                                             i++;
                                                             
                                                             //On
                                                            digitalWrite(ledPin,HIGH);
                                                            delayI=Blinkpattern[i]*20;
                                                            delay(delayI);
                           
                           }//endoffor
                           
                          //Off again
                         digitalWrite(ledPin,LOW);
                         //wait short time
                         delay(50);


}//endof Blinkpattern





//function Printpattern Prints the Pattern to the Serial Port

void Printpattern(int Blinkpattern []){
            
                                          Serial.println("Printpattern");
  
                                         for(int i=0;i<NumberofRings*2;i++){
                                                                          
                                                                           Serial.print(Blinkpattern[i]);
                                                                           Serial.print("  ");
                                                                           
                                                                            
                                         } 
                                  Serial.println(" ");

}//endof PrintPattern
